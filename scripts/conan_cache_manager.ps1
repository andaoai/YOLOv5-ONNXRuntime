# Conan Cache Management Script
# This script helps manage Conan cache and binary packages

param(
    [string]$Action = "status",
    [switch]$Help = $false
)

if ($Help) {
    Write-Host "Conan Cache Manager" -ForegroundColor Green
    Write-Host ""
    Write-Host "Usage: .\scripts\conan_cache_manager.ps1 [-Action <action>] [-Help]"
    Write-Host ""
    Write-Host "Actions:"
    Write-Host "  status     Show cache status and disk usage"
    Write-Host "  clean      Clean cache (removes all cached packages)"
    Write-Host "  optimize   Optimize cache (remove old revisions)"
    Write-Host "  backup     Create backup of current cache"
    Write-Host "  restore    Restore cache from backup"
    Write-Host "  export     Export built packages for sharing"
    Write-Host "  import     Import shared packages"
    Write-Host ""
    exit 0
}

function Show-CacheStatus {
    Write-Host "=== Conan Cache Status ===" -ForegroundColor Cyan
    
    # Get Conan home directory
    $conanHome = $env:CONAN_HOME
    if (-not $conanHome) {
        $conanHome = "$env:USERPROFILE\.conan2"
    }
    
    Write-Host "Conan Home: $conanHome" -ForegroundColor Yellow
    
    if (Test-Path $conanHome) {
        $cacheSize = (Get-ChildItem $conanHome -Recurse -File | Measure-Object -Property Length -Sum).Sum
        $cacheSizeMB = [math]::Round($cacheSize / 1MB, 2)
        Write-Host "Cache Size: $cacheSizeMB MB" -ForegroundColor Yellow
        
        # Show package count
        $packageDir = Join-Path $conanHome "p"
        if (Test-Path $packageDir) {
            $packageCount = (Get-ChildItem $packageDir -Directory).Count
            Write-Host "Cached Packages: $packageCount" -ForegroundColor Yellow
        }
    } else {
        Write-Host "Cache directory does not exist" -ForegroundColor Red
    }
    
    # Show Conan configuration
    Write-Host ""
    Write-Host "=== Conan Configuration ===" -ForegroundColor Cyan
    conan config show
}

function Clean-Cache {
    Write-Host "=== Cleaning Conan Cache ===" -ForegroundColor Yellow
    Write-Host "This will remove all cached packages. Continue? (y/N): " -NoNewline
    $confirm = Read-Host
    
    if ($confirm -eq "y" -or $confirm -eq "Y") {
        conan cache clean "*"
        Write-Host "Cache cleaned successfully!" -ForegroundColor Green
    } else {
        Write-Host "Cache cleaning cancelled." -ForegroundColor Gray
    }
}

function Optimize-Cache {
    Write-Host "=== Optimizing Conan Cache ===" -ForegroundColor Cyan
    
    # Remove old revisions (keep only latest)
    Write-Host "Removing old package revisions..." -ForegroundColor Yellow
    conan cache clean "*" --older-than=30d
    
    Write-Host "Cache optimization completed!" -ForegroundColor Green
}

function Backup-Cache {
    Write-Host "=== Creating Cache Backup ===" -ForegroundColor Cyan
    
    $backupDir = "conan_cache_backup_$(Get-Date -Format 'yyyyMMdd_HHmmss')"
    $conanHome = $env:CONAN_HOME
    if (-not $conanHome) {
        $conanHome = "$env:USERPROFILE\.conan2"
    }
    
    if (Test-Path $conanHome) {
        Write-Host "Creating backup: $backupDir" -ForegroundColor Yellow
        Copy-Item $conanHome $backupDir -Recurse
        Write-Host "Backup created successfully!" -ForegroundColor Green
    } else {
        Write-Host "No cache directory found to backup" -ForegroundColor Red
    }
}

function Export-Packages {
    Write-Host "=== Exporting Built Packages ===" -ForegroundColor Cyan
    
    $exportDir = "exported_packages"
    if (-not (Test-Path $exportDir)) {
        New-Item -ItemType Directory -Name $exportDir | Out-Null
    }
    
    # Export OpenCV package if built
    Write-Host "Exporting OpenCV packages..." -ForegroundColor Yellow
    conan cache path opencv* | ForEach-Object {
        if ($_ -and (Test-Path $_)) {
            $packageName = Split-Path $_ -Leaf
            $exportPath = Join-Path $exportDir "$packageName.tgz"
            tar -czf $exportPath -C $_ .
            Write-Host "Exported: $exportPath" -ForegroundColor Green
        }
    }
    
    Write-Host "Package export completed!" -ForegroundColor Green
}

# Main script logic
switch ($Action.ToLower()) {
    "status" { Show-CacheStatus }
    "clean" { Clean-Cache }
    "optimize" { Optimize-Cache }
    "backup" { Backup-Cache }
    "export" { Export-Packages }
    default {
        Write-Host "Unknown action: $Action" -ForegroundColor Red
        Write-Host "Use -Help to see available actions" -ForegroundColor Gray
        exit 1
    }
}
