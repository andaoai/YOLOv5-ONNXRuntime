#include <opencv2/opencv.hpp>
#include <onnxruntime_cxx_api.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <string>
#include <iomanip>
#include <limits>

// YOLOv5 检测结果结构
struct Detection {
    cv::Rect box;
    float confidence;
    int class_id;
};

// COCO 数据集类别名称
const std::vector<std::string> class_names = {
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

// 图像预处理函数
cv::Mat preprocess_image(const cv::Mat& image, int input_width, int input_height) {
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

// NMS (非极大值抑制)
std::vector<Detection> apply_nms(std::vector<Detection>& detections, float nms_threshold) {
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

int main() {
    std::cout << "YOLOv5 ONNX 推理测试" << std::endl;

    // 模型和图片路径
    const std::string model_path = "../assets/models/yolov5n.onnx";
    const std::string image_path = "../assets/images/bus.jpg";

    try {
        // 1. 加载图像
        cv::Mat image = cv::imread(image_path);
        if (image.empty()) {
            std::cerr << "错误: 无法加载图像 " << image_path << std::endl;
            return -1;
        }
        std::cout << "图像: " << image.cols << "x" << image.rows << std::endl;

        // 2. 创建 ONNX Runtime 环境
        Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "YOLOv5");
        Ort::SessionOptions session_options;
        session_options.SetIntraOpNumThreads(4);
        session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);

        // 3. 加载模型
        Ort::Session session(env, model_path.c_str(), session_options);

        // 4. 获取模型输入输出信息
        Ort::AllocatorWithDefaultOptions allocator;

        // 使用已知的输入输出名称（从简单测试中获得）
        std::vector<const char*> input_node_names = {"images"};
        std::vector<const char*> output_node_names = {"output0"};

        // 获取输入形状
        Ort::TypeInfo input_type_info = session.GetInputTypeInfo(0);
        auto input_tensor_info = input_type_info.GetTensorTypeAndShapeInfo();
        std::vector<int64_t> input_node_dims = input_tensor_info.GetShape();



        // 5. 图像预处理
        int input_width = static_cast<int>(input_node_dims[3]);
        int input_height = static_cast<int>(input_node_dims[2]);


        cv::Mat preprocessed = preprocess_image(image, input_width, input_height);

        // 6. 准备输入张量 - 转换为 float16
        std::vector<Ort::Float16_t> input_tensor_values;
        input_tensor_values.reserve(input_width * input_height * 3);

        // 转换为 CHW 格式并同时转换为 float16
        for (int c = 0; c < 3; ++c) {
            for (int h = 0; h < input_height; ++h) {
                for (int w = 0; w < input_width; ++w) {
                    float val = preprocessed.at<cv::Vec3f>(h, w)[c];
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

        // 7. 运行推理

        try {
            auto output_tensors = session.Run(Ort::RunOptions{nullptr},
                                            input_node_names.data(), &input_tensor, 1,
                                            output_node_names.data(), 1);
            std::cout << "推理完成" << std::endl;

            // 8. 后处理 - 解析检测结果
            auto output_shape = output_tensors[0].GetTensorTypeAndShapeInfo().GetShape();
            Ort::Float16_t* output_data_fp16 = output_tensors[0].GetTensorMutableData<Ort::Float16_t>();

            // YOLOv5 输出格式: [batch, 25200, 85] (85 = 4 + 1 + 80)
            int num_detections = static_cast<int>(output_shape[1]);
            int num_classes = static_cast<int>(output_shape[2]) - 5;

            std::vector<Detection> detections;
            float confidence_threshold = 0.5f;
            float nms_threshold = 0.4f;

            // 计算缩放因子（用于坐标转换）
            float scale = std::min(float(input_width) / image.cols, float(input_height) / image.rows);
            int x_offset = (input_width - int(image.cols * scale)) / 2;
            int y_offset = (input_height - int(image.rows * scale)) / 2;

            // 解析检测结果
            for (int i = 0; i < num_detections; ++i) {
                // 获取当前检测框的数据（转换 float16 到 float32）
                std::vector<float> detection_data(85);
                for (int j = 0; j < 85; ++j) {
                    detection_data[j] = static_cast<float>(output_data_fp16[i * 85 + j]);
                }

                float objectness = detection_data[4];
                if (objectness < confidence_threshold) continue;

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
                if (confidence < confidence_threshold) continue;

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
                x1 = std::max(0.0f, std::min(float(image.cols), x1));
                y1 = std::max(0.0f, std::min(float(image.rows), y1));
                x2 = std::max(0.0f, std::min(float(image.cols), x2));
                y2 = std::max(0.0f, std::min(float(image.rows), y2));

                Detection det;
                det.box = cv::Rect(int(x1), int(y1), int(x2 - x1), int(y2 - y1));
                det.confidence = confidence;
                det.class_id = max_class_id;
                detections.push_back(det);
            }

            // 9. 应用 NMS
            std::vector<Detection> final_detections = apply_nms(detections, nms_threshold);

            std::cout << "检测到 " << final_detections.size() << " 个目标:" << std::endl;

            // 10. 输出检测结果
            for (const auto& det : final_detections) {
                std::cout << "  - " << class_names[det.class_id]
                         << " (置信度: " << std::fixed << std::setprecision(2) << det.confidence << ")"
                         << " 位置: [" << det.box.x << ", " << det.box.y << ", "
                         << det.box.width << ", " << det.box.height << "]" << std::endl;
            }

            // 11. 绘制结果并保存
            cv::Mat result_image = image.clone();
            for (const auto& det : final_detections) {
                // 绘制边界框
                cv::rectangle(result_image, det.box, cv::Scalar(0, 255, 0), 2);

                // 绘制标签
                std::string label = class_names[det.class_id] + ": " +
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

            // 保存结果
            std::string output_path = "../assets/images/bus_result.jpg";
            cv::imwrite(output_path, result_image);
            std::cout << "结果已保存到: " << output_path << std::endl;

        } catch (const Ort::Exception& e) {
            std::cerr << "推理失败: " << e.what() << std::endl;
            return -1;
        }



    } catch (const Ort::Exception& e) {
        std::cerr << "ONNX Runtime 错误: " << e.what() << std::endl;
        return -1;
    } catch (const cv::Exception& e) {
        std::cerr << "OpenCV 错误: " << e.what() << std::endl;
        return -1;
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return -1;
    }

    std::cout << "YOLOv5 推理测试完成！" << std::endl;
    return 0;
}
