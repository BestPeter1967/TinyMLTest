#pragma once
/* ==========================================================================
 * @(#)File: utils/include/CMath.cpp
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

#include "CVector.hpp"
#include <cstdlib>
#include <algorithm>

namespace utils {
/*!
 * @brief A Mathematical library that uses "drivers" for its realization.
 *
 * The Purpose of a custom driver is to allow the CMath object use spicialized (optimized)
 * algorithms or to give tests a change to implementa a spy for mathematial functions that are
 * covered by CMath.
 *
 * @author "Hans-Peter Beständig"<hdusel@tangerine-soft.de>
 */
class CMath
{
public:
    /*!
     * @brief Interface for a driver that is used by CMath
     *
     * @author "Hans-Peter Beständig"<hdusel@tangerine-soft.de>
     */
    class IMathDriver
    {
    public:
        IMathDriver() = default;
        virtual ~IMathDriver() = default;

        /*!
         * @brief Implementation for a dot product calcualtions of two Vectors.
         *
         * This Implementation has to realize the calculation of \f$OutVector = inVectorA \cdot inVectorB + offset\f$
         *
         * @param inVectorA The left hand side Vector to perform the calculation.
         * @param inVectorB The right hand side Vector to perform the calculation.
         * @param offset A custom ofset for the dot product calcualtion. It is assumed that this is zero if not used.
         * @return The result \f$f(inVector) = inVectorA \cdot inVectorB + offset\f$
         */
        virtual float calcDotF32(const CVectorF32& inVectorA, const CVectorF32& inVectorB, float offset) const = 0;
        
        /*!
         * @brief Interface for a Method that is supposed to perform an integration of all components of a given
         * vector and return this result.
         *
         * @param inVector The vector whose components are subject of integration
         * @return The result \f$f(inVector) = \Sum{inVector}_{0}^{inNrOfElements}\f$
         *
         * @see sumUpF32(const CVectorF32&, unsigned int, unsigned int) const
         */
        virtual float sumUpF32(const CVectorF32& inVector) const = 0;

        /*!
         * @brief Interface for a Method that is supposed to perform an integration of a specific range of components of a given
         * vector and return this result.
         *
         * @warning The caller of thsi method expects that the implementation will clamp the index to valid ranges of the vector.
         * This means that if the offset (plus nrOfElements) would exceed the vectors elements then the implementation
         * must take care to adjust (clmap) the boundaries to valid range in order to respect the vectors range of elements!
         *
         * @param inVector The vector whose components are subject of integration
         * @param inIndexOfFirstElement The 0 based index of the first element of \p inVector to integrate to.
         * @param inNrOfElements The number of elements of the vector \p inVector starting at index
         *   \p inIndexOfFirstElement to intgerate to.
         * @return The result \f$f(inVector, offset, inNrOfElements) = \Sum{inVector}_{offset}^{inNrOfElements}\f$
         *
         * @see sumUpF32(const CVectorF32&) const
         */
        virtual float sumUpF32(const CVectorF32& inVector, unsigned int inIndexOfFirstElement, unsigned int inNrOfElements) const = 0;
    };

    /*!
     * @brief Creates an CMath object with a default implementation of a IMathDriver that performs the
     * desired operations by using the CPU.
     *
     * Custom Implementations may use #CMath(const IMathDriver&) and hand over optimized drivers that
     * for example may utilize SIMD or other target specific mathematical accellearations.
     *
     * @see CMath(const IMathDriver&)
     */
    CMath();

    
    /*!
     * @brief Creates an CMath object with a customized implementation of a IMathDriver that performs the
     * desired operations by using the drivers implementation strategy. (see IDriver for details)
     *
     * @param inDriver The driver that will be used to perform optimized calcualtions of this object.
     */
    CMath(const IMathDriver& inDriver)
    : m_Driver(inDriver)
    {}

    ~CMath() = default;

    
    /*!
     * @brief Implementation for a dot product calcualtions of two Vectors.
     *
     * This Implementation has to realize the calculation of \f$OutVector = inVectorA \cdot inVectorB + offset\f$
     *
     * @param inVectorA The left hand side Vector to perform the calculation.
     * @param inVectorB The right hand side Vector to perform the calculation.
     * @param offset A custom offset for the dot product calculation. It is assumed that this is zero if not used.
     * @return The result \f$f(inVector) = inVectorA \cdot inVectorB + offset\f$
     */
    float calcDotF32(const CVectorF32& inVectorA, const CVectorF32& inVectorB, float offset = 0.f) const
    {
        return m_Driver.calcDotF32(inVectorA, inVectorB, offset);
    }

    /*!
     * @brief Perform an integration of all components of a given vector and return this result
     * of this sum.
     *
     * @param inVector The vector whose components are subject of integration
     * @return The result \f$OutVector = \Sum{inVectorA \cdot inVectorB + offset}\f$
     *
     * @see sumUpF32(const CVectorF32&, unsigned int, unsigned int) const
     */
    float sumUpVector(const CVectorF32& inVector) const
    {
        return m_Driver.sumUpF32(inVector);
    }

    /*!
     * @brief Perform an integration of a specific range of components of a given vector and return this result.
     *
     * @param inVector The vector whose components are subject of integration
     * @param inIndexOfFirstElement The 0 based index of the first element of \p inVector to integrate to.
     * @param inNrOfElements The number of elements of the vector \p inVector starting at index
     *   \p inIndexOfFirstElement to intgerate to.
     * @return The result \f$f(inVector, offset, inNrOfElements) = \Sum{inVector}_{offset}^{inNrOfElements}\f$
     *
     * @see sumUpF32(const CVectorF32&) const
     */
    float sumUpVector(const CVectorF32& inVector, unsigned int offset, unsigned int inNrOfElements) const
    {
        return m_Driver.sumUpF32(inVector, offset, inNrOfElements);
    }
    static float randF32(float inMin, float inMax);

private:
    const IMathDriver& m_Driver;
}; // class CMath
} // namespace utils
