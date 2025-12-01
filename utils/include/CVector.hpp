#pragma once
/* ==========================================================================
 * @(#)File: utils/include/CVector.cpp
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
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cfloat>
#include <memory.h>
#include <functional>

namespace utils {
template <typename T>
class CVector
{
public:
    CVector() : CVector(0){}


    /*!
     * @brief Create a new Vector with a given Dimension.
     *
     * @note For performance sakes the Vectors elements are **not** initialized
     * by this ctor! So don't assume a certain value after construction!
     *
     * @param inNrOfElements The number of elements of the vector.
     */
    CVector(size_t inNrOfElements)
    : m_NrOfElements(inNrOfElements)
    , m_Elements(new(std::nothrow) T[inNrOfElements])
    {
        assert(m_Elements);
    }

    /*!
     * @brief Create a copy of another Vector as a new vector.
     * @param inVector The vector to create a copy from.
     */
    CVector(const CVector& inVector)
    : CVector(inVector.m_NrOfElements)
    {
        memcpy(m_Elements, inVector.m_Elements, sizeof(T) * inVector.m_NrOfElements);
    }

    /*!
     * @brief Move another Vector as this vector by applying move semantic.
     * @param inMoveVector The vector to move to this.
     */
    CVector(CVector&& inMoveVector)
    : m_NrOfElements(inMoveVector.m_NrOfElements)
    , m_Elements(inMoveVector.m_Elements)
    {
        inMoveVector.m_NrOfElements = 0;
        inMoveVector.m_Elements = nullptr;
    }

    /*!
     * @brief Assign another Vector as this vector by applying copy semantic.
     * This Vector becomes the dimension among of its values of the other vector.
     * @param inRHSVector The vector to copy from.
     * @return This Vector after assign the elements of \p inRHSVector
     */
    CVector& operator=(const CVector& inRHSVector)
    {
        if (this != &inRHSVector)
        {
            if (inRHSVector.m_NrOfElements > m_NrOfElements)
            {
                delete [] m_Elements;
                m_Elements = new(std::nothrow) T[inRHSVector.m_NrOfElements];
                assert(m_Elements);
            }
            m_NrOfElements = inRHSVector.m_NrOfElements;
            memcpy(&m_Elements[0], &inRHSVector.m_Elements[0], sizeof(float) * m_NrOfElements);
        }
        return *this;
    }

    /*!
     * @brief Move anothers Vector data to this vector by applying move semantic.
     * This Vector becomes the dimension among of its values of the other vector.
     * @param inRHSVector The vector to copy from. This vector is invalid afterwards.
     * @return This Vector after assign the elements of \p inRHSVector
     */
    CVector& operator=(CVector&& inRHSMoveVector)
    {
        if (this != &inRHSMoveVector)
        {
            m_NrOfElements = inRHSMoveVector.m_NrOfElements;
            m_Elements = inRHSMoveVector.m_Elements ;

            inRHSMoveVector.m_NrOfElements = 0;
            inRHSMoveVector.m_Elements = nullptr;
        }
        return *this;
    }

    /*!
     * @brief Destroys the vectors storage.
     */
    ~CVector()
    {
        delete [] m_Elements;
        m_Elements = nullptr;
        m_NrOfElements = 0;
    }

    /*!
     * @brief Ask for all elements of this vector.
     * This method returns a pointer to mutable data! So take care!
     */
    T* operator*() {
        return m_Elements;
    }

    /*!
     * @brief Ask for all elements of this vector.
     * This method returns a pointer to immutable data!
     */
    const T* operator*() const {
        return m_Elements;
    }

    /*!
     * @brief Return a **mutable** reference to a certain element of this
     * vector.
     * @param inIndex The index of the mutable component to return to.
     * @note For performance sakes this method **does not** perform
     * any range check if \p inIndex is within the vectors components boundaries!
     */
    T& operator[](size_t inIndex) {
        assert(inIndex < m_NrOfElements);
        return m_Elements[inIndex];
    }

    /*!
     * @brief Return a non **inmutable** reference to a certain element of this
     * vector.
     * @param inIndex The index of the inmutable component to return to.
     * @note For performance sakes this method **does not** perform
     * any range check if \p inIndex is within the vectors components boundaries!
     */
    const T& operator[](size_t inIndex) const {
        assert(inIndex < m_NrOfElements);
        return m_Elements[inIndex];
    }

    /*!
     * @brief Return the number of components of this vector as it has been
     * constructed.
     * @return The number of this vectors components.
     */
    size_t size() const
    {
        return m_NrOfElements;
    }

    /// @brief A mutable visitor function that is used by #for_each(const MutableVisitor&)
    using MutableVisitor = std::function<void(T&)>;

    /*!
     * @brief function that performs visiting all vectors components
     * as **mutable** references.
     * @param inVisitor The Mutable functor that is used to visit all
     * vectors elements.
     * @see for_each(const InmutableVisitor& inVisitor) const
     */
    void for_each(const MutableVisitor& inVisitor)
    {
        for (unsigned int i=0; i < m_NrOfElements; ++i)
        {
            inVisitor(m_Elements[i]);
        }
    }

    /// @brief An inmutable visitor function that is used by #for_each(const InmutableVisitor&) const
    using InmutableVisitor = std::function<void(const T&)>;

    /*!
     * @brief function that performs visiting all vectors components
     * as **inmutable** references.
     * @param inVisitor The Mutable functor that is used to visit all
     * vectors elements.
     * @see for_each(const MutableVisitor& inVisitor)
     */
    void for_each(const InmutableVisitor& inVisitor) const
    {
        for (unsigned int i=0; i < m_NrOfElements; ++i)
        {
            inVisitor(m_Elements[i]);
        }
    }

    /*!
     * @brief Set **all vectors components** to a given value.
     * @param inValue The Value to use to set all components to.
     */
    void setAll(const T& inValue) const
    {
        for (unsigned int i=0; i < m_NrOfElements; ++i)
        {
            m_Elements[i] = inValue;
        }
    }

    /*!
     * @brief Return the greatest element of this vector.
     * @return The greates element of this vector.
     * @see min() const
     * @see average() const
     */
    T max() const
    {
        T maxValue;
        if (m_NrOfElements > 0)
        {
            maxValue = m_Elements[0];
            for (unsigned int i=1; i < m_NrOfElements; ++i)
            {
                if (m_Elements[i] > maxValue)
                {
                    maxValue = m_Elements[i];
                }
            }
        }
        return maxValue;
    }

    /*!
     * @brief Return the smallest element of this vector.
     * @return The greates smallest of this vector.
     * @see max() const
     * @see average() const
     */
    T min() const
    {
        T minValue;
        if (m_NrOfElements > 0)
        {
            minValue = m_Elements[0];
            for (unsigned int i=1; i < m_NrOfElements; ++i)
            {
                if (m_Elements[i] < minValue)
                {
                    minValue = m_Elements[i];
                }
            }
        }
        return minValue;
    }

    /*!
     * @brief Return the average value of this vectors components.
     * @return The average value of this vectors components.
     * @see min() const
     * @see max() const
     */
    T average() const
    {
        T retValue;
        assert (m_NrOfElements > 0);
        if (m_NrOfElements > 0)
        {
            T sum = 0;
            for (unsigned int i=0; i < m_NrOfElements; ++i)
            {
                sum += m_Elements[i];
            }
            retValue = sum / m_NrOfElements;
        }
        return retValue;
    }

    /*!
     * @brief Calculate the vectors length
     * \note This method employs the calculation of the Square Root
     * which is a expensive function. Sometimes one may be fine by using
     * the 'Square of the length' which may be obtained by #lengthSqu() const
     * @return The vectors length.
     * @see lengthSqu() const
     */
    T length() const
    {
        return sqrtf(lengthSqu());
    }

    /*!
     * @brief Calculate the vectors **square** length.
     * @return The **square** of the vectors length.
     * @see length() const
     */
    T lengthSqu() const
    {
        T retValue = 0;
        assert (m_NrOfElements > 0);
        if (m_NrOfElements > 0)
        {
            T sum = 0;
            for (unsigned int i=0; i < m_NrOfElements; ++i)
            {
                sum += m_Elements[i] * m_Elements[i];
            }
            retValue = sum;
        }
        return retValue;
    }

    /*!
     * @brief Multiply (scale) all elements of this vector by a given factor and
     * return the new Vector.
     * @param inScale The scale to appy to every vector component.
     * @return This Vector after performing the discussed action.
     */
    CVector<T>& operator*=(const T& inScale)
    {
        for (unsigned int i=0; i < m_NrOfElements; ++i)
        {
            m_Elements[i] *= inScale;
        }
        return *this;
    }

    /*!
     * @brief Add another vector to this vector.
     * This method effectively adds all single components of another vector
     * to this vector.
     * @param inRHSVector The other vector that is add to this vector.
     * @return This Vector after performing the discussed action.
     */
    CVector<T>& operator+=(const CVector<T>& inRHSVector)
    {
        assert(inRHSVector.size() == this->size());
        if (inRHSVector.size() == this->size())
        {
            for (unsigned int i=0; i < m_NrOfElements; ++i)
            {
                m_Elements[i] += inRHSVector[i];
            }
        }
        return *this;
    }

