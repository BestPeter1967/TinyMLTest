#pragma once
/* ==========================================================================
 * @(#)File: kilib/include/CLayer.hpp
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
#include <functional>
#include "utils/include/CVector.hpp"
#include <vector>

namespace utils {class CMath;} // Forward decl.

namespace kilib {
#if 0
/*!
 * @brief The *reLU* (<em><b>re</b>ctified <b>L</b>inear <b>U</b>nit</em>) Activation Function.
 *
 * The Math of this activation function is:
 * \f[
 *     y({inValue}) = \left\{\begin{array}{c l}
 *              0        &\textrm{if } {inValue} \lt 0\\
 *           {inValue}   &\textrm{otherwise}
 *     \end{array}\right.
 * \f]
 *
 * @param inValue The Input Value
 * @return the Output of this Activation function according the documentation above.
 * @see ActivationFunctionLeakyReLu(float, float)
 */
float ActivationFunctionReLu(float inValue)
{
    return std::fmaxf(0.f, inValue);
}

/*!
 * @brief The *LeakyreLU* (Leaky-<em><b>re</b>ctified <b>L</b>inear <b>U</b>nit</em>) Activation Function.
 *
 * Compared with the reLU Function this one takes care not to clamp negative Values
 * agains 0 but to the input value with a given scale.
 *
 * The Math of this activation function is:
 * \f[
 *     y({inValue},{inAlpha}) = \left\{\begin{array}{c l}
 *           {inValue \cdot inAlpha}  &\textrm{if } {inValue} \lt 0\\
 *           {inValue}   &\textrm{otherwise}
 *     \end{array}\right.
 * \f]
 *
 * @param inValue The Input Value
 * @param inAlpha The Learning rate.
 * @return the Output of this Activation function according the documentation above.
 * @see ActivationFunctionReLu(float)
 */
float ActivationFunctionLeakyReLu(float inValue, float inAlpha)
{
    return std::fmaxf(0.f, inValue);
}

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
float ActivationFunctionSigmoid(float inValue)
{
    return 1.f / (1.f + expf(-inValue));
}

/*!
 * @brief The *tanh* Activation Function.
 *
 * The Math of this activation function is
 * \f[
 *      y({inValue}) = \textrm{tanh}({inValue}) = \frac{2}{1 + e^{-2 \cdot {inValue}}} - 1\\
 *      = 2 \cdot \textrm{sigmoid}(2\cdot inValue) - 1
 * \f]
 *
 * @param inValue The Input Value
 * @return the Output of this Activation function according the documentation above.
 */
float ActivationFunctionTanh(float inValue)
{
    return 2.f * ActivationFunctionSigmoid(2.f * inValue) - 1.f;
}

/*!
 * @brief The *tanh* Activation Function.
 *
 * The Math of this activation function is
 * \f[
 *      y({inValue}) = \textrm{softmax}({inValue, inIntegralValue}) = \frac{inValue, inIntegralValue}{}
 * \f]
 *
 * @param inValue The Input Value
 * @param inIntegralValue The Integral Part of all Layers output Values.
 *
 * @return the Output of this Activation function according the documentation above.
 */
float ActivationFunctionSoftmax(float inValue, float inIntegralValue)
{
    assert(0 != inIntegralValue);
    if (0.f != inIntegralValue)
    {
        return inValue / inIntegralValue;
    }
}
#endif

/*!
 * Model of a neuronal Network Layer. A Layer class consists of
 * - A specific number of Neurons
 * - A reference to a Parent Layer (if it is not the Input Layer)
 * - As many Weightnings as the parent Layer
 * - A bias
 * - An optional Activation function
 * @author "Hans-Peter Beständig" <hdusel@tangerine-soft.de>
 */
class CLayer
{
public:
    /*!
     * @brief The Actication class is supposed to model a Activation function that
     * will be used for the layers forward Proagation.
     * This is a Interface that needs an implementation.
     * One Implementation is the 
     * @author "Hans-Peter Beständig" <hdusel@tangerine-soft.de>
     */
    class IActivation
    {
    public:
        IActivation() = default;
        virtual ~IActivation() = default;

