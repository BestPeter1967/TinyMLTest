/* ==========================================================================
 * @(#)File: kilib/src/CLayer.cpp
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
#include "kilib/include/CLayer.hpp"
#include "kilib/include/Activation.hpp"
#include "utils/include/CMath.hpp"
#include <cassert>
#include <new>

static kilib::CActivationNull nullActivation;

static utils::CVectorF32* _allocateOutputValueVector(unsigned int inNrOfNeurons)
{
    utils::CVectorF32* ret = new(std::nothrow) utils::CVectorF32(inNrOfNeurons + 1);
    assert(ret);
    if (ret)
    {
        (*ret)[inNrOfNeurons] = 1.f; // Neutral Part for weightning offset calculation!
    }
    return ret;
}

static bool _allocateWeightningsVector(
    const kilib::CLayer* inParentLayer,
    unsigned int inNrOfNeurons,
    std::vector<utils::CVectorF32*>& outVector)
{
    bool success = true;

    if (inParentLayer)
    {
        outVector.reserve(inNrOfNeurons);
        const unsigned int nrOfParentNeurons = inParentLayer->nrOfNeurons();

        for (unsigned int thisNeuronIndex = 0; thisNeuronIndex < inNrOfNeurons; ++thisNeuronIndex)
        {
            utils::CVectorF32* thisVector = new(std::nothrow) utils::CVectorF32(nrOfParentNeurons+1);
            assert(thisVector);
            if (nullptr == thisVector)
            {
                success = false;
                break;
            }

            outVector.push_back(thisVector);

            // Randomize the Weightnings!
            thisVector->for_each([](float& vectorValue)->void{
                vectorValue = utils::CMath::randF32(0.f, 1.f);
            });
            (*thisVector)[thisVector->size()-1] = 0.f;
        }
    }

    return success;
}

namespace kilib {
CLayer::~CLayer()
{
    _cleanup();
}

// ==========================================================================
// CLayer - public
// ==========================================================================
bool CLayer::init(
        unsigned int inNrOfNeurons,
        const IActivation& inActivation,
        utils::CMath& inMath,
        CLayer* inParentLayer)
{
    bool success = false;
    if (inNrOfNeurons > 0)
    {
        _cleanup();

        m_Activation = &inActivation;
        m_ParentLayer = inParentLayer;
        m_Math = &inMath;

        m_OutputVector = _allocateOutputValueVector(inNrOfNeurons);
        assert(m_OutputVector);
        if (nullptr != m_OutputVector)
        {
            if (true == _allocateWeightningsVector(inParentLayer, inNrOfNeurons, m_WeightningVectors))
            {
                success = true;
            }
            else // Failed to allocate the Weightning Buffer
            {
                _cleanup();
            }
        }
    }
    return success;
}

const utils::CVectorF32* CLayer::forwardPropagation(bool recalcParentLayers)
{
    if (recalcParentLayers && m_ParentLayer)
    {
        m_ParentLayer->forwardPropagation(recalcParentLayers);
    }

    utils::CVectorF32* ret = nullptr;
    assert(m_OutputVector);
    assert(m_Math);

    if (isInited())
    {
        if (m_ParentLayer)
        {
            for (unsigned int thisNeuronIndex = 0; thisNeuronIndex < nrOfNeurons(); ++thisNeuronIndex)
            {
                const utils::CVectorF32* weightningValues = _neuronWeightningVector(thisNeuronIndex);
                assert(weightningValues);

                const utils::CVectorF32* parentOutputValues = _parentNeuronOutputVector();
                assert(parentOutputValues);

                assert(parentOutputValues->size() == weightningValues->size());

                (*m_OutputVector)[thisNeuronIndex] = m_Math->calcDotF32(*weightningValues, *parentOutputValues);
            }

        } // if (m_ParentLayer)
        if (m_Activation)
        {
            float integralPart = 1.f;
            if (m_Activation->needsIntegralPart())
            {
                integralPart = m_Math->sumUpVector(*m_OutputVector, 0, m_OutputVector->size() - 1);
            }

            for (unsigned int thisNeuronIndex = 0; thisNeuronIndex < nrOfNeurons(); ++thisNeuronIndex)
            {
                (*m_OutputVector)[thisNeuronIndex] = m_Activation->activation(0.f, integralPart, (*m_OutputVector)[thisNeuronIndex]);
            }
        }
    } // if (isInited())
    return m_OutputVector;
}

const utils::CVectorF32* CLayer::neuronOutputVector() const
{
    return m_OutputVector;
}

utils::CVectorF32* CLayer::neuronOutputVector()
{
    return m_OutputVector;
}

unsigned int CLayer::nrOfNeurons() const
{
    return static_cast<unsigned int>(m_OutputVector ? m_OutputVector->size() - 1 : 0);
}

float* CLayer::weightningAtIndexForNeuron(unsigned int inNeuronIndex, unsigned int inWeightningIndex)
{
    float* weightningPtr = nullptr;

    utils::CVectorF32* weightningVectorPtr = CLayer::weightningVectorForNeuronAtIndex(inNeuronIndex);
    if (weightningVectorPtr && (weightningVectorPtr->size() > 0) && (inWeightningIndex < weightningVectorPtr->size() - 1))
    {
        weightningPtr = &((*weightningVectorPtr)[inWeightningIndex]);
    }

    return weightningPtr;
}

const float* CLayer::weightningAtIndexForNeuron(unsigned int inNeuronIndex, unsigned int inWeightningIndex) const
{
    return const_cast<const float*>(const_cast<CLayer*>(this)->weightningAtIndexForNeuron(inNeuronIndex, inWeightningIndex));
}

float* CLayer::biasForNeuron(unsigned int inNeuronIndex)
{
    float* biasPtr = nullptr;

    utils::CVectorF32* weightningVectorPtr = CLayer::weightningVectorForNeuronAtIndex(inNeuronIndex);
    if (weightningVectorPtr && (weightningVectorPtr->size() > 0))
    {
        biasPtr = &((*weightningVectorPtr)[weightningVectorPtr->size() - 1]);
    }
    return biasPtr;
}

const float* CLayer::biasForNeuron(unsigned int inNeuronIndex) const
{
    return const_cast<const float*>(const_cast<CLayer*>(this)->biasForNeuron(inNeuronIndex));
}

bool CLayer::isInputLayer() const
{
    return nullptr == m_ParentLayer;
}

bool CLayer::isInited() const
{
    return (nullptr != m_Math) && (nullptr != m_OutputVector);
}

utils::CVectorF32* CLayer::weightningVectorForNeuronAtIndex(unsigned int inNeuronIndex)
{
    utils::CVectorF32* weightningVector = nullptr;
    if (isInited() && m_ParentLayer)
    {
        weightningVector = _neuronWeightningVector(inNeuronIndex);
    }
    return weightningVector;
}

const utils::CVectorF32* CLayer::weightningVectorForNeuronAtIndex(unsigned int inNeuronIndex) const
{
    return const_cast<utils::CVectorF32*>(const_cast<CLayer*>(this)->weightningVectorForNeuronAtIndex(inNeuronIndex));
}

unsigned int CLayer::nrOfWeightnings() const
{
    return _nrOfParentNeurons();
}

// ==========================================================================
// CLayer - private
// ==========================================================================
void CLayer::_cleanup()
{
    delete m_OutputVector;
    m_OutputVector = nullptr;

    // Delete every Vector
    for (utils::CVectorF32* thisVector : m_WeightningVectors)
    {
        assert(thisVector);
        delete thisVector;
    }
    m_WeightningVectors.clear();
}

utils::CVectorF32* CLayer::_neuronWeightningVector(unsigned int forNeuronIndex)
{
    utils::CVectorF32* weightningVectorPtr = nullptr;
    if (forNeuronIndex < m_WeightningVectors.size())
    {
        weightningVectorPtr = m_WeightningVectors[forNeuronIndex];
    }
    return weightningVectorPtr;
}

unsigned int CLayer::_nrOfParentNeurons() const
{
    return m_ParentLayer ? m_ParentLayer->nrOfNeurons() : 0;
}

const utils::CVectorF32* CLayer::_parentNeuronOutputVector() const
{
    assert(m_ParentLayer);
    return m_ParentLayer ? m_ParentLayer->neuronOutputVector() : 0;
}

} // namespace kilib
