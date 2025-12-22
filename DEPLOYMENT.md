# Deployment Guide

## Quick Deploy to GW2

### Prerequisites
- **CLOSE GUILD WARS 2** before deploying (DLL is locked while game is running)

### Deploy Command

```bash
cp "C:\Users\Aurora\Projects\GW2Radial\bin\x64\Debug\gw2addon_gw2radial.dll" \
   "/c/Program Files/Guild Wars 2/addons/gw2radial/"
```

### Deployment Checklist

1. ✅ Build succeeded (check for `GW2Radial.vcxproj -> ...gw2addon_gw2radial.dll`)
2. ✅ Close Guild Wars 2
3. ✅ Run copy command above
4. ✅ Launch Guild Wars 2
5. ✅ Test the addon:
   - Press your radial menu keybind (e.g., Alt+M)
   - Open settings (gear icon)
   - Scroll to bottom
   - **Look for "Chat Commands" section** 📝

## What's New - Chat Commands

### New UI Section

The TemplateWheel settings now include a **"Chat Commands"** section at the bottom with:

- Description of the feature
- Placeholder UI showing planned features:
  - Preset squad commands
  - Quick target calls
  - Keybind shortcuts for common messages

### Current Status

- ✅ UI section added and visible in settings
- ⏳ Backend functionality to be implemented later
- ⏳ Actual command configuration to be added
- ⏳ Chat message sending to be implemented

This is a **simple, non-intrusive placeholder** that establishes the UI presence for future expansion.

## Build Info

- **DLL Location**: `bin\x64\Debug\gw2addon_gw2radial.dll`
- **DLL Size**: 52MB (Debug build with symbols)
- **GW2Common Commit**: 1a7253a
- **Built**: 2025-12-22

## Testing

After deploying:

1. Launch GW2
2. Verify addon loads (check `gw2radial.log` for errors)
3. Press radial menu keybind
4. Click settings icon
5. Scroll to bottom of settings
6. Confirm **"Chat Commands"** section is visible

## Troubleshooting

### DLL Won't Copy
**Error**: `Device or resource busy`
**Solution**: Close Guild Wars 2 completely

### Addon Won't Load
**Check**: `C:\Program Files\Guild Wars 2\gw2al_log.txt`
**Common causes**:
- Old DLL still loaded (restart GW2)
- Missing dependencies (rebuild project)
- Incompatible GW2Common version (use commit 1a7253a)

### Chat Commands Section Not Visible
**Verify**:
- DLL was actually copied (check file timestamp)
- GW2 was restarted after copying
- Looking in Template Wheel settings (not other wheels)

## Reverting

To revert to v1.0.0 release:

```bash
# Download from GitHub releases
curl -L "https://github.com/BlueElliott/Elliotts-Radial-Menu/releases/download/v1.0.0/gw2addon_gw2radial.dll" \
  -o "/c/Program Files/Guild Wars 2/addons/gw2radial/gw2addon_gw2radial.dll"
```

---

_Created: 2025-12-22_
_For commit: 5ed9dde_
