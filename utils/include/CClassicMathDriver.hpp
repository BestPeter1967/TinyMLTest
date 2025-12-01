#pragma once
/* ==========================================================================
 * @(#)File: utils/include/CClassicMathDriver.cpp
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
#include "CMath.hpp"
#include <stdlib.h>

namespace utils {
class CClassicMathDriver : public CMath::IMathDriver
{
public:
    CClassicMathDriver() = default;
    virtual ~CClassicMathDriver() = default;

    virtual float calcDotF32(const CVectorF32& inVectorA, const CVectorF32& inVectorB, float offset) const override;
    virtual float sumUpF32(const CVectorF32& inVector) const override;
    virtual float sumUpF32(const CVectorF32& inVector, unsigned int offset, unsigned int inNrOfElements) const override;
}; // class CClassicMathDriver
} // namespace utils
