#pragma once
/* ==========================================================================
 * @(#)File: kilib/include/Activation.hpp
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

namespace kilib {
/*!
 * @brief The *null* Activation Function is actual an activation without any
 * side effect.
 *
 * The "Null Activation" simply forwards the input value "as is" as an output
 * disregard of any learning rate or integral value.
 *
 * @author "Hans-Peter Beständig"<hdusel@tangerine-soft.de>
 */
class CActivationNull : public CLayer::IActivation
{
public:
    CActivationNull() = default;

    /*!
     * @brief The NULL Activation **does not** need the integral Part.
     * So return false here!
     *
     * @return false in any case.
     */
    virtual bool needsIntegralPart() const override {return false;}

    /*!
     * @brief The *NULL* Activation Function.
     *
     * The Math of this activation function is:
     * \f[
     *     y({inThisValue}) = inThisValue
     * \f]
     *
     * @param inLearningRate **Not used for this Implementation**!
     * @param inIntegratedValue **Not used for this Implementation**!
     * @param inThisValue The Input Value
     * @return the Output of this Activation function according the documentation above.
     */
    virtual float activation(float inLearningRate, float inIntegratedValue, float inThisValue) const override;
}; // class CActivationNull;

/*!
 * @brief The *reLU* (<em><b>re</b>ctified <b>L</b>inear <b>U</b>nit</em>) Activation Function.
 *
 * @author "Hans-Peter Beständig"<hdusel@tangerine-soft.de>
 */
class CActivationReLU : public CLayer::IActivation
{
public:
    CActivationReLU() = default;

    /*!
     * @brief The reLU **does not** need the integral Part. So return false here!
     * @return false in any case.
     */
    virtual bool needsIntegralPart() const override {return false;}

    /*!
     * @brief The *reLU* (<em><b>re</b>ctified <b>L</b>inear <b>U</b>nit</em>) Activation Function.
     *
     * The Math of this activation function is:
     * \f[
     *     y({inThisValue}) = \left\{\begin{array}{c l}
     *              0        &\textrm{if } {inThisValue} \lt 0\\
     *           {inThisValue}   &\textrm{otherwise}
     *     \end{array}\right.
     * \f]
     *
     * @param inLearningRate **Not used for this Implementation**!
     * @param inIntegratedValue **Not used for this Implementation**!
     * @param inThisValue The Input Value
     * @return the Output of this Activation function according the documentation above.
     */
    virtual float activation(float inLearningRate, float inIntegratedValue, float inThisValue) const override;
}; // class CActivationReLU;

/*!
 * @brief The *reLU* (<em><b>re</b>ctified <b>L</b>inear <b>U</b>nit</em>) Activation Function.
 *
 * @author "Hans-Peter Beständig"<hdusel@tangerine-soft.de>
 */
class CActivationLeakyReLU : public CLayer::IActivation
{
public:
    CActivationLeakyReLU() = default;

    /*!
     * @brief The reLU **does not** need the integral Part. So return false here!
     * @return false in any case.
     */
    virtual bool needsIntegralPart() const override {return false;}

    /*!
     * @brief The *reLU* (<em><b>re</b>ctified <b>L</b>inear <b>U</b>nit</em>) Activation Function.
     *
     * The Math of this activation function is:
     * \f[
     *     y({inThisValue},{inLearningRate}) = \left\{\begin{array}{c l}
     *           {inThisValue \cdot inLearningRate}  &\textrm{if } {inThisValue} \lt 0\\
     *           {inThisValue}   &\textrm{otherwise}
     *     \end{array}\right.
     * \f]
     *
     * @param inLearningRate This determines the amount of negative Values
     * @param inIntegratedValue **Not used for this Implementation**!
     * @param inThisValue The Input Value
     * @return the Output of this Activation function according the documentation above.
     */
    virtual float activation(float inLearningRate, float inIntegratedValue, float inThisValue) const override;
}; // class CActivationLeakyReLU;

/*!
 * @brief The *simoid* Activation Function.
 *
 * @author "Hans-Peter Beständig"<hdusel@tangerine-soft.de>
 */
class CActivationSigmoid : public CLayer::IActivation
{
public:
    CActivationSigmoid() = default;

    /*!
     * @brief The simoid **does not** need the integral Part. So return false here!
     * @return false in any case.
     */
    virtual bool needsIntegralPart() const override {return false;}

    /*!
     * @brief The *tanh* Activation Function.
     *
     * The Math of this activation function is
     * \f[
     *      y({inThisValue}) = \frac{1}{1 + e^{-{inThisValue}}}
     * \f]
     *
     *
     * @param inLearningRate **Not used for this Implementation**!
     * @param inIntegratedValue **Not used for this Implementation**!
     * @param inThisValue The Input Value
     * @return the Output of this Activation function according the documentation above.
     */
    virtual float activation(float inLearningRate, float inIntegratedValue, float inThisValue) const override;
}; // class CActivationSigmoid;

/*!
 * @brief The *tanh* Activation Function.
 *
 * @author "Hans-Peter Beständig"<hdusel@tangerine-soft.de>
 */
class CActivationTanh : public CLayer::IActivation
{
public:
    CActivationTanh() = default;

    /*!
     * @brief The Tanh **does not** need the integral Part. So return false here!
     * @return false in any case.
     */
    virtual bool needsIntegralPart() const override {return false;}

    /*!
     * @brief The *tanh* Activation Function.
     *
     * The Math of this activation function is
     * \f[
     *      y({inThisValue}) = \textrm{tanh}({inThisValue}) = \frac{2}{1 + e^{-2 \cdot {inThisValue}}} - 1\\
     *      = 2 \cdot \textrm{sigmoid}(2\cdot inThisValue) - 1
     * \f]
     *
     *
     * @param inLearningRate **Not used for this Implementation**!
     * @param inIntegratedValue **Not used for this Implementation**!
     * @param inThisValue The Input Value
     * @return the Output of this Activation function according the documentation above.
     */
    virtual float activation(float inLearningRate, float inIntegratedValue, float inThisValue) const override;
}; // class CActivationTanh;

/*!
 * @brief The *softmax* Activation Function.
 *
 * @author "Hans-Peter Beständig"<hdusel@tangerine-soft.de>
 */
class CActivationSoftmax : public CLayer::IActivation
{
public:
    CActivationSoftmax() = default;

    /*!
     * @brief The sotmax **does** need the integral Part. So return true here!
     * @return false in any case.
     */
    virtual bool needsIntegralPart() const override {return true;}

    /*!
     * @brief The *softmax* Activation Function.
     *
     * The Math of this activation function is
     * \f[
     *      y({inValue}) = \textrm{softmax}({inValue, inIntegralValue}) = \frac{inValue, inIntegralValue}{}
     * \f]
     *
     * @param inLearningRate **Not used for this Implementation**!
     * @param inIntegratedValue This is the precalculated integrated sum of all
     *     neurons that are subject for this calculation.
     * @param inThisValue The Input Value
     * @return the Output of this Activation function according the documentation above.
     */
    virtual float activation(float inLearningRate, float inIntegratedValue, float inThisValue) const override;
}; // class CActivationSoftmax;

} // namespace kilib
