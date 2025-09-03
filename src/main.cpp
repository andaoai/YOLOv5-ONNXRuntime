#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <iomanip>
#include <chrono>
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

        // 3. 使用分步执行并统计时间
        std::cout << "\n=== YOLOv5 分步执行时间统计 ===" << std::endl;

        // 3.1 预处理阶段
        auto start_preprocess = std::chrono::high_resolution_clock::now();
        cv::Mat preprocessed = detector.preprocess(image);
        auto end_preprocess = std::chrono::high_resolution_clock::now();
        auto preprocess_time = std::chrono::duration_cast<std::chrono::microseconds>(end_preprocess - start_preprocess);

        if (preprocessed.empty()) {
            std::cerr << "错误: 预处理失败" << std::endl;
            return -1;
        }

        std::cout << "预处理时间: " << preprocess_time.count() / 1000.0 << " ms" << std::endl;

        // 3.2 模型推理阶段
        auto start_inference = std::chrono::high_resolution_clock::now();
        std::vector<float> inference_output = detector.inference(preprocessed);
        auto end_inference = std::chrono::high_resolution_clock::now();
        auto inference_time = std::chrono::duration_cast<std::chrono::microseconds>(end_inference - start_inference);

        if (inference_output.empty()) {
            std::cerr << "错误: 推理失败" << std::endl;
            return -1;
        }

        std::cout << "模型推理时间: " << inference_time.count() / 1000.0 << " ms" << std::endl;

        // 3.3 后处理阶段
        auto start_postprocess = std::chrono::high_resolution_clock::now();
        std::vector<Detection> detections = detector.postprocess(inference_output, image);
        auto end_postprocess = std::chrono::high_resolution_clock::now();
        auto postprocess_time = std::chrono::duration_cast<std::chrono::microseconds>(end_postprocess - start_postprocess);

        std::cout << "后处理时间: " << postprocess_time.count() / 1000.0 << " ms" << std::endl;

        // 3.4 总时间统计
        auto total_time = preprocess_time + inference_time + postprocess_time;
        std::cout << "总处理时间: " << total_time.count() / 1000.0 << " ms" << std::endl;

        std::cout << "\n=== 时间分布 ===" << std::endl;
        std::cout << "预处理占比: " << std::fixed << std::setprecision(1)
                 << (double)preprocess_time.count() / total_time.count() * 100 << "%" << std::endl;
        std::cout << "推理占比: " << std::fixed << std::setprecision(1)
                 << (double)inference_time.count() / total_time.count() * 100 << "%" << std::endl;
        std::cout << "后处理占比: " << std::fixed << std::setprecision(1)
                 << (double)postprocess_time.count() / total_time.count() * 100 << "%" << std::endl;

        std::cout << "\n检测到 " << detections.size() << " 个目标:" << std::endl;

        // 4. 输出检测结果
        for (const auto& det : detections) {
            std::cout << "  - " << detector.get_class_name(det.class_id)
                     << " (置信度: " << std::fixed << std::setprecision(2) << det.confidence << ")"
                     << " 位置: [" << det.box.x << ", " << det.box.y << ", "
                     << det.box.width << ", " << det.box.height << "]" << std::endl;
        }

        // 5. 绘制结果并保存
        auto start_draw = std::chrono::high_resolution_clock::now();
        cv::Mat result_image = detector.draw_detections(image, detections);
        auto end_draw = std::chrono::high_resolution_clock::now();
        auto draw_time = std::chrono::duration_cast<std::chrono::microseconds>(end_draw - start_draw);

        std::cout << "\n绘制结果时间: " << draw_time.count() / 1000.0 << " ms" << std::endl;

        // 保存结果
        std::string output_path = "/workspaces/YOLOv5-ONNXRuntime/assets/images/bus_result.jpg";
        cv::imwrite(output_path, result_image);
        std::cout << "结果已保存到: " << output_path << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return -1;
    }

    std::cout << "YOLOv5 推理测试完成！" << std::endl;
    return 0;
}
