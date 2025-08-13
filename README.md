# OpenCV Image Reader Project

一个使用现代化 Conan + CMake + Git 子模块的 C++ 图像处理项目。

## 🚀 项目特点

- **现代 C++17** 标准
- **Conan 2.x** 使用 `conanfile.py` 进行高级依赖管理
- **Git 子模块** 版本化的第三方依赖管理
- **优化构建** 减少重复编译时间
- **标准项目结构** 符合业界最佳实践

## 项目结构

```
test_inference/
├── CMakeLists.txt              # 主 CMake 配置文件
├── conanfile.py                # Conan 依赖配置 (Python 版本)
├── conanprofile                # Conan 编译器配置
├── README.md                   # 项目说明文档
├── .gitignore                  # Git 忽略文件
├── .gitmodules                 # Git 子模块配置
├──
├── src/                        # 源代码目录
│   └── main.cpp               # 主程序入口
├──
├── include/                    # 项目公共头文件
├── tests/                      # 测试代码目录
├──
├── third_party/               # 第三方源码依赖 (Git 子模块)
│   └── opencv/                # OpenCV 源码 (Git 子模块)
├── 
├── assets/                    # 资源文件
│   ├── images/
│   │   └── bus.jpg           # 测试图片
│   └── models/
│       └── yolov5n.onnx      # ONNX 模型文件
├── 
├── build/                     # 构建输出目录
│   └── bin/main.exe          # 编译生成的可执行文件
└── scripts/                   # 构建和部署脚本
    ├── build.ps1             # PowerShell 构建脚本
    ├── clean.ps1             # 清理脚本
    └── conan_cache_manager.ps1 # Conan 缓存管理脚本
```

## 功能

- 读取图片文件
- 显示图片的尺寸信息（高度、宽度、通道数）
- 使用 OpenCV 4.12.0 进行图像处理

## 🛠️ 构建要求

- **CMake** 3.15+
- **Conan** 2.x
- **Git** (用于子模块管理)
- **MinGW-w64** (GCC 8.1.0+)
- **Windows** 10/11

## 📦 依赖管理架构

本项目采用现代化的多层依赖管理策略：

### 1. Conan 包管理器 (conanfile.py)
- 使用 `conanfile.py` 而非 `conanfile.txt` 获得更强的控制能力
- 支持条件依赖和动态配置
- 自动化构建工具链管理

### 2. Git 子模块管理
- OpenCV 作为 Git 子模块，实现版本锁定
- 避免在主仓库中存储大量源码
- 支持团队间的版本一致性

### 3. 预编译包缓存
- Conan 本地缓存减少重复编译
- 支持二进制包共享
- 优化的构建配置减少编译时间

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

## 🚀 推荐构建方式

使用提供的构建脚本（自动处理子模块和依赖）：

```powershell
# 完整构建（包含清理和子模块初始化）
.\scripts\build.ps1 -Clean

# 快速构建
.\scripts\build.ps1
```

## 📊 Conan 缓存管理

使用缓存管理脚本优化构建性能：

```powershell
# 查看缓存状态
.\scripts\conan_cache_manager.ps1 -Action status

# 清理缓存
.\scripts\conan_cache_manager.ps1 -Action clean

# 优化缓存（删除旧版本）
.\scripts\conan_cache_manager.ps1 -Action optimize

# 导出预编译包
.\scripts\conan_cache_manager.ps1 -Action export
```

## 📋 输出示例

```
Image shape: 1080 x 810 x 3
Height: 1080
Width: 810
Channels: 3
```

## 🔧 高级配置

### conanfile.py 特性

- **条件依赖**: 根据平台和配置动态选择依赖
- **自定义选项**: 控制 OpenCV 模块和功能
- **构建优化**: 自动配置并行构建和缓存

### Git 子模块管理

```bash
# 更新子模块到最新版本
git submodule update --remote

# 切换到特定 OpenCV 版本
cd third_party/opencv
git checkout 4.12.0
cd ../..
git add third_party/opencv
git commit -m "Update OpenCV to 4.12.0"
```

## 🚀 最佳实践

1. **使用构建脚本**: 自动化处理所有依赖和配置
2. **定期清理缓存**: 避免磁盘空间占用过多
3. **版本锁定**: 通过 Git 子模块确保团队版本一致
4. **增量构建**: 利用 Conan 缓存减少重复编译
