#include "yolov5.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <iomanip>

// COCO 数据集类别名称
const std::vector<std::string> YOLOv5Detector::class_names_ = {
    "person", "bicycle", "car", "motorcycle", "airplane", "bus", "train", "truck", "boat", "traffic light",
    "fire hydrant", "stop sign", "parking meter", "bench", "bird", "cat", "dog", "horse", "sheep", "cow",
    "elephant", "bear", "zebra", "giraffe", "backpack", "umbrella", "handbag", "tie", "suitcase", "frisbee",
    "skis", "snowboard", "sports ball", "kite", "baseball bat", "baseball glove", "skateboard", "surfboard",
    "tennis racket", "bottle", "wine glass", "cup", "fork", "knife", "spoon", "bowl", "banana", "apple",
    "sandwich", "orange", "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair", "couch",
    "potted plant", "bed", "dining table", "toilet", "tv", "laptop", "mouse", "remote", "keyboard", "cell phone",
    "microwave", "oven", "toaster", "sink", "refrigerator", "book", "clock", "vase", "scissors", "teddy bear",
    "hair drier", "toothbrush"
};

YOLOv5Detector::YOLOv5Detector(const std::string& model_path,
                               float confidence_threshold,
                               float nms_threshold) {
    confidence_threshold_ = confidence_threshold;
    nms_threshold_ = nms_threshold;
    model_path_ = model_path;

    // 加载模型
    load_model(model_path);
}

YOLOv5Detector::~YOLOv5Detector() {
    // 智能指针会自动清理资源
}

bool YOLOv5Detector::load_model(const std::string& model_path) {
    try {
        // 创建 ONNX Runtime 环境
        env_ = std::make_unique<Ort::Env>(ORT_LOGGING_LEVEL_WARNING, "YOLOv5");

        // 创建会话选项
        session_options_ = std::make_unique<Ort::SessionOptions>();
        session_options_->SetIntraOpNumThreads(4);
        session_options_->SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);

        // 加载模型
        session_ = std::make_unique<Ort::Session>(*env_, model_path.c_str(), *session_options_);

        // 设置输入输出节点名称
        input_node_names_ = {"images"};
        output_node_names_ = {"output0"};

        // 获取输入形状
        Ort::TypeInfo input_type_info = session_->GetInputTypeInfo(0);
        auto input_tensor_info = input_type_info.GetTensorTypeAndShapeInfo();
        input_node_dims_ = input_tensor_info.GetShape();

        model_loaded_ = true;
        model_path_ = model_path;

        std::cout << "YOLOv5 模型加载成功: " << model_path << std::endl;
        return true;

    } catch (const std::exception& e) {
        std::cerr << "模型加载失败: " << e.what() << std::endl;
        model_loaded_ = false;
        return false;
    }
}

cv::Mat YOLOv5Detector::preprocess(const cv::Mat& input_image) {
    if (!model_loaded_) {
        std::cerr << "错误: 模型未加载" << std::endl;
        return cv::Mat();
    }

    int input_width = static_cast<int>(input_node_dims_[3]);
    int input_height = static_cast<int>(input_node_dims_[2]);

    return preprocess_image(input_image, input_width, input_height);
}

