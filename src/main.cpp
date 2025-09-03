#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <fmt/format.h>
#include <fmt/color.h>
#include "yolov5.h"

int main() {
    fmt::print(fmt::fg(fmt::color::cyan) | fmt::emphasis::bold,
               "🚀 YOLOv5 ONNX 推理测试\n\n");

    // 模型和图片路径
    const std::string model_path = "/workspaces/YOLOv5-ONNXRuntime/assets/models/yolov5n.onnx";
    const std::string image_path = "/workspaces/YOLOv5-ONNXRuntime/assets/images/bus.jpg";

    try {
        // 1. 加载图像
        cv::Mat image = cv::imread(image_path);
        if (image.empty()) {
            fmt::print(fmt::fg(fmt::color::red), "❌ 错误: 无法加载图像 {}\n", image_path);
            return -1;
        }
        fmt::print("📷 图像尺寸: {}x{}\n", image.cols, image.rows);

        // 2. 创建 YOLOv5 检测器
        YOLOv5Detector detector(model_path, 0.5f, 0.4f);

        // 3. 使用分步执行并统计时间
        fmt::print("\n");
        fmt::print(fmt::fg(fmt::color::yellow) | fmt::emphasis::bold,
                   "⏱️  YOLOv5 分步执行时间统计\n");
        fmt::print("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");

        // 3.1 预处理阶段
        auto start_preprocess = std::chrono::high_resolution_clock::now();
        cv::Mat preprocessed = detector.preprocess(image);
        auto end_preprocess = std::chrono::high_resolution_clock::now();
        auto preprocess_time = std::chrono::duration_cast<std::chrono::microseconds>(end_preprocess - start_preprocess);

        if (preprocessed.empty()) {
            fmt::print(fmt::fg(fmt::color::red), "❌ 错误: 预处理失败\n");
            return -1;
        }

        fmt::print("🔄 预处理时间: {:.1f} ms\n", preprocess_time.count() / 1000.0);

        // 3.2 模型推理阶段
        auto start_inference = std::chrono::high_resolution_clock::now();
        std::vector<float> inference_output = detector.inference(preprocessed);
        auto end_inference = std::chrono::high_resolution_clock::now();
        auto inference_time = std::chrono::duration_cast<std::chrono::microseconds>(end_inference - start_inference);

        if (inference_output.empty()) {
            fmt::print(fmt::fg(fmt::color::red), "❌ 错误: 推理失败\n");
            return -1;
        }

        fmt::print("🧠 模型推理时间: {:.1f} ms\n", inference_time.count() / 1000.0);

        // 3.3 后处理阶段
        auto start_postprocess = std::chrono::high_resolution_clock::now();
        std::vector<Detection> detections = detector.postprocess(inference_output, image);
        auto end_postprocess = std::chrono::high_resolution_clock::now();
        auto postprocess_time = std::chrono::duration_cast<std::chrono::microseconds>(end_postprocess - start_postprocess);

        fmt::print("⚙️  后处理时间: {:.1f} ms\n", postprocess_time.count() / 1000.0);

        // 3.4 总时间统计
        auto total_time = preprocess_time + inference_time + postprocess_time;
        fmt::print(fmt::fg(fmt::color::green) | fmt::emphasis::bold,
                   "⏰ 总处理时间: {:.1f} ms\n", total_time.count() / 1000.0);

        // 时间分布表格
        fmt::print("\n");
        fmt::print(fmt::fg(fmt::color::magenta) | fmt::emphasis::bold, "📊 时间分布统计\n");
        fmt::print("┌─────────────┬──────────┬─────────┬──────────────────────┐\n");
        fmt::print("│ 阶段        │ 时间(ms) │ 占比(%) │ 进度条               │\n");
        fmt::print("├─────────────┼──────────┼─────────┼──────────────────────┤\n");

        double preprocess_pct = (double)preprocess_time.count() / total_time.count() * 100;
        double inference_pct = (double)inference_time.count() / total_time.count() * 100;
        double postprocess_pct = (double)postprocess_time.count() / total_time.count() * 100;

        auto make_progress_bar = [](double pct) -> std::string {
            int filled = static_cast<int>(pct / 5);  // 每5%一个字符，总共20个字符
            return std::string(filled, '=') + std::string(20 - filled, ' ');
        };

        fmt::print("│ 预处理      │ {:8.1f} │ {:6.1f}  │ {} │\n",
                   preprocess_time.count() / 1000.0, preprocess_pct, make_progress_bar(preprocess_pct));
        fmt::print("│ 模型推理    │ {:8.1f} │ {:6.1f}  │ {} │\n",
                   inference_time.count() / 1000.0, inference_pct, make_progress_bar(inference_pct));
        fmt::print("│ 后处理      │ {:8.1f} │ {:6.1f}  │ {} │\n",
                   postprocess_time.count() / 1000.0, postprocess_pct, make_progress_bar(postprocess_pct));
        fmt::print("└─────────────┴──────────┴─────────┴──────────────────────┘\n");

        fmt::print("\n🎯 检测到 {} 个目标:\n", detections.size());

        // 4. 输出检测结果
        for (size_t i = 0; i < detections.size(); ++i) {
            const auto& det = detections[i];
            fmt::print("  {} {} (置信度: {:.1f}%) 位置: [{}, {}, {}, {}]\n",
                       i + 1 < 10 ? fmt::format(" {}", i + 1) : fmt::format("{}", i + 1),
                       detector.get_class_name(det.class_id),
                       det.confidence * 100,
                       det.box.x, det.box.y, det.box.width, det.box.height);
        }

        // 5. 绘制结果并保存
        auto start_draw = std::chrono::high_resolution_clock::now();
        cv::Mat result_image = detector.draw_detections(image, detections);
        auto end_draw = std::chrono::high_resolution_clock::now();
        auto draw_time = std::chrono::duration_cast<std::chrono::microseconds>(end_draw - start_draw);

        fmt::print("\n🎨 绘制结果时间: {:.1f} ms\n", draw_time.count() / 1000.0);

        // 保存结果
        std::string output_path = "/workspaces/YOLOv5-ONNXRuntime/assets/images/bus_result.jpg";
        cv::imwrite(output_path, result_image);
        fmt::print(fmt::fg(fmt::color::green), "💾 结果已保存到: {}\n", output_path);

    } catch (const std::exception& e) {
        fmt::print(fmt::fg(fmt::color::red) | fmt::emphasis::bold,
                   "💥 错误: {}\n", e.what());
        return -1;
    }

    fmt::print(fmt::fg(fmt::color::green) | fmt::emphasis::bold,
               "\n✅ YOLOv5 推理测试完成！\n");
    return 0;
}
