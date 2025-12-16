@echo off
if not exist build mkdir build
g++ -std=c++17 -Iinclude src/main.cpp -o build/RaceLogic.exe
if %errorlevel% neq 0 (
    echo Build Failed!
    exit /b %errorlevel%
)
echo Build Success!
build\RaceLogic.exe
pause
