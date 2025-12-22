# GW2Radial Build Documentation

## Build Issue Summary

**CRITICAL**: The current codebase **CANNOT** be built due to dependency version mismatches. This document explains the issues and potential solutions.

## The v1.0.0 Release Mystery

The v1.0.0 release (38MB DLL, published Dec 18, 2025) works perfectly and includes the TemplateWheel feature. However, **we cannot reproduce this build** from the current repository state.

### What We Know About v1.0.0

1. **Git Tag**: Points to commit `aa26d90` ("Disable GitHub Actions")
2. **Working DLL**: Available at https://github.com/BlueElliott/Elliotts-Radial-Menu/releases/tag/v1.0.0
3. **DLL Size**: 38MB (neither Debug 52MB nor Release 41MB match this)
4. **Location**: Works when placed at `/c/Program Files/Guild Wars 2/addons/gw2radial/gw2addon_gw2radial.dll`
5. **Code State**: Uses old API with `Direct3D11Loader` and `neargye/semver.hpp`

### Critical Dependency Issues

#### 1. GW2Common Submodule State

The v1.0.0 code at `src/Main.cpp` includes:
```cpp
#include <Direct3D11Loader.h>

gw2al_api_ret gw2addon_load(gw2al_core_vtable* core_api)
{
    Direct3D11Loader::reset();
    Direct3D11Loader::i().Init(core_api);
    return GW2AL_OK;
}
```

**Problem**: The GW2Common submodule is recorded at commit `46ecca8` in git, but this commit doesn't have `Direct3D11Loader`. It was removed in commit `dd3552f` ("Many changes and fixes, new addon api").

**Current Working State**: The actual working directory has GW2Common at commit `2a2fca4` ("Moved D3D11 loader") which DOES have Direct3D11Loader.

```bash
$ cd extern/GW2Common && git log --oneline -1
2a2fca4 Moved font resources. Moved most Core functionality. Moved D3D11 loader.
```

#### 2. vcpkg neargye-semver Version Mismatch

The code expects:
```cpp
#include <neargye/semver.hpp>
const semver::version& GetAddonVersion();  // Regular class
```

**Problem**: Current vcpkg's `neargye-semver` package:
- Is installed at `semver.hpp` (not `neargye/semver.hpp`)
- Changed `semver::version` from a regular class to a template class
- Breaks API compatibility with old code

**Workaround Attempted**: Created `neargye/` directory and copied `semver.hpp`
- Result: File found but API is incompatible (template errors)

#### 3. Submodule Dependencies

GW2Common requires:
- imgui (submodule) ✅ Can be initialized
- ziplib (submodule) ✅ Can be initialized
- neargye-semver (vcpkg) ❌ API incompatible
- glm, directxtk, nlohmann-json, etc. (vcpkg) ✅ Can be installed

### Build Commands Tried

```bash
# Clean build
"/c/Program Files/Microsoft Visual Studio/2022/Community/MSBuild/Current/Bin/MSBuild.exe" \
  GW2Radial.vcxproj -p:Configuration=Debug -p:Platform=x64 -t:Clean

# Debug build (produces 52MB DLL)
"/c/Program Files/Microsoft Visual Studio/2022/Community/MSBuild/Current/Bin/MSBuild.exe" \
  GW2Radial.vcxproj -p:Configuration=Debug -p:Platform=x64 -v:minimal

# Release build (produces 41MB DLL)
"/c/Program Files/Microsoft Visual Studio/2022/Community/MSBuild/Current/Bin/MSBuild.exe" \
  GW2Radial.vcxproj -p:Configuration=Release -p:Platform=x64 -v:minimal

# With vcpkg manifest enabled
"/c/Program Files/Microsoft Visual Studio/2022/Community/MSBuild/Current/Bin/MSBuild.exe" \
  GW2Radial.vcxproj -p:Configuration=Debug -p:Platform=x64 \
  -p:VcpkgEnableManifest=true -p:VcpkgTriplet=x64-windows-static -v:minimal
```

**Result**: All builds fail with dependency errors.

### Required Build Environment (Best Guess)

To reproduce v1.0.0, you would need:

1. **GW2Common at commit 2a2fca4**:
   ```bash
   cd extern/GW2Common
   git checkout 2a2fca4
   git submodule update --init --recursive
   ```

2. **Old vcpkg with compatible neargye-semver**:
   - The package must install to `neargye/semver.hpp` (not `semver.hpp`)
   - The `semver::version` must be a regular class (not a template)
   - Unknown which vcpkg version/date has this

3. **Unknown build configuration**:
   - The 38MB DLL size doesn't match Debug (52MB) or Release (41MB)
   - Suggests custom build settings or different optimization level

