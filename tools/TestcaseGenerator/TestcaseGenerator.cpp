#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include <functional>
#include <libgen.h>

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

static float _randFloat()
{
    return 0.1f * (random() % 1000);
}

std::string str_printf(const char * __restrict __format, ...)
{
    va_list valist;
    va_start(valist, __format);
    char* buffer;
    int rc = vasprintf(&buffer, __format, valist);
    const std::string s(buffer);
    free(buffer);
    return s;
}

static std::string remove_string(std::string inString, const std::string& removeString)
{
    const auto foundPos = inString.find(removeString);
    if (foundPos != std::string::npos)
    {
        inString.replace(foundPos, removeString.size(), std::string());
        return remove_string(inString, removeString);
    }
    return inString;
}

struct Layer
{
    Layer(Layer* inParentLayer, unsigned int inNrOfNeurons)
    : _neuronCnt(inNrOfNeurons)
    , _parentLayer(inParentLayer)
    , _neuronWeights()
    , _biasValues(new(std::nothrow) float[_neuronCnt])
    , _neuronOutputs(new(std::nothrow) float[_neuronCnt])
    {
        assert(_biasValues);
        assert(_neuronOutputs);

        for (unsigned int i=0; i < _neuronCnt; ++i)
        {
            _biasValues[i] = _parentLayer ? _randFloat() : 0.f;
        }

        if (_parentLayer)
        {
            _neuronWeights.reserve(_neuronCnt);
            for (unsigned int i=0; i < _neuronCnt; ++i)
            {
                _neuronWeights.push_back(new(std::nothrow) float [_parentLayer->_neuronCnt]);
                assert(_neuronWeights[i]);

                for (unsigned int j = 0; j < _parentLayer->_neuronCnt; ++j )
                {
                    _neuronWeights[i][j] = _randFloat();
                }
            }
        }
    }

    ~Layer()
    {
    return;
        for (auto *weight : _neuronWeights)
        {
            delete weight;
        }
        _neuronWeights.clear();

        delete [] _neuronOutputs;
        _neuronOutputs = nullptr;

        delete [] _biasValues;
        _biasValues = nullptr;
    }

    // This class is not ought to be copied or assigned!
    Layer(const Layer&) = delete;
    Layer(Layer&&) = delete;
    Layer& operator= (const Layer&) = delete;
    Layer& operator= (Layer&&) = delete;

    bool isInputLayer() const
    {
        return nullptr == _parentLayer;
    }

    unsigned int nrOfNeurons() const
    {
        return _neuronCnt;
    }

    float outputValueForNeuronAtIndex(unsigned int outputIndex) const
    {
        assert(_neuronOutputs);
        assert(outputIndex < _neuronCnt);
        return _neuronOutputs[outputIndex];
    }

    void setOutpuValueForNeuronTo(unsigned int inNeuronIndex, float inValue)
    {
        assert(_neuronOutputs);
        if (_neuronOutputs)
        {
            assert (inNeuronIndex < _neuronCnt);
            if (inNeuronIndex < _neuronCnt)
            {
                _neuronOutputs[inNeuronIndex] = inValue;
            }
        }
    }

    unsigned int nrOfParentNeurons() const
    {
        return _parentLayer ? _parentLayer->_neuronCnt : 0;
    }

    using WeightningsEnumerator = std::function<void(unsigned int weightningIndex, float weightning)>;
    void enumerateWeightningsForNeuronAtIndex(unsigned int inNeuronIndex, const WeightningsEnumerator& inWeightningsEnumerator) const
    {
        if (inNeuronIndex < _neuronCnt)
        {
            for (unsigned int weightningIndex = 0; weightningIndex < nrOfWeightnings(); ++weightningIndex)
            {
                inWeightningsEnumerator(weightningIndex, _neuronWeights[inNeuronIndex][weightningIndex]);
            }
        }
    }

    using Enumerator = std::function<void(unsigned int neuronIndex, unsigned int weightningIndex, float weightning, float inputValue, float& OutputValueRef)>;
    void enumerate(const Enumerator& inEnumerator) const
    {
        assert(_neuronOutputs);
        if (_neuronOutputs)
        {
            for (unsigned int neuronIndex = 0; neuronIndex < _neuronCnt; ++neuronIndex)
            {
                enumerateForNeuronAtIndex(neuronIndex, inEnumerator);
            }
        }
    }

