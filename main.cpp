#include "kilib/include/CNeuronalNet.hpp"
#include "utils/include/CMath.hpp"

#include <vector>
#include <array>

#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cassert>

// Poor mans unittest ;-)
#if defined(__arm__)
    #define UT_ASSERT(a) do {if (!(a)){asm("bkpt");}}while(0)
#else
    #define UT_ASSERT(a) do {assert(a);}while(0)
#endif

#define UT_EXPECT_TRUE(a)  do{UT_ASSERT(a);}while(0)
#define UT_EXPECT_FALSE(a) do{UT_ASSERT(!(a));}while(0)
#define UT_EXPECT_EQ(a,b)  do{UT_ASSERT((a)==(b));}while(0)
#define UT_EXPECT_NE(a,b)  do{UT_ASSERT((a)!=(b));}while(0)

#define dimof(x) (sizeof(x) / sizeof(*(x)))

// Our network shall consist of 3 Layers (from left to right):

static float reLU(float inValue)
{
    return fmaxf(0.f, inValue);
}

static float sigmoid(float inValue)
{
    return 1.f / (1.f + expf(-inValue));
}

static float output(float inValue)
{
    return 1.f / (1.f + expf(-inValue));
}

/* ========================================================================== *
 * Main entry
 * ========================================================================== */
int main(int argc, char* argv[])
{
    {

        utils::CMath math;

        kilib::CNeuronalNet neuronalNet;


        neuronalNet.init({
                 3 // - An input Layer with 3 Neurons
              , 10  // - A first hidden layer with 16 Neurons
              , 16  // - A second hidden layer with 16 Neurons
              , 8  // - An output layer with 8 Neurons
        },
            reLU,
            output,
            math
        );

        // Train the Network

        // Step 1 Generate Input Data (a Bitwise rpräsentation of the Training data)
        kilib::CLayer* inputLayer = neuronalNet.inputLayer();
        const kilib::CLayer* outputLayer = neuronalNet.outputLayer();
        if (inputLayer && outputLayer)
        {
            utils::CVectorF32* outputVector = inputLayer->neuronOutputVector();
            assert(outputVector);

            const std::array<uint8_t, 8> kTrainingData = {0,1,2,3,4,5,6,7};
            for (const uint8_t trainingInput : kTrainingData)
            {
                (*outputVector)[0] = (trainingInput & 0b001 ? 1 : 0);
                (*outputVector)[1] = (trainingInput & 0b010 ? 1 : 0);
                (*outputVector)[2] = (trainingInput & 0b100 ? 1 : 0);


                for (unsigned int epochCnt = 0; epochCnt < 1; ++epochCnt)
                {
                    const bool gradientKnown = (0 != epochCnt);
                    // Calculate the Cost
                    neuronalNet.forwardPropagation([&](unsigned int index, float value)->void{
                        printf("Epoch %d: v[%d] = %.2f\n", epochCnt+1, index, value);
                    });

                    // Change the weight
                    {
                    }
                } // for (unsigned int epochCnt = 0; epochCnt < 100; ++epochCnt)
            }
        }
    }

    return EXIT_SUCCESS;
}
