#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <fmt/format.h>
#include <fmt/color.h>
#include <numeric>
#include <algorithm>
#include "yolov5.h"

// 批量推理测试函数
void benchmark_inference(YOLOv5Detector& detector, const cv::Mat& image, int iterations = 100) {
    fmt::print(fmt::fg(fmt::color::cyan) | fmt::emphasis::bold,
               "\n🔥 开始批量推理性能测试 ({}次)\n", iterations);
    fmt::print("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");

    std::vector<double> preprocess_times;
    std::vector<double> inference_times;
    std::vector<double> postprocess_times;
    preprocess_times.reserve(iterations);
    inference_times.reserve(iterations);
    postprocess_times.reserve(iterations);

    fmt::print("🚀 正在进行 {} 次完整推理测试...\n", iterations);

    // 进行批量推理测试
    for (int i = 0; i < iterations; ++i) {
        // 预处理阶段计时
        auto start_preprocess = std::chrono::high_resolution_clock::now();
        cv::Mat preprocessed = detector.preprocess(image);
        auto end_preprocess = std::chrono::high_resolution_clock::now();

        if (preprocessed.empty()) {
            fmt::print(fmt::fg(fmt::color::red), "❌ 错误: 第{}次预处理失败\n", i + 1);
            continue;
        }

        // 推理阶段计时
        auto start_inference = std::chrono::high_resolution_clock::now();
        std::vector<float> inference_output = detector.inference(preprocessed);
        auto end_inference = std::chrono::high_resolution_clock::now();

        if (inference_output.empty()) {
            fmt::print(fmt::fg(fmt::color::red), "❌ 错误: 第{}次推理失败\n", i + 1);
            continue;
        }

        // 后处理阶段计时
        auto start_postprocess = std::chrono::high_resolution_clock::now();
        std::vector<Detection> detections = detector.postprocess(inference_output, image);
        auto end_postprocess = std::chrono::high_resolution_clock::now();

        // 记录时间（转换为毫秒）
        auto preprocess_time = std::chrono::duration_cast<std::chrono::microseconds>(end_preprocess - start_preprocess);
        auto inference_time = std::chrono::duration_cast<std::chrono::microseconds>(end_inference - start_inference);
        auto postprocess_time = std::chrono::duration_cast<std::chrono::microseconds>(end_postprocess - start_postprocess);

        preprocess_times.push_back(preprocess_time.count() / 1000.0);
        inference_times.push_back(inference_time.count() / 1000.0);
        postprocess_times.push_back(postprocess_time.count() / 1000.0);

        // 每10次显示进度
        if ((i + 1) % 10 == 0) {
            fmt::print("  ✓ 完成 {}/{} 次推理\n", i + 1, iterations);
        }
    }

    // 统计分析
    if (inference_times.empty()) {
        fmt::print(fmt::fg(fmt::color::red), "❌ 没有成功的推理结果\n");
        return;
    }

    // 计算统计数据
    std::sort(preprocess_times.begin(), preprocess_times.end());
    std::sort(inference_times.begin(), inference_times.end());
    std::sort(postprocess_times.begin(), postprocess_times.end());

    double pre_mean = std::accumulate(preprocess_times.begin(), preprocess_times.end(), 0.0) / preprocess_times.size();
    double pre_median = preprocess_times[preprocess_times.size() / 2];
    double pre_min = preprocess_times.front();
    double pre_max = preprocess_times.back();
    double pre_p95 = preprocess_times[static_cast<size_t>(preprocess_times.size() * 0.95)];

    double inf_mean = std::accumulate(inference_times.begin(), inference_times.end(), 0.0) / inference_times.size();
    double inf_median = inference_times[inference_times.size() / 2];
    double inf_min = inference_times.front();
    double inf_max = inference_times.back();
    double inf_p95 = inference_times[static_cast<size_t>(inference_times.size() * 0.95)];

    double post_mean = std::accumulate(postprocess_times.begin(), postprocess_times.end(), 0.0) / postprocess_times.size();
    double post_median = postprocess_times[postprocess_times.size() / 2];
    double post_min = postprocess_times.front();
    double post_max = postprocess_times.back();
    double post_p95 = postprocess_times[static_cast<size_t>(postprocess_times.size() * 0.95)];

    // 显示详细统计结果
    fmt::print("\n");
    fmt::print(fmt::fg(fmt::color::green) | fmt::emphasis::bold, "📊 推理性能统计结果\n");
    fmt::print("┌─────────────────┬──────────┬──────────┬──────────┬──────────┬──────────┐\n");
    fmt::print("│ 阶段            │ 平均值   │ 中位数   │ 最小值   │ 最大值   │ P95      │\n");
    fmt::print("├─────────────────┼──────────┼──────────┼──────────┼──────────┼──────────┤\n");
    fmt::print("│ 预处理 (ms)     │ {:8.2f} │ {:8.2f} │ {:8.2f} │ {:8.2f} │ {:8.2f} │\n",
               pre_mean, pre_median, pre_min, pre_max, pre_p95);
    fmt::print("│ 模型推理 (ms)   │ {:8.2f} │ {:8.2f} │ {:8.2f} │ {:8.2f} │ {:8.2f} │\n",
               inf_mean, inf_median, inf_min, inf_max, inf_p95);
    fmt::print("│ 后处理 (ms)     │ {:8.2f} │ {:8.2f} │ {:8.2f} │ {:8.2f} │ {:8.2f} │\n",
               post_mean, post_median, post_min, post_max, post_p95);
    fmt::print("└─────────────────┴──────────┴──────────┴──────────┴──────────┴──────────┘\n");

    // 计算FPS
    double total_time_per_frame = pre_mean + inf_mean + post_mean;
    double fps = 1000.0 / total_time_per_frame;

    fmt::print("\n");
    fmt::print(fmt::fg(fmt::color::magenta) | fmt::emphasis::bold, "🎯 性能指标\n");
    fmt::print("  • 平均单帧处理时间: {:.2f} ms (预处理: {:.2f} + 推理: {:.2f} + 后处理: {:.2f})\n",
               total_time_per_frame, pre_mean, inf_mean, post_mean);
    fmt::print("  • 理论最大FPS: {:.1f}\n", fps);
    fmt::print("  • 推理稳定性: {:.1f}% (基于P95/平均值)\n", (inf_p95 / inf_mean) * 100);

    // 时间分布百分比
    double pre_pct = (pre_mean / total_time_per_frame) * 100;
    double inf_pct = (inf_mean / total_time_per_frame) * 100;
    double post_pct = (post_mean / total_time_per_frame) * 100;

    fmt::print("  • 时间分布: 预处理 {:.1f}% | 推理 {:.1f}% | 后处理 {:.1f}%\n",
               pre_pct, inf_pct, post_pct);
}

