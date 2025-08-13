from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMakeDeps, cmake_layout
from conan.tools.files import copy, get, rmdir
from conan.tools.scm import Git
import os


class OpenCVImageProcessingConan(ConanFile):
    name = "opencv-image-processing"
    version = "1.0.0"

    # Package metadata
    description = "OpenCV Image Processing Project with modern Conan dependency management"
    topics = ("opencv", "image-processing", "computer-vision", "cpp")
    url = "https://github.com/your-org/opencv-image-processing"
    license = "MIT"
    
    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "with_opencv_contrib": [True, False],
        "opencv_version": ["4.12.0", "4.10.0", "4.8.1"],
        "enable_testing": [True, False],
    }
    default_options = {
        "shared": False,
        "with_opencv_contrib": False,
        "opencv_version": "4.12.0",
        "enable_testing": False,
    }
    
    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "src/*", "include/*", "assets/*", "CMakeLists.txt"
    
    def layout(self):
        cmake_layout(self)
    
    def requirements(self):
        """Define package dependencies"""
        # For production use, we would use:
        # self.requires(f"opencv/{self.options.opencv_version}")
        
        # Since we're using OpenCV as a git submodule, we don't add it as a requirement
        # but we can add other dependencies here
        pass
    
    def build_requirements(self):
        """Define build-time dependencies"""
        self.tool_requires("cmake/[>=3.15]")
        if self.settings.os == "Windows":
            self.tool_requires("pkgconf/1.7.4")
    
    def configure(self):
        """Configure package options based on settings"""
        if self.settings.compiler.cppstd:
            # Ensure C++17 or higher
            if str(self.settings.compiler.cppstd) < "17":
                self.settings.compiler.cppstd = "17"
    
    def generate(self):
        """Generate necessary files for the build"""
        # Generate CMake toolchain and dependencies
        tc = CMakeToolchain(self)
        
        # Add custom CMake definitions
        tc.variables["CMAKE_CXX_STANDARD"] = "17"
        tc.variables["CMAKE_CXX_STANDARD_REQUIRED"] = "ON"
        
        # OpenCV build configuration - 极简配置，只构建必要模块
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

        # 禁用硬件加速和第三方库支持
        tc.variables["WITH_CUDA"] = "OFF"
        tc.variables["WITH_OPENCL"] = "OFF"
        tc.variables["WITH_IPP"] = "OFF"
        tc.variables["WITH_TBB"] = "OFF"
        tc.variables["WITH_EIGEN"] = "OFF"
        tc.variables["WITH_LAPACK"] = "OFF"

        # 禁用视频和媒体相关功能
        tc.variables["WITH_FFMPEG"] = "OFF"
        tc.variables["WITH_GSTREAMER"] = "OFF"
        tc.variables["WITH_V4L"] = "OFF"
        tc.variables["WITH_DSHOW"] = "OFF"

        # 只构建最核心的模块
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
        
        # Enable contrib modules if requested
        if self.options.with_opencv_contrib:
            tc.variables["OPENCV_EXTRA_MODULES_PATH"] = os.path.join(
                self.source_folder, "third_party", "opencv_contrib", "modules"
            )
        
        # Configure for faster builds
        tc.variables["CMAKE_BUILD_PARALLEL_LEVEL"] = str(os.cpu_count())
        
        tc.generate()
        
        # Generate CMake dependencies
        deps = CMakeDeps(self)
        deps.generate()
    
    def build(self):
        """Build the package"""
        from conan.tools.cmake import CMake
        
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        
        # Run tests if enabled
        if self.options.enable_testing:
            cmake.test()
    
    def package(self):
        """Package the built artifacts"""
        from conan.tools.cmake import CMake
        
        cmake = CMake(self)
        cmake.install()
        
        # Copy license
        copy(self, "LICENSE", src=self.source_folder, dst=os.path.join(self.package_folder, "licenses"))
        
        # Copy assets
        copy(self, "*", src=os.path.join(self.source_folder, "assets"), 
             dst=os.path.join(self.package_folder, "assets"))
    
    def package_info(self):
        """Define package information for consumers"""
        self.cpp_info.libs = ["opencv_image_reader"]
        self.cpp_info.includedirs = ["include"]
        
        # Add runtime paths for assets
        self.cpp_info.bindirs = ["bin"]
        self.runenv_info.define_path("OPENCV_IMAGE_READER_ASSETS", 
                                   os.path.join(self.package_folder, "assets"))