    void enumerateForNeuronAtIndex(unsigned int inNeuronIndex, const Enumerator& inEnumerator) const
    {
        assert(_neuronOutputs);
        if (_neuronOutputs)
        {
            if (_parentLayer)
            {
                for (unsigned int weightningIndex = 0; weightningIndex < nrOfWeightnings(); ++weightningIndex)
                {
                    inEnumerator(
                         inNeuronIndex,                                 // unsigned int neuronIndex
                         weightningIndex,                               // unsigned int weightningIndex
                        _neuronWeights[inNeuronIndex][weightningIndex], // float weightning
                        _parentLayer->_neuronOutputs[weightningIndex],  // float inputValue
                        _neuronOutputs[inNeuronIndex]                   // float& OutputValueRef
                        );
                }
            }
            else
            {
                inEnumerator(
                    inNeuronIndex,                  // unsigned int neuronIndex
                    -1,                             // unsigned int weightningIndex
                    1.f,                            // float weightning
                    _neuronOutputs[inNeuronIndex],  // float inputValue
                    _neuronOutputs[inNeuronIndex]   // float& OutputValueRef
                    );
            }
        }
    }

    using BiasEnumerator = std::function<void(unsigned int neuronIndex, float biasValue)>;
    void enumerateBiasValues(const BiasEnumerator& inBiasEnumerator) const
    {
        assert(_biasValues);
        if (_biasValues)
        {
            for (unsigned int neuronIndex = 0; neuronIndex < _neuronCnt; ++neuronIndex)
            {
                if (_parentLayer)
                {
                    inBiasEnumerator(neuronIndex, _biasValues[neuronIndex]);
                }
            }
        }
    }

    float weightningForNeuron(unsigned int inNeuronIndex, unsigned int weightningIndex) const
    {
        float weightning = 0;
        if ((inNeuronIndex < _neuronCnt) && (weightningIndex < nrOfWeightnings()))
        {
            weightning = _neuronWeights[inNeuronIndex][weightningIndex];
        }
        return weightning;
    }

    unsigned int nrOfWeightnings() const
    {
        return _parentLayer ? _parentLayer->_neuronCnt : 0;
    }

    float biasForNeuronAtIndex(unsigned int inNeuronIndex) const
    {
        float v;
        assert(_biasValues);
        assert(inNeuronIndex < _neuronCnt);
        if (_biasValues && inNeuronIndex < _neuronCnt)
        {
            v = _biasValues[inNeuronIndex];
        }
        return v;
    }

    std::string dotProductTermForNeuronAtIndex(unsigned int inNeuronIndex, const std::function<std::string(unsigned int neuronInLayerIdx)>& inlabelGenerator, bool useFullQualified = false) const
    {
        std::string term;

        enumerateForNeuronAtIndex(inNeuronIndex, [&](unsigned int neuronIndex, unsigned int weightningIndex, float weightning, float inValue, float& OutputValueRef)->void{
            if (_parentLayer->isInputLayer())
            {
                term += str_printf("%4.1ff * %4.1ff + ", weightning, inValue);
            }
            else
            {
                if (useFullQualified)
                {
                    term += str_printf("%4.1ff * ( %s ) + ", weightning, _parentLayer->dotProductTermForNeuronAtIndex(weightningIndex, inlabelGenerator, useFullQualified).c_str());
                }
                else
                {
                    term += str_printf("%4.1ff * %s + ", weightning, inlabelGenerator(weightningIndex).c_str());
                }
            }
        });
        term += str_printf("%4.1ff", biasForNeuronAtIndex(inNeuronIndex));


        return term;
    }

    unsigned int _neuronCnt = 0;
    Layer* _parentLayer = nullptr;
    std::vector<float*> _neuronWeights;
    float* _biasValues = nullptr;
    float* _neuronOutputs = nullptr;
};


static std::string _dotProductTermLabelForNeuronAtIndex(unsigned int layerIdx, unsigned int neuronInLayerIdx)
{
    return str_printf("dotProdL%dN%d", layerIdx, neuronInLayerIdx);
}

struct GeneratorProps
{

    GeneratorProps(const std::vector<unsigned int>& inLayers)
    : _layers()
    {
        _layers.reserve(inLayers.size());

        Layer* parentLayer = nullptr;
        for (unsigned int layerIdx = 0; layerIdx < inLayers.size(); ++layerIdx)
        {
            Layer* thisLayer = new(std::nothrow) Layer(parentLayer, inLayers[layerIdx]);
            assert(thisLayer);

            _layers.push_back(thisLayer);
            parentLayer = thisLayer;

            if (0 == layerIdx)
            {
                for (unsigned int neuronIdx = 0; neuronIdx < thisLayer->nrOfNeurons(); ++neuronIdx)
                {
                    thisLayer->setOutpuValueForNeuronTo(neuronIdx, _randFloat());
                }
            }
        }
    }

    ~GeneratorProps()
    {
        for (auto *layer : _layers)
        {
            delete layer;
        }
        _layers.clear();
    }