int main() {
    fmt::print(fmt::fg(fmt::color::cyan) | fmt::emphasis::bold,
               "🚀 YOLOv5 ONNX 推理性能测试\n\n");

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

        // 3. 首次单次推理测试（用于验证功能和预热）
        fmt::print("\n");
        fmt::print(fmt::fg(fmt::color::yellow) | fmt::emphasis::bold,
                   "⏱️  首次推理测试（预热）\n");
        fmt::print("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");

        // 3.1 预处理阶段（预热）
        auto start_preprocess = std::chrono::high_resolution_clock::now();
        cv::Mat preprocessed = detector.preprocess(image);
        auto end_preprocess = std::chrono::high_resolution_clock::now();
        auto preprocess_time = std::chrono::duration_cast<std::chrono::microseconds>(end_preprocess - start_preprocess);

        if (preprocessed.empty()) {
            fmt::print(fmt::fg(fmt::color::red), "❌ 错误: 预处理失败\n");
            return -1;
        }

        fmt::print("🔄 预处理时间: {:.1f} ms\n", preprocess_time.count() / 1000.0);

        // 3.2 模型推理阶段（预热）
        auto start_inference = std::chrono::high_resolution_clock::now();
        std::vector<float> inference_output = detector.inference(preprocessed);
        auto end_inference = std::chrono::high_resolution_clock::now();
        auto inference_time = std::chrono::duration_cast<std::chrono::microseconds>(end_inference - start_inference);

        if (inference_output.empty()) {
            fmt::print(fmt::fg(fmt::color::red), "❌ 错误: 推理失败\n");
            return -1;
        }

        fmt::print("🧠 模型推理时间: {:.1f} ms\n", inference_time.count() / 1000.0);

        // 3.3 后处理阶段（预热）
        auto start_postprocess = std::chrono::high_resolution_clock::now();
        std::vector<Detection> detections = detector.postprocess(inference_output, image);
        auto end_postprocess = std::chrono::high_resolution_clock::now();
        auto postprocess_time = std::chrono::duration_cast<std::chrono::microseconds>(end_postprocess - start_postprocess);

        fmt::print("⚙️  后处理时间: {:.1f} ms\n", postprocess_time.count() / 1000.0);

        // 3.4 总时间统计（预热）
        auto total_time = preprocess_time + inference_time + postprocess_time;
        fmt::print(fmt::fg(fmt::color::green) | fmt::emphasis::bold,
                   "⏰ 预热总处理时间: {:.1f} ms\n", total_time.count() / 1000.0);

        fmt::print("\n🎯 预热检测到 {} 个目标\n", detections.size());

        // 4. 显示预热检测结果（简化版）
        if (!detections.empty()) {
            fmt::print("  检测到的目标类型: ");
            for (size_t i = 0; i < std::min(detections.size(), size_t(3)); ++i) {
                const auto& det = detections[i];
                fmt::print("{} ({:.1f}%)", detector.get_class_name(det.class_id), det.confidence * 100);
                if (i < std::min(detections.size(), size_t(3)) - 1) fmt::print(", ");
            }
            if (detections.size() > 3) {
                fmt::print(" 等{}个目标", detections.size());
            }
            fmt::print("\n");
        }

        // 5. 执行批量推理性能测试
        benchmark_inference(detector, image, 100);

        // 6. 绘制结果并保存（使用预热的检测结果）
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
               "\n✅ YOLOv5 批量推理性能测试完成！\n");
    return 0;
}
