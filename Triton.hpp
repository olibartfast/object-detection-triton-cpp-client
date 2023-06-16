#pragma once
#include "common.hpp"
#include "Yolo.hpp"

namespace Triton{

    enum ProtocolType { HTTP = 0, GRPC = 1 };


    struct TritonModelInfo {
        std::string output_name_;
        std::vector<std::string> output_names_;
        std::string input_name_;
        std::string input_datatype_;
        // The shape of the input
        int input_c_;
        int input_h_;
        int input_w_;
        // The format of the input
        std::string input_format_;
        int type1_;
        int type3_;
        int max_batch_size_;

        std::vector<int64_t> shape_;

    };


    TritonModelInfo setModel(const int batch_size, const int input_width, const int input_height, const std::string& modelType ){
        TritonModelInfo info;
        info.input_name_ = "images";
        if(modelType.find("yolov5") != std::string::npos || modelType.find("yolov8") != std::string::npos)
        {
            info.output_names_ = std::vector<std::string>{"output0"};
        }        
        else if(modelType.find("yolov6") != std::string::npos)
        {
            info.output_names_ = std::vector<std::string>{"outputs"};
        }
        else if(modelType.find("yolov7") != std::string::npos)
        {
            info.output_names_ = std::vector<std::string>{"output"};
        }   
        else if(modelType.find("yolonas") != std::string::npos)
        {
            info.output_names_ = std::vector<std::string>{"913", "904"};
        }                 

        info.input_datatype_ = std::string("FP32");
        // The shape of the input
        info.input_c_ = 3;
        info.input_w_ = input_width;
        info.input_h_ = input_height;
        // The format of the input
        info.input_format_ = "FORMAT_NCHW";
        info.type1_ = CV_32FC1;
        info.type3_ = CV_32FC3;
        info.max_batch_size_ = 32;
        info.shape_.push_back(batch_size);
        info.shape_.push_back(info.input_c_);
        info.shape_.push_back(info.input_h_);
        info.shape_.push_back(info.input_w_);
        return info;
    }

    union TritonClient
    {
        TritonClient()
        {
            new (&httpClient) std::unique_ptr<tc::InferenceServerHttpClient>{};
        }
        ~TritonClient() {}

        std::unique_ptr<tc::InferenceServerHttpClient> httpClient;
        std::unique_ptr<tc::InferenceServerGrpcClient> grpcClient;
    };


    auto
    getInferResults(
        tc::InferResult* result,
        const size_t batch_size,
        const std::vector<std::string>& output_names, const bool batching)
    {
        if (!result->RequestStatus().IsOk())
        {
            std::cerr << "inference  failed with error: " << result->RequestStatus()
                << std::endl;
            exit(1);
        }

        std::vector<float> infer_results;
        std::vector<int64_t> infer_shape;


        float* outputData;
        size_t outputByteSize;
        for (auto outputName : output_names)
        {
            result->RawData(
                outputName, (const uint8_t**)&outputData, &outputByteSize);

            tc::Error err = result->Shape(outputName, &infer_shape);
            infer_results = std::vector<float>(outputByteSize / sizeof(float));
            std::memcpy(infer_results.data(), outputData, outputByteSize);
            if (!err.IsOk())
            {
                std::cerr << "unable to get data for " << outputName << std::endl;
                exit(1);
            }

        }

        return make_tuple(infer_results, infer_shape);
    }


}
