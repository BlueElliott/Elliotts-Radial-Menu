# Chat Commands Feature - Implementation Guide

## Quick Start

### 1. Deploy the New Build

**IMPORTANT**: Close Guild Wars 2 first, then run:
```bash
deploy.bat
```

Or manually:
```bash
cp bin/x64/Debug/gw2addon_gw2radial.dll "/c/Program Files/Guild Wars 2/addons/gw2radial/"
```

### 2. Test in Game

1. Launch Guild Wars 2
2. Press your radial menu keybind (e.g., Alt+M)
3. Click the settings icon (gear)
4. Scroll to the **bottom** of settings
5. **Look for "Chat Commands" section** ✨

## What's New

### Chat Commands UI Section

A new section has been added to the Template Wheel settings menu:

**Location**: Template Wheel → Settings → Bottom of page

**Contents**:
- Title: "Chat Commands"
- Description: "Quick chat commands for commanding. Configure preset messages to send to squad or team chat."
- Placeholder showing planned features:
  - Preset squad commands
  - Quick target calls
  - Keybind shortcuts for common messages

### Current Status

- ✅ UI section visible in settings
- ✅ Non-intrusive placeholder
- ✅ Build process fixed and documented
- ⏳ Backend functionality (to be implemented)
- ⏳ Actual command configuration (future work)
- ⏳ Chat message sending (future work)

## Build Process (FIXED!)

### The Problem We Had

The repository couldn't be built due to GW2Common version mismatch:
- Git recorded GW2Common at commit `46ecca8` (missing Direct3D11Loader)
- Code expected commit `1a7253a` (has Direct3D11Loader)
- Newer version also had breaking API changes in neargye/semver.hpp

### The Solution

**Correct GW2Common commit**: `1a7253a`

This commit:
- ✅ Has `Direct3D11Loader.h`
- ✅ No problematic `neargye/semver.hpp` dependency
- ✅ Matches the successful build from commit `70932d8`

### How to Build

```bash
# 1. Ensure GW2Common is at correct commit (already done)
cd extern/GW2Common
git checkout 1a7253a
git submodule update --init --recursive --force

# 2. Build the project
cd ../..
"/c/Program Files/Microsoft Visual Studio/2022/Community/MSBuild/Current/Bin/MSBuild.exe" \
  GW2Radial.vcxproj -p:Configuration=Debug -p:Platform=x64 -v:minimal

# 3. First build only: Copy resource file
mkdir -p int/GW2Common/x64/Debug
cp extern/GW2Common/int/GW2Common/x64/Debug/BaseResource.res int/GW2Common/x64/Debug/

# 4. Deploy (after closing GW2)
deploy.bat
```

**Build Output**: `bin/x64/Debug/gw2addon_gw2radial.dll` (52MB Debug build)

## Development Status

### Completed ✅

1. **Build Process**
   - Identified correct GW2Common version
   - Fixed all dependency issues
   - Can now build modifications successfully
   - Comprehensive documentation created

2. **Chat Commands UI**
   - Added settings section to TemplateWheel
   - Clean, minimal placeholder design
   - Shows feature intent and planned capabilities
   - Doesn't interfere with existing functionality

3. **Documentation**
   - [BUILD_DOCUMENTATION.md](BUILD_DOCUMENTATION.md) - Complete build solution
   - [HOW_TO_FIX_BUILD.md](HOW_TO_FIX_BUILD.md) - Investigation steps
   - [DEPLOYMENT.md](DEPLOYMENT.md) - Deployment guide
   - [SESSION_SUMMARY.md](SESSION_SUMMARY.md) - Complete session recap

### Next Steps (Future Work) ⏳

To make Chat Commands fully functional:

1. **Backend Storage**
   - Create `ChatCommand` struct to store commands
   - Add configuration options for each command
   - Implement save/load for command presets

