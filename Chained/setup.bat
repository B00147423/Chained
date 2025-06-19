@echo off
echo [1/3] Checking for vcpkg...

IF NOT EXIST "vcpkg\vcpkg.exe" (
    echo Cloning vcpkg...
    git clone https://github.com/microsoft/vcpkg.git
    cd vcpkg
    call bootstrap-vcpkg.bat
    cd ..
)

echo [2/3] Installing dependencies...
vcpkg\vcpkg.exe install --overlay-ports=vendor\vcpkg-ports

echo [3/3] Setup complete!
echo -----------------------------
echo You can now open Chained.sln
echo -----------------------------
pause
