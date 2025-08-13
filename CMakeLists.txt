cmake_minimum_required(VERSION 3.15)
project(opencv_image_processing VERSION 1.0.0 LANGUAGES CXX)

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

# 检查 Git 子模块是否已初始化
if(NOT EXISTS "${PROJECT_THIRD_PARTY_DIR}/opencv/CMakeLists.txt")
    message(FATAL_ERROR "OpenCV submodule not found. Please run: git submodule update --init --recursive")
endif()

# OpenCV 构建选项现在通过 Conan toolchain 统一管理
# 所有 BUILD_* 和 WITH_* 变量都在 conanfile.py 中定义
message(STATUS "OpenCV build options managed by Conan toolchain (see conanfile.py)")

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
message(STATUS "Building OpenCV Image Processing Project")
message(STATUS "Project version: ${PROJECT_VERSION}")
message(STATUS "Source directory: ${PROJECT_SOURCE_DIR}")
message(STATUS "Include directory: ${PROJECT_INCLUDE_DIR}")
message(STATUS "Third party directory: ${PROJECT_THIRD_PARTY_DIR}")
message(STATUS "Assets directory: ${PROJECT_ASSETS_DIR}")
message(STATUS "OpenCV source directory: ${PROJECT_THIRD_PARTY_DIR}/opencv")