        virtual bool needsIntegralPart() const = 0;
        virtual float activation(float inLearningRate, float inIntegtedValue, float inThisValue) const = 0;
    }; // class IActivation

    CLayer() = default;
    ~CLayer();

    // This class is not ought to be copied or assigned!
    CLayer(const CLayer&) = delete;
    CLayer(CLayer&&) = delete;
    CLayer& operator= (const CLayer&) = delete;
    CLayer& operator= (CLayer&&) = delete;

    /*!
     * @brief initializes this neuronal layer.
     *
     * A neuronal layers properties are:
     *
     *   - It total ount of neurons
     *   - An activation Function
     *   - A Math Object supposed to do the calculation. This handed over as
     *     an explicit dependent object in order to allow to perform optimized
     *     calculations (see CMath for details)
     *   - A parent Layer (if this is not the input layer)
     *
     * @note Every usage of this objects others methods needs an prevoious call
     *       if init once! Otherwise their function is undefined!
     *
     * @param inNrOfNeurons The number of neurons of this layer. This needs to
     *        be at least 1!
     *
     * @param inActivation The activation object that will be used to
     *     calculate the neurons output value.
     *     If this is \p nullptr then no explictit activation function will be
     *     employed.
     *
     * @param inMath The Mathematical instance that will be employed to perform
     *     the calculation.
     * @param inParentLayer The Parent layer. That is the Layer that feeds in this
     *     Layer by its output. For the Input Layer this is a \p nullptr.
     *
     * @return true for success, false upon \p inNrOfNeurons is 0 which is an error.
     */
    bool init(
        unsigned int inNrOfNeurons,
        const IActivation& inActivation,
        utils::CMath& inMath,
        CLayer* inParentLayer = nullptr);

    /*!
     * @brief The number of Neurons of this layer.
     * @return The number of neurons of this layer. this is 0 if init has not
     * been called yet
     */
    unsigned int nrOfNeurons() const;

    /*!
     * @brief Performs a forward propagation of this layers Values.
     *
     * This propagation does honor the Activation function to calcualte its
     * output values.
     *
     * @note The User has the freedom to perform a forward propagation by triggering
     * a forward propagation of all its feeding layers in advance or not.
     *
     * @param recalcParentLayers This allows the recalculation (that is a recursive
     * call of forwardPropagation() towards all parent layers) of this Layer.
     * If this is false (default if omitted) then the actual Value of the previous
     * layers neurons output will be used.
     *
     * @return A Vector that contains the output values after the propagation.
     * this is nullptr if this object has not been init already.
     */
    const utils::CVectorF32* forwardPropagation(bool recalcParentLayers = true);

    /*!
     * @brief Returns an Pointer to an immutable Vector that represents the
     * Output of all neurons that belong to this layer.
     * The Vector has a total count of the number of neurons of this layer (#nrOfNeurons())
     * **plus one** neutral element **as its last element** tha has to be ignored by the user!
     *
     * @return A Pointer that points to the Output data of all neurons of this layer.
     * This pointer will be nullptr if init has not be called once before!
     * @see neuronOutputVector()
     * @see nrOfNeurons()
     */
    const utils::CVectorF32* neuronOutputVector() const;

    /*!
     * @brief Returns an Pointer to an mutable Vector that represents the
     * Output of all neurons that belong to this layer.
     * The Vector has a total count of the number of neurons of this layer (#nrOfNeurons())
     * **plus one** neutral element **as its last element** tha has to be ignored by the user!
     *
     * @return A Pointer that points to the Output data of all neurons of this layer.
     * This pointer will be nullptr if init has not be called once before!
     * @see neuronOutputVector() const
     * @see nrOfNeurons()
     */
    utils::CVectorF32* neuronOutputVector();