std::vector<float> YOLOv5Detector::inference(const cv::Mat& preprocessed_image) {
    if (!model_loaded_ || preprocessed_image.empty()) {
        std::cerr << "错误: 模型未加载或预处理图像为空" << std::endl;
        return {};
    }

    int input_width = static_cast<int>(input_node_dims_[3]);
    int input_height = static_cast<int>(input_node_dims_[2]);

    // 准备输入张量 - 转换为 float16
    std::vector<Ort::Float16_t> input_tensor_values;
    input_tensor_values.reserve(input_width * input_height * 3);

    // 转换为 CHW 格式并同时转换为 float16
    for (int c = 0; c < 3; ++c) {
        for (int h = 0; h < input_height; ++h) {
            for (int w = 0; w < input_width; ++w) {
                float val = preprocessed_image.at<cv::Vec3f>(h, w)[c];
                input_tensor_values.push_back(Ort::Float16_t(val));
            }
        }
    }

    // 创建 float16 输入张量
    std::vector<int64_t> input_shape = {1, 3, input_height, input_width};
    auto memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
    Ort::Value input_tensor = Ort::Value::CreateTensor<Ort::Float16_t>(
        memory_info, input_tensor_values.data(), input_tensor_values.size(),
        input_shape.data(), input_shape.size());

    // 运行推理
    try {
        auto output_tensors = session_->Run(Ort::RunOptions{nullptr},
                                          input_node_names_.data(), &input_tensor, 1,
                                          output_node_names_.data(), 1);

        // 将输出转换为float向量
        auto output_shape = output_tensors[0].GetTensorTypeAndShapeInfo().GetShape();
        Ort::Float16_t* output_data_fp16 = const_cast<Ort::Float16_t*>(output_tensors[0].GetTensorData<Ort::Float16_t>());

        size_t output_size = 1;
        for (auto dim : output_shape) {
            output_size *= dim;
        }

        std::vector<float> output_data(output_size);
        for (size_t i = 0; i < output_size; ++i) {
            output_data[i] = static_cast<float>(output_data_fp16[i]);
        }

        return output_data;

    } catch (const Ort::Exception& e) {
        std::cerr << "推理失败: " << e.what() << std::endl;
        return {};
    }
}

std::vector<Detection> YOLOv5Detector::postprocess(const std::vector<float>& inference_output,
                                                  const cv::Mat& original_image) {
    if (inference_output.empty() || original_image.empty()) {
        return {};
    }

    int input_width = static_cast<int>(input_node_dims_[3]);
    int input_height = static_cast<int>(input_node_dims_[2]);

    // YOLOv5 输出格式: [batch, 25200, 85] (85 = 4 + 1 + 80)
    int num_detections = 25200;  // 典型的YOLOv5输出
    int num_classes = 80;        // COCO数据集类别数

    std::vector<Detection> detections;

    // 计算缩放因子（用于坐标转换）
    float scale = std::min(float(input_width) / original_image.cols, float(input_height) / original_image.rows);
    int x_offset = (input_width - int(original_image.cols * scale)) / 2;
    int y_offset = (input_height - int(original_image.rows * scale)) / 2;

    // 解析检测结果
    for (int i = 0; i < num_detections; ++i) {
        int base_idx = i * 85;
        if (base_idx + 84 >= static_cast<int>(inference_output.size())) break;

        float objectness = inference_output[base_idx + 4];
        if (objectness < confidence_threshold_) continue;

        // 找到最大类别概率
        float max_class_prob = 0.0f;
        int max_class_id = 0;
        for (int j = 0; j < num_classes; ++j) {
            if (inference_output[base_idx + 5 + j] > max_class_prob) {
                max_class_prob = inference_output[base_idx + 5 + j];
                max_class_id = j;
            }
        }

        float confidence = objectness * max_class_prob;
        if (confidence < confidence_threshold_) continue;

        // 转换边界框坐标（从模型输出坐标转换为原图坐标）
        float cx = inference_output[base_idx + 0];
        float cy = inference_output[base_idx + 1];
        float w = inference_output[base_idx + 2];
        float h = inference_output[base_idx + 3];

        // 转换回原图坐标
        float x1 = (cx - w / 2 - x_offset) / scale;
        float y1 = (cy - h / 2 - y_offset) / scale;
        float x2 = (cx + w / 2 - x_offset) / scale;
        float y2 = (cy + h / 2 - y_offset) / scale;

        // 确保坐标在图像范围内
        x1 = std::max(0.0f, std::min(float(original_image.cols), x1));
        y1 = std::max(0.0f, std::min(float(original_image.rows), y1));
        x2 = std::max(0.0f, std::min(float(original_image.cols), x2));
        y2 = std::max(0.0f, std::min(float(original_image.rows), y2));

        Detection det;
        det.box = cv::Rect(int(x1), int(y1), int(x2 - x1), int(y2 - y1));
        det.confidence = confidence;
        det.class_id = max_class_id;
        detections.push_back(det);
    }

    // 应用 NMS
    return apply_nms(detections, nms_threshold_);
}

