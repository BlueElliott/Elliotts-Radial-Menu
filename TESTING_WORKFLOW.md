# GW2Radial - Testing & Development Workflow

## 🎮 Your Current Setup

I can see you already have GW2Radial running with:
- ✅ Installed at: `C:\Program Files\Guild Wars 2\addons\gw2radial\`
- ✅ Current version: DLL from Nov 24, 2024 (34.6 MB)
- ✅ Custom config with your keybinds and preferences
- ✅ Custom wheels (build templates, equipment templates)
- ✅ Other addons: ArcDPS, Clarity, Commander's Toolkit, etc.

**IMPORTANT:** Your `config.ini` contains all your keybinds and settings. We'll preserve this!

## 🔄 Recommended Development Workflow

### Method 1: Automatic Copy (Recommended for Active Development)

This is the **best** approach when you're actively developing and testing frequently.

#### Setup (One-Time)
1. Set environment variable to auto-copy after each build:
   ```cmd
   setx GW2RADIAL_TARGET_DIR "C:\Program Files\Guild Wars 2"
   ```

2. Restart Visual Studio (or your terminal) for the variable to take effect

3. Now every time you build, the DLL automatically copies to your GW2 folder!

#### Daily Workflow
```
1. Make code changes in Visual Studio
2. Press F7 to build
3. Close Guild Wars 2 (if running)
4. DLL is automatically copied
5. Launch GW2 to test
6. Check gw2radial.log for any errors
7. Repeat!
```

**Pros:**
- ✅ Fastest workflow
- ✅ No manual copying
- ✅ Your config.ini is preserved

**Cons:**
- ⚠️ Must close GW2 before building (Windows locks the DLL while game is running)
- ⚠️ Overwrites the stable version immediately

---

### Method 2: Development vs. Production DLLs (Safest)

Keep your stable version separate from development builds.

#### Setup (One-Time)
1. **Backup your current stable DLL:**
   ```cmd
   cd "C:\Program Files\Guild Wars 2\addons\gw2radial"
   copy gw2addon_gw2radial.dll gw2addon_gw2radial_STABLE.dll
   ```

2. **Create a swap script** - I'll make this for you!

#### Daily Workflow
```
1. Make code changes
2. Build in Visual Studio
3. Close GW2
4. Run: swap_to_dev.bat   (loads your dev build)
5. Test in GW2
6. If broken, run: swap_to_stable.bat   (restores working version)
```

**Pros:**
- ✅ Safe - can always revert to stable
- ✅ Your config.ini is preserved
- ✅ Good for testing risky changes

**Cons:**
- ⚠️ Manual swap required
- ⚠️ Extra disk space for two DLLs

---

### Method 3: Manual Copy (Simple, Good for Occasional Testing)

Just manually copy when you want to test.

#### Workflow
```cmd
# After building
cd C:\Users\Aurora\Projects\GW2Radial
copy bin\x64\Debug\gw2addon_gw2radial.dll "C:\Program Files\Guild Wars 2\addons\gw2radial\"
```

**Pros:**
- ✅ Simple
- ✅ Full control over when to update
- ✅ Your config.ini is preserved

**Cons:**
- ⚠️ Manual steps
- ⚠️ Easy to forget

---

## 🛡️ Safety Tips

### Your Config is Safe!
Your `config.ini` file (with all your keybinds) is **separate** from the DLL. Updating the DLL will **NOT** affect your settings. They're preserved automatically.

### Before Making Changes:
1. **Backup your stable DLL:**
   ```cmd
   cd "C:\Program Files\Guild Wars 2\addons\gw2radial"
   copy gw2addon_gw2radial.dll gw2addon_gw2radial_BACKUP.dll
   ```

2. **Backup your config too (optional but recommended):**
   ```cmd
   copy config.ini config_BACKUP.ini
   ```

### If Something Goes Wrong:
1. Close GW2
2. Restore the backup:
   ```cmd
   cd "C:\Program Files\Guild Wars 2\addons\gw2radial"
   copy gw2addon_gw2radial_BACKUP.dll gw2addon_gw2radial.dll
   ```
3. Launch GW2

---

## 🔧 Build Configurations

### Debug Build (For Development)
- **Larger DLL** (~35-40 MB)
- **Includes debug symbols** (.pdb file)
- **Better error messages**
- **Slower performance** (but negligible for testing)

```cmd
# Build Debug
cd C:\Users\Aurora\Projects\GW2Radial
build_debug.bat
# Output: bin\x64\Debug\gw2addon_gw2radial.dll
```

### Release Build (For Distribution/Final Testing)
- **Smaller DLL** (~2-5 MB)
- **Optimized performance**
- **No debug info**

```cmd
# Build Release (open Developer Command Prompt)
cd C:\Users\Aurora\Projects\GW2Radial
msbuild GW2Radial.sln /t:Build /p:Configuration=Release /p:Platform=x64
# Output: bin\x64\Release\gw2addon_gw2radial.dll
```

**Recommendation:** Use Debug builds during development, Release for final testing.

---

## 📝 Testing Checklist

After deploying a new build:

```
☐ Close GW2 completely
☐ Copy new DLL to GW2 folder
☐ Launch GW2
☐ Check gw2radial.log for errors
☐ Press Shift+Alt+M (verify UI loads)
☐ Test mount radial menu
☐ Test your custom features
☐ Check performance (FPS stable?)
☐ Test in different game modes (PvE, WvW, etc.)
```

### Reading Logs

**Main log:**
```
C:\Program Files\Guild Wars 2\gw2radial.log
```

**Check for:**
- Loading success: "GW2Radial loaded successfully"
- Errors: Look for "ERROR", "FATAL", "EXCEPTION"
- Warnings: May indicate non-critical issues

**If addon won't load:**
```
C:\Program Files\Guild Wars 2\gw2al_log.txt
```

---

## 🎯 My Recommendation for You

Based on your setup, I recommend **Method 1 (Automatic Copy)** because:

1. ✅ You're actively developing - fast iteration is key
2. ✅ Your config.ini is already customized and will be preserved
3. ✅ You have other stable addons (ArcDPS) - if GW2Radial breaks, you can still play
4. ✅ Fastest workflow - just build and test

### Quick Setup:
```cmd
# Run this once (as Administrator)
setx GW2RADIAL_TARGET_DIR "C:\Program Files\Guild Wars 2"

