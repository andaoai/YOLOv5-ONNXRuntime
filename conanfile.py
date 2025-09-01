from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMakeDeps, cmake_layout


class InferenceProjectConan(ConanFile):
    """
    简化的 YOLOv5 ONNX 推理项目 Conan 配置
    直接使用 Conan Center 官方包，无需自定义 recipes
    """
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps"

    def requirements(self):
        """定义包依赖"""
        # 使用 Conan Center 官方 OpenCV 包
        self.requires("opencv/4.8.1")
        # 使用 Conan Center 官方 ONNX Runtime 包
        self.requires("onnxruntime/1.18.1")

    def configure(self):
        """配置选项"""
        # 配置 OpenCV 选项，简化配置避免冲突
        self.options["opencv"].contrib = False
        self.options["opencv"].dnn = False
        self.options["opencv"].with_jpeg = "libjpeg"
        self.options["opencv"].with_png = True
        self.options["opencv"].with_tiff = False
        self.options["opencv"].with_webp = False
        self.options["opencv"].with_openexr = False
        self.options["opencv"].with_protobuf = False

    def build_requirements(self):
        """定义构建时依赖"""
        self.tool_requires("cmake/[>=3.15]")

    def layout(self):
        """使用标准的 CMake 布局"""
        cmake_layout(self)
        # 如果指定了 output-folder，则不使用嵌套的 build 目录
        if self.folders.base_build:
            self.folders.build = self.folders.base_build

    def generate(self):
        """生成构建文件"""
        # 设置 C++ 标准
        tc = CMakeToolchain(self)
        tc.variables["CMAKE_CXX_STANDARD"] = "17"
        tc.variables["CMAKE_CXX_STANDARD_REQUIRED"] = "ON"
        tc.generate()
        
        # CMakeDeps 由 generators 属性自动处理
