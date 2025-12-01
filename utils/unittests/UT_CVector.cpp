/*
 * @file utils/unittests/UT_CVector.cpp
 * @brief Unittest for CVector
 * @author "Hans-Peter Beständig"<hdusel@tangerine-soft.de>
 */
#include "utils/include/CVector.hpp"
#include "tsunit/TSUnit.hpp"
#include "tsunit/TSUnitTestAddOns.hpp"

TSUNIT_TEST(utils_CVector, TestIf_size_returnsTheConstructedSize)
{
    for (int i=0; i < 10; ++i)
    {
        utils::CVector<float> v(i);
        UT_EXPECT_EQ(i, v.size());
    }
}

// ==========================================================================
// Size Tests
// ==========================================================================
TSUNIT_TEST(utils_CVector, TestIf_copyCtor_returnsTheConstructedSize)
{
    utils::CVector<float> v1(5);
    utils::CVector<float> v2(v1);

    UT_EXPECT_EQ(5, v2.size());

    // Check, that the source is still alive!
    UT_EXPECT_EQ(5, v1.size());
    UT_EXPECT_NE(nullptr, *v1);
}

TSUNIT_TEST(utils_CVector, TestIf_moveCtor_returnsTheConstructedSize)
{
    utils::CVector<float> v1(5);
    utils::CVector<float> v2(std::move(v1));
    UT_EXPECT_EQ(5, v2.size());

    // Check, that the source object has been really destroyed!
    UT_EXPECT_EQ(0, v1.size());
    UT_EXPECT_EQ(nullptr, *v1);
}

TSUNIT_TEST(utils_CVector, TestIf_assignmentOperator_returnsTheConstructedSize)
{
    utils::CVector<float> v1(5);
    utils::CVector<float> v2(0);
    v2 = v1;

    UT_EXPECT_EQ(5, v2.size());
    // Check, that the source is still alive!
    UT_EXPECT_EQ(5, v1.size());
    UT_EXPECT_NE(nullptr, *v1);
}

TSUNIT_TEST(utils_CVector, TestIf_assignmentMoveOperator_returnsTheConstructedSize)
{
    utils::CVector<float> v2(0);
    utils::CVector<float> v1(5);

    v2 = std::move(v1);
    UT_EXPECT_EQ(5, v2.size());

    // Check, that the source object has been really destroyed!
    UT_EXPECT_EQ(0, v1.size());
    UT_EXPECT_EQ(nullptr, *v1);
}

// ==========================================================================
// Data Integrity Tests
// ==========================================================================
TSUNIT_TEST(utils_CVector, TestIf_copyCtor_performsAProperDataCopy)
{
    utils::CVector<int> v1(5);
    v1[0] =  1;
    v1[1] = -2;
    v1[2] =  3;
    v1[3] = -5;
    v1[4] =  8;

    const utils::CVector<int> v2(v1);
    UT_EXPECT_EQ(+1, v2[0]);
    UT_EXPECT_EQ(-2, v2[1]);
    UT_EXPECT_EQ(+3, v2[2]);
    UT_EXPECT_EQ(-5, v2[3]);
    UT_EXPECT_EQ(+8, v2[4]);
}

TSUNIT_TEST(utils_CVector, TestIf_moveCtor_performsAProperDataCopy)
{
    utils::CVector<int> v1(5);
    v1[0] =  1;
    v1[1] = -2;
    v1[2] =  3;
    v1[3] = -5;
    v1[4] =  8;

    utils::CVector<int> v2(std::move(v1));
    UT_EXPECT_EQ(+1, v2[0]);
    UT_EXPECT_EQ(-2, v2[1]);
    UT_EXPECT_EQ(+3, v2[2]);
    UT_EXPECT_EQ(-5, v2[3]);
    UT_EXPECT_EQ(+8, v2[4]);
}

TSUNIT_TEST(utils_CVector, TestIf_assignmentOperator_performsAProperDataCopy)
{
    utils::CVector<int> v1(5);
    v1[0] =  1;
    v1[1] = -2;
    v1[2] =  3;
    v1[3] = -5;
    v1[4] =  8;

    utils::CVector<int> v2(0);
    v2 = v1;

    UT_EXPECT_EQ(+1, v2[0]);
    UT_EXPECT_EQ(-2, v2[1]);
    UT_EXPECT_EQ(+3, v2[2]);
    UT_EXPECT_EQ(-5, v2[3]);
    UT_EXPECT_EQ(+8, v2[4]);
}