    /*!
     * @brief Returns an Pointer to an mutable Vector that represents the
     * Weigthnings of a certain neuron that belong to this layer.
     *
     * The returned Vector has a total count of weightnings that equals
     * the number of the neurons of the parent layer (the layer that feeds this layer)
     * **plus one** element that represents theneurons bias value.
     * the count of weigthnings may be querid by the method #nrOfWeightnings()
     *
     * @return A Pointer that points to the weigthnins of all neurons of this layer.
     * This pointer will be nullptr if init has not be called once before!
     * Forthermore this will be nullptr if this layer is an input layer because
     * input layers does not use weightnings and biases as well.
     *
     * @see weightningVectorForNeuronAtIndex(unsigned int) const
     * @see nrOfWeightnings()
     */
    utils::CVectorF32* weightningVectorForNeuronAtIndex(unsigned int inNeuronIndex);

    /*!
     * @brief Returns an Pointer to an immutable Vector that represents the
     * Weigthnings of a certain neuron that belong to this layer.
     *
     * The returned Vector has a total count of weightnings that equals
     * the number of the neurons of the parent layer (the layer that feeds this layer)
     * **plus one** element that represents theneurons bias value.
     * the count of weigthnings may be querid by the method #nrOfWeightnings()
     *
     * @return A Pointer that points to the weigthnins of all neurons of this layer.
     * This pointer will be nullptr if init has not be called once before!
     * Forthermore this will be nullptr if this layer is an input layer because
     * input layers does not use weightnings and biases as well.
     *
     * @see weightningVectorForNeuronAtIndex(unsigned int) const
     * @see nrOfWeightnings()
     */
    const utils::CVectorF32* weightningVectorForNeuronAtIndex(unsigned int inNeuronIndex) const;

    /*!
     * @brief Asks about the count of weightnings of each neuron of this layer.
     * @return The number of Weightnings. This is 0 if either the layer has not
     * yet be initialized or of the Layer is the Input layer. Input layers does
     * not contain weightnings.
     */
    unsigned int nrOfWeightnings() const;

    /*!
     * @brief Returns a Pointer to an immutable float of a specific weightning.
     * Since every Neuron has n weightnings that are dependent from the Neurnal layer it
     * gets it date (le left handed layer) every weightnings is adressed by two indices:
     *
     * 1. The Neuron of this Layer [0... #nrOfNeurons-1]
     * 2. The Weightning of this neurons input in respect of its feeding (left handded (output) layer.
     *    [0... parents_layer.#nrOfNeurons-1]
     *
     * @param inNeuronIndex The index of the neuron to gain access the weightning
     *          [0... #nrOfNeurons-1]
     * @param inWeightningIndex The index of the weighting of the neurons input to
     *          gain access the weightning [0... #nrOfNeurons-1]
     * @return A Pointer that points to the Output data of all neurons of this layer.
     *      If either \p inNeuronIndex or \p inWeightningIndex is out of bounds
     *      then the returned pointer is nullptr!
     *
     *      If this Layer is an **Input Layer** then this pointer is nullptr in any case
     *      because a Input Layer does not have a weightning
     *
     *      This pointer will be nullptr either if init has not be called once before!
     *
     * @see weightningAtIndexForNeuron()
     * @see nrOfNeurons()
     */
    const float* weightningAtIndexForNeuron(unsigned int inNeuronIndex, unsigned int inWeightningIndex) const;

    /*!
     * @brief Returns a Pointer to an mutable float of a specific weightning.
     * Since every Neuron has n weightnings that are dependent from the Neurnal layer it
     * gets it date (le left handed layer) every weightnings is adressed by two indices:
     *
     * 1. The Neuron of this Layer [0... #nrOfNeurons-1]
     * 2. The Weightning of this neurons input in respect of its feeding (left handded (output) layer.
     *    [0... parents_layer.#nrOfNeurons-1]
     *
     * @param inNeuronIndex The index of the neuron to gain access the weightning
     *          [0... #nrOfNeurons-1]
     * @param inWeightningIndex The index of the weighting of the neurons input to
     *          gain access the weightning [0... #nrOfNeurons-1]
     * @return A Pointer that points to the Output data of all neurons of this layer.
     *      If either \p inNeuronIndex or \p inWeightningIndex is out of bounds
     *      then the returned pointer is nullptr!
     *
     *      If this Layer is an **Input Layer** then this pointer is nullptr in any case
     *      because a Input Layer does not have a weightning
     *
     *      This pointer will be nullptr either if init has not be called once before!
     *
     * @see weightningAtIndexForNeuron() const
     * @see nrOfNeurons()
     */
    float* weightningAtIndexForNeuron(unsigned int inNeuronIndex, unsigned int inWeightningIndex);

