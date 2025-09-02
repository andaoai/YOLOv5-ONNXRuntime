# YOLOv5-ONNXRuntime

一个基于 OpenCV 和 ONNX Runtime 的现代化 C++ 目标检测项目，实现了完整的 YOLOv5 推理流程。

🔗 **项目地址**: [https://github.com/andaoai/YOLOv5-ONNXRuntime](https://github.com/andaoai/YOLOv5-ONNXRuntime)

## 📋 目录

- [🚀 项目特点](#-项目特点)
- [🐳 使用 VS Code Dev Container 开发（推荐）](#-使用-vs-code-dev-container-开发推荐)
- [📁 项目结构](#-项目结构)
- [🎯 核心功能](#-核心功能)
- [🛠️ 系统要求](#️-系统要求)
- [🚀 快速开始](#-快速开始)
- [📊 运行结果](#-运行结果)
- [🔧 高级配置](#-高级配置)
- [💻 代码实现详解](#-代码实现详解)
- [🏗️ 技术架构](#️-技术架构)
- [🔧 开发环境配置](#-开发环境配置)
- [🔧 故障排除](#-故障排除)
- [📚 学习资源](#-学习资源)
- [🤝 贡献](#-贡献)
- [📄 许可证](#-许可证)

## 🚀 项目特点

- **🎯 YOLOv5 目标检测**：完整的 ONNX 模型推理实现
- **🖼️ 图像处理**：基于 OpenCV 4.8.1 的图像预处理和后处理
- **⚡ 高性能推理**：ONNX Runtime 1.18.1 优化推理引擎
- **🏗️ 现代 C++17**：符合现代 C++ 标准的代码实现
- **📦 Conan 2.x**：自动化依赖管理，直接使用 Conan Center 官方包
- **🔧 标准构建系统**：支持 Release/Debug 多配置构建
- **🐳 Dev Container 支持**：一键在 Ubuntu 容器内开发
- **💻 VSCode 集成**：完整的 C++ 开发环境配置

## 🐳 使用 VS Code Dev Container 开发（推荐）

为了避免 Windows 环境的各种兼容性问题，本项目配置了 VS Code Dev Container，可以在 Ubuntu 容器内进行开发。

### 前置要求

1. 安装 [Docker Desktop](https://www.docker.com/products/docker-desktop/)
2. 安装 [VS Code](https://code.visualstudio.com/)
3. 安装 VS Code 扩展：[Dev Containers](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers)

### 使用步骤

1. 在 VS Code 中打开项目文件夹
2. 按 `Ctrl+Shift+P` 打开命令面板
3. 输入并选择 `Dev Containers: Reopen in Container`
4. 等待容器构建完成（首次可能需要几分钟）
5. 容器启动后，按照下面的构建步骤进行开发

### Dev Container 特性

- 🐧 基于 Ubuntu 22.04
- 🛠️ 预装 C++ 开发工具链
- 🐍 Python 3.11 + Conan 2.x
- 📦 自动安装 OpenCV 和 ONNX Runtime 系统依赖
- 🔧 预配置的 VS Code 扩展和设置
- 🚀 开箱即用的开发环境

## 📁 项目结构

```
YOLOv5-ONNXRuntime/
├── .devcontainer/              # Dev Container 配置
│   ├── devcontainer.json      # 容器配置
│   ├── Dockerfile             # 容器镜像定义
│   └── reinstall-cmake.sh     # CMake 重新安装脚本
├── .vscode/                    # VSCode 配置
│   ├── c_cpp_properties.json  # C++ 智能提示配置
│   ├── launch.json            # 调试启动配置
│   ├── settings.json          # 编辑器设置
│   └── tasks.json             # 构建任务配置
├── CMakeLists.txt              # 主 CMake 配置文件
├── CMakeUserPresets.json       # CMake 用户预设配置
├── conanfile.py                # Conan 依赖配置
├── conanprofile                # Conan 编译器配置
├── README.md                   # 项目说明文档
├── compile_commands.json       # VSCode 编译数据库（生成）
├── src/                        # 源代码目录
│   └── main.cpp               # YOLOv5 推理主程序
├── assets/                     # 资源文件
│   ├── images/                # 图像文件
│   │   ├── bus.jpg           # 测试图片
│   │   └── bus_result.jpg    # 检测结果图片
│   └── models/                # 模型文件
│       └── yolov5n.onnx      # YOLOv5 ONNX 模型
└── build/                      # 构建输出目录（生成）
    ├── Release/               # Release 构建
    │   ├── generators/        # Conan 生成文件
    │   └── bin/main          # 可执行文件
    └── Debug/                 # Debug 构建
        ├── generators/        # Conan 生成文件
        └── bin/main          # Debug 可执行文件
```

## 🎯 核心功能

### YOLOv5 目标检测
- **🖼️ 图像预处理**：自动缩放、填充、归一化（保持宽高比）
- **🧠 ONNX 推理**：支持 Float16 优化推理，多线程加速
- **🎯 目标检测**：检测 80 种 COCO 类别目标
- **📊 后处理**：置信度过滤（0.5）、NMS 非极大值抑制（0.4）
- **🎨 结果可视化**：绘制绿色检测框和标签，显示置信度百分比

### 支持的目标类别
支持 COCO 数据集的 80 种类别，包括：
- **人物**：person
- **交通工具**：car, bus, truck, bicycle, motorcycle
- **动物**：cat, dog, horse, sheep, cow, bird
- **日常物品**：chair, table, laptop, cell phone, book
- 等等...

## 🛠️ 系统要求

### 基础环境
- **CMake** 3.15+
- **Conan** 2.x
- **C++ 编译器**：GCC 11+ / Clang 12+ / MSVC 2019+
- **操作系统**：Linux / macOS（推荐使用 Dev Container）
- **Docker Desktop**：用于 Dev Container 开发环境

### 依赖库（Conan 自动管理）
- **OpenCV 4.8.1**：计算机视觉库，禁用 DNN 和 contrib 模块
- **ONNX Runtime 1.18.1**：机器学习推理引擎，支持 Float16 优化

## 🚀 快速开始

### 方法一：使用 Dev Container（推荐）

1. **安装前置要求**：
   - [Docker Desktop](https://www.docker.com/products/docker-desktop/)
   - [VS Code](https://code.visualstudio.com/)
   - [Dev Containers 扩展](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers)

2. **启动开发环境**：
   ```bash
   # 在 VS Code 中打开项目
   # 按 Ctrl+Shift+P，选择 "Dev Containers: Reopen in Container"
   ```

3. **构建和运行**：

   **Release 版本（生产环境）**：
   ```bash
   # 首次使用需要创建 Conan 配置文件
   conan profile detect

   # 安装 Release 依赖
   conan install . --build=missing -s build_type=Release

   # 使用 VSCode CMake Tools 扩展构建
   # 按 Ctrl+Shift+P → "CMake: Configure"
   # 按 F7 或点击状态栏的构建按钮

   # 或手动构建
   cd build/Release
   cmake ../.. -DCMAKE_TOOLCHAIN_FILE=generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
   make -j$(nproc)

   # 运行推理
   cd ../..
   ./build/Release/bin/main
   ```

   **Debug 版本（开发调试）**：

   **方法一：完整 Debug 构建（依赖库也是 Debug 版本）**：
   ```bash
   # 在项目根目录下执行以下命令
   cd /workspaces/YOLOv5-ONNXRuntime

   # 首次使用需要创建 Conan 配置文件（如果之前没有运行过）
   conan profile detect

   # 安装 Debug 依赖（所有库都构建为 Debug 版本）
   conan install . --build=missing -s build_type=Debug

   # 配置 Debug 构建
   cmake -S . -B build/Debug -G "Unix Makefiles" \
     -DCMAKE_TOOLCHAIN_FILE=build/Debug/generators/conan_toolchain.cmake \
     -DCMAKE_BUILD_TYPE=Debug

   # 编译 Debug 版本
   cmake --build build/Debug --config Debug -j$(nproc)

   # 运行 Debug 版本
   ./build/Debug/bin/main

   # 或在 VSCode 中按 F5 启动调试
   ```

   **方法二：混合构建（推荐用于日常调试）**：
   ```bash
   # 在项目根目录下执行以下命令
   cd /workspaces/YOLOv5-ONNXRuntime

   # 首次使用需要创建 Conan 配置文件（如果之前没有运行过）
   conan profile detect

   # 使用 Release 版本的依赖库（更快的构建和运行速度）
   conan install . --build=missing -s build_type=Release

   # 配置 Debug 构建（使用 Release 版本的依赖库）
   cmake -S . -B build/Debug \
      -DCMAKE_TOOLCHAIN_FILE=build/Release/generators/conan_toolchain.cmake \
      -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_MAP_IMPORTED_CONFIG_DEBUG=Release

   # 编译 Debug 版本
   cmake --build build/Debug --config Debug -j$(nproc)

   # 运行 Debug 版本
   ./build/Debug/bin/main

   # 或在 VSCode 中按 F5 启动调试
   ```

   **💡 调试方法选择建议**：
   - **方法一**：当需要调试依赖库内部代码时使用
   - **方法二**：日常开发调试推荐，构建更快，依赖库性能更好

   **🔧 混合构建原理说明**：
   - `CMAKE_MAP_IMPORTED_CONFIG_DEBUG=Release`：告诉 CMake 当项目构建类型为 Debug 时，使用 Release 版本的导入目标（依赖库）
   - 这样可以实现：项目代码编译为 Debug（可调试），但链接 Release 版本的依赖库（性能更好）
   - CMakeLists.txt 中已自动配置此选项，无需手动设置

### 方法二：本地环境构建

1. **安装依赖**：
   ```bash
   # 首次使用需要创建 Conan 配置文件
   conan profile detect

   # 安装 Conan 依赖
   conan install . --output-folder=build --build=missing -s build_type=Release
   ```

2. **配置和构建**：
   ```bash
   cd build/Release
   cmake ../.. -DCMAKE_TOOLCHAIN_FILE=generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
   make -j$(nproc)
   ```

3. **运行程序**：
   ```bash
   cd ../..
   ./build/Release/bin/main
   ```

## 📊 运行结果

程序运行后会输出检测结果：

```
YOLOv5 ONNX 推理测试
图像: 810x1080
推理完成
检测到 4 个目标:
  - person (置信度: 0.82) 位置: [219, 408, 130, 454]
  - person (置信度: 0.80) 位置: [52, 401, 154, 493]
  - person (置信度: 0.64) 位置: [680, 369, 129, 504]
  - bus (置信度: 0.51) 位置: [55, 236, 754, 531]
结果已保存到: ../assets/images/bus_result.jpg
YOLOv5 推理测试完成！
```

检测结果图像会保存到 `assets/images/bus_result.jpg`，包含：
- 🟢 **绿色边界框**：标识检测到的目标
- 🏷️ **类别标签**：显示目标类别和置信度百分比
- 📊 **坐标信息**：输出格式为 [x, y, width, height]

## 🔧 高级配置

### 多配置构建

项目支持标准的多配置构建：

```bash
# 首次使用需要创建 Conan 配置文件
conan profile detect

# Release 构建（优化版本）
conan install . --output-folder=build -s build_type=Release
cd build/Release && cmake ../.. -DCMAKE_TOOLCHAIN_FILE=generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release

# Debug 构建（调试版本）
conan install . --output-folder=build -s build_type=Debug
cd build/Debug && cmake ../.. -DCMAKE_TOOLCHAIN_FILE=generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug
```



### 自定义模型

要使用自己的 YOLOv5 模型：

1. 将 ONNX 模型文件放到 `assets/models/` 目录
2. 修改 `src/main.cpp` 中的模型路径：
   ```cpp
   const std::string model_path = "/workspaces/YOLOv5-ONNXRuntime/assets/models/your_model.onnx";
   ```
3. 如果使用不同的图片，也需要修改图片路径：
   ```cpp
   const std::string image_path = "/workspaces/YOLOv5-ONNXRuntime/assets/images/your_image.jpg";
   ```
4. 重新编译运行

## 🔧 开发环境配置

### VSCode 配置

项目已配置完整的 VSCode C++ 开发环境：

- ✅ **智能提示**：自动识别 OpenCV 和 ONNX Runtime 头文件
- ✅ **错误检测**：实时语法和类型检查
- ✅ **调试支持**：支持断点调试
- ✅ **代码跳转**：Ctrl+Click 跳转到定义
- ✅ **编译数据库**：使用 `compile_commands.json`

### 🐛 调试配置

项目已配置完整的调试环境，支持在 VSCode 中进行断点调试：

#### 快速调试步骤

1. **构建 Debug 版本**：

   **推荐方法（混合构建）**：
   ```bash
   # 在项目根目录下执行以下命令
   cd /workspaces/YOLOv5-ONNXRuntime

   # 首次使用需要创建 Conan 配置文件（如果之前没有运行过）
   conan profile detect

   # 使用 Release 版本的依赖库（构建更快）
   conan install . --build=missing -s build_type=Release

   # 配置 Debug 构建（使用 Release 依赖库，但项目代码为 Debug）
   cmake -S . -B build/Debug -G "Unix Makefiles" \
     -DCMAKE_TOOLCHAIN_FILE=build/Release/generators/conan_toolchain.cmake \
     -DCMAKE_BUILD_TYPE=Debug \
     -DCMAKE_MAP_IMPORTED_CONFIG_DEBUG=Release

   # 编译 Debug 版本
   cmake --build build/Debug --config Debug -j$(nproc)
   ```

   **完整 Debug 构建（如需调试依赖库）**：
   ```bash
   # 在项目根目录下执行以下命令
   cd /workspaces/YOLOv5-ONNXRuntime

   # 安装 Debug 依赖（所有库都是 Debug 版本）
   conan install . --output-folder=build --build=missing -s build_type=Debug

   # 配置 Debug 构建
   cmake -S . -B build/Debug -G "Unix Makefiles" \
     -DCMAKE_TOOLCHAIN_FILE=build/Debug/generators/conan_toolchain.cmake \
     -DCMAKE_BUILD_TYPE=Debug

   # 编译 Debug 版本
   cmake --build build/Debug --config Debug -j$(nproc)
   ```

2. **设置断点**：
   - 在代码行号左侧点击设置红色断点
   - 或按 `F9` 在当前行设置断点

3. **启动调试**：
   - 按 `F5` 启动调试
   - 或点击调试面板的 "▶️ Debug C++ (main)" 按钮

4. **调试操作**：
   - `F5`：继续执行
   - `F10`：单步跳过
   - `F11`：单步进入
   - `Shift+F11`：单步跳出
   - `Shift+F5`：停止调试

#### 调试配置文件

项目包含以下调试配置文件：

- **`.vscode/launch.json`**：调试启动配置
- **`.vscode/tasks.json`**：构建任务配置
- **`.vscode/c_cpp_properties.json`**：C++ 智能提示配置

#### 命令行调试（可选）

也可以使用 GDB 进行命令行调试：

```bash
# 启动 GDB
gdb build/Debug/bin/main

# 设置断点
(gdb) break main
(gdb) break src/main.cpp:160

# 运行程序
(gdb) run

# 调试命令
(gdb) next      # 下一行
(gdb) step      # 进入函数
(gdb) continue  # 继续执行
(gdb) print var # 打印变量
(gdb) quit      # 退出
```





## 🔧 故障排除

### 常见问题

1. **Conan 默认配置文件不存在**
   ```
   ERROR: The default build profile '/home/vscode/.conan2/profiles/default' doesn't exist.
   ```
   **解决方案**：运行 `conan profile detect` 创建默认配置文件

2. **Conan 找不到编译器**
   - 检查编译器是否在 PATH 中
   - 确保使用兼容的编译器版本

2. **CMake 配置失败**
   - 检查 CMake 版本是否 >= 3.15
   - 确保指定了正确的工具链文件

3. **链接错误**
   - 清理构建目录重新构建
   - 检查 Conan 依赖是否正确安装

4. **VSCode 头文件错误**
   - 确保 `compile_commands.json` 文件存在
   - 重新加载 VSCode IntelliSense

## 💻 代码实现详解

### 主要文件说明

- **`src/main.cpp`**：主程序文件，包含完整的 YOLOv5 推理流程
- **`CMakeLists.txt`**：CMake 构建配置，支持 Debug/Release 多配置
- **`conanfile.py`**：Conan 依赖管理，自动下载 OpenCV 和 ONNX Runtime
- **`assets/models/yolov5n.onnx`**：YOLOv5 Nano 模型（最轻量版本）
- **`assets/images/bus.jpg`**：测试图像

### 关键技术实现

1. **Float16 优化推理**：
   - 使用 `Ort::Float16_t` 类型减少内存占用
   - 支持硬件加速（如果可用）
   - 保持推理精度的同时提升性能

2. **智能图像预处理**：
   - 保持宽高比的 letterbox 缩放
   - 自动计算填充偏移量
   - BGR→RGB 颜色空间转换

3. **高效后处理**：
   - 向量化的置信度过滤
   - 基于 IoU 的 NMS 算法
   - 坐标系自动转换

## 🏗️ 技术架构

### 核心组件

项目采用函数式设计，主要包含以下核心组件：

<augment_code_snippet path="src/main.cpp" mode="EXCERPT">
````cpp
// 检测结果结构
struct Detection {
    cv::Rect box;        // 边界框
    float confidence;    // 置信度
    int class_id;       // 类别ID
};

// COCO 数据集 80 种类别名称
const std::vector<std::string> class_names = {
    "person", "bicycle", "car", "motorcycle", "airplane", "bus", "train", "truck", "boat", "traffic light",
    // ... 完整的 80 种类别
};
````
</augment_code_snippet>

**核心函数实现**：

1. **图像预处理函数**：
<augment_code_snippet path="src/main.cpp" mode="EXCERPT">
````cpp
// 保持宽高比的缩放、填充、归一化和 BGR→RGB 转换
cv::Mat preprocess_image(const cv::Mat& image, int input_width, int input_height);
````
</augment_code_snippet>

2. **NMS 非极大值抑制**：
<augment_code_snippet path="src/main.cpp" mode="EXCERPT">
````cpp
// IoU 计算和重叠检测框过滤
std::vector<Detection> apply_nms(std::vector<Detection>& detections, float nms_threshold);
````
</augment_code_snippet>

3. **主推理流程**（在 main() 函数中实现）：
   - 加载图像和 ONNX 模型
   - 创建 ONNX Runtime 会话（4线程优化）
   - 图像预处理（640x640 输入）
   - **Float16 推理**（内存和性能优化）
   - 后处理和 NMS（置信度 0.5，NMS 0.4）
   - 结果可视化和保存

### 数据流

```
输入图像 → 预处理 → Float16推理 → 后处理 → NMS → 可视化 → 输出结果
    ↓         ↓         ↓         ↓      ↓      ↓
  原始图像   缩放填充   特征提取   解析输出  去重  绘制边界框
  810x1080  640x640   25200x85  检测框   过滤   保存图片
```

**详细处理流程**：

1. **图像加载**：OpenCV 读取 JPG 图像（BGR 格式）
2. **预处理**：
   - 保持宽高比缩放到 640x640
   - 灰色填充（letterbox）
   - 归一化到 [0,1] 范围
   - BGR → RGB 颜色空间转换
   - HWC → CHW 维度转换
3. **推理**：
   - 转换为 Float16 格式（内存优化）
   - ONNX Runtime 4线程推理
   - 输出：[1, 25200, 85] 张量
4. **后处理**：
   - 置信度过滤（> 0.5）
   - 坐标反变换（640x640 → 原图尺寸）
   - NMS 去重（IoU > 0.4）
5. **可视化**：
   - 绘制绿色边界框
   - 添加类别标签和置信度
   - 保存结果图像

### 依赖关系

- **OpenCV 4.8.1**：图像 I/O、预处理、可视化、BGR↔RGB转换
- **ONNX Runtime 1.18.1**：Float16 模型推理引擎，多线程优化
- **C++17 STL**：数据结构和算法，向量操作
- **Conan 2.x**：自动化依赖管理和构建

## 📚 学习资源

- [YOLOv5 官方文档](https://github.com/ultralytics/yolov5)
- [OpenCV 官方文档](https://docs.opencv.org/)
- [ONNX Runtime 文档](https://onnxruntime.ai/)
- [Conan 包管理器文档](https://docs.conan.io/)
- [CMake 官方教程](https://cmake.org/cmake/help/latest/guide/tutorial/)

## 🤝 贡献

欢迎提交 Issue 和 Pull Request！

### 贡献指南

1. Fork 项目：[https://github.com/andaoai/YOLOv5-ONNXRuntime](https://github.com/andaoai/YOLOv5-ONNXRuntime)
2. 创建特性分支：`git checkout -b feature/amazing-feature`
3. 提交更改：`git commit -m 'Add amazing feature'`
4. 推送分支：`git push origin feature/amazing-feature`
5. 提交 Pull Request

## 📄 许可证

MIT License

---

**🎯 项目状态**：✅ 生产就绪
**📊 测试覆盖率**：基础功能已验证
**🔄 持续集成**：支持 Dev Container 环境
