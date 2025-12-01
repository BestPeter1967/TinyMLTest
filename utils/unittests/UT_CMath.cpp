/*
 * @file utils/unittests/UT_CMath.cpp
 * @brief Unittest for CMath
 * @author "Hans-Peter Beständig"<hdusel@tangerine-soft.de>
 */
#include "utils/include/CMath.hpp"
#include "tsunit/TSUnit.hpp"
#include "tsunit/TSUnitTestAddOns.hpp"
#include <cstdint>
#include <array>

#include <sys/time.h>

uint64_t getCurrentTimeUS()
{
    struct timeval tv;
    const int rc(::gettimeofday(&tv, NULL));
    assert(0 == rc);
    return tv.tv_sec * 1000000LL + tv.tv_usec;
}

uint64_t getCurrentTimeMS()
{
	return getCurrentTimeUS()/1000ULL;
}

// ==========================================================================
// Random generator limits test
// ==========================================================================
TSUNIT_TEST(utils_CMath, TestRandomRange)
{
    constexpr size_t  kNumberOfSamples = 100ul;

    float maxValue = FLT_MIN;
    float minValue = FLT_MAX;

    for (unsigned int i=0; i < kNumberOfSamples; ++i)
    {
        const float value = utils::CMath::randF32(-7.f, +3.f);
        if (value > maxValue)
        {
            maxValue = value;
        }

        if (value < minValue)
        {
            minValue = value;
        }
    }

    const float kTolerance = 0.01f; // 1 pecent of Tolerance is acceptable

    // Check the Min Value:
    UT_EXPECT_TRUE(minValue >= -7.f); // >= -7.0 ?
    UT_EXPECT_TRUE(minValue <= -7.f * (1.f - kTolerance)); // Less than the Tolerance towards the upper value?

    // Check the Max Value:
    UT_EXPECT_TRUE(maxValue <= +3.f); // <= +3.0 ?
    UT_EXPECT_TRUE(maxValue >= +3.f * (1.f - kTolerance)); // Greater than the Tolerance towards the lower value?

}

// ==========================================================================
// Random generator Entropy tests
// ==========================================================================
TSUNIT_TEST(utils_CMath, TestRandomEntropy)
{
    size_t collectedValues[16] = {0};
    constexpr size_t  kNumberOfSamples = 100'000ul;

    for (unsigned int i=0; i < kNumberOfSamples; ++i)
    {
        const float value = utils::CMath::randF32(-1.f, 1.f);
        const int16_t collectedValue = roundf(32767.f * value);
        for (unsigned int j = 0; j < 16; ++j)
        {
            collectedValues[j] += (0 != ((1u << j) & collectedValue));
        }
    }

    const float kTolerance = 0.01f; // 1 pecent of Tolerance is acceptable

    const uint32_t kLimitMin = roundf((1.f - kTolerance) * (kNumberOfSamples >> 1));
    const uint32_t kLimitMax = roundf((1.f + kTolerance) * (kNumberOfSamples >> 1));

    for (unsigned int i=0; i < 16; ++i)
    {
        UT_EXPECT_TRUE((collectedValues[i] >= kLimitMin)
                    && (collectedValues[i] <= kLimitMax));
    }
}

// ==========================================================================
// MathDriverTests
// ==========================================================================
TSUNIT_TEST(utils_CMath_MathDriverTests, DefaultDriver)
{
    utils::CMath math;
    utils::CVector<float> v1(5);
    v1[0] = +3.f;
    v1[1] = +1.f;
    v1[2] = +2.f;
    v1[3] = +7.f;
    v1[4] = -3.f;

    utils::CVector<float> v2(5);
    v2[0] = +7.f;
    v2[1] = +2.f;
    v2[2] = -5.f;
    v2[3] = -4.f;
    v2[4] = +3.f;

    const float dot = math.calcDotF32(v1, v2);
    const float expectedResult =
        (+3.f * +7.f)
      + (+1.f * +2.f)
      + (+2.f * -5.f)
      + (+7.f * -4.f)
      + (-3.f * +3.f);

    UT_EXPECT_EQ(expectedResult, dot);
}

TSUNIT_TEST(utils_CMath_MathDriverTests, CustomDriver)
{
    class CustomDriver : public utils::CMath::IMathDriver
    {
    public:
        CustomDriver() = default;
        virtual ~CustomDriver() = default;

        virtual float calcDotF32(const utils::CVectorF32& inVectorA,
                                 const utils::CVectorF32& inVectorB,
                                 float offset) const override
        {
            ++m_calcDotF32Called; // Spy
            return 0;
        }
        
        virtual float sumUpF32(const utils::CVectorF32& inVector) const override
        {
            return 0;
        }

        mutable unsigned int m_calcDotF32Called = 0;
    }; // class CustomDriver

    CustomDriver customDriver;

    utils::CMath math(customDriver);
    utils::CVector<float> v1(5);
    utils::CVector<float> v2(5);

    (float)math.calcDotF32(v1, v2);

    // It is expected, that the drivers #calcDotF32() method will be called.
    // So ask the spy...
    UT_EXPECT_EQ(1, customDriver.m_calcDotF32Called);
}
