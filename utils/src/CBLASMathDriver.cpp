/* ==========================================================================
 * @(#)File: utils/src/CBLASMathDriver.cpp
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
#if !defined(__APPLE__)
#error "This File is for MacOS Only!"
#endif

// ==========================================================================
// Includes
// ==========================================================================
#include <Accelerate/Accelerate.h>
#include "utils/include/CBLASMathDriver.hpp"
#include <algorithm>

// ==========================================================================
// Macros
// ==========================================================================

// ==========================================================================
// Typedefs
// ==========================================================================

// ==========================================================================
// Local Functions
// ==========================================================================

namespace utils {
// ==========================================================================
// class CBLASMathDriver : public CMath::IMathDriver
// ==========================================================================

float CBLASMathDriver::calcDotF32(const CVectorF32& inVectorA, const CVectorF32& inVectorB, float offset) const
{
    float res = 0;
    assert(inVectorA.size() == inVectorB.size());
    if (inVectorA.size() == inVectorB.size())
    {
        res = cblas_sdsdot(static_cast<int>(inVectorA.size()), offset, *inVectorA, 1, *inVectorB, 1);
    }
    return res;
}

float CBLASMathDriver::sumUpF32(const CVectorF32& inVector) const
{
    const float scale = 1.f;
    return cblas_dsdot(static_cast<int>(inVector.size()), *inVector, 1, &scale, 0);
}

float CBLASMathDriver::sumUpF32(const CVectorF32& inVector, unsigned int inIndexOfFirstElement, unsigned int inNrOfElements) const
{
    const float scale = 1.f;
    inNrOfElements = std::min<unsigned int>(inIndexOfFirstElement + inNrOfElements, static_cast<unsigned int>(inVector.size()));
    return cblas_dsdot(static_cast<int>(inNrOfElements), (*inVector) + inIndexOfFirstElement, 1, &scale, 0);
}
} // namespace utils
