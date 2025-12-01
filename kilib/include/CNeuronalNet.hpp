#pragma once
/* ==========================================================================
 * @(#)File: kilib/include/CNeuronalNet.hpp
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
#include <vector>
#include <functional>

// kilib/include/CNeuronalNet.hpp
namespace kilib {

class CNeuronalNet
{
public:
    enum struct Error
    {
        ok, notInited, param, tooLessLayers, zeroNeuronsInLayer, outOfMemory
    };

    CNeuronalNet() = default;
    ~CNeuronalNet();

    // This class is not ought to be copied or assigned!
    CNeuronalNet(const CNeuronalNet&) = delete;
    CNeuronalNet(CNeuronalNet&&) = delete;
    CNeuronalNet& operator= (const CNeuronalNet&) = delete;
    CNeuronalNet& operator= (CNeuronalNet&&) = delete;

    using VisitorFunct =  std::function<void(CLayer&)>;
    void visitLayers(const VisitorFunct& inVisitorFunct);

/*!
 * - Error::ok
 * - Error::zeroNeuronsInLayer
 * - Error::tooLessLayers
 */
    Error init(
        const std::vector<unsigned int>& inNeuronLayers,
        const CLayer::IActivation& inHiddenLayerActivation,
        const CLayer::IActivation& inOutputActivation,
        utils::CMath& inMath
        );

    CLayer* layer(unsigned int inIndex);
    CLayer* inputLayer();
    CLayer* outputLayer();

    const CLayer* outputLayer() const;

    using ValueVisitor = std::function<void(unsigned int index, float value)>;
    void forwardPropagation(const ValueVisitor&);

    // Information
    unsigned int nrOfLayers() const;
    unsigned int neuronsInLayer(unsigned int inLayerIndex) const;


    float* weightningForNeuronInLayer(unsigned int inLayerIndex, unsigned int inNeuronIndex, unsigned int inWeightningIndex);
    const float* weightningForNeuronInLayer(unsigned int inLayerIndex, unsigned int inNeuronIndex, unsigned int inWeightningIndex) const;

    float* biasForNeuronInLayer(unsigned int inLayerIndex, unsigned int inNeuronIndex);
    const float* biasForNeuronInLayer(unsigned int inLayerIndex, unsigned int inNeuronIndex) const;

private: // Private Methods
    void _cleanup();
    void _connect(CLayer* inInputLayer, CLayer* inOutputLayer);

private:
    std::vector<CLayer*> m_Layers;

    const CLayer::IActivation* m_HiddenLayerActivation = nullptr;
    const CLayer::IActivation* m_OutputActivation = nullptr;
    const utils::CMath* m_Math = nullptr;
};
} // namespace kilib
