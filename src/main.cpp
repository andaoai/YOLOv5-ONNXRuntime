#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <iomanip>
#include "yolov5.h"

int main() {
    std::cout << "YOLOv5 ONNX 推理测试" << std::endl;

    // 模型和图片路径
    const std::string model_path = "/workspaces/YOLOv5-ONNXRuntime/assets/models/yolov5n.onnx";
    const std::string image_path = "/workspaces/YOLOv5-ONNXRuntime/assets/images/bus.jpg";

    try {
        // 1. 加载图像
        cv::Mat image = cv::imread(image_path);
        if (image.empty()) {
            std::cerr << "错误: 无法加载图像 " << image_path << std::endl;
            return -1;
        }
        std::cout << "图像: " << image.cols << "x" << image.rows << std::endl;

        // 2. 创建 YOLOv5 检测器
        YOLOv5Detector detector(model_path, 0.5f, 0.4f);

        // 3. 执行检测
        std::vector<Detection> detections = detector.detect(image);

        std::cout << "检测到 " << detections.size() << " 个目标:" << std::endl;

        // 4. 输出检测结果
        for (const auto& det : detections) {
            std::cout << "  - " << detector.get_class_name(det.class_id)
                     << " (置信度: " << std::fixed << std::setprecision(2) << det.confidence << ")"
                     << " 位置: [" << det.box.x << ", " << det.box.y << ", "
                     << det.box.width << ", " << det.box.height << "]" << std::endl;
        }

        // 5. 绘制结果并保存
        cv::Mat result_image = detector.draw_detections(image, detections);

        // 保存结果
        std::string output_path = "../assets/images/bus_result.jpg";
        cv::imwrite(output_path, result_image);
        std::cout << "结果已保存到: " << output_path << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return -1;
    }

    std::cout << "YOLOv5 推理测试完成！" << std::endl;
    return 0;
}
