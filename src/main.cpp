#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>

int main() {
    // 读取图片（使用相对于可执行文件的路径）
    cv::Mat image = cv::imread("assets/images/bus.jpg");

    if (image.empty()) {
        std::cerr << "Error: Could not load image 'assets/images/bus.jpg'" << std::endl;
        std::cerr << "Please make sure the image file exists in the assets/images/ directory" << std::endl;
        return -1;
    }

    // 打印图片的 shape (高度, 宽度, 通道数)
    std::cout << "Image shape: " << image.rows << " x " << image.cols << " x " << image.channels() << std::endl;
    std::cout << "Height: " << image.rows << std::endl;
    std::cout << "Width: " << image.cols << std::endl;
    std::cout << "Channels: " << image.channels() << std::endl;
    std::cout << "andaoai-x" << image.cols << std::endl;
    return 0;
}
