cmake_minimum_required(VERSION 3.15)
project(yolov5_inference VERSION 1.0.0 LANGUAGES CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 禁用 OpenCV 中的 ITT 支持
add_definitions(-DOPENCV_DISABLE_ITT=1)

# 查找 Conan 生成的工具链文件
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/build/Release/generators/conan_toolchain.cmake" AND 
   NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/build/Debug/generators/conan_toolchain.cmake")
    message(FATAL_ERROR "Conan toolchain not found. Please run: conan install . --output-folder=build --build=missing")
endif()

# 包含 Conan 工具链
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/build/Release/generators/conan_toolchain.cmake")
    include("${CMAKE_CURRENT_SOURCE_DIR}/build/Release/generators/conan_toolchain.cmake")
    message(STATUS "Using Release Conan toolchain")
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/build/Debug/generators/conan_toolchain.cmake")
    include("${CMAKE_CURRENT_SOURCE_DIR}/build/Debug/generators/conan_toolchain.cmake")
    message(STATUS "Using Debug Conan toolchain")
endif()

# 支持混合构建：Debug 项目代码 + Release 依赖库
# 当使用 Release 依赖库但构建 Debug 项目时，映射 Debug 配置到 Release
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_MAP_IMPORTED_CONFIG_DEBUG Release)
    message(STATUS "Mixed build: Debug project with Release dependencies")
endif()

# 查找 Conan 管理的包
find_package(opencv REQUIRED)
find_package(onnxruntime REQUIRED)

message(STATUS "Using Conan-managed OpenCV and ONNX Runtime")

# 创建可执行文件
add_executable(main src/main.cpp)

# 设置编译选项
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(main PRIVATE DEBUG_BUILD=1)
    target_compile_options(main PRIVATE -g -O0 -Wall -Wextra)
    message(STATUS "Building in DEBUG mode")
else()
    target_compile_definitions(main PRIVATE RELEASE_BUILD=1)
    target_compile_options(main PRIVATE -O3 -DNDEBUG)
    message(STATUS "Building in RELEASE mode")
endif()

# 链接 Conan 管理的依赖库
target_link_libraries(main
    opencv::opencv
    onnxruntime::onnxruntime
)

# 包含头文件目录 (当前项目不需要额外的头文件目录)
# target_include_directories(main PRIVATE include)

# 设置可执行文件输出目录
set_target_properties(main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 输出构建信息
message(STATUS "Building YOLOv5 ONNX Inference Project")
message(STATUS "Project version: ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
