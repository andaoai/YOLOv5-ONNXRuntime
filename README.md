# YOLOv5 ONNX 推理项目

一个基于 OpenCV 和 ONNX Runtime 的现代化 C++ 目标检测项目，实现了完整的 YOLOv5 推理流程。

## 🚀 项目特点

- **🎯 YOLOv5 目标检测**：完整的 ONNX 模型推理实现
- **🖼️ 图像处理**：基于 OpenCV 4.8.1 的图像预处理和后处理
- **⚡ 高性能推理**：ONNX Runtime 1.18.1 优化推理引擎
- **🏗️ 现代 C++17**：符合现代 C++ 标准的代码实现
- **📦 Conan 2.x**：自动化依赖管理，无需手动安装库
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
5. 容器启动后，在终端中运行：
   ```bash
   ./build.sh
   ```

### Dev Container 特性

- 🐧 基于 Ubuntu 22.04
- 🛠️ 预装 C++ 开发工具链
- 🐍 Python 3.11 + Conan 2.x
- 📦 自动安装 OpenCV 和 ONNX Runtime 系统依赖
- 🔧 预配置的 VS Code 扩展和设置
- 🚀 开箱即用的开发环境

## 📁 项目结构

```
test_inference/
├── .devcontainer/              # Dev Container 配置
│   ├── devcontainer.json      # 容器配置
│   └── setup.sh              # 环境设置脚本
├── CMakeLists.txt              # 主 CMake 配置文件
├── conanfile.py                # Conan 依赖配置
├── conanprofile                # Conan 编译器配置
├── README.md                   # 项目说明文档
├── compile_commands.json       # VSCode 编译数据库
├──
├── src/                        # 源代码目录
│   └── main.cpp               # YOLOv5 推理主程序
├──
├── include/                    # 项目头文件目录
├── tests/                      # 测试代码目录
├──
├── assets/                    # 资源文件
│   ├── images/
│   │   ├── bus.jpg           # 测试图片
│   │   └── bus_result.jpg    # 检测结果图片
│   └── models/
│       └── yolov5n.onnx      # YOLOv5 ONNX 模型
├──
├── build/                     # 构建输出目录
│   ├── Release/              # Release 构建
│   │   ├── generators/       # Conan 生成文件
│   │   ├── bin/main         # 可执行文件
│   │   └── compile_commands.json
│   └── Debug/               # Debug 构建
│       └── ...
├──
├── scripts/                   # 构建脚本
│   ├── build.ps1             # PowerShell 构建脚本
│   ├── clean.ps1             # 清理脚本
│   └── conan_cache_manager.ps1 # Conan 缓存管理
└── recipes/                   # 自定义 Conan 配方
    ├── opencv/               # OpenCV 配方
    └── onnxruntime/         # ONNX Runtime 配方
```

## 🎯 核心功能

### YOLOv5 目标检测
- **🖼️ 图像预处理**：自动缩放、填充、归一化
- **🧠 ONNX 推理**：支持 Float16 优化推理
- **🎯 目标检测**：检测 80 种 COCO 类别目标
- **📊 后处理**：置信度过滤、NMS 非极大值抑制
- **🎨 结果可视化**：绘制检测框和标签

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
- **操作系统**：Linux / Windows / macOS

### 依赖库（自动管理）
- **OpenCV 4.8.1**：计算机视觉库
- **ONNX Runtime 1.18.1**：机器学习推理引擎

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
   ```bash
   # 安装依赖
   conan install . --output-folder=build --build=missing -s build_type=Release

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

### 方法二：本地环境构建

1. **安装依赖**：
   ```bash
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
结果已保存到: assets/images/bus_result.jpg
YOLOv5 推理测试完成！
```

检测结果图像会保存到 `assets/images/bus_result.jpg`，包含：
- 🟢 **绿色边界框**：标识检测到的目标
- 🏷️ **类别标签**：显示目标类别和置信度

## 🔧 高级配置

### 多配置构建

项目支持标准的多配置构建：

```bash
# Release 构建（优化版本）
conan install . --output-folder=build -s build_type=Release
cd build/Release && cmake ../.. -DCMAKE_TOOLCHAIN_FILE=generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release

# Debug 构建（调试版本）
conan install . --output-folder=build -s build_type=Debug
cd build/Debug && cmake ../.. -DCMAKE_TOOLCHAIN_FILE=generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug
```

### Windows PowerShell 脚本

项目提供了 Windows 环境的构建脚本：

```powershell
# 完整构建
.\scripts\build.ps1 -Clean

# 快速构建
.\scripts\build.ps1

# 缓存管理
.\scripts\conan_cache_manager.ps1 -Action status
```

### 自定义模型

要使用自己的 YOLOv5 模型：

1. 将 ONNX 模型文件放到 `assets/models/` 目录
2. 修改 `src/main.cpp` 中的模型路径：
   ```cpp
   const std::string model_path = "assets/models/your_model.onnx";
   ```
3. 重新编译运行

## 🔧 开发环境配置

### VSCode 配置

项目已配置完整的 VSCode C++ 开发环境：

- ✅ **智能提示**：自动识别 OpenCV 和 ONNX Runtime 头文件
- ✅ **错误检测**：实时语法和类型检查
- ✅ **调试支持**：支持断点调试
- ✅ **代码跳转**：Ctrl+Click 跳转到定义
- ✅ **编译数据库**：使用 `compile_commands.json`

### 多配置构建

项目支持标准的多配置构建：

```bash
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
   const std::string model_path = "assets/models/your_model.onnx";
   ```
3. 重新编译运行

## 🔧 故障排除

### 常见问题

1. **Conan 找不到编译器**
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

## 🏗️ 技术架构

### 核心组件

```cpp
// YOLOv5 推理流程
class YOLOv5Detector {
public:
    // 初始化 ONNX Runtime 会话
    bool initialize(const std::string& model_path);

    // 完整的检测流程
    std::vector<Detection> detect(const cv::Mat& image);

private:
    // 图像预处理：缩放、填充、归一化
    cv::Mat preprocess(const cv::Mat& image);

    // ONNX 推理
    std::vector<float> inference(const cv::Mat& input);

    // 后处理：解析输出、NMS
    std::vector<Detection> postprocess(const std::vector<float>& output);

    // 结果可视化
    void visualize(cv::Mat& image, const std::vector<Detection>& detections);
};
```

### 数据流

```
输入图像 → 预处理 → ONNX推理 → 后处理 → 可视化 → 输出结果
    ↓         ↓        ↓        ↓        ↓
  原始图像   标准化    特征提取   目标检测   绘制边界框
```

### 依赖关系

- **OpenCV 4.8.1**：图像 I/O、预处理、可视化
- **ONNX Runtime 1.18.1**：模型推理引擎
- **C++17 STL**：数据结构和算法

## 📚 学习资源

- [YOLOv5 官方文档](https://github.com/ultralytics/yolov5)
- [OpenCV 官方文档](https://docs.opencv.org/)
- [ONNX Runtime 文档](https://onnxruntime.ai/)
- [Conan 包管理器文档](https://docs.conan.io/)
- [CMake 官方教程](https://cmake.org/cmake/help/latest/guide/tutorial/)

## 🤝 贡献

欢迎提交 Issue 和 Pull Request！

### 贡献指南

1. Fork 项目
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
