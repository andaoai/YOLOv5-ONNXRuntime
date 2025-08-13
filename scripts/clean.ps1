# PowerShell clean script for OpenCV Image Reader Project

Write-Host "========================================" -ForegroundColor Yellow
Write-Host "Cleaning OpenCV Image Reader Project" -ForegroundColor Yellow
Write-Host "========================================" -ForegroundColor Yellow

# Remove build directory
if (Test-Path "build") {
    Write-Host "Removing build directory..." -ForegroundColor Cyan
    Remove-Item "build" -Recurse -Force
    Write-Host "Build directory removed." -ForegroundColor Green
} else {
    Write-Host "Build directory does not exist." -ForegroundColor Gray
}

# Remove install directory
if (Test-Path "install") {
    Write-Host "Removing install directory..." -ForegroundColor Cyan
    Remove-Item "install" -Recurse -Force
    Write-Host "Install directory removed." -ForegroundColor Green
}

Write-Host ""
Write-Host "Clean completed!" -ForegroundColor Green
