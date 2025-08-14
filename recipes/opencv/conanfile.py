from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout
from conan.tools.files import copy, get, rmdir, save
from conan.tools.scm import Git
import os


class OpenCVConan(ConanFile):
    name = "opencv"
    version = "4.12.0"

    # Package metadata
    description = "OpenCV (Open Source Computer Vision Library) from source"
    topics = ("opencv", "computer-vision", "image-processing", "cpp")
    url = "https://github.com/opencv/opencv"
    license = "Apache-2.0"
    
    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "with_contrib": [True, False],
        "with_cuda": [True, False],
        "with_opencl": [True, False],
        "with_tbb": [True, False],
        "with_eigen": [True, False],
        "with_ffmpeg": [True, False],
        "minimal_build": [True, False],  # 极简构建选项
    }
    default_options = {
        "shared": False,
        "fPIC": True,
        "with_contrib": False,
        "with_cuda": False,
        "with_opencl": False,
        "with_tbb": False,
        "with_eigen": False,
        "with_ffmpeg": False,
        "minimal_build": True,  # 默认极简构建
    }

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def layout(self):
        cmake_layout(self)

    def source(self):
        """从 GitHub 获取 OpenCV 源码"""
        git = Git(self)
        # 克隆主仓库
        git.clone(url="https://github.com/opencv/opencv.git", target=".")
        # 检出指定版本的标签
        git.checkout(f"{self.version}")

        # 总是克隆 contrib 仓库，在 generate() 方法中根据选项决定是否使用
        contrib_folder = os.path.join(self.source_folder, "opencv_contrib")
        git_contrib = Git(self, folder=contrib_folder)
        git_contrib.clone(url="https://github.com/opencv/opencv_contrib.git", target=".")
        git_contrib.checkout(f"{self.version}")

    def generate(self):
        """生成构建配置"""
        tc = CMakeToolchain(self)
        
        # 基本配置
        tc.variables["CMAKE_CXX_STANDARD"] = "17"
        tc.variables["CMAKE_CXX_STANDARD_REQUIRED"] = "ON"
        tc.variables["BUILD_SHARED_LIBS"] = self.options.shared
        
        # 极简构建配置
        if self.options.minimal_build:
            # 禁用示例、测试和文档
            tc.variables["BUILD_EXAMPLES"] = "OFF"
            tc.variables["BUILD_TESTS"] = "OFF"
            tc.variables["BUILD_PERF_TESTS"] = "OFF"
            tc.variables["BUILD_DOCS"] = "OFF"
            tc.variables["BUILD_opencv_apps"] = "OFF"

            # 禁用语言绑定
            tc.variables["BUILD_opencv_python2"] = "OFF"
            tc.variables["BUILD_opencv_python3"] = "OFF"
            tc.variables["BUILD_opencv_java"] = "OFF"

            # 只构建核心模块
            tc.variables["BUILD_opencv_calib3d"] = "OFF"
            tc.variables["BUILD_opencv_dnn"] = "OFF"
            tc.variables["BUILD_opencv_features2d"] = "OFF"
            tc.variables["BUILD_opencv_flann"] = "OFF"
            tc.variables["BUILD_opencv_gapi"] = "OFF"
            tc.variables["BUILD_opencv_ml"] = "OFF"
            tc.variables["BUILD_opencv_objdetect"] = "OFF"
            tc.variables["BUILD_opencv_photo"] = "OFF"
            tc.variables["BUILD_opencv_stitching"] = "OFF"
            tc.variables["BUILD_opencv_video"] = "OFF"
            tc.variables["BUILD_opencv_videoio"] = "OFF"
        
        # 硬件加速选项
        tc.variables["WITH_CUDA"] = self.options.with_cuda
        tc.variables["WITH_OPENCL"] = self.options.with_opencl
        tc.variables["WITH_TBB"] = self.options.with_tbb
        tc.variables["WITH_EIGEN"] = self.options.with_eigen
        tc.variables["WITH_IPP"] = "OFF"  # 默认关闭 IPP
        
        # 媒体支持
        tc.variables["WITH_FFMPEG"] = self.options.with_ffmpeg
        tc.variables["WITH_GSTREAMER"] = "OFF"
        tc.variables["WITH_V4L"] = "OFF"
        tc.variables["WITH_DSHOW"] = "OFF"
        
        # Contrib 模块
        if self.options.with_contrib:
            tc.variables["OPENCV_EXTRA_MODULES_PATH"] = os.path.join(
                self.source_folder, "opencv_contrib", "modules"
            )
        
        # 优化构建速度
        tc.variables["CMAKE_BUILD_PARALLEL_LEVEL"] = str(os.cpu_count())
        
        tc.generate()

    def build(self):
        """构建 OpenCV"""
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        """打包 OpenCV"""
        cmake = CMake(self)
        cmake.install()
        
        # 复制许可证
        copy(self, "LICENSE", src=self.source_folder, 
             dst=os.path.join(self.package_folder, "licenses"))

    def package_info(self):
        """定义包信息供消费者使用"""
        # 设置库名称（根据极简构建选项）
        if self.options.minimal_build:
            # 只包含核心模块
            self.cpp_info.libs = [
                "opencv_core",
                "opencv_imgproc", 
                "opencv_imgcodecs",
                "opencv_highgui"
            ]
        else:
            # 包含所有构建的模块
            self.cpp_info.libs = [
                "opencv_core", "opencv_imgproc", "opencv_imgcodecs", 
                "opencv_highgui", "opencv_calib3d", "opencv_features2d",
                "opencv_flann", "opencv_ml", "opencv_objdetect",
                "opencv_photo", "opencv_stitching", "opencv_video",
                "opencv_videoio"
            ]
        
        # 设置包含目录
        self.cpp_info.includedirs = ["include"]
        
        # 设置库目录
        self.cpp_info.libdirs = ["lib"]
        
        # 设置二进制目录
        self.cpp_info.bindirs = ["bin"]
        
        # 系统库依赖
        if self.settings.os == "Linux":
            self.cpp_info.system_libs.extend(["pthread", "m", "dl"])
        elif self.settings.os == "Windows":
            self.cpp_info.system_libs.extend(["ws2_32", "user32", "gdi32", "comdlg32", "advapi32", "shell32", "ole32", "oleaut32", "uuid", "odbc32", "odbccp32"])
