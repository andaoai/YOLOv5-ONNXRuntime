# OpenCV Image Reader Project

一个使用 Conan 包管理器和 OpenCV 的 C++ 图像处理项目。

## 项目结构

```
test_inference/
├── CMakeLists.txt              # 主 CMake 配置文件
├── conanfile.txt               # Conan 依赖配置
├── conanprofile                # Conan 编译器配置
├── README.md                   # 项目说明
├── 
├── src/                        # 源代码目录
│   └── main.cpp               # 主程序入口
├── 
├── include/                    # 项目公共头文件
├── 
├── tests/                      # 测试代码
├── 
├── third_party/               # 第三方源码依赖
│   └── opencv-4.12.0/        # OpenCV 源码
├── 
├── assets/                    # 资源文件
│   ├── images/
│   │   └── bus.jpg           # 测试图片
│   └── models/
│       └── yolov5n.onnx      # ONNX 模型文件
├── 
├── build/                     # 构建输出目录
└── scripts/                   # 构建和部署脚本
```

## 功能

- 读取图片文件
- 显示图片的尺寸信息（高度、宽度、通道数）
- 使用 OpenCV 4.12.0 进行图像处理

## 构建要求

- CMake 3.15+
- Conan 2.x
- MinGW-w64 (GCC 8.1.0+)
- Windows 10/11

## 构建步骤

1. 设置环境变量：
   ```bash
   $env:PATH = "C:\mingw64\bin;" + $env:PATH
   ```

2. 创建构建目录并安装依赖：
   ```bash
   mkdir build
   cd build
   conan install .. --build=missing --profile=../conanprofile
   ```

3. 配置和构建：
   ```bash
   cmake .. -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE="Release/generators/conan_toolchain.cmake" -DCMAKE_C_COMPILER=C:/mingw64/bin/gcc.exe -DCMAKE_CXX_COMPILER=C:/mingw64/bin/g++.exe -DCMAKE_MAKE_PROGRAM=C:/mingw64/bin/make.exe -DCMAKE_BUILD_TYPE=Release
   cmake --build . --config Release
   ```

4. 运行程序：
   ```bash
   cd ..
   .\build\bin\main.exe
   ```

## 输出示例

```
Image shape: 1080 x 810 x 3
Height: 1080
Width: 810
Channels: 3
```
