@echo off
echo ========================================
echo Axy-GS-8.51 Build Script
echo ========================================
echo.

REM Check if Visual Studio is installed
where msbuild >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: MSBuild not found. Please install Visual Studio 2022 with C++ build tools.
    pause
    exit /b 1
)

echo Cleaning build directory...
if exist x64 rmdir /s /q x64
if exist Volcano-8.51\*.ilk del /q Volcano-8.51\*.ilk
if exist Volcano-8.51\*.pdb del /q Volcano-8.51\*.pdb

echo.
echo Building Axy-GS-8.51.dll (Release/x64)...
echo.

cd VolcanoV2
msbuild Volcano-8.51.sln /p:Configuration=Release /p:Platform=x64 /v:minimal /nologo

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo BUILD SUCCESSFUL!
    echo ========================================
    echo.
    echo Output DLL: x64\Release\Axy-GS-8.51.dll
    echo.
    echo Ready for injection into Project Reboot 3.0!
    echo ========================================
) else (
    echo.
    echo ========================================
    echo BUILD FAILED!
    echo ========================================
    echo.
    echo Please check the error messages above.
    echo Common issues:
    echo   - Missing SDK files
    echo   - Missing library files (libcurl.lib, zlib.lib)
    echo   - Visual Studio 2022 not properly installed
    echo   - C++20 language standard not supported
    echo.
)

cd ..
pause
