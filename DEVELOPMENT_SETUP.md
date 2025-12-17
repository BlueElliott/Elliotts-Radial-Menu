# GW2Radial - Development Setup Guide

This is your personal development fork of GW2Radial for Guild Wars 2.

## Prerequisites

✅ **Installed and Configured:**
- Visual Studio 2022 Community Edition
  - Location: `C:\Program Files\Microsoft Visual Studio\2022\Community`
  - Platform Toolset: v143
- vcpkg Package Manager
  - Location: `C:\dev\vcpkg`
  - Integrated with Visual Studio: ✅
- Git
  - User: BlueElliott
  - Email: elliott.ramdass10@gmail.com

## Project Structure

```
GW2Radial/
├── src/                  # C++ source files
├── include/              # Header files
├── shaders/              # HLSL shader files
├── extern/GW2Common/     # Git submodule with common utilities
├── art/                  # SVG icons and art assets
├── custom_examples/      # Example custom radial menus
├── build_debug.bat       # Build script for Debug configuration
├── build_simple.bat      # Simple build script
├── GW2Radial.sln         # Visual Studio solution
├── GW2Radial.vcxproj     # Project file
└── vcpkg.json            # Package dependencies
```

## Dependencies (from vcpkg.json)

The project uses these C++ libraries (automatically managed by vcpkg):
- **glm** - OpenGL Mathematics
- **directxtk** - DirectX Tool Kit
- **nlohmann-json** - JSON for Modern C++
- **simpleini** - INI file reader/writer
- **tinyxml2** - XML parser
- **xxhash** - Extremely fast hash algorithm
- **range-v3** - Range library for C++
- **atomic-queue** - Lock-free queue
- **freetype** - Font rendering
- **libzippp** - ZIP file handling

## Building the Project

### Option 1: Using Visual Studio (Recommended)
1. Open `GW2Radial.sln` in Visual Studio 2022
2. Select "Debug" or "Release" configuration
3. Select "x64" platform
4. Press F7 or Build → Build Solution
5. Output DLL will be in `bin\x64\Debug\` or `bin\x64\Release\`

### Option 2: Using Command Line
1. Open "Developer Command Prompt for VS 2022" from Start Menu
2. Navigate to project folder:
   ```cmd
   cd C:\Users\Aurora\Projects\GW2Radial
   ```
3. Run the build script:
   ```cmd
   build_debug.bat
   ```
4. Or use MSBuild directly:
   ```cmd
   msbuild GW2Radial.sln /t:Build /p:Configuration=Debug /p:Platform=x64
   ```

### First Build Notes
- **First build will take 10-30 minutes** because vcpkg compiles all dependencies from source
- Subsequent builds are much faster (< 1 minute for incremental changes)
- vcpkg caches compiled libraries in `C:\dev\vcpkg\installed\x64-windows\`

## Output Files

After a successful build:
- **DLL**: `bin\x64\Debug\gw2addon_gw2radial.dll` (or `Release`)
- **PDB**: `bin\x64\Debug\gw2addon_gw2radial.pdb` (debug symbols)

## Installing for Testing

### Manual Installation
1. Build the project (Debug or Release)
2. Ensure you have the [GW2 Addon Loader](https://github.com/gw2-addon-loader/loader-core) installed
3. Copy `gw2addon_gw2radial.dll` to:
   ```
   C:\Program Files\Guild Wars 2\addons\gw2radial\
   ```
4. Launch Guild Wars 2

### Automatic Copy (Optional)
Set an environment variable to auto-copy the DLL after each build:
1. Open System Environment Variables
2. Create new variable:
   - Name: `GW2RADIAL_TARGET_DIR`
   - Value: `C:\Program Files\Guild Wars 2` (or your GW2 install path)
3. The post-build event will automatically copy the DLL

## Development Workflow

1. **Make changes** to source files in `src/` or `include/`
2. **Build** using Visual Studio or build script
3. **Test** in Guild Wars 2
4. **Commit** your changes:
   ```bash
   git add .
   git commit -m "Description of your changes"
   ```
5. **Push** to your repository:
   ```bash
   git push
   ```

## Troubleshooting

### Build Fails - vcpkg not found
- Ensure `C:\dev\vcpkg` exists
- Run vcpkg integration again:
  ```cmd
  C:\dev\vcpkg\vcpkg integrate install
  ```

### Build Fails - Missing dependencies
- vcpkg will automatically download and build them
- First build takes much longer
- Check your internet connection

### DLL not loading in game
- Check `gw2radial.log` in your GW2 install folder
- Ensure addon loader is properly installed
- Verify the DLL is in the correct location: `GW2\addons\gw2radial\`

### Submodule issues
```bash
git submodule update --init --recursive
```

## Project Configuration

### Key Files
- **vcpkg.json**: Package dependencies
- **GW2Radial.vcxproj**: Build settings, include paths, compiler flags
- **.gitmodules**: Git submodule configuration (GW2Common)
- **Main.def**: DLL export definitions

### Compiler Settings
- C++ Standard: Latest (`stdcpplatest`)
- Runtime Library: Multi-threaded Debug (Debug) / Multi-threaded (Release)
- Platform: x64 only
- Windows Target: Windows 10

## Next Steps

1. ✅ Environment is set up
2. ✅ Dependencies configured (vcpkg)
3. ⏳ **Try building the project** using one of the methods above
4. ⏳ Create your GitHub repository
5. ⏳ Start developing your custom features!

## Useful Resources

- [Original GW2Radial Repository](https://github.com/Friendly0Fire/GW2Radial)
- [GW2 Addon Loader](https://github.com/gw2-addon-loader)
- [vcpkg Documentation](https://vcpkg.io/)
- [DirectX 11 Documentation](https://learn.microsoft.com/en-us/windows/win32/direct3d11/dx-graphics-overviews)

## License

This project is MIT licensed (same as the original). You're free to modify and distribute as you wish.