TSUNIT_TEST(utils_CVector, TestIf_assignmentMoveOperator_performsAProperDataCopy)
{
    utils::CVector<int> v1(5);
    v1[0] =  1;
    v1[1] = -2;
    v1[2] =  3;
    v1[3] = -5;
    v1[4] =  8;

    utils::CVector<int> v2(0);
    v2 = std::move(v1);

    UT_EXPECT_EQ(+1, v2[0]);
    UT_EXPECT_EQ(-2, v2[1]);
    UT_EXPECT_EQ(+3, v2[2]);
    UT_EXPECT_EQ(-5, v2[3]);
    UT_EXPECT_EQ(+8, v2[4]);
}

// ==========================================================================
// Value evaluation Tests
// ==========================================================================
TSUNIT_TEST(utils_CVector, TestIf_min_returnsTheMinimalValue)
{
    utils::CVector<int> v1(5);
    v1[0] =  1;
    v1[1] = -2;
    v1[2] =  8;
    v1[3] = -5;
    v1[4] =  3;

    UT_EXPECT_EQ(-5, v1.min());
}

TSUNIT_TEST(utils_CVector, TestIf_max_returnsTheMinimalValue)
{
    utils::CVector<int> v1(5);
    v1[0] =  1;
    v1[1] = -2;
    v1[2] =  8;
    v1[3] = -5;
    v1[4] =  3;

    UT_EXPECT_EQ(+8, v1.max());
}

TSUNIT_TEST(utils_CVector, TestIf_average_returnsTheMinimalValue)
{
    utils::CVector<float> v1(5);
    v1[0] =  1;
    v1[1] = -2;
    v1[2] =  8;
    v1[3] = -5;
    v1[4] =  3;

    UT_EXPECT_EQ((1-2+8-5+3) / 5.f, v1.average());
}

TSUNIT_TEST(utils_CVector, TestIf_lengthSqu_returnsTheProperValue)
{
    utils::CVector<float> v1(5);
    v1[0] =  1;
    v1[1] = -2;
    v1[2] =  8;
    v1[3] = -5;
    v1[4] =  3;

    UT_EXPECT_EQ((1*1 + 2*2 + 8*8 + 5*5 + 3*3), v1.lengthSqu());
}

TSUNIT_TEST(utils_CVector, TestIf_length_returnsTheProperValue)
{
    utils::CVector<float> v1(5);
    v1[0] =  1;
    v1[1] = -2;
    v1[2] =  8;
    v1[3] = -5;
    v1[4] =  3;

    UT_EXPECT_EQ((sqrtf(1*1 + 2*2 + 8*8 + 5*5 + 3*3)), v1.length());
}

// ==========================================================================
// Vector intrinsic manipulation
// ==========================================================================
TSUNIT_TEST(utils_CVector, TestIf_multiplyOp_returnsTheScaledVector)
{
    utils::CVector<float> v1(5);
    v1[0] =  1;
    v1[1] = -2;
    v1[2] =  8;
    v1[3] = -5;
    v1[4] =  3;

    v1 *= 1.25f;

    UT_EXPECT_EQ(+1.f * 1.25f, v1[0]);
    UT_EXPECT_EQ(-2.f * 1.25f, v1[1]);
    UT_EXPECT_EQ(+8.f * 1.25f, v1[2]);
    UT_EXPECT_EQ(-5.f * 1.25f, v1[3]);
    UT_EXPECT_EQ(+3.f * 1.25f, v1[4]);
}

