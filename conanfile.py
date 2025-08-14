from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMakeDeps, cmake_layout


class InferenceProjectConan(ConanFile):
    """
    消费者 conanfile.py - 使用 recipes 文件夹下的 OpenCV 和 ONNX Runtime 包
    """
    settings = "os", "compiler", "build_type", "arch"

    def requirements(self):
        """定义包依赖"""
        # 引用 recipes 文件夹下的自定义包
        self.requires("opencv/4.12.0")
        # 使用 Conan 官方的 ONNX Runtime 包
        self.requires("onnxruntime/1.18.1")

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