    /*!
     * @brief Returns a Pointer to an immutable float of the neurons bias value.
     * Since every Neuron has one bias that are dependent from the Neurnal layer it
     * gets it date (le left handed layer) every bias value is adressed by the
     * index of the neuron it belongs to.
     *
     * @param inNeuronIndex The index of the neuron to gain access the weightning
     *          [0... #nrOfNeurons-1]
     *
     * @return A Pointer that points to the Output data of all neurons of this layer.
     *      If \p inNeuronIndex is out of bounds then the returned pointer is nullptr!
     *
     *      If this Layer is an **Input Layer** then this pointer is nullptr in any case
     *      because a Input Layer does not have a bias value!
     *
     *      This pointer will be nullptr either if init has not be called once before!
     *
     * @see biasForNeuron()
     * @see nrOfNeurons()
     */
    const float* biasForNeuron(unsigned int inNeuronIndex) const;

    /*!
     * @brief Returns a Pointer to an mutable float of the neurons bias value.
     * Since every Neuron has one bias that are dependent from the Neurnal layer it
     * gets it date (le left handed layer) every bias value is adressed by the
     index of the neuron it belongs to.
     *
     * @param inNeuronIndex The index of the neuron to gain access the weightning
     *          [0... #nrOfNeurons-1]
     *
     * @return A Pointer that points to the Output data of all neurons of this layer.
     *      If \p inNeuronIndex is out of bounds then the returned pointer is nullptr!
     *
     *      If this Layer is an **Input Layer** then this pointer is nullptr in any case
     *      because a Input Layer does not have a bias value!
     *
     *      This pointer will be nullptr either if init has not be called once before!
     *
     * @see biasForNeuron() const
     * @see nrOfNeurons()
     */
    float* biasForNeuron(unsigned int inNeuronIndex);

    /*!
     * @brief Ask if this Layer is an input layer.
     * @note This call will either claim to be the input layer if the init
     * method has not yet been called! Ao please consider to call isInited()
     * too if unsure!
     *
     * @return true if this is the input layer (only valid if inited!)
     * @see isInited() const
     * @see init(unsigned int inNrOfNeurons,
     *           ActivationFunction inActivationFunction,
     *           utils::CMath& inMath,
     *           const CLayer* inParentLayer)
     */
    bool isInputLayer() const;

    /*!
     * @brief Ask if this Layer is an inite layer. this object is initd if the
     * init method has been called once already and this responded with success!
     *
     * @return true if this is this is successful inited already
     * @see init(unsigned int inNrOfNeurons,
     *           ActivationFunction inActivationFunction,
     *           utils::CMath& inMath,
     *           const CLayer* inParentLayer)
     */
    bool isInited() const;

private:
    void _cleanup();

    utils::CVectorF32* _neuronWeightningVector(unsigned int forNeuronIndex);
    const utils::CVectorF32* _parentNeuronOutputVector() const;
    unsigned int _nrOfParentNeurons() const;

private:
    utils::CMath* m_Math = nullptr;
    CLayer* m_ParentLayer = nullptr;

    utils::CVectorF32*              m_OutputVector = nullptr;
    std::vector<utils::CVectorF32*> m_WeightningVectors;

//    ActivationFunction m_ActivationFunction = nullptr;
    const IActivation* m_Activation = nullptr;
}; // class CLayer

} // namespace kilib