TSUNIT_TEST(utils_CVector, TestIfSelf_addWithOtherVector_returnsTheProperVector)
{
    utils::CVector<float> v1(5);
    v1[0] =  1;
    v1[1] = -2;
    v1[2] =  8;
    v1[3] = -5;
    v1[4] =  3;

    utils::CVector<float> v2(5);
    v2[0] = - 3;
    v2[1] = -44;
    v2[2] = +23;
    v2[3] = -21;
    v2[4] =  80;

    v1 += v2;

    UT_EXPECT_EQ(+1.f -  3.f, v1[0]);
    UT_EXPECT_EQ(-2.f - 44.f, v1[1]);
    UT_EXPECT_EQ(+8.f + 23.f, v1[2]);
    UT_EXPECT_EQ(-5.f - 21.f, v1[3]);
    UT_EXPECT_EQ(+3.f + 80.f, v1[4]);
}

// ==========================================================================
// Vector extrinsic manipulation
// ==========================================================================
TSUNIT_TEST(utils_CVector, TestIf_addWitOtherVector_returnsTheProperVector)
{
    utils::CVector<float> v1(5);
    v1[0] =  1;
    v1[1] = -2;
    v1[2] =  8;
    v1[3] = -5;
    v1[4] =  3;

    utils::CVector<float> v2(5);
    v2[0] = - 3;
    v2[1] = -44;
    v2[2] = +23;
    v2[3] = -21;
    v2[4] =  80;

    const utils::CVector<float> v3(v1 + v2);

    UT_EXPECT_EQ(+1.f -  3.f, v3[0]);
    UT_EXPECT_EQ(-2.f - 44.f, v3[1]);
    UT_EXPECT_EQ(+8.f + 23.f, v3[2]);
    UT_EXPECT_EQ(-5.f - 21.f, v3[3]);
    UT_EXPECT_EQ(+3.f + 80.f, v3[4]);
}

// ==========================================================================
// Enumeration Test
// ==========================================================================
TSUNIT_TEST(utils_CVector, TestIf_for_each_returnsTheProperValueByVisitingThem)
{
    utils::CVector<int> v1(5);
    v1[0] =  1;
    v1[1] = -2;
    v1[2] =  8;
    v1[3] = -5;
    v1[4] =  3;

    int valueReadout[5] = {0};
    int* readoutPtr = &valueReadout[0];

    v1.for_each([&readoutPtr](const int& value)->void{
        *readoutPtr++ = value;
    });

    UT_EXPECT_EQ(5, readoutPtr - &valueReadout[0]);

    UT_EXPECT_EQ(+1, valueReadout[0]);
    UT_EXPECT_EQ(-2, valueReadout[1]);
    UT_EXPECT_EQ(+8, valueReadout[2]);
    UT_EXPECT_EQ(-5, valueReadout[3]);
    UT_EXPECT_EQ(+3, valueReadout[4]);
}

// ==========================================================================
// Pointer Tests
// ==========================================================================
TSUNIT_TEST(utils_CVector, TestIf_deferencingTheInternalStorageAsPointerToConstReturnsTheProperData)
{
    utils::CVector<int> v1(5);
    v1[0] = -1;
    v1[1] = +2;
    v1[2] = -8;
    v1[3] = +5;
    v1[4] = -3;

    const int* readoutPtr = *v1;

    UT_EXPECT_EQ(-1, readoutPtr[0]);
    UT_EXPECT_EQ(+2, readoutPtr[1]);
    UT_EXPECT_EQ(-8, readoutPtr[2]);
    UT_EXPECT_EQ(+5, readoutPtr[3]);
    UT_EXPECT_EQ(-3, readoutPtr[4]);
}

TSUNIT_TEST(utils_CVector, TestIf_deferencingTheInternalStorageAsPointerToNonConstReturnsTheProperData)
{
    utils::CVector<int> v1(5);
    v1[0] = -1;
    v1[1] = +2;
    v1[2] = -8;
    v1[3] = +5;
    v1[4] = -3;

    int* readoutPtr = *v1;

    readoutPtr[0] *=    +3;
    readoutPtr[1] *=    -7;
    readoutPtr[2] *=  -221;
    readoutPtr[3] *=   +30;
    readoutPtr[4] *= -2233;

    UT_EXPECT_EQ(-1 *    +3, v1[0]);
    UT_EXPECT_EQ(+2 *    -7, v1[1]);
    UT_EXPECT_EQ(-8 *  -221, v1[2]);
    UT_EXPECT_EQ(+5 *   +30, v1[3]);
    UT_EXPECT_EQ(-3 * -2233, v1[4]);
}