    std::string dump() const
    {
        std::string s;
        s.append("// =====================================================================\n");
        s.append(str_printf("// = Neuronal Network that contains %d Layers (1 Input Layer,\n"
                            , _layers.size()));
        s.append(str_printf("// = %d hidden Layer and 1 output Layer)\n"
                            , _layers.size() - 2));
        s.append("// = \n");
        s.append(str_printf("// =  - Input Layer with %d neurons\n", _layers[0]->nrOfNeurons()));
        for (unsigned int layerIndex = 1; layerIndex < _layers.size()-1; ++layerIndex)
        {
            s.append(str_printf("// =  - Hidden Layer %d with %d neurons\n", layerIndex, _layers[layerIndex]->nrOfNeurons()));
        }
        s.append(str_printf("// =  - Output Layer with %d neurons\n", _layers.back()->nrOfNeurons()));

        // Statistics
        {
            unsigned int sumOfNeurons = 0;
            unsigned int sumOfWeightnings = 0;
            unsigned int sumOfBiases = 0;

            for (unsigned int layerIndex = 1; layerIndex < _layers.size(); ++layerIndex)
            {
                const Layer* thisLayer = _layers[layerIndex];
                const Layer* parentLayer = thisLayer->_parentLayer;

                sumOfNeurons += thisLayer->nrOfNeurons();
                sumOfWeightnings += thisLayer->nrOfNeurons() * parentLayer->nrOfNeurons();
            }

            s.append("// = ---------------------- S T A T I S T I C S ------------------------\n");
            s.append(str_printf("// = Sum of Neurons: %d\n", sumOfNeurons));
            s.append(str_printf("// = Sum of Weightnings: %d\n", sumOfWeightnings));
            s.append(str_printf("// = Sum of Biases: %d\n", sumOfNeurons));
            s.append("// =====================================================================\n");
        }

        s.append("utils::CMath math;\n");
        s.append("static kilib::CActivationNull nullActivation;\n");

        s.append(str_printf("kilib::CLayer layer[%3d];\n", _layers.size()));

        for (unsigned int layerIdx = 0; layerIdx < _layers.size(); ++layerIdx)
        {
            s.append(str_printf("layer[%3d].init(%3d, nullActivation, math, %s);\n",
                layerIdx,
                _layers[layerIdx]->nrOfNeurons(),
                layerIdx == 0 ? "nullptr" : str_printf("&layer[%3d]", layerIdx-1).c_str()));
        }


        unsigned int layerIdx = 0;
        {
            s.append("\n");
            s.append("// Define the neuronal network input...\n");
            const Layer& thisLayer(*_layers[layerIdx]);
            for (unsigned int neuronInLayerIdx = 0; neuronInLayerIdx < thisLayer.nrOfNeurons(); ++neuronInLayerIdx)
            {
                const Layer& thisLayer(*_layers[layerIdx]);
                s.append(str_printf("constexpr float kInputValueN%d = %.1ff;\n",
                    neuronInLayerIdx,
                    thisLayer.outputValueForNeuronAtIndex(neuronInLayerIdx)
                    ));
            }
        }
        {
            s.append("\n");
            s.append("// Assign some Values to the Input layers neurons...\n");
            const Layer& thisLayer(*_layers[layerIdx]);
            for (unsigned int neuronInLayerIdx = 0; neuronInLayerIdx < thisLayer.nrOfNeurons(); ++neuronInLayerIdx)
            {
                const Layer& thisLayer(*_layers[layerIdx]);
                s.append(str_printf("(*layer[%3d].neuronOutputVector())[%3d] = kInputValueN%d;\n",
                    layerIdx,
                    neuronInLayerIdx,
                    neuronInLayerIdx
                    ));
            }
        }

        {
            s.append("\n");
            s.append("// ========================================================================\n");
            s.append("// Define the neuronal network Weightnings...\n");
            s.append("// ========================================================================\n");
            for (unsigned int layerIdx = 1; layerIdx < _layers.size(); ++layerIdx)
            {
                const Layer& thisLayer(*_layers[layerIdx]);

                s.append(str_printf("\n// %d * %d Weightnings for Neurons of Layer %d (%d neurons)\n",
                                    thisLayer.nrOfNeurons(), thisLayer.nrOfWeightnings(), layerIdx, thisLayer.nrOfNeurons()));
                thisLayer.enumerate([&](unsigned int neuronIndex, unsigned int weightningIndex, float weightning, float InputValue, float& OutputValueRef)->void{
                    s.append(str_printf("*layer[%d].weightningAtIndexForNeuron(%3d, %3d) = %4.1ff;\n",
                        layerIdx,
                        neuronIndex,
                        weightningIndex,
                        weightning));
                });

                s.append(str_printf("\n// Bias Values for Neurons of Layer %d (%d neurons)\n", layerIdx, thisLayer.nrOfNeurons()));
                thisLayer.enumerateBiasValues([&](unsigned int neuronIndex, float biasValue)->void{
                    s.append(str_printf("*layer[%3d].biasForNeuron(%3d) = %4.1ff;\n",
                        layerIdx,
                        neuronIndex,
                        biasValue));
                });
            }
        }

        {
            for (unsigned int layerIdx = 1; layerIdx < _layers.size(); ++layerIdx)
            {
                s.append("\n");
                s.append(str_printf("// Calculate the Dot Product for Layer %d\n", layerIdx));

                const Layer& thisLayer(*_layers[layerIdx]);
                for (unsigned int neuronInLayerIdx = 0; neuronInLayerIdx < thisLayer.nrOfNeurons(); ++neuronInLayerIdx)
                {
                    s.append(str_printf("\n// Dot Product for Neuron at index %d\n", neuronInLayerIdx));
                    s.append(str_printf("constexpr float %s = %s;\n",
                        _dotProductTermLabelForNeuronAtIndex(layerIdx, neuronInLayerIdx).c_str(),
                        thisLayer.dotProductTermForNeuronAtIndex(neuronInLayerIdx, [&](unsigned int neuronInLayerIdx)->std::string{
                            return _dotProductTermLabelForNeuronAtIndex(layerIdx-1, neuronInLayerIdx);
                        }, true).c_str()
                        ));
                }
            }
        }

        // Test code
        {
            s.append("\n");
            s.append("// Invoke the Tests\n");

            Layer* outputLayer = _layers[_layers.size()-1];
            s.append(str_printf("const utils::CVectorF32* result = layer[%d].forwardPropagation();\n", _layers.size()-1));

            s.append("UT_EXPECT_NE(nullptr, result);\n");
            s.append("if (result)\n");
            s.append("{\n");

            for (unsigned int thisNeuronIndex = 0; thisNeuronIndex < outputLayer->nrOfNeurons(); ++thisNeuronIndex)
            {
                s.append(str_printf("    UT_EXPECT_EQ(dotProdL%dN%d, (*result)[%3d]);\n",
                    _layers.size()-1, thisNeuronIndex, thisNeuronIndex));
            }
            s.append("}\n");

        }
        return s;
    }

