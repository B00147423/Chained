#!/bin/bash

echo "[1/3] Cloning vcpkg..."
if [ ! -d "vcpkg" ]; then
  git clone https://github.com/microsoft/vcpkg.git
  ./vcpkg/bootstrap-vcpkg.sh
fi

echo "[2/3] Installing dependencies..."
./vcpkg/vcpkg install --overlay-ports=vendor/vcpkg-ports

echo "[3/3] Setup complete!"
