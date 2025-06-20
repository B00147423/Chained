# Chained Project

## Quick Start
1. Clone this repository
2. Run `x64/Release/Chained.exe`
3. Enjoy!

## For Developers
If you want to build from source:
1. Install Visual Studio and vcpkg
2. Run `vcpkg install --triplet x64-windows`
3. Open `Chained.sln` and build

## Prerequisites

- Visual Studio 2022 (or later)
- [vcpkg](https://github.com/Microsoft/vcpkg) installed and integrated with Visual Studio (`vcpkg integrate install`)

## Build Instructions (Manual Setup)

This project requires manual installation of dependencies via vcpkg.

### 1. Clone the Repository
```bash
git clone <your-repo-url>
cd Chained
```

### 2. Install Dependencies Manually
Navigate to the project directory (the one containing `vcpkg.json`) and run the following command. This will read the `vcpkg.json` file and install all required libraries.

```bash
# For 64-bit Windows
vcpkg install --triplet x64-windows
```

### 3. Build the Project
- Open `Chained.sln` in Visual Studio.
- Build the solution (Ctrl+Shift+B).

## Dependencies

- **Handled by vcpkg:** See `vcpkg.json` for the full list. You must install these manually using the command above.
- **Vendored in repo:** `ImGuizmo` is included directly in the `vendor/` folder and does not require installation.

## Project Structure

- `src/` - Source code
- `assets/` - Game assets (textures, shaders, etc.)
- `vendor/` - Third-party libraries not in vcpkg (ImGuizmo)
- `vcpkg.json` - Dependency manifest (installs automatically)

## Troubleshooting

If you get build errors:
1. Make sure vcpkg is properly installed and integrated
2. Clean and rebuild the solution
3. Check that vcpkg integration is enabled in Visual Studio 