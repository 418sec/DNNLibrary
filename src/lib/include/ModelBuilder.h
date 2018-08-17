//
// Created by daquexian on 2017/11/8.
//

#ifndef NNAPIEXAMPLE_MODELBUILDER_H
#define NNAPIEXAMPLE_MODELBUILDER_H

#include <android/NeuralNetworks.h>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <optional>
#include "Model.h"

class ModelBuilder {
public:
    using Index = uint32_t;
    using IndexSeq = std::vector<Index>;
    using Shape = std::vector<uint32_t>;
    // FIXME: It's a quick fix, refactor it later
    std::map<std::string, Index> blobNameToIndex;

private:
    ANeuralNetworksModel* model = nullptr;
    std::unique_ptr<Model> dnn_model_;
    std::vector<char *> charBufPointers;
    std::vector<float *> floatBufPointers;
    // NHWC
    std::map<Index, Shape> dimensMap;
    IndexSeq inputIndexVector;
    IndexSeq outputIndexVector;
    std::map<uint32_t , Index> uint32OperandMap;
    std::map<int32_t , Index> int32OperandMap;
    std::map<float, Index> float32OperandMap;
    std::map<float, Index> float32AsTensorOperandMap;

    uint32_t missingInt32OperandIndex = UINT32_MAX;
    uint32_t missingFloat32OperandIndex = UINT32_MAX;

    uint32_t nextIndex = 0;

    static const uint32_t WRONG_INPUT = UINT32_MAX -1;
    static const uint32_t WRONG_POOLING_TYPE = UINT32_MAX -2;
    static const int WRONG_OPERAND_INDEX = -10;

    uint32_t addNewOperand(ANeuralNetworksOperandType *type);

    // IndexSeq addOperation(int op, IndexSeq input_indexes, Shape... shapes);
    template <typename... Shapes>
    IndexSeq addOperation(int op, IndexSeq input_indexes, Shapes... shapes);

    Index addOperand(int32_t value);
    Index addOperand(float value);
    Index addOperand(uint32_t value);
    Index addFloat32AsTensorOperand(float value);
    Index addInt32NullOperand();
    Index addFloat32NullOperand();
    Index addFloat32NullOperandWithDims(Shape &dims);
    Index addFloat32ZeroOperandWithDims(Shape &dims);

    ANeuralNetworksOperandType getFloat32OperandTypeWithDims(Shape &dims);
    ANeuralNetworksOperandType getInt32OperandTypeWithDims(Shape &dims);

    ANeuralNetworksOperandType getInt32OperandType();
    ANeuralNetworksOperandType getFloat32OperandType();

public:
    ModelBuilder();
    ~ModelBuilder();

    static const int MAX_POOL = 0;
    static const int AVE_POOL = 1;

    static const uint32_t ACTIVATION_NONE = ANEURALNETWORKS_FUSED_NONE;
    static const uint32_t ACTIVATION_RELU = ANEURALNETWORKS_FUSED_RELU;

    static const uint32_t PREFERENCE_FAST_SINGLE_ANSWER = ANEURALNETWORKS_PREFER_FAST_SINGLE_ANSWER;
    static const uint32_t PREFERENCE_SUSTAINED_SPEED = ANEURALNETWORKS_PREFER_SUSTAINED_SPEED;
    static const uint32_t PREFERENCE_LOW_POWER = ANEURALNETWORKS_PREFER_LOW_POWER;

    static const uint32_t MF_LAYER_END = 0;
    static const uint32_t MF_CONV = 1;
    static const uint32_t MF_MAX_POOL = 2;
    static const uint32_t MF_AVE_POOL = 3;
    static const uint32_t MF_FC = 4;
    static const uint32_t MF_SOFTMAX = 5;
    static const uint32_t MF_INPUT = 6;
    static const uint32_t MF_MUL = 7;
    static const uint32_t MF_ADD = 8;
    static const uint32_t MF_RELU = 9;
    static const uint32_t MF_CONCAT = 10;
    static const uint32_t MF_LRN = 11;
    static const uint32_t MF_DEPTH_CONV = 12;
#if __ANDROID_API__ >= __ANDROID_API_P__
    static const uint32_t MF_STRIDED_SLICE = 13;
#endif

    static const uint32_t MF_ACTIVATION_NONE = 0;
    static const uint32_t MF_ACTIVATION_RELU = 1;

    static const uint32_t MF_TENSOR_OP = 0;
    static const uint32_t MF_SCALAR_OP = 1;
    static const uint32_t MF_ARRAY_OP = 2;

    static const uint32_t MF_STRING_END = 0;

