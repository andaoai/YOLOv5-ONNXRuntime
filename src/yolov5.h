#ifndef YOLOV5_H
#define YOLOV5_H

#include "Algorithm.h"
#include <opencv2/opencv.hpp>
#include <onnxruntime_cxx_api.h>
#include <vector>
#include <string>

// YOLOv5 检测结果结构
struct Detection {
    cv::Rect box;           // 边界框
    float confidence;       // 置信度
    int class_id;          // 类别ID

    Detection() : confidence(0.0f), class_id(-1) {}

    Detection(const cv::Rect& bbox, float conf, int cls_id)
        : box(bbox), confidence(conf), class_id(cls_id) {}
};

// YOLOv5 检测器类 - 继承Algorithm抽象类，使用Detection结果类型
class YOLOv5Detector : public Algorithm<Detection> {
public:
    // 构造函数
    YOLOv5Detector(const std::string& model_path,
                   float confidence_threshold = 0.5f,
                   float nms_threshold = 0.4f);

    // 析构函数
    ~YOLOv5Detector() override;

    // 实现Algorithm抽象接口
    bool load_model(const std::string& model_path) override;
    cv::Mat preprocess(const cv::Mat& input_image) override;
    std::vector<float> inference(const cv::Mat& preprocessed_image) override;
    std::vector<Detection> postprocess(const std::vector<float>& inference_output,
                                     const cv::Mat& original_image) override;
    std::vector<Detection> detect(const cv::Mat& image) override;
    cv::Mat draw_results(const cv::Mat& image, const std::vector<Detection>& results) override;

    // 配置接口实现
    void set_confidence_threshold(float threshold) override;
    void set_nms_threshold(float threshold) override;
    float get_confidence_threshold() const override;
    float get_nms_threshold() const override;

    // 模型信息接口实现
    bool is_model_loaded() const override;
    std::string get_model_info() const override;
    std::string get_class_name(int class_id) const override;

    // 保持原有的绘制接口（向后兼容）
    cv::Mat draw_detections(const cv::Mat& image, const std::vector<Detection>& detections);

private:
    // 内部辅助函数
    cv::Mat preprocess_image(const cv::Mat& image, int input_width, int input_height);
    std::vector<Detection> apply_nms(std::vector<Detection>& detections, float nms_threshold);
    std::vector<Detection> postprocess_internal(const Ort::Value& output_tensor,
                                               const cv::Mat& original_image,
                                               int input_width, int input_height);

    // ONNX Runtime 相关成员变量
    std::unique_ptr<Ort::Env> env_;
    std::unique_ptr<Ort::Session> session_;
    std::unique_ptr<Ort::SessionOptions> session_options_;

    std::vector<const char*> input_node_names_;
    std::vector<const char*> output_node_names_;
    std::vector<int64_t> input_node_dims_;
    
    // COCO 数据集类别名称
    static const std::vector<std::string> class_names_;
};

#endif // YOLOV5_H