std::vector<Detection> YOLOv5Detector::detect(const cv::Mat& image) {
    if (image.empty()) {
        std::cerr << "错误: 输入图像为空" << std::endl;
        return {};
    }

    if (!model_loaded_) {
        std::cerr << "错误: 模型未加载" << std::endl;
        return {};
    }

    // 执行完整的检测流程
    cv::Mat preprocessed = preprocess(image);
    if (preprocessed.empty()) {
        return {};
    }

    std::vector<float> inference_output = inference(preprocessed);
    if (inference_output.empty()) {
        return {};
    }

    return postprocess(inference_output, image);
}

cv::Mat YOLOv5Detector::preprocess_image(const cv::Mat& image, int input_width, int input_height) {
    cv::Mat resized, normalized;

    // 保持宽高比的缩放
    float scale = std::min(float(input_width) / image.cols, float(input_height) / image.rows);
    int new_width = int(image.cols * scale);
    int new_height = int(image.rows * scale);

    cv::resize(image, resized, cv::Size(new_width, new_height));

    // 创建填充图像
    cv::Mat padded = cv::Mat::zeros(input_height, input_width, CV_8UC3);
    int x_offset = (input_width - new_width) / 2;
    int y_offset = (input_height - new_height) / 2;
    resized.copyTo(padded(cv::Rect(x_offset, y_offset, new_width, new_height)));

    // 归一化到 [0, 1] 并转换为 RGB
    padded.convertTo(normalized, CV_32F, 1.0 / 255.0);
    cv::cvtColor(normalized, normalized, cv::COLOR_BGR2RGB);

    return normalized;
}

std::vector<Detection> YOLOv5Detector::apply_nms(std::vector<Detection>& detections, float nms_threshold) {
    std::sort(detections.begin(), detections.end(),
              [](const Detection& a, const Detection& b) { return a.confidence > b.confidence; });

    std::vector<Detection> result;
    std::vector<bool> suppressed(detections.size(), false);

    for (size_t i = 0; i < detections.size(); ++i) {
        if (suppressed[i]) continue;

        result.push_back(detections[i]);

        for (size_t j = i + 1; j < detections.size(); ++j) {
            if (suppressed[j]) continue;

            // 计算 IoU
            cv::Rect intersection = detections[i].box & detections[j].box;
            float intersection_area = intersection.area();
            float union_area = detections[i].box.area() + detections[j].box.area() - intersection_area;
            float iou = intersection_area / union_area;

            if (iou > nms_threshold) {
                suppressed[j] = true;
            }
        }
    }

    return result;
}

cv::Mat YOLOv5Detector::draw_results(const cv::Mat& image, const std::vector<Detection>& results) {
    return draw_detections(image, results);
}

// 配置接口实现
void YOLOv5Detector::set_confidence_threshold(float threshold) {
    confidence_threshold_ = threshold;
}

void YOLOv5Detector::set_nms_threshold(float threshold) {
    nms_threshold_ = threshold;
}

float YOLOv5Detector::get_confidence_threshold() const {
    return confidence_threshold_;
}

float YOLOv5Detector::get_nms_threshold() const {
    return nms_threshold_;
}

// 模型信息接口实现
bool YOLOv5Detector::is_model_loaded() const {
    return model_loaded_;
}

std::string YOLOv5Detector::get_model_info() const {
    if (!model_loaded_) {
        return "模型未加载";
    }

    std::string info = "YOLOv5 模型信息:\n";
    info += "模型路径: " + model_path_ + "\n";
    info += "输入尺寸: " + std::to_string(input_node_dims_[3]) + "x" + std::to_string(input_node_dims_[2]) + "\n";
    info += "置信度阈值: " + std::to_string(confidence_threshold_) + "\n";
    info += "NMS阈值: " + std::to_string(nms_threshold_);

    return info;
}

