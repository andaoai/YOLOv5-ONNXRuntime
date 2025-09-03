#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <memory>

// 算法基类 - 使用模板支持不同的结果类型
template<typename ResultType>
class Algorithm {
public:
    // 构造函数和析构函数
    Algorithm() = default;
    virtual ~Algorithm() = default;

    // 禁用拷贝构造和赋值
    Algorithm(const Algorithm&) = delete;
    Algorithm& operator=(const Algorithm&) = delete;

    // 核心接口 - 分步执行
    virtual bool load_model(const std::string& model_path) = 0;
    virtual cv::Mat preprocess(const cv::Mat& input_image) = 0;
    virtual std::vector<float> inference(const cv::Mat& preprocessed_image) = 0;
    virtual std::vector<ResultType> postprocess(const std::vector<float>& inference_output,
                                               const cv::Mat& original_image) = 0;

    // 检测接口 - 完整的检测流程
    virtual std::vector<ResultType> detect(const cv::Mat& image) = 0;

    // 结果可视化
    virtual cv::Mat draw_results(const cv::Mat& image,
                               const std::vector<ResultType>& results) = 0;

    // 配置接口
    virtual void set_confidence_threshold(float threshold) = 0;
    virtual void set_nms_threshold(float threshold) = 0;
    virtual float get_confidence_threshold() const = 0;
    virtual float get_nms_threshold() const = 0;

    // 模型信息
    virtual bool is_model_loaded() const = 0;
    virtual std::string get_model_info() const = 0;

    // 类别名称接口
    virtual std::string get_class_name(int class_id) const = 0;

protected:
    // 受保护的成员变量
    bool model_loaded_ = false;
    float confidence_threshold_ = 0.5f;
    float nms_threshold_ = 0.4f;
    std::string model_path_;
};

#endif // ALGORITHM_H
