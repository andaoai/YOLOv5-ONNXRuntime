from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMakeToolchain, CMakeDeps


class OnnxruntimeConan(ConanFile):
    name = "onnxruntime-wrapper"
    version = "1.14.1"

    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        self.requires("onnxruntime/1.14.1")


    def configure(self):
        # MinGW 特定配置
        if self.settings.compiler == "gcc":
            # 确保使用正确的 C++ 标准库
            if self.settings.compiler.libcxx == "libstdc++":
                self.settings.compiler.libcxx = "libstdc++11"

    def generate(self):
        # 生成 CMake 工具链和依赖
        tc = CMakeToolchain(self)
        # 添加 CMake 策略来解决兼容性问题
        tc.variables["CMAKE_POLICY_VERSION_MINIMUM"] = "3.5"
        tc.variables["CMAKE_POLICY_DEFAULT_CMP0000"] = "NEW"
        # 添加全局 CMake 参数
        tc.cache_variables["CMAKE_POLICY_VERSION_MINIMUM"] = "3.5"
        tc.generate()

        deps = CMakeDeps(self)
        deps.generate()

    def layout(self):
        cmake_layout(self)