2. **UI Implementation**
   - Replace placeholder with actual input fields
   - Add text boxes for command content
   - Add keybind assignment per command
   - Add squad/team/say chat selector

3. **Chat Integration**
   - Research GW2 chat message API
   - Implement message sending functionality
   - Add chat channel targeting
   - Handle special characters and formatting

4. **Quality of Life**
   - Command templates/presets
   - Import/export command sets
   - Macro variables (e.g., target name, location)
   - Command history

## File Changes

### Modified Files

- [src/TemplateWheel.cpp](src/TemplateWheel.cpp:325-334) - Added Chat Commands UI section
- [BUILD_DOCUMENTATION.md](BUILD_DOCUMENTATION.md) - Updated with solution
- `extern/GW2Common` - Set to commit 1a7253a

### New Files

- `deploy.bat` - Automated deployment script
- `DEPLOYMENT.md` - Deployment instructions
- `HOW_TO_FIX_BUILD.md` - Build troubleshooting
- `SESSION_SUMMARY.md` - Complete session record
- `README_CHAT_COMMANDS.md` - This file

## Troubleshooting

### DLL Won't Deploy

**Error**: "Device or resource busy"
**Cause**: Guild Wars 2 is running and has the DLL loaded
**Solution**: Close GW2 completely, then run `deploy.bat`

### Chat Commands Section Not Visible

**Check**:
1. Did you deploy the new DLL? (Check file timestamp)
2. Did you restart GW2 after deploying?
3. Are you looking in Template Wheel settings? (Not Mount Wheel or Novelty Wheel)
4. Did you scroll to the bottom of the settings?

### Build Fails

**Check**:
1. Is GW2Common at commit 1a7253a?
   ```bash
   cd extern/GW2Common && git log --oneline -1
   ```
   Should show: `1a7253a Fix build compatibility`

2. Are submodules initialized?
   ```bash
   cd extern/GW2Common && git submodule update --init --recursive --force
   ```

3. First build? Copy resource file:
   ```bash
   mkdir -p int/GW2Common/x64/Debug
   cp extern/GW2Common/int/GW2Common/x64/Debug/BaseResource.res int/GW2Common/x64/Debug/
   ```

## Technical Details

### Code Location

The Chat Commands UI is in [src/TemplateWheel.cpp](src/TemplateWheel.cpp) at line 325:

```cpp
UI::Title("Chat Commands");
ImGui::TextWrapped("Quick chat commands for commanding. Configure preset messages to send to squad or team chat.");
ImGui::Spacing();

// Placeholder for future chat command configuration
ImGui::TextDisabled("Chat command configuration will be added here.");
ImGui::TextDisabled("Features planned:");
ImGui::BulletText("Preset squad commands");
ImGui::BulletText("Quick target calls");
ImGui::BulletText("Keybind shortcuts for common messages");
```

### Build Configuration

- **Configuration**: Debug
- **Platform**: x64
- **Compiler**: MSVC (Visual Studio 2022)
- **Output**: 52MB DLL with debug symbols
- **GW2Common**: Commit 1a7253a
- **Dependencies**: No vcpkg required with this GW2Common version

## Git Commits

Recent commits related to this feature:

```
6177166 Add comprehensive session summary
4a4f867 Add deployment guide for Chat Commands feature
5ed9dde Add Chat Commands UI section and fix build process
ae5176e Add actionable steps to fix build process
eea4ee9 Add comprehensive build documentation
```

## Contact & Support

- **Repository**: Check git history for implementation details
- **Build Issues**: See [BUILD_DOCUMENTATION.md](BUILD_DOCUMENTATION.md)
- **Deployment**: See [DEPLOYMENT.md](DEPLOYMENT.md)

---

**Status**: ✅ Build process fixed, UI added, ready to test in-game
**Date**: 2025-12-22
**Build**: 52MB Debug DLL at `bin/x64/Debug/gw2addon_gw2radial.dll`
