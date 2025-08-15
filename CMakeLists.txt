cmake_minimum_required(VERSION 3.15)
project(opencv_onnxruntime_project VERSION 1.0.0 LANGUAGES CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 禁用 OpenCV 中的 ITT 支持
add_definitions(-DOPENCV_DISABLE_ITT=1)

# 使用 Conan 管理的 OpenCV 和 ONNX Runtime
# 根据构建类型使用对应版本的库

# 根据构建类型选择对应的 Conan 生成器
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CONAN_GENERATORS_PATHS
        "${CMAKE_SOURCE_DIR}/build/Debug/generators"
        "${CMAKE_BINARY_DIR}/../Debug/generators"
        "${CMAKE_BINARY_DIR}/generators"
    )
else()
    set(CONAN_GENERATORS_PATHS
        "${CMAKE_SOURCE_DIR}/build/Release/generators"
        "${CMAKE_BINARY_DIR}/../Release/generators"
        "${CMAKE_BINARY_DIR}/generators"
    )
endif()

set(CONAN_GENERATORS_DIR "")
foreach(path ${CONAN_GENERATORS_PATHS})
    if(EXISTS "${path}/cmakedeps_macros.cmake")
        set(CONAN_GENERATORS_DIR ${path})
        message(STATUS "Found Conan generators at: ${path}")
        break()
    endif()
endforeach()

if(CONAN_GENERATORS_DIR STREQUAL "")
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(FATAL_ERROR "Could not find Conan generators directory. Please run: conan install . --output-folder=build --build=missing -s build_type=Debug")
    else()
        message(FATAL_ERROR "Could not find Conan generators directory. Please run: conan install . --output-folder=build --build=missing -s build_type=Release")
    endif()
endif()

message(STATUS "Using Conan generators from: ${CONAN_GENERATORS_DIR}")
message(STATUS "Program build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Libraries: ${CMAKE_BUILD_TYPE} version")

include(${CONAN_GENERATORS_DIR}/cmakedeps_macros.cmake)
include(${CONAN_GENERATORS_DIR}/conandeps_legacy.cmake)
find_package(opencv REQUIRED)
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

# 设置编译选项：程序和库都使用对应的构建类型
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(main PRIVATE DEBUG_BUILD=1)
    target_compile_options(main PRIVATE -g -O0 -Wall -Wextra)
    message(STATUS "Building program in DEBUG mode with Debug libraries")
else()
    target_compile_definitions(main PRIVATE RELEASE_BUILD=1)
    target_compile_options(main PRIVATE -O3 -DNDEBUG)
    message(STATUS "Building program in RELEASE mode with Release libraries")
endif()

# 链接 Conan 管理的依赖库（始终使用 Release 版本）
# 注意：由于 Conan 生成器表达式问题，我们需要强制链接 Release 版本的库
target_link_libraries(main
    opencv::opencv
    onnxruntime::onnxruntime
)

# 调试链接库信息
get_target_property(MAIN_LINK_LIBS main LINK_LIBRARIES)
message(STATUS "🔗 main 目标链接的库: ${MAIN_LINK_LIBS}")

# 强制添加 Release 版本的库文件（解决生成器表达式问题）
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "🔧 Debug 模式：强制链接 Release 版本的库文件和依赖")

    # 手动添加 OpenCV 库文件
    find_library(OPENCV_CORE_LIB opencv_core
        PATHS "/home/vscode/.conan2/p/b/openc87b6fbbb87431/p/lib"
        NO_DEFAULT_PATH
    )

    if(OPENCV_CORE_LIB)
        message(STATUS "✅ 找到 OpenCV 核心库: ${OPENCV_CORE_LIB}")
        # 添加所有 OpenCV 库
        file(GLOB OPENCV_LIBS "/home/vscode/.conan2/p/b/openc87b6fbbb87431/p/lib/libopencv_*.a")
        target_link_libraries(main ${OPENCV_LIBS})
        message(STATUS "✅ 添加 OpenCV 库: ${OPENCV_LIBS}")

        # 添加 OpenCV 依赖的第三方库
        find_library(ZLIB_LIB z)
        find_library(JPEG_LIB jpeg)
        find_library(PNG_LIB png)
        find_library(TIFF_LIB tiff)

        if(ZLIB_LIB)
            target_link_libraries(main ${ZLIB_LIB})
            message(STATUS "✅ 添加 zlib: ${ZLIB_LIB}")
        endif()

        if(JPEG_LIB)
            target_link_libraries(main ${JPEG_LIB})
            message(STATUS "✅ 添加 libjpeg: ${JPEG_LIB}")
        endif()

        if(PNG_LIB)
            target_link_libraries(main ${PNG_LIB})
            message(STATUS "✅ 添加 libpng: ${PNG_LIB}")
        endif()

        # 添加系统库
        target_link_libraries(main dl pthread)
        message(STATUS "✅ 添加系统库: dl pthread")
    endif()

    # 手动添加 ONNX Runtime 库文件
    find_library(ONNX_SESSION_LIB onnxruntime_session
        PATHS "/home/vscode/.conan2/p/b/onnxr37b44fb2c3935/p/lib"
        NO_DEFAULT_PATH
    )

    if(ONNX_SESSION_LIB)
        message(STATUS "✅ 找到 ONNX Runtime 库: ${ONNX_SESSION_LIB}")
        # 添加所有 ONNX Runtime 库
        file(GLOB ONNX_LIBS "/home/vscode/.conan2/p/b/onnxr37b44fb2c3935/p/lib/libonnxruntime_*.a")
        target_link_libraries(main ${ONNX_LIBS})
        message(STATUS "✅ 添加 ONNX Runtime 库: ${ONNX_LIBS}")
    endif()

    # 添加 Conan 管理的第三方库
    file(GLOB CONAN_LIBS "/home/vscode/.conan2/p/b/*/p/lib/lib*.a")
    if(CONAN_LIBS)
        target_link_libraries(main ${CONAN_LIBS})
        message(STATUS "✅ 添加 Conan 第三方库: ${CONAN_LIBS}")
    endif()
