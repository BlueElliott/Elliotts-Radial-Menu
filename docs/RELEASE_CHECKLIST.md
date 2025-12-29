# Release Checklist

This document contains the checklist for creating new releases of Elliott's Radial Menu.

## Version Information

**Current Version**: v1.1.1
**Repository**: https://github.com/BlueElliott/Elliotts-Radial-Menu
**Update Checker**: Configured to check BlueElliott/Elliotts-Radial-Menu

## Pre-Release Checklist

### 1. Update Version Number

**File**: `include/Version.h` (UTF-16 LE encoded)

```cpp
#define GIT_VER 1,1,1,0              // Update to new version (major, minor, patch, build)
#define GIT_VER_STR "1.1.1 (hash+)\0" // Update version string with git commit hash
```

**Note**: This file is gitignored and auto-generated during build. For manual updates:
- File encoding is **UTF-16 LE** with BOM (`\xFF\xFE`)
- Line endings are `\r\n` (CRLF)
- Update both `GIT_VER` and `GIT_VER_STR`
- Git commit hash can be obtained with: `git rev-parse --short HEAD`

**Version Format**:
- `GIT_VER`: `major, minor, patch, build` (e.g., `1,1,1,0`)
- `GIT_VER_STR`: `"major.minor.patch (hash+)\0"` (e.g., `"1.1.1 (f3de3c3+)\0"`)

### 2. Verify Repository Configuration

**File**: `include/Core.h` (line 78)

```cpp
const wchar_t* GetGithubRepoSubUrl() const override
{
    return L"BlueElliott/Elliotts-Radial-Menu"; // ✅ Correct
}
```

**Never change this back to** `Friendly0Fire/GW2Radial` unless intentionally reverting to upstream!

### 3. Build the Project

```batch
# Clean build (recommended for releases)
"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" ^
    GW2Radial.vcxproj ^
    -p:Configuration=Release ^
    -p:Platform=x64 ^
    -p:VcpkgEnableManifest=true ^
    -t:Rebuild ^
    -v:minimal
```

**Output**: `bin\x64\Release\gw2addon_gw2radial.dll`

### 4. Test the Build

1. Copy DLL to GW2 addons folder:
   ```
   C:\Program Files\Guild Wars 2\addons\gw2radial\gw2addon_gw2radial.dll
   ```

2. Launch Guild Wars 2 and verify:
   - ✅ Addon loads without errors
   - ✅ All wheels work (Mount, Novelty, Marker, Template, ChatWheel)
   - ✅ Settings menu opens with `Shift+Alt+M`
   - ✅ Version shown in Misc tab matches your release version
   - ✅ Welcome message shows "Elliott's Radial Menu" (if first launch)
   - ✅ Update checker doesn't show false updates

3. Check logs for errors:
   - `gw2radial.log` in GW2 install folder
   - `gw2al_log.txt` for addon loader issues

## Release Process

### 1. Commit Changes

```bash
git add include/Core.h src/Core.cpp [other changed files]
git commit -m "Release v1.x.x - [Brief description]"
git push
```

### 2. Tag the Release

```bash
git tag v1.x.x
git push origin v1.x.x
```

### 3. Create GitHub Release

```bash
gh release create v1.x.x \
    --title "Elliott's Radial Menu v1.x.x - [Feature Name]" \
    --notes "[Release notes - see template below]" \
    bin\x64\Release\gw2addon_gw2radial.dll
```

### Release Notes Template

```markdown
# Elliott's Radial Menu v1.x.x - [Feature Name]

## 🎉 What's New

[List new features with emojis and descriptions]

### New Feature Name
- Feature point 1
- Feature point 2
- Feature point 3

## 🔧 Improvements

[List improvements and enhancements]

## 🐛 Bug Fixes

[List bug fixes]

## 📖 Usage

[Brief usage instructions for new features]

## 🔄 Upgrade Notes

[Any special instructions for upgrading from previous versions]

## Installation

1. Download `gw2addon_gw2radial.dll` from this release
2. Place it in `<GW2 Install>/addons/gw2radial/gw2addon_gw2radial.dll`
3. Restart Guild Wars 2
4. Press `Shift+Alt+M` to configure

**Requirements**: [GW2 Addon Loader](https://github.com/gw2-addon-loader/loader-core)

---

**Full Changelog**: https://github.com/BlueElliott/Elliotts-Radial-Menu/compare/v1.previous.x...v1.x.x

🤖 Generated with [Claude Code](https://claude.com/claude-code)
```

## Update Checker System

### How It Works

1. **Version Check**: Compares `GIT_VER` in DLL against latest GitHub release tag
2. **Tag Format**: Expects tags in format `vMAJOR.MINOR.PATCH` (e.g., `v1.1.1`)
3. **Comparison**: Converts both to 64-bit number and compares
4. **Frequency**: Checks on addon load, max 10 attempts with 1-second intervals
5. **User Control**: Can be disabled in settings under "Automatically check for update"

### Version Encoding

```cpp
// Tag v1.2.3 becomes:
u64 version = (1 << 48) | (2 << 32) | (3 << 16) | (0 << 0)
            = 0x0001000200030000

// Your DLL version (1,1,1,0) becomes:
u64 version = (1 << 48) | (1 << 32) | (1 << 16) | (0 << 0)
            = 0x0001000100010000
```

### API Endpoint

```
https://api.github.com/repos/BlueElliott/Elliotts-Radial-Menu/releases/latest
```

Returns JSON with `tag_name` field that gets parsed and compared.

## Common Issues

### "Update available!" shows immediately
- **Cause**: DLL version < latest GitHub release version
- **Fix**: Update `Version.h` to match or exceed latest release version
- **Check**: Ensure `GIT_VER` matches your release tag

### Update checker shows wrong repository
- **Cause**: `GetGithubRepoSubUrl()` returns wrong repo
- **Fix**: Verify `include/Core.h` line 78 returns `L"BlueElliott/Elliotts-Radial-Menu"`

### Version.h changes keep getting lost
- **Note**: This file is gitignored (auto-generated)
- **Workaround**: Update manually before each release build
- **Check**: Verify version matches before building release DLL

## File Locations Reference

| File | Purpose | Encoding | Git Tracked? |
|------|---------|----------|--------------|
| `include/Version.h` | Version number | UTF-16 LE | ❌ No (gitignored) |
| `include/Core.h` | Repo URL config | UTF-8 | ✅ Yes |
| `src/Core.cpp` | Welcome message | UTF-8 | ✅ Yes |
| `src/Main.cpp` | Version functions | UTF-8 | ✅ Yes |
| `bin/x64/Release/*.dll` | Release binary | Binary | ❌ No |

## Version History

- **v1.1.1** (2024-12-29): Update checker fix - points to correct repo
- **v1.1.0** (2024-12-29): ChatWheel feature release
- **v1.0.0** (2024-12-18): Initial fork with Template Wheel

## Quick Reference Commands

```bash
# Get current git hash (short)
git rev-parse --short HEAD

# Create release with DLL
gh release create v1.x.x bin/x64/Release/gw2addon_gw2radial.dll

# Upload DLL to existing release
gh release upload v1.x.x bin/x64/Release/gw2addon_gw2radial.dll

# Check current version in built DLL (if possible)
# Version is shown in GW2 settings menu under Misc tab

# Build release DLL
msbuild GW2Radial.vcxproj -p:Configuration=Release -p:Platform=x64 -t:Rebuild

# Deploy debug DLL for testing
cd scripts && deploy.bat
```

---

**Last Updated**: 2024-12-29
**Maintainer**: BlueElliott with Claude Code
