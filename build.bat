@echo off
setlocal
cd /d "%~dp0"

set "GPP=g++"
where g++ >nul 2>&1
if errorlevel 1 (
    if exist "C:\laragon\bin\w64devkit\w64devkit\bin\g++.exe" (
        set "GPP=C:\laragon\bin\w64devkit\w64devkit\bin\g++.exe"
    ) else (
        echo g++ was not found.
        echo Install MinGW, then open a new terminal and run this file again.
        exit /b 1
    )
)

"%GPP%" -std=c++17 -Wall -Wextra -pedantic -static-libgcc -static-libstdc++ -I include -o shop.exe src\main.cpp src\Product.cpp src\InventorySystem.cpp
if errorlevel 1 (
    echo.
    echo Build failed.
    exit /b 1
)

echo.
echo Build succeeded. From this folder, run: shop.exe
exit /b 0