    static const uint32_t MF_PARAM_END = 0;
    static const uint32_t MF_PADDING_LEFT = 1;
    static const uint32_t MF_PADDING_RIGHT = 2;
    static const uint32_t MF_PADDING_TOP = 3;
    static const uint32_t MF_PADDING_BOTTOM = 4;
    static const uint32_t MF_STRIDE_X = 5;
    static const uint32_t MF_STRIDE_Y = 6;
    static const uint32_t MF_FILTER_HEIGHT = 7;
    static const uint32_t MF_FILTER_WIDTH = 8;
    static const uint32_t MF_NUM_OUTPUT = 9;
    static const uint32_t MF_WEIGHT = 10;
    static const uint32_t MF_BIAS = 11;
    static const uint32_t MF_ACTIVATION = 12;
    static const uint32_t MF_TOP_NAME = 13;
    static const uint32_t MF_BETA = 14;
    static const uint32_t MF_LRN_ALPHA = 15;
    static const uint32_t MF_LRN_BETA = 16;
    static const uint32_t MF_LOCAL_SIZE = 17;
    static const uint32_t MF_GROUP = 18;

    static const int NN_PROCEDURE_MASK = ((1U << 16U) - 1) << 16U;
    static const int NN_CAUSE_MASK = ((1U << 16U) - 1);
    static const int NN_IDENTIFY_IO = 1U << 16U;
    static const int NN_MODEL_FINISH = 1U << 17U;
    static const int NN_CREATE = 1U << 18U;
    static const int NN_PREFERENCE = 1U << 19U;
    static const int NN_COMP_FINISH = 1U << 20U;

    static std::string getErrorProcedure(int errorCode);
    static std::string getErrorCause(int errorCode);

    ANeuralNetworksCompilation* compilation = nullptr;

    Index getBlobIndex(std::string blobName);
    Shape getBlobDim(std::string blobName);
    Shape getBlobDim(Index index);
    Index addInput(uint32_t height, uint32_t width, uint32_t depth);
    Index addDepthWiseConv(Index input, int32_t strideX, int32_t strideY, int32_t paddingLeft, int32_t paddingRight,
                           int32_t paddingBottom, int32_t paddingTop, int32_t activation, int32_t depthMultiplier,
                           uint32_t weightIndex, std::optional<uint32_t> biasIndex);
    Index addConv(Index input, int32_t strideX, int32_t strideY, int32_t paddingLeft, int32_t paddingRight, int32_t paddingBottom,
                      int32_t paddingTop, int32_t activation, uint32_t weightIndex, std::optional<uint32_t> biasIndex);
    Index addTensorFromBuffer(const float *buffer, std::vector<uint32_t> dimen);
    Index addTensorFromBuffer(const int32_t *buffer, std::vector<uint32_t> dimen);
    Index addTensorFromMemory(const unsigned char *addr, Shape dimen);
    Index addFC(Index input, int32_t activation, uint32_t weightIndex, uint32_t biasIndex);
    Index addCaffePool(Index input, int32_t strideX, int32_t strideY, int32_t paddingLeft,
                          int32_t paddingRight, int32_t paddingTop, int32_t paddingBottom,
                          int32_t height, int32_t width, int32_t activation,
                          uint32_t poolingType);
    Index
    addPool(Index input, int32_t strideX, int32_t strideY, int32_t paddingLeft, int32_t paddingRight, int32_t paddingTop,
            int32_t paddingBottom, int32_t height, int32_t width, int32_t activation, uint32_t poolingType);
    Index addSoftMax(Index input, float beta);
    Index addAddScalar(Index input, float scalar);
    Index addAddTensor(Index input1, Index input2);
    Index addMulScalar(Index input, float scalar);
    Index addMulTensor(Index input1, Index input2);
    Index addReLU(Index input);
    Index addConcat(const IndexSeq &inputs, uint32_t axis);
    Index addLRN(Index input, uint32_t local_size, float bias, float alpha, float beta);
#if __ANDROID_API__ >= __ANDROID_API_P__
    Index addStridedSlice(Index input, const std::vector<int32_t> &starts, const std::vector<int32_t> &ends,
                             const std::vector<int32_t> &strides, int32_t beginMask, int32_t endMask,
                             int32_t shrinkAxismask);
#endif
    void addIndexIntoOutput(Index index);
    int compile(uint32_t preference);
    void prepareForExecution();
    IndexSeq getInputIndexes();
    IndexSeq getOutputIndexes();
    void registerBufferPointer(char *pointer);
    void registerBufferPointer(float *pointer);

    // ModelBuilder &readFromBuffer(const char *buffer);
    ModelBuilder& readFromFile(const std::string &filename);
    ModelBuilder& simplestModel();

    void prepare();
    void setMemory(int fd, size_t size, size_t offset);
    void setBuffer(unsigned char *data, size_t data_size);
    std::unique_ptr<Model> finish();

    template <typename... Args>
    void addOperands(IndexSeq &indexes, Args... args) {
        (indexes.push_back(addOperand(args)), ...);
    }
};


uint32_t product(const std::vector<uint32_t> &v);
#endif //NNAPIEXAMPLE_MODELBUILDER_H
