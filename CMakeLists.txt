cmake_minimum_required(VERSION 3.15)
project(opencv_onnxruntime_project VERSION 1.0.0 LANGUAGES CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 禁用 OpenCV 中的 ITT 支持
add_definitions(-DOPENCV_DISABLE_ITT=1)

# 使用 Conan 管理的 OpenCV 和 ONNX Runtime
include(${CMAKE_BINARY_DIR}/Release/generators/conan_toolchain.cmake OPTIONAL)
find_package(OpenCV REQUIRED)
find_package(onnxruntime REQUIRED)

message(STATUS "Using Conan-managed OpenCV and ONNX Runtime")
message(STATUS "OpenCV target: opencv::opencv")
message(STATUS "ONNX Runtime target: onnxruntime::onnxruntime")

# 设置项目目录变量
set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(PROJECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(PROJECT_ASSETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets)

# 创建可执行文件
add_executable(main ${PROJECT_SOURCE_DIR}/main.cpp)

# 链接 Conan 管理的依赖库
target_link_libraries(main
    opencv::opencv
    onnxruntime::onnxruntime
)

# 包含头文件目录
target_include_directories(main PRIVATE
    ${PROJECT_INCLUDE_DIR}
    ${opencv_INCLUDE_DIRS_RELEASE}/opencv4
)

# 设置可执行文件输出目录
set_target_properties(main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 移除手动第三方库设置，依赖 Conan 管理

# 输出构建信息
message(STATUS "Building OpenCV and ONNX Runtime Project")
message(STATUS "Project version: ${PROJECT_VERSION}")
message(STATUS "Source directory: ${PROJECT_SOURCE_DIR}")
message(STATUS "Include directory: ${PROJECT_INCLUDE_DIR}")
message(STATUS "Assets directory: ${PROJECT_ASSETS_DIR}")
message(STATUS "Using Conan-managed OpenCV and ONNX Runtime dependencies")
