/* ==========================================================================
 * @(#)File: kilib/src/CNeuronalNet.cpp
 * --------------------------------------------------------------------------
 *  (c)1982-2025 Tangerine-Software
 *
 *       Hans-Peter Beständig
 *       Kühbachstr. 8
 *       81543 München
 *       GERMANY
 *
 *       mailto:hdusel@tangerine-soft.de
 *       http://hdusel.tangerine-soft.de
 * --------------------------------------------------------------------------
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to
 *
 *       Free Software Foundation, Inc.
 *       59 Temple Place - Suite 330
 *       Boston, MA  02111-1307, USA
 *
 *   Notice, that ``free software'' addresses the fact that this program
 *   is __distributed__ under the term of the GNU General Public License
 *   and because of this, it can be redistributed and modified under the
 *   conditions of this license, but the software remains __copyrighted__
 *   by the author. Don't intermix this with the general meaning of
 *   Public Domain software or such a derivated distribution label.
 *
 *   The author reserves the right to distribute following releases of
 *   this program under different conditions or license agreements.
 *
 * ========================================================================== */
#include "kilib/include/CNeuronalNet.hpp"
#include "kilib/include/CLayer.hpp"
#include "kilib/include/Activation.hpp"
#include <cassert>
#include <cstdio>
#include <algorithm>

namespace kilib {
// ==========================================================================
// class CNeuronalNet - public
// ==========================================================================

CNeuronalNet::~CNeuronalNet()
{
    _cleanup();
}

CLayer* CNeuronalNet::layer(unsigned int inIndex)
{
    return inIndex < m_Layers.size() ? m_Layers[inIndex] : nullptr;
}

CLayer* CNeuronalNet::outputLayer()
{
    return m_Layers.empty() ? nullptr : m_Layers.back();
}

const CLayer* CNeuronalNet::outputLayer() const
{
    return m_Layers.empty() ? nullptr : m_Layers.back();
}

CLayer* CNeuronalNet::inputLayer()
{
    return m_Layers.empty() ? nullptr : m_Layers.front();
}

auto CNeuronalNet::init(
    const std::vector<unsigned int>& inNeuronLayers,
    const CLayer::IActivation& inHiddenLayerActivation,
    const CLayer::IActivation& inOutputActivation,
    utils::CMath& inMath
    ) -> Error
{
    Error error;

    printf("*** inNeurons.size = %ld\n", inNeuronLayers.size());
    if (inNeuronLayers.empty())
    {
        error = Error::tooLessLayers;
    }
    else
    {
        // Be optimistic
        error = Error::ok;

        // Step 1: Create the Layers
        m_Layers.reserve(inNeuronLayers.size());

        CLayer *parentLayer = nullptr;
        for (unsigned int i = 0; i < inNeuronLayers.size(); ++i)
        {
            const unsigned int nrOfNeuronsInThisLayer = inNeuronLayers[i];
            if (0 == nrOfNeuronsInThisLayer)
            {
                error = Error::zeroNeuronsInLayer;
                _cleanup();
                break;
            }

            CLayer* thisLayer = new(std::nothrow) CLayer();
            assert(thisLayer);
            if (thisLayer)
            {
                const bool isFirstLayer = (0 == i);
                const bool isOutputLayer = (inNeuronLayers.size()-1 == i);

                const CLayer::IActivation* thisActivation;
                if (isFirstLayer)
                {
                    static kilib::CActivationNull nullActivation;
                    thisActivation = &nullActivation;
                }
                else if (isOutputLayer)
                {
                    thisActivation = &inOutputActivation;
                }
                else
                {
                    thisActivation = &inHiddenLayerActivation;
                }

                thisLayer->init(nrOfNeuronsInThisLayer,
                                *thisActivation,
                                inMath,
                                parentLayer);

                m_Layers.push_back(thisLayer);
            }
            else
            {
                error = Error::outOfMemory;
                _cleanup();
                break;
            }
            parentLayer = thisLayer;
        }
    }
    return error;
}

void CNeuronalNet::visitLayers(const VisitorFunct& inVisitorFunct)
{
    std::for_each(m_Layers.begin(), m_Layers.end(), [this, &inVisitorFunct](CLayer* thisLayer){
        assert(thisLayer);
        if (thisLayer)
        {
            inVisitorFunct(*thisLayer);
        }
    });
}

void CNeuronalNet::forwardPropagation(const ValueVisitor& inValueVisitor)
{
    const CLayer* _outputLayer = outputLayer();
    assert(_outputLayer);
    if (_outputLayer)
    {
        if (_outputLayer->nrOfNeurons() > 0)
        {
            const utils::CVectorF32* res = _outputLayer->neuronOutputVector();
            assert(res);
            assert(res->size() == _outputLayer->nrOfNeurons() + 1);
            if (res)
            {
#if 0
                // Iterate through all Layers
                visitLayers([](CLayer& thisLayer)->void{
                    thisLayer.forwardPropagation();
                });
#else
                const_cast<CLayer*>(_outputLayer)->forwardPropagation(true);
#endif
                for (unsigned int neronIndex = 0; neronIndex < _outputLayer->nrOfNeurons(); ++neronIndex)
                {
                    inValueVisitor(neronIndex, (*res)[neronIndex]);
                }
            }
        }
    }
}

unsigned int CNeuronalNet::nrOfLayers() const
{
    return static_cast<unsigned int>(m_Layers.size());
}

unsigned int CNeuronalNet::neuronsInLayer(unsigned int inLayerIndex) const
{
    return (inLayerIndex < m_Layers.size()) ? m_Layers[inLayerIndex]->nrOfNeurons() : 0;
}

float* CNeuronalNet::weightningForNeuronInLayer(unsigned int inLayerIndex, unsigned int inNeuronIndex, unsigned int inWeightningIndex)
{
    float* weightningPtr = nullptr;

    if (inLayerIndex < m_Layers.size())
    {
        CLayer* thisLayer = m_Layers[inLayerIndex];
        weightningPtr = thisLayer->weightningAtIndexForNeuron(inNeuronIndex, inWeightningIndex);
    }
    return weightningPtr;
}

const float* CNeuronalNet::weightningForNeuronInLayer(unsigned int inLayerIndex, unsigned int inNeuronIndex, unsigned int inWeightningIndex) const
{
    return const_cast<const float*>(const_cast<CNeuronalNet*>(this)->weightningForNeuronInLayer(inLayerIndex, inNeuronIndex, inWeightningIndex));
}

float* CNeuronalNet::biasForNeuronInLayer(unsigned int inLayerIndex, unsigned int inNeuronIndex)
{
    float* weightningPtr = nullptr;

    if (inLayerIndex < m_Layers.size())
    {
        CLayer* thisLayer = m_Layers[inLayerIndex];
        weightningPtr = thisLayer->biasForNeuron(inNeuronIndex);
    }
    return weightningPtr;
}

const float* CNeuronalNet::biasForNeuronInLayer(unsigned int inLayerIndex, unsigned int inNeuronIndex) const
{
    return const_cast<const float*>(const_cast<CNeuronalNet*>(this)->biasForNeuronInLayer(inLayerIndex, inNeuronIndex));
}

// ==========================================================================
// class CNeuronalNet - private
// ==========================================================================
void CNeuronalNet::_cleanup()
{
    for (auto thisLayer : m_Layers)
    {
        delete thisLayer;
    }
    m_Layers.clear();

    m_HiddenLayerActivation = nullptr;
    m_OutputActivation = nullptr;
    m_Math = nullptr;
}

} // namespace kilib
