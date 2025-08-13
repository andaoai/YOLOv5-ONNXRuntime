cmake_minimum_required(VERSION 3.15)
project(opencv_image_reader VERSION 1.0.0 LANGUAGES CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含 Conan 生成的文件
include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake OPTIONAL)

# 设置项目目录变量
set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(PROJECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(PROJECT_THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party)
set(PROJECT_ASSETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets)

# 设置 OpenCV 构建选项
set(BUILD_EXAMPLES OFF CACHE BOOL "Build examples")
set(BUILD_TESTS OFF CACHE BOOL "Build tests")
set(BUILD_PERF_TESTS OFF CACHE BOOL "Build performance tests")
set(BUILD_DOCS OFF CACHE BOOL "Build documentation")
set(BUILD_opencv_apps OFF CACHE BOOL "Build OpenCV apps")
set(BUILD_opencv_python2 OFF CACHE BOOL "Build Python2 bindings")
set(BUILD_opencv_python3 OFF CACHE BOOL "Build Python3 bindings")
set(BUILD_opencv_java OFF CACHE BOOL "Build Java bindings")
set(WITH_CUDA OFF CACHE BOOL "Include CUDA support")
set(WITH_OPENCL OFF CACHE BOOL "Include OpenCL support")
set(WITH_IPP OFF CACHE BOOL "Include Intel IPP support")
set(WITH_TBB OFF CACHE BOOL "Include Intel TBB support")

# 检查 Git 子模块是否已初始化
if(NOT EXISTS "${PROJECT_THIRD_PARTY_DIR}/opencv/CMakeLists.txt")
    message(FATAL_ERROR "OpenCV submodule not found. Please run: git submodule update --init --recursive")
endif()

# 设置 OpenCV 构建选项 - 极简配置，只构建必要模块
message(STATUS "Configuring OpenCV with minimal build options for faster compilation...")

# 禁用示例、测试和文档
set(BUILD_EXAMPLES OFF CACHE BOOL "Build examples")
set(BUILD_TESTS OFF CACHE BOOL "Build tests")
set(BUILD_PERF_TESTS OFF CACHE BOOL "Build performance tests")
set(BUILD_DOCS OFF CACHE BOOL "Build documentation")
set(BUILD_opencv_apps OFF CACHE BOOL "Build OpenCV apps")

# 禁用语言绑定
set(BUILD_opencv_python2 OFF CACHE BOOL "Build Python2 bindings")
set(BUILD_opencv_python3 OFF CACHE BOOL "Build Python3 bindings")
set(BUILD_opencv_java OFF CACHE BOOL "Build Java bindings")

# 禁用硬件加速和第三方库支持
set(WITH_CUDA OFF CACHE BOOL "Include CUDA support")
set(WITH_OPENCL OFF CACHE BOOL "Include OpenCL support")
set(WITH_IPP OFF CACHE BOOL "Include Intel IPP support")
set(WITH_TBB OFF CACHE BOOL "Include Intel TBB support")
set(WITH_EIGEN OFF CACHE BOOL "Include Eigen support")
set(WITH_LAPACK OFF CACHE BOOL "Include LAPACK support")

# 禁用视频和媒体相关功能
set(WITH_FFMPEG OFF CACHE BOOL "Include FFMPEG support")
set(WITH_GSTREAMER OFF CACHE BOOL "Include GStreamer support")
set(WITH_V4L OFF CACHE BOOL "Include Video4Linux support")
set(WITH_DSHOW OFF CACHE BOOL "Include DirectShow support")

# 只构建最核心的模块
set(BUILD_opencv_calib3d OFF CACHE BOOL "Build calib3d module")
set(BUILD_opencv_dnn OFF CACHE BOOL "Build DNN module")
set(BUILD_opencv_features2d OFF CACHE BOOL "Build features2d module")
set(BUILD_opencv_flann OFF CACHE BOOL "Build flann module")
set(BUILD_opencv_gapi OFF CACHE BOOL "Build G-API module")
set(BUILD_opencv_ml OFF CACHE BOOL "Build ML module")
set(BUILD_opencv_objdetect OFF CACHE BOOL "Build objdetect module")
set(BUILD_opencv_photo OFF CACHE BOOL "Build photo module")
set(BUILD_opencv_stitching OFF CACHE BOOL "Build stitching module")
set(BUILD_opencv_video OFF CACHE BOOL "Build video module")
set(BUILD_opencv_videoio OFF CACHE BOOL "Build videoio module")

# 添加 OpenCV 源码作为子项目
add_subdirectory(${PROJECT_THIRD_PARTY_DIR}/opencv)

# 创建可执行文件
add_executable(main ${PROJECT_SOURCE_DIR}/main.cpp)

# 确保 main 在 OpenCV 模块构建完成后再构建
add_dependencies(main opencv_core opencv_imgproc opencv_imgcodecs opencv_highgui)

# 链接 OpenCV 库（只需要核心模块）
target_link_libraries(main
    opencv_core
    opencv_imgproc
    opencv_imgcodecs
    opencv_highgui
)

# 包含头文件目录
target_include_directories(main PRIVATE
    ${PROJECT_INCLUDE_DIR}
    ${PROJECT_THIRD_PARTY_DIR}/opencv/include
    ${PROJECT_THIRD_PARTY_DIR}/opencv/modules/core/include
    ${PROJECT_THIRD_PARTY_DIR}/opencv/modules/imgproc/include
    ${PROJECT_THIRD_PARTY_DIR}/opencv/modules/imgcodecs/include
    ${PROJECT_THIRD_PARTY_DIR}/opencv/modules/highgui/include
    ${CMAKE_BINARY_DIR}  # 包含生成的头文件，如 opencv_modules.hpp
)

# 设置可执行文件输出目录
set_target_properties(main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 输出构建信息
message(STATUS "Building OpenCV Image Reader Project")
message(STATUS "Project version: ${PROJECT_VERSION}")
message(STATUS "Source directory: ${PROJECT_SOURCE_DIR}")
message(STATUS "Include directory: ${PROJECT_INCLUDE_DIR}")
message(STATUS "Third party directory: ${PROJECT_THIRD_PARTY_DIR}")
message(STATUS "Assets directory: ${PROJECT_ASSETS_DIR}")
message(STATUS "OpenCV source directory: ${PROJECT_THIRD_PARTY_DIR}/opencv")
