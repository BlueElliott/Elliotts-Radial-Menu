# GW2Radial - Quick Start Guide

## 🎯 What You Have Now

A clean, ready-to-build GW2Radial development environment with:

✅ Visual Studio 2022 Community Edition
✅ vcpkg package manager (C:\dev\vcpkg)
✅ All submodules properly initialized
✅ Build scripts ready to use
✅ Documentation for setup and deployment

## 🚀 Quick Actions

### 1. Build the Project (First Time)

**Option A: Using Visual Studio** (Easiest)
```
1. Double-click: GW2Radial.sln
2. Wait for Visual Studio to load
3. Select "Debug" and "x64" from the dropdowns
4. Press F7 or Build → Build Solution
5. Wait 10-30 minutes for first build (vcpkg compiles dependencies)
```

**Option B: Using Command Line**
```cmd
# Open "Developer Command Prompt for VS 2022" from Start Menu
cd C:\Users\Aurora\Projects\GW2Radial
build_debug.bat
```

### 2. Create Your GitHub Repository

```
1. Go to https://github.com/new
2. Name: "GW2Radial" (or any name you prefer)
3. Description: "Personal GW2Radial fork with custom modifications"
4. Public or Private (your choice)
5. DO NOT initialize with README
6. Click "Create repository"
```

### 3. Push to Your New Repository

```bash
cd C:\Users\Aurora\Projects\GW2Radial

# Remove connection to original repo
git remote remove origin

# Add your new repo (REPLACE YOUR_USERNAME)
git remote add origin https://github.com/YOUR_USERNAME/GW2Radial.git

# Push everything
git push -u origin master
```

### 4. Test in Guild Wars 2

After building:
```
1. Install GW2 Addon Loader if you haven't
2. Copy: bin\x64\Debug\gw2addon_gw2radial.dll
3. To: C:\Program Files\Guild Wars 2\addons\gw2radial\
4. Launch GW2
5. Press Shift+Alt+M in-game for settings
```

## 📁 Project Structure

```
C:\Users\Aurora\Projects\GW2Radial\
├── 📄 QUICKSTART.md          ← You are here!
├── 📄 DEVELOPMENT_SETUP.md   ← Detailed setup guide
├── 📄 GITHUB_SETUP.md        ← GitHub repository guide
├── 📄 readme.md              ← Original project README
│
├── 🔧 build_simple.bat       ← Simple build script
├── 🔧 build_debug.bat        ← Debug build with output
├── 🔧 GW2Radial.sln          ← Visual Studio solution
│
├── 📂 src\                   ← C++ source files (edit these!)
├── 📂 include\               ← Header files
├── 📂 shaders\               ← DirectX shaders (HLSL)
├── 📂 extern\GW2Common\      ← Submodule (common utilities)
│
└── 📂 bin\x64\Debug\         ← Build output (after building)
    └── gw2addon_gw2radial.dll
```

## 💡 Common Tasks

### Make Code Changes
```
1. Edit files in src\ or include\
2. Build (F7 in Visual Studio or run build_debug.bat)
3. Test in GW2
4. Commit: git add . && git commit -m "Description"
5. Push: git push
```

### Update from Original Project (Optional)
```bash
# Add original repo as 'upstream' (one time)
git remote add upstream https://github.com/Friendly0Fire/GW2Radial.git

# Pull latest changes
git fetch upstream
git merge upstream/master

# Resolve any conflicts, then:
git push
```

### Auto-Copy to GW2 After Build
```
1. Open System Properties → Environment Variables
2. Create new variable:
   Name: GW2RADIAL_TARGET_DIR
   Value: C:\Program Files\Guild Wars 2
3. Restart Visual Studio
4. Now builds auto-copy the DLL!
```

## 🐛 Troubleshooting

### "vcpkg not found"
```bash
cd C:\dev\vcpkg
vcpkg integrate install
```

### "MSBuild not found"
- Use "Developer Command Prompt for VS 2022" from Start Menu
- Or open Visual Studio and build from there

### "DLL won't load in GW2"
- Check: C:\Program Files\Guild Wars 2\gw2radial.log
- Ensure addon loader is installed
- Verify DLL is in: GW2\addons\gw2radial\gw2addon_gw2radial.dll

### Build fails on first try
- First build takes 10-30 minutes (vcpkg compiles all dependencies)
- Be patient, it's normal!

## 📚 Next Steps

1. ✅ **Read this guide** (you're doing great!)
2. ⏳ **Build the project** (see section 1 above)
3. ⏳ **Create GitHub repo** (see section 2 above)
4. ⏳ **Test in GW2** (see section 4 above)
5. ⏳ **Start coding your custom features!**

## 🆘 Need Help?

- **Detailed setup**: See [DEVELOPMENT_SETUP.md](DEVELOPMENT_SETUP.md)
- **GitHub setup**: See [GITHUB_SETUP.md](GITHUB_SETUP.md)
- **Original docs**: See [readme.md](readme.md)
- **Original project**: https://github.com/Friendly0Fire/GW2Radial

## 📝 Git Cheat Sheet

```bash
# Check status
git status

# Stage all changes
git add .

# Commit with message
git commit -m "Your message here"

# Push to GitHub
git push

# Pull from GitHub
git pull

# View history
git log --oneline -10

# Undo last commit (keep changes)
git reset --soft HEAD~1

# Discard all local changes (DANGER!)
git reset --hard HEAD
```

## 🎮 Happy Modding!

You now have everything you need to:
- Build GW2Radial from source
- Make custom modifications
- Test in Guild Wars 2
- Version control with Git
- Share on GitHub

Remember:
- **Commit often** - Don't lose your work
- **Test in-game** - Make sure it works
- **Have fun!** - This is your personal project

---

**Your Setup Summary:**
- Git User: BlueElliott (elliott.ramdass10@gmail.com)
- vcpkg: C:\dev\vcpkg
- Visual Studio: 2022 Community
- Project: C:\Users\Aurora\Projects\GW2Radial

**Ready to code!** 🚀
