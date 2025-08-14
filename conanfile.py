from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMakeDeps, cmake_layout


class InferenceProjectConan(ConanFile):
    """
    消费者 conanfile.py - 使用 recipes 文件夹下的 OpenCV 和 ONNX Runtime 包
    """
    settings = "os", "compiler", "build_type", "arch"

    def requirements(self):
        """定义包依赖"""
        # 使用 OpenCV 从源码构建，简化配置
        self.requires("opencv/4.8.1")
        # 暂时注释掉 ONNX Runtime，专注于 OpenCV 构建
        # self.requires("onnxruntime/1.18.1")

    def configure(self):
        """配置选项"""
        # 配置 OpenCV 选项，禁用一些可能引起冲突的功能
        self.options["opencv"].contrib = False
        self.options["opencv"].dnn = False
        self.options["opencv"].with_jpeg = "libjpeg"  # 使用标准 libjpeg
        self.options["opencv"].with_png = True
        self.options["opencv"].with_tiff = False  # 禁用 TIFF 支持来避免 libjpeg 冲突
        self.options["opencv"].with_webp = False
        self.options["opencv"].with_openexr = False
        self.options["opencv"].with_protobuf = False

    def build_requirements(self):
        """定义构建时依赖"""
        self.tool_requires("cmake/[>=3.15]")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        """生成构建文件"""
        # 生成 CMake 工具链
        tc = CMakeToolchain(self)
        tc.variables["CMAKE_CXX_STANDARD"] = "17"
        tc.variables["CMAKE_CXX_STANDARD_REQUIRED"] = "ON"
        tc.generate()

        # 生成 CMake 依赖文件
        deps = CMakeDeps(self)
        deps.generate()
