#include <opencv2/opencv.hpp>
#include <onnxruntime_cxx_api.h>
#include <iostream>

int main() {
    std::cout << "OpenCV 和 ONNX Runtime 集成测试" << std::endl;

    // 测试 OpenCV 版本信息
    std::cout << "OpenCV 版本: " << CV_VERSION << std::endl;

    // 创建一个彩色图像
    cv::Mat image = cv::Mat::zeros(300, 400, CV_8UC3);
    std::cout << "创建了一个 " << image.rows << "x" << image.cols << " 的图像" << std::endl;
    std::cout << "通道数: " << image.channels() << std::endl;

    // 测试绘图功能
    cv::rectangle(image, cv::Point(50, 50), cv::Point(200, 150), cv::Scalar(0, 255, 0), 2);
    cv::putText(image, "Hello OpenCV!", cv::Point(60, 100), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);
    std::cout << "绘制矩形和文字成功" << std::endl;

    // 测试颜色转换
    cv::Mat gray;
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    std::cout << "颜色转换成功，灰度图像大小: " << gray.rows << "x" << gray.cols << std::endl;

    // 测试图像缩放
    cv::Mat resized;
    cv::resize(image, resized, cv::Size(200, 150));
    std::cout << "图像缩放成功，新大小: " << resized.rows << "x" << resized.cols << std::endl;

    // 测试基本的矩阵操作
    cv::Mat copy = image.clone();
    std::cout << "矩阵克隆成功" << std::endl;

    std::cout << "OpenCV 功能测试完成！" << std::endl;

    // 测试 ONNX Runtime
    std::cout << "\n=== ONNX Runtime 测试 ===" << std::endl;
    try {
        // 创建 ONNX Runtime 环境
        Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "test");
        std::cout << "ONNX Runtime 环境创建成功" << std::endl;

        // 获取可用的执行提供者
        std::vector<std::string> providers = Ort::GetAvailableProviders();
        std::cout << "可用的执行提供者: ";
        for (const auto& provider : providers) {
            std::cout << provider << " ";
        }
        std::cout << std::endl;

        // 创建会话选项
        Ort::SessionOptions session_options;
        session_options.SetIntraOpNumThreads(1);
        session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);
        std::cout << "ONNX Runtime 会话选项配置成功" << std::endl;

    } catch (const Ort::Exception& e) {
        std::cerr << "ONNX Runtime 错误: " << e.what() << std::endl;
        return -1;
    }

    std::cout << "ONNX Runtime 测试完成！" << std::endl;
    std::cout << "\n所有测试完成！OpenCV 和 ONNX Runtime 都正常工作。" << std::endl;
    return 0;
}
