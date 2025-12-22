# How to Fix the Build Process

This document contains actionable steps to investigate and potentially fix the build issues documented in BUILD_DOCUMENTATION.md.

## Investigation Steps

### Step 1: Check for vcpkg Lock Files

Look for vcpkg version constraints that might reveal the exact package versions used:

```bash
# Check for vcpkg lock files
find . -name "vcpkg.lock" -o -name "vcpkg-configuration.json"

# Check git history for these files
git log --all --full-history -- "*vcpkg.lock" "*vcpkg-configuration.json"
```

### Step 2: Examine Successful Build Commits

Check commits that mention successful builds:

```bash
# View commit that claims "Build successful!"
git show 70932d8

# Check what changed in GW2Common at that time
cd extern/GW2Common
git log --oneline --graph 70932d8^..70932d8
```

### Step 3: Find the Exact GW2Common Commit for v1.0.0

The v1.0.0 tag should record the exact submodule commit:

```bash
# Check submodule commit at v1.0.0 tag
git ls-tree v1.0.0 extern/GW2Common

# This should show the actual commit hash that v1.0.0 expects
```

### Step 4: Search for Build Scripts or CI Configuration

```bash
# Look for any build automation
find . -name "*.sh" -o -name "*.bat" -o -name "*.ps1" | xargs grep -l "MSBuild\|vcpkg"

# Check for GitHub Actions (even though it was "disabled")
git log --all --full-history -- ".github/workflows/*"
```

### Step 5: Check Original GW2Radial Repository

The codebase is forked from https://github.com/Friendly0Fire/GW2Radial. Check if the original repo has:

1. Working build instructions
2. vcpkg configuration
3. Compatible GW2Common version
4. Build scripts or CI

```bash
# Add original repo as remote if not already added
git remote add upstream https://github.com/Friendly0Fire/GW2Radial.git
git fetch upstream

# Check their latest buildable version
git log upstream/master --oneline -20
```

## Potential Fixes

### Fix Option A: Downgrade vcpkg neargye-semver

If you can find the old version of neargye-semver that had the regular class API:

1. Add `vcpkg-configuration.json`:
```json
{
  "$schema": "https://raw.githubusercontent.com/microsoft/vcpkg-tool/main/docs/vcpkg-configuration.schema.json",
  "default-registry": {
    "kind": "git",
    "repository": "https://github.com/microsoft/vcpkg",
    "baseline": "COMMIT_HASH_HERE"
  },
  "registries": [],
  "overrides": [
    {
      "name": "neargye-semver",
      "version": "OLD_VERSION_HERE"
    }
  ]
}
```

2. Find the right baseline by checking vcpkg history:
```bash
cd C:/dev/vcpkg
git log --all --full-history -- ports/neargye-semver
```

### Fix Option B: Update Code to New GW2Common API

This is more invasive but makes the code compatible with current dependencies:

#### Changes to src/Main.cpp:

```cpp
// Remove this:
#include <Direct3D11Loader.h>

// Change gw2addon_load to:
HMODULE g_hModule = nullptr;

bool WINAPI DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            g_hModule = hModule;  // Save for later
            g_logStream = std::ofstream("gw2radial.log");
            // DON'T call GW2Radial::Core::Init here
            break;
        case DLL_PROCESS_DETACH:
            break;
    }
    return true;
}

gw2al_api_ret gw2addon_load(gw2al_core_vtable* core_api)
{
    // New API initialization
    GW2Load_API api(core_api);
    GW2Radial::Core::Init(g_hModule);
    return GW2AL_OK;
}
```

#### Changes to include/Core.h:

```cpp
// Change signature from:
void InnerInternalInit() override;

// To:
void InnerInternalInit(GW2Load_API& api) override;
```

#### Changes to src/Core.cpp:

Update the implementation to match new signature and use the API object.

#### Update GW2Common submodule:

```bash
cd extern/GW2Common
git checkout 46ecca8  # Or latest compatible commit
git submodule update --init --recursive
cd ../..
git add extern/GW2Common
```

### Fix Option C: Use Pre-built v1.0.0 and Document Modifications

Since v1.0.0 works, you could:

1. Keep v1.0.0 DLL as the "base"
2. Document all desired modifications in code
3. Ask the original builder to compile with changes
4. OR set up a VM with the exact build environment that produced v1.0.0

## Testing Any Fix

After attempting a fix:

```bash
# Clean build
"/c/Program Files/Microsoft Visual Studio/2022/Community/MSBuild/Current/Bin/MSBuild.exe" \
  GW2Radial.vcxproj -p:Configuration=Debug -p:Platform=x64 -t:Clean

# Build
"/c/Program Files/Microsoft Visual Studio/2022/Community/MSBuild/Current/Bin/MSBuild.exe" \
  GW2Radial.vcxproj -p:Configuration=Debug -p:Platform=x64 -v:minimal

# Check DLL size
ls -lh "bin/x64/Debug/gw2addon_gw2radial.dll"

# Deploy to test location
cp "bin/x64/Debug/gw2addon_gw2radial.dll" \
   "/c/Program Files/Guild Wars 2/addons/gw2radial/"

# Test in game
# Launch GW2 and check if addon loads without crashing
# Check gw2radial.log for errors
```

## Important Notes

- **Always backup the working v1.0.0 DLL** before testing new builds
- **Test in-game** - a successful build doesn't guarantee runtime compatibility
- **Check logs** - GW2 addon loader logs errors in the game directory
- **Version mismatch crashes** - If the DLL loads but crashes, it's often due to ABI incompatibility

## Questions to Answer

To solve this definitively, we need to know:

1. What machine/environment was v1.0.0 built on?
2. What vcpkg commit or version was used?
3. Was there a custom build script or configuration?
4. Why is the DLL 38MB instead of 41MB (Release) or 52MB (Debug)?
5. What is the actual GW2Common commit that v1.0.0 uses?

## Success Criteria

You've fixed the build when:

- ✅ `MSBuild` completes without errors
- ✅ DLL size is reasonable (38-52MB range)
- ✅ DLL loads in GW2 without crashing
- ✅ All existing features (TemplateWheel) work correctly
- ✅ Build can be reproduced by following documented steps

---

_Created: 2025-12-22_
_Purpose: Actionable steps to fix build process_