endif()

# 包含头文件目录
target_include_directories(main PRIVATE
    ${PROJECT_INCLUDE_DIR}
)

# 调试 Conan 目标属性
message(STATUS "=== 调试 OpenCV 目标属性 ===")
get_target_property(OPENCV_INCLUDE_DIRS opencv::opencv INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(OPENCV_LINK_LIBS opencv::opencv INTERFACE_LINK_LIBRARIES)
get_target_property(OPENCV_COMPILE_DEFS opencv::opencv INTERFACE_COMPILE_DEFINITIONS)

message(STATUS "OpenCV INTERFACE_INCLUDE_DIRECTORIES: ${OPENCV_INCLUDE_DIRS}")
message(STATUS "OpenCV INTERFACE_LINK_LIBRARIES: ${OPENCV_LINK_LIBS}")
message(STATUS "OpenCV INTERFACE_COMPILE_DEFINITIONS: ${OPENCV_COMPILE_DEFS}")

message(STATUS "=== 调试 ONNX Runtime 目标属性 ===")
get_target_property(ONNX_INCLUDE_DIRS onnxruntime::onnxruntime INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(ONNX_LINK_LIBS onnxruntime::onnxruntime INTERFACE_LINK_LIBRARIES)
get_target_property(ONNX_COMPILE_DEFS onnxruntime::onnxruntime INTERFACE_COMPILE_DEFINITIONS)

message(STATUS "ONNX INTERFACE_INCLUDE_DIRECTORIES: ${ONNX_INCLUDE_DIRS}")
message(STATUS "ONNX INTERFACE_LINK_LIBRARIES: ${ONNX_LINK_LIBS}")
message(STATUS "ONNX INTERFACE_COMPILE_DEFINITIONS: ${ONNX_COMPILE_DEFS}")

# 手动添加头文件路径
# 解决 Conan 生成器表达式问题：强制使用 Release 版本的头文件路径

# 1. 处理 OpenCV 头文件路径
if(OPENCV_INCLUDE_DIRS AND NOT OPENCV_INCLUDE_DIRS STREQUAL "OPENCV_INCLUDE_DIRS-NOTFOUND")
    target_include_directories(main PRIVATE ${OPENCV_INCLUDE_DIRS})
    message(STATUS "✅ 成功添加 OpenCV 头文件路径: ${OPENCV_INCLUDE_DIRS}")
else()
    message(STATUS "🔍 OpenCV 目标没有头文件路径，手动查找...")
    # 从 Release 版本的编译命令中提取的实际路径
    set(OPENCV_MANUAL_PATHS
        "/home/vscode/.conan2/p/b/openc4f12223446590/p/include"
        "/home/vscode/.conan2/p/b/openc4f12223446590/p/include/opencv4"
    )

    foreach(path ${OPENCV_MANUAL_PATHS})
        if(EXISTS "${path}/opencv2/opencv.hpp")
            target_include_directories(main PRIVATE ${path})
            message(STATUS "✅ 手动添加 OpenCV 头文件路径: ${path}")
            set(OPENCV_FOUND TRUE)
            break()
        endif()
    endforeach()

    if(NOT OPENCV_FOUND)
        message(FATAL_ERROR "❌ 无法找到 OpenCV 头文件路径")
    endif()
endif()

# 2. 处理 ONNX Runtime 头文件路径（解决生成器表达式问题）
message(STATUS "🔍 ONNX 原始路径: ${ONNX_INCLUDE_DIRS}")

# 手动添加 ONNX Runtime 头文件路径（从调试信息中提取）
set(ONNX_MANUAL_PATHS
    "/home/vscode/.conan2/p/b/onnxr1c2c0b60fe458/p/include"
    "/home/vscode/.conan2/p/b/onnxr1c2c0b60fe458/p/include/onnxruntime/core/session"
)

foreach(path ${ONNX_MANUAL_PATHS})
    if(EXISTS ${path})
        target_include_directories(main PRIVATE ${path})
        message(STATUS "✅ 手动添加 ONNX Runtime 头文件路径: ${path}")
    endif()
endforeach()

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
