#include <onnx.proto3.pb.h>

#include <glog/logging.h>
#include <common/StrKeyMap.h>
#include <daq_generated.h>

class OnnxConverter {
private:
    using Shape = std::vector<int>;

    template <typename T>
    struct Tensor {
        std::vector<T> data;
        Shape shape;
    };

    using FTensor = Tensor<float>;

    enum class FuseCode {
        FUSED_NONE,
        FUSED_RELU,
        FUSED_RELU1,
        FUSED_RELU6
    };

    std::map<std::string, std::string> name_map;

    std::string m(const std::string &str);

    flatbuffers::FlatBufferBuilder builder;

    std::vector<std::string> operands;
    StrKeyMap<FTensor> nnapi_tensors;
    StrKeyMap<FTensor> onnx_tensors;
    std::vector<flatbuffers::Offset<DNN::Layer>> layers;

    std::vector<flatbuffers::Offset<DNN::Tensor>> tensors;

    DNN::FuseCode convert_fuse_code_type(FuseCode fuse_code);
    std::pair<std::optional<std::string>, FuseCode> find_activation(const onnx::ModelProto &model_proto, const onnx::NodeProto &node);

    void add_conv(const std::string &input_name, const std::vector<int> &strides, const std::vector<int> &pads, 
            const std::vector<int> &dilations, int group, 
            const std::pair<std::optional<std::string>, FuseCode>& activation,
            const std::string &ori_weight_name, const std::optional<std::string> &bias_name, const std::string &output_name);

    template <typename T>
    uint32_t product(const std::vector<T> &v) {
        return static_cast<uint32_t> (accumulate(v.begin(), v.end(), 1, std::multiplies<T>()));
    }
    /**
     * onnx: [filter_out_channel, filter_in_channel / group, height, width]
     * nnapi: [1, height, width, depth_out]
     */
    template <typename T>
    Tensor<T> onnx2nnapi_dw(const Tensor<T> &src) {
        Tensor<T> dest;
        dest.data.resize(product(src.shape));
        // t for total
        auto out_t = src.shape[0], in_t = src.shape[1], h_t = src.shape[2], w_t = src.shape[3];
        CHECK_EQ(in_t, 1);
        for (int out = 0; out < out_t; out++) {
            for (int in = 0; in < in_t; in++) {
                for (int h = 0; h < h_t; h++) {
                    for (int w = 0; w < w_t; w++) {
                        auto onnx_idx = out * in_t * h_t * w_t + in * h_t * w_t + h * w_t + w;
                        auto nnapi_idx = h * w_t * out_t + w * out_t + out;
                        dest.data[nnapi_idx] = src.data[onnx_idx];
                    }
                }
            }
        }
        dest.shape = {in_t, h_t, w_t, out_t};
        return dest;
    }

    /**
     * onnx: [filter_out_channel, filter_in_channel, height, width]
     * nnapi: [depth_out, height, width, depth_in]
     */
    template <typename T>
    Tensor<T> onnx2nnapi_vanilla(const Tensor<T> &src) {
        Tensor<T> dest;
        dest.data.resize(product(src.shape));
        // t for total
        auto out_t = src.shape[0], in_t = src.shape[1], h_t = src.shape[2], w_t = src.shape[3];
        for (int out = 0; out < out_t; out++) {
            for (int in = 0; in < in_t; in++) {
                for (int h = 0; h < h_t; h++) {
                    for (int w = 0; w < w_t; w++) {
                        auto onnx_idx = out * in_t * h_t * w_t + in * h_t * w_t + h * w_t + w;
                        auto nnapi_idx = out * h_t * w_t * in_t + h * w_t * in_t + w * in_t + in;
                        dest.data[nnapi_idx] = src.data[onnx_idx];
                    }
                }
            }
        }
        dest.shape = {out_t, h_t, w_t, in_t};
        return dest;
    }

public:
    void convert(const onnx::ModelProto &model, const std::string &filepath);
};