## Potential Solutions

### Option 1: Find the Original Build Environment

- Determine exact vcpkg version used for v1.0.0
- Check if there's a vcpkg-configuration.json or vcpkg.lock
- Try building on the exact same machine/environment as v1.0.0

### Option 2: Update Code to Current APIs

Update `extern/GW2Common` to commit `46ecca8` and modify code:

1. Remove `Direct3D11Loader` dependency from [src/Main.cpp](src/Main.cpp)
2. Update to new `GW2Load_API` initialization pattern
3. Update [include/Core.h](include/Core.h) to match new `InnerInternalInit(GW2Load_API& api)` signature
4. Replace neargye-semver usage with compatible version handling

This would require testing to ensure no runtime compatibility issues.

### Option 3: Pin vcpkg to Old Version

- Find vcpkg commit with compatible neargye-semver
- Use vcpkg baseline or version constraints
- Add vcpkg-configuration.json to lock dependencies

## **SOLUTION FOUND! ✅**

### Working Build Process

The build issue has been **SOLVED**. The key was using the correct GW2Common commit.

**Correct GW2Common commit**: `1a7253a` ("Fix build compatibility: PowerShell 5.1 support")
- This commit HAS `Direct3D11Loader.h` ✅
- This commit does NOT have `Defs.h` with `neargye/semver.hpp` dependency ✅
- This is the commit used by the successful build at `70932d8` ✅

### Build Commands That Work

```bash
# 1. Set GW2Common to correct commit
cd extern/GW2Common
git checkout 1a7253a
git submodule update --init --recursive --force

# 2. Build GW2Radial
cd ../..
"/c/Program Files/Microsoft Visual Studio/2022/Community/MSBuild/Current/Bin/MSBuild.exe" \
  GW2Radial.vcxproj -p:Configuration=Debug -p:Platform=x64 -v:minimal

# 3. Copy resource file if needed (first build only)
mkdir -p int/GW2Common/x64/Debug
cp extern/GW2Common/int/GW2Common/x64/Debug/BaseResource.res int/GW2Common/x64/Debug/

# 4. Deploy to GW2
cp bin/x64/Debug/gw2addon_gw2radial.dll "/c/Program Files/Guild Wars 2/addons/gw2radial/"
```

### Build Output

- **DLL Size**: 52MB (Debug build)
- **Location**: `bin\x64\Debug\gw2addon_gw2radial.dll`
- **Deployed**: `/c/Program Files/Guild Wars 2/addons/gw2radial/`

## Current State

- **Working v1.0.0 DLL**: ✅ Downloaded and works in game
- **Can build from source**: ✅ **SOLVED** - Using GW2Common commit 1a7253a
- **Can modify and rebuild**: ✅ **READY** - Build process works
- **GW2Common commit**: **1a7253a** (the correct one)
- **vcpkg state**: Not needed with this GW2Common version

## Deployment Location

The DLL must be placed at:
```
C:\Program Files\Guild Wars 2\addons\gw2radial\gw2addon_gw2radial.dll
```

NOT at:
```
C:\Program Files\Guild Wars 2\addons\gw2addon_gw2radial.dll  # Wrong location
```

## Next Steps

**Before any feature development can continue**, we MUST:

1. Determine how v1.0.0 was actually built
2. Document and reproduce that build process
3. OR update all code to work with current dependencies
4. Test that builds actually load in GW2 without crashing

**Until the build process is fixed, no modifications can be made to the codebase.**

---

## Repository State Analysis

### Git Submodule Status
```bash
$ git submodule status
+2a2fca4a048efcba4a70f95c829708cac14028ed extern/GW2Common (2a2fca4)
```

The `+` indicates uncommitted changes. The submodule is at 2a2fca4 but git records 46ecca8 in `.gitmodules`.

### GW2Common vcpkg Dependencies

From `extern/GW2Common/vcpkg.json` at commit 2a2fca4:
```json
{
  "name": "gw2common",
  "version": "0.0.1",
  "dependencies": [
    "glm",
    "directxtk",
    "nlohmann-json",
    "simpleini",
    "tinyxml2",
    "xxhash",
    "neargye-semver"
  ]
}
```

### Build History

From git history, the last successful builds were likely:
- Commit `5401628`: "Add Template Wheel feature"
- Commit `70932d8`: "Build successful! Fixed compatibility issues"

These commits might have build documentation or working configurations.

---

_Document created: 2025-12-22_
_Last verified build: v1.0.0 (Dec 18, 2025)_
_Status: Build process cannot be reproduced from current repository state_