std::vector<Detection> YOLOv5Detector::postprocess_internal(const Ort::Value& output_tensor,
                                                           const cv::Mat& original_image,
                                                           int input_width, int input_height) {
    // 获取输出形状和数据
    auto output_shape = output_tensor.GetTensorTypeAndShapeInfo().GetShape();
    Ort::Float16_t* output_data_fp16 = const_cast<Ort::Float16_t*>(output_tensor.GetTensorData<Ort::Float16_t>());

    // YOLOv5 输出格式: [batch, 25200, 85] (85 = 4 + 1 + 80)
    int num_detections = static_cast<int>(output_shape[1]);
    int num_classes = static_cast<int>(output_shape[2]) - 5;

    std::vector<Detection> detections;

    // 计算缩放因子（用于坐标转换）
    float scale = std::min(float(input_width) / original_image.cols, float(input_height) / original_image.rows);
    int x_offset = (input_width - int(original_image.cols * scale)) / 2;
    int y_offset = (input_height - int(original_image.rows * scale)) / 2;

    // 解析检测结果
    for (int i = 0; i < num_detections; ++i) {
        // 获取当前检测框的数据（转换 float16 到 float32）
        std::vector<float> detection_data(85);
        for (int j = 0; j < 85; ++j) {
            detection_data[j] = static_cast<float>(output_data_fp16[i * 85 + j]);
        }

        float objectness = detection_data[4];
        if (objectness < confidence_threshold_) continue;

        // 找到最大类别概率
        float max_class_prob = 0.0f;
        int max_class_id = 0;
        for (int j = 0; j < num_classes; ++j) {
            if (detection_data[5 + j] > max_class_prob) {
                max_class_prob = detection_data[5 + j];
                max_class_id = j;
            }
        }

        float confidence = objectness * max_class_prob;
        if (confidence < confidence_threshold_) continue;

        // 转换边界框坐标（从模型输出坐标转换为原图坐标）
        float cx = detection_data[0];
        float cy = detection_data[1];
        float w = detection_data[2];
        float h = detection_data[3];

        // 转换回原图坐标
        float x1 = (cx - w / 2 - x_offset) / scale;
        float y1 = (cy - h / 2 - y_offset) / scale;
        float x2 = (cx + w / 2 - x_offset) / scale;
        float y2 = (cy + h / 2 - y_offset) / scale;

        // 确保坐标在图像范围内
        x1 = std::max(0.0f, std::min(float(original_image.cols), x1));
        y1 = std::max(0.0f, std::min(float(original_image.rows), y1));
        x2 = std::max(0.0f, std::min(float(original_image.cols), x2));
        y2 = std::max(0.0f, std::min(float(original_image.rows), y2));

        Detection det;
        det.box = cv::Rect(int(x1), int(y1), int(x2 - x1), int(y2 - y1));
        det.confidence = confidence;
        det.class_id = max_class_id;
        detections.push_back(det);
    }

    // 应用 NMS
    return apply_nms(detections, nms_threshold_);
}

cv::Mat YOLOv5Detector::draw_detections(const cv::Mat& image, const std::vector<Detection>& detections) {
    cv::Mat result_image = image.clone();

    for (const auto& det : detections) {
        // 绘制边界框
        cv::rectangle(result_image, det.box, cv::Scalar(0, 255, 0), 2);

        // 绘制标签
        std::string label = get_class_name(det.class_id) + ": " +
                           std::to_string(int(det.confidence * 100)) + "%";

        int baseline;
        cv::Size text_size = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseline);

        cv::Point text_origin(det.box.x, det.box.y - 5);
        cv::rectangle(result_image,
                     cv::Point(text_origin.x, text_origin.y - text_size.height),
                     cv::Point(text_origin.x + text_size.width, text_origin.y + baseline),
                     cv::Scalar(0, 255, 0), -1);

        cv::putText(result_image, label, text_origin, cv::FONT_HERSHEY_SIMPLEX, 0.5,
                   cv::Scalar(0, 0, 0), 1);
    }

    return result_image;
}

std::string YOLOv5Detector::get_class_name(int class_id) const {
    if (class_id >= 0 && class_id < static_cast<int>(class_names_.size())) {
        return class_names_[class_id];
    }
    return "unknown";
}
