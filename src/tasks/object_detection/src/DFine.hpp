#pragma once
#include "common.hpp"
#include "TaskInterface.hpp"

class DFine : public TaskInterface {
public:
    DFine(const TritonModelInfo& model_info);

    
    std::vector<Result> postprocess(const cv::Size& frame_size, const std::vector<std::vector<TensorElement>>& infer_results,
                                    const std::vector<std::vector<int64_t>>& infer_shapes) override;

 
    std::vector<std::vector<uint8_t>> preprocess(const std::vector<cv::Mat>& imgs) override;                                

private:
    std::vector<uint8_t> preprocess_image(const cv::Mat& img, const std::string& format, int img_type1, int img_type3,
                                    size_t img_channels, const cv::Size& img_size);
    std::vector<uint8_t> preprocess_orig_size(const cv::Mat& img, const std::vector<int64_t>& shape);                                 
    int input_width_;
    int input_height_;
    int input_channels_;
};
