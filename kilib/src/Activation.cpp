/* ==========================================================================
 * @(#)File: kilib/src/Activations.cpp
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
#include "kilib/include/Activation.hpp"
#include <cmath>

/*!
 * @brief The *sigmoid* Activation Function.
 *
 * The Math of this activation function is
 * \f[
 *      y({inValue}) = \frac{1}{1 + e^{-{inValue}}}
 * \f]
 *
 * @param inValue The Input Value
 * @return the Output of this Activation function according the documentation above.
 */
static float _activationFunctionSigmoid(float inValue)
{
    return 1.f / (1.f + expf(-inValue));
}

namespace kilib {

// ==========================================================================
// class CActivationNull : public CLayer::IActivation
// ==========================================================================
float CActivationNull::activation(float inLearningRate, float inIntegratedValue, float inThisValue) const
{
    return inThisValue;
}

// ==========================================================================
// class CActivationReLU : public CLayer::IActivation
// ==========================================================================
float CActivationReLU::activation(float inLearningRate, float inIntegratedValue, float inThisValue) const
{
    return std::fmaxf(0.f, inThisValue);
}

// ==========================================================================
// class CActivationLeakyReLU : public CLayer::IActivation
// ==========================================================================
float CActivationLeakyReLU::activation(float inLearningRate, float inIntegratedValue, float inThisValue) const
{
    return (inThisValue >= 0) ? inThisValue : inThisValue * inLearningRate;
}

// ==========================================================================
// class CActivationSigmoid : public CLayer::IActivation
// ==========================================================================
float CActivationSigmoid::activation(float inLearningRate, float inIntegratedValue, float inThisValue) const
{
    return _activationFunctionSigmoid(inThisValue) - 1.f;
}

// ==========================================================================
// class CActivationTanh : public CLayer::IActivation
// ==========================================================================
float CActivationTanh::activation(float inLearningRate, float inIntegratedValue, float inThisValue) const
{
    return 2.f * _activationFunctionSigmoid(2.f * inThisValue) - 1.f;
}

// ==========================================================================
// class CActivationSoftmax : public CLayer::IActivation
// ==========================================================================
float CActivationSoftmax::activation(float inLearningRate, float inIntegratedValue, float inThisValue) const
{
    assert(0 != inIntegratedValue);
    if (0.f != inIntegratedValue)
    {
        return inThisValue / inIntegratedValue;
    }
    return 0;
}

} // namespace kilib