private:
    size_t m_NrOfElements = 0;
    T* m_Elements = nullptr;
}; // struct CVector

/*!
 * @brief Add two Vectors.
 *
 * \note the Vectors has to have the same dimension! Otherwise the resulting
 * vector has a dimension of ZERO
 *
 * @param inLHSVector The left hand side Vector to add.
 * @param inRHSVector The right hand side Vector to add.
 * @return the resulting vector that is \p inLHSVector + \p inRHSVector
 */
template<typename T>
CVector<T> operator+(const CVector<T>& inLHSVector, const CVector<T>& inRHSVector)
{
    assert (inLHSVector.size() == inRHSVector.size());
    if (inLHSVector.size() == inRHSVector.size())
    {
        CVector<T> resV(inLHSVector);
        return resV+=(inRHSVector);
    }
    else
    {
        return CVector<T>(0);
    }
}

/*!
 * @brief Add two Vectors.
 *
 * \note the Vectors has to have the same dimension! Otherwise the resulting
 * vector has a dimension of ZERO
 *
 * @param inLHSVector The left hand side Vector to add.
 * @param inRHSVector The right hand side Vector to add.
 * @return the resulting vector that is \p inLHSVector + \p inRHSVector
 */
template<typename T>
CVector<T> operator-(const CVector<T>& inLHSVector, const CVector<T>& inRHSVector)
{
    assert (inLHSVector.size() == inRHSVector.size());
    if (inLHSVector.size() == inRHSVector.size())
    {
        CVector<T> resV(inLHSVector);
        for (unsigned int i=0; i < inLHSVector.size(); ++i)
        {
            inLHSVector[i] -= inRHSVector[i];
        }
        return resV;
    }
    else
    {
        return CVector<T>(0);
    }
}

using CVectorF32 = CVector<float>;
} // namespace utils
