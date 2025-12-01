/* ==========================================================================
 * @(#)File: TSUnitTestAddOns.hpp
 * Created: 2023-10-22
 * --------------------------------------------------------------------------
 *  (c)1982-2024 Tangerine-Software
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
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * ========================================================================== */
#include <cstdint>

// Some Useful addons for Unittest...

#if !defined(dimof)
#define dimof(x) (sizeof(x) / sizeof(*(x)))
#endif

namespace tsunit {

// ==========================================================================
// Some Bit rotation Functions.
// ==========================================================================
/*!
 * Rotate a 32 bit number by a given numer of bits to the left and return this result.
 * \param val The value to rotate
 * \param bitsToRotate The number of bits to rotate the number [0..31]
 * \return value rotated by \p bitsToRotate Bits to the left
 * \see rotr(std::uint32, std::size_t)
 */
template<unsigned int NR_OF_BITS>
static uint32_t rotl(std::uint32_t val)
{
    return (0 != (NR_OF_BITS & 0x1f)) ? (val << (NR_OF_BITS & 0x1f)) | (val >> (32 - (NR_OF_BITS & 0x1f))) : val;
}

/*!
 * Rotate a 32 bit number by a given numer of bits to the right and return this result.
 * \param val The value to rotate
 * \param bitsToRotate The number of bits to rotate the number [0..31]
 * \return value rotated by \p bitsToRotate Bits to the right
 * \see rotl(std::uint32, std::size_t)
 */
template<unsigned int NR_OF_BITS>
static uint32_t rotr(std::uint32_t val)
{
    return rotl<32-NR_OF_BITS>(val);
}

// ==========================================================================
// Some "Pseudo random" functions.
// ==========================================================================
/*!
 * Calculate a Hash for a given bunch of memry contents and return this as an uint32_t.
 * \param inDataPtr The Pointer to the Data to calculate a hash for.
 * \param inDataSize The number of bytes \p inDataPtr points to.
 * \return An Hash for the data \p inDataPtr points to and of the size \p inDataSize.
 */
std::uint32_t hash(const void* inDataPtr, unsigned int inDataSize);

/*!
 * Helper class that allows to collect several calculations of a Hash Value.
 * The purpose of this class is to remember and reuse the last calculated
 * has value and append new hash calculations in order to perform a chained calculation
 * of the hash of a bunch of different input data.
 */
class CHasher
{
public:
    /*!
     * Creates a new Hasher object with a given initial hash value.
     * \param inInitialHashValue The inital Has Value. This parameter will
     * default to \p 0x5acB4821ul if omitted.
     *
     * \see reset(std::uint32_t)
     */
    CHasher(std::uint32_t inInitialHashValue = 0x5acB4821ul)
    : m_Hash(inInitialHashValue){}

    /*!
     * Adds a new value to this has calculation.
     * \param inValue The generic value of type <T>
     * \return An reference to this object **after** \p inValue has been added.
     * \see add(const void*, unsigned int)
     */
    template <typename T>
    CHasher& operator+=(T inValue)
    {
        _addHash(tsunit::hash(&inValue, sizeof(inValue)));
        return *this;
    }

    /*!
     * Adds the contents of some memory to this hash.
     * \param inDataPtr The generic value of type <T>
     * \param inDataSizeInBytes The size of the data \p inDataPtr points to in [bytes]
     * \return An reference to this object **after** the hash of this data has
     *         been added.
     * \see operator+=<T>(T inValue)
     */
    CHasher& add(const void* inDataPtr, unsigned int inDataSizeInBytes)
    {
        _addHash(tsunit::hash(inDataPtr, inDataSizeInBytes));
        return *this;
    }

    /*!
     * Resets the hash to a given value.
     * \param inInitialHashValue The hash value to reset this objects current hash value.
     *        when omitted this value becomes 0x5acB4821ul.
     */
    void reset(std::uint32_t inInitialHashValue = 0x5acB4821ul)
    {
        m_Hash = inInitialHashValue;
    }

    /*!
     * Returns the current hash value as an std::uint32_t value.
     * \return The current hash value as an std::uint32_t value.
     */
    std::uint32_t value() const
    {
        return m_Hash;
    }

private:
    void _addHash(uint32_t inNewHash)
    {
        m_Hash = (rotl<7>(m_Hash) ^ rotr<12>(m_Hash)) - inNewHash;
    }

private:
    std::uint32_t m_Hash = 0x5acB4821ul;
}; // class CHasher


// ==========================================================================
// Some "Pseudo random" functions.
// ==========================================================================
/*!
 * Sets the seed of the Pseudo Random Generator to a given value.
 * \param inSeed The new seed. Since this is an **pseudo** random number generator
 * one may able to reproduce the same "random" sequence if she starts at the **same
 * seed**.
 *
 * \see pseudoRandomsetSeed(std::uint32_t)
 * \see pseudoRandom()
 */
void pseudoRandomsetSeed(std::uint32_t inSeed);

/*!
 * Returns the current pseudo random number as an uint32_t number that will
 * change from call to call. The returned value will use the full 32 bit scale!
 *
 * \return The current pseudo random number as an uint32_t scalar value.
 * \see pseudoRandomFloat(float minValue, float)
 */
std::uint32_t pseudoRandom();

/*!
 * Returns the current pseudo random number for a given range as an floating point
 * number. The value will change from call to call.
 *
 * \param minValue The minimal value (possible included in the return) for the
 * random number. This will default to 0 if omitted.
 * \param maxValue The max value (possible **almost not** be included in the return)
 * for the random number. This will default to 1.0 if omitted.
 *
 * \return The current pseudo random number as an float VAlue that will be in a
 * range of [\p minValue ... \p minalue[ Meaning that \p maxValue may part of the
 * returned Random number **but** \p maxValue almost but never really.
 *
 * \see pseudoRandomFloat(float minValue, float)
 */
float pseudoRandomFloat(float minValue = 0.f, float maxValue = 1.f);

} // namespace tsunit
