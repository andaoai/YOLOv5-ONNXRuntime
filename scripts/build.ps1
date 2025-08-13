# PowerShell build script for OpenCV Image Reader Project

param(
    [string]$BuildType = "Release",
    [switch]$Clean = $false,
    [switch]$Help = $false
)

if ($Help) {
    Write-Host "Usage: .\scripts\build.ps1 [-BuildType <Debug|Release>] [-Clean] [-Help]"
    Write-Host ""
    Write-Host "Parameters:"
    Write-Host "  -BuildType    Build configuration (Debug or Release, default: Release)"
    Write-Host "  -Clean        Clean build directory before building"
    Write-Host "  -Help         Show this help message"
    exit 0
}

Write-Host "========================================" -ForegroundColor Green
Write-Host "Building OpenCV Image Reader Project" -ForegroundColor Green
Write-Host "Build Type: $BuildType" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green

# Set environment variables
$env:PATH = "C:\mingw64\bin;" + $env:PATH

# Check MinGW installation
if (!(Test-Path "C:\mingw64\bin\gcc.exe")) {
    Write-Error "MinGW-w64 not found at C:\mingw64"
    Write-Host "Please install MinGW-w64 and ensure it's located at C:\mingw64"
    exit 1
}

# Check required files
if (!(Test-Path "third_party\opencv-4.12.0\CMakeLists.txt")) {
    Write-Error "OpenCV source not found in third_party\opencv-4.12.0"
    exit 1
}

if (!(Test-Path "assets\images\bus.jpg")) {
    Write-Warning "Test image bus.jpg not found in assets\images\"
    Write-Host "The program may fail to load the image"
}

# Clean build directory if requested
if ($Clean -and (Test-Path "build")) {
    Write-Host "Cleaning build directory..." -ForegroundColor Yellow
    Remove-Item "build" -Recurse -Force
}

# Create build directory
if (!(Test-Path "build")) {
    New-Item -ItemType Directory -Name "build" | Out-Null
}

Set-Location "build"

try {
    Write-Host ""
    Write-Host "Step 1: Installing Conan dependencies..." -ForegroundColor Cyan
    conan install .. --build=missing --profile=../conanprofile
    if ($LASTEXITCODE -ne 0) {
        throw "Conan install failed"
    }

    Write-Host ""
    Write-Host "Step 2: Configuring CMake..." -ForegroundColor Cyan
    cmake .. -G "MinGW Makefiles" `
        -DCMAKE_TOOLCHAIN_FILE="Release/generators/conan_toolchain.cmake" `
        -DCMAKE_C_COMPILER=C:/mingw64/bin/gcc.exe `
        -DCMAKE_CXX_COMPILER=C:/mingw64/bin/g++.exe `
        -DCMAKE_MAKE_PROGRAM=C:/mingw64/bin/make.exe `
        -DCMAKE_BUILD_TYPE=$BuildType
    
    if ($LASTEXITCODE -ne 0) {
        throw "CMake configuration failed"
    }

    Write-Host ""
    Write-Host "Step 3: Building project..." -ForegroundColor Cyan
    cmake --build . --config $BuildType
    
    if ($LASTEXITCODE -ne 0) {
        throw "Build failed"
    }

    Write-Host ""
    Write-Host "========================================" -ForegroundColor Green
    Write-Host "Build completed successfully!" -ForegroundColor Green
    Write-Host "========================================" -ForegroundColor Green
    Write-Host ""
    Write-Host "Executable location: build\bin\main.exe" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "To run the program:" -ForegroundColor Yellow
    Write-Host "  cd .." -ForegroundColor Gray
    Write-Host "  .\build\bin\main.exe" -ForegroundColor Gray
    Write-Host ""

} catch {
    Write-Error "Build failed: $_"
    Set-Location ".."
    exit 1
}

Set-Location ".."