    std::vector<Layer*> _layers;

};

static std::string _genCode(const std::vector<unsigned int>& inLayers)
{
    GeneratorProps props(inLayers);
    return props.dump();
}


void usage(const char* prgName)
{
    printf("Usage of %s:\n", prgName);
    printf("    %s <list_of_neuronal_layers>\n", prgName);
    printf("Whereby <list_of_neuronal_layers> means a list of number, each element\n"
           "separated by a comma\n"
           "\n"
           "Example: The command '%s 4,3,2,10,5'\n"
           "will generate Unittests code for a neuronal net that consists of 5 layers:\n"
           "- An Input Layer that conisists of 4 Neurons\n"
           "- A 1st Hidden Layer that is fed by the input layer. This layer conisists of 3 Neurons\n"
           "- A 2nd Hidden Layer that is fed by the 1st hidden layer. This layer conisists of 2 Neurons\n"
           "- A 3rd Hidden Layer that is fed by the 2nd hidden layer. This layer conisists of 10 Neurons\n"
           "- An Output Layer that conisists of 5 Neurons\n", prgName);
}

/* ========================================================================== *
 * Main entry
 * ========================================================================== */
int main(int argc, char* argv[])
{
    const char* prgName = basename(argv[0]);
    if (argc < 2)
    {
        printf("*** Error using '%s': Parameter Missing!\n\n", prgName);
        usage(prgName);
        return EXIT_FAILURE;
    }

    char* param = strdup(argv[1]);
    if (param)
    {
        std::vector<unsigned int> neuralNetTopolgy;

        char* token;
        char* paramPtr = param;
        while (nullptr != (token = strsep(&paramPtr, ",")))
        {
            neuralNetTopolgy.push_back(atoi(token));
        }

        if (neuralNetTopolgy.size() < 1)
        {
            printf("*** Error using '%s': A Neuronal net must contain at least 2 Layers!\n\n", prgName);
            return EXIT_FAILURE;
        }

        if (std::find(neuralNetTopolgy.begin(), neuralNetTopolgy.end(), 0) != neuralNetTopolgy.end())
        {
            printf("*** Error using '%s': Every Layer of the neuronal net must contain at least 1 neuron!\n\n", prgName);
            return EXIT_FAILURE;
        }

        const std::string s(_genCode(neuralNetTopolgy));

        printf("%s", s.c_str());

        free(param);
    }


    return EXIT_SUCCESS;
}