# Restart Visual Studio

# Now when you build, it auto-copies!
```

### Your Daily Loop Will Be:
```
1. Edit code in VS
2. Press F7 (build - takes ~10 seconds after first build)
3. Close GW2 if running
4. Launch GW2
5. Test your changes
6. Commit to git if it works!
```

---

## 🔐 Important Security Note

The post-build event automatically copies files to `Program Files`. This requires:
- ✅ Visual Studio running as Administrator, OR
- ✅ GW2 installed in a user-writable location (like `C:\Games\`)

If you get "Access Denied" errors:
1. **Option A:** Run Visual Studio as Administrator
2. **Option B:** Use manual copy method instead
3. **Option C:** Install GW2 outside Program Files (if reinstalling)

---

## 📦 Files in GW2Radial Folder

```
C:\Program Files\Guild Wars 2\addons\gw2radial\
├── gw2addon_gw2radial.dll      ← The addon DLL (YOU REPLACE THIS)
├── config.ini                   ← Your settings (PRESERVED)
├── imgui_config.ini             ← UI settings (PRESERVED)
├── custom\                      ← Custom wheels (PRESERVED)
│   ├── build\
│   └── equipment\
└── gw2addon_gw2radial.pdb      ← Debug symbols (optional)
```

**Only replace:** `gw2addon_gw2radial.dll`
**Never touch:** `config.ini`, `imgui_config.ini`, `custom\`

---

## 🚀 Ready to Test?

1. **Build your project** (see DEVELOPMENT_SETUP.md)
2. **Choose your method** (I recommend Method 1)
3. **Test in GW2**
4. **Happy developing!**

Need help? Check the logs or restore your backup!
