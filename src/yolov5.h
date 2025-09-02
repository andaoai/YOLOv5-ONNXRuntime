#ifndef YOLOV5_H
#define YOLOV5_H

#include <opencv2/opencv.hpp>
#include <onnxruntime_cxx_api.h>
#include <vector>
#include <string>

// YOLOv5 检测结果结构
struct Detection {
    cv::Rect box;
    float confidence;
    int class_id;
};

// YOLOv5 检测器类
class YOLOv5Detector {
public:
    // 构造函数
    YOLOv5Detector(const std::string& model_path, 
                   float confidence_threshold = 0.5f, 
                   float nms_threshold = 0.4f);
    
    // 析构函数
    ~YOLOv5Detector();
    
    // 检测函数
    std::vector<Detection> detect(const cv::Mat& image);
    
    // 绘制检测结果
    cv::Mat draw_detections(const cv::Mat& image, const std::vector<Detection>& detections);
    
    // 获取类别名称
    std::string get_class_name(int class_id) const;
    
    // 设置阈值
    void set_confidence_threshold(float threshold);
    void set_nms_threshold(float threshold);

private:
    // 图像预处理
    cv::Mat preprocess_image(const cv::Mat& image, int input_width, int input_height);
    
    // 非极大值抑制
    std::vector<Detection> apply_nms(std::vector<Detection>& detections, float nms_threshold);
    
    // 后处理
    std::vector<Detection> postprocess(const Ort::Value& output_tensor, 
                                     const cv::Mat& original_image,
                                     int input_width, int input_height);
    
    // 成员变量
    std::unique_ptr<Ort::Env> env_;
    std::unique_ptr<Ort::Session> session_;
    std::unique_ptr<Ort::SessionOptions> session_options_;
    
    std::vector<const char*> input_node_names_;
    std::vector<const char*> output_node_names_;
    std::vector<int64_t> input_node_dims_;
    
    float confidence_threshold_;
    float nms_threshold_;
    
    // COCO 数据集类别名称
    static const std::vector<std::string> class_names_;
};

#endif // YOLOV5_H
