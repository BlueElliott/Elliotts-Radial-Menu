# Action Chain Test Procedure

## 🎯 What We're Testing

**Test Implementation:** The Raptor mount now has an action chain attached that will:
1. Send Ctrl+1 (Build Template 1)
2. Wait 75ms
3. Send Ctrl+Shift+1 (Equipment Template 1)

**Expected Result:** When you select Raptor from the mount wheel, both your build and equipment templates should switch to Template 1.

---

## 📋 Pre-Test Checklist

Before testing, ensure:
- [ ] Guild Wars 2 is **closed** (DLL must not be locked)
- [ ] You have Build Template 1 configured in-game
- [ ] You have Equipment Template 1 configured in-game
- [ ] You're currently using a **different** template (so you can see the switch happen)
- [ ] Your mount radial keybind is set (default: Alt+M or Shift+Alt+M)

---

## 🔨 Step 1: Build the Project

The build has been started. Wait for it to complete (first build: 10-30 minutes).

**Expected output:**
```
===============================================
  BUILD SUCCEEDED!
===============================================

DLL created at: bin\x64\Debug\gw2addon_gw2radial.dll
```

If build fails, check `build_detailed.log` for errors.

---

## 📦 Step 2: Deploy to GW2

Once built successfully:

```cmd
deploy_to_gw2.bat
```

Or manually:
```cmd
copy bin\x64\Debug\gw2addon_gw2radial.dll "C:\Program Files\Guild Wars 2\addons\gw2radial\"
copy bin\x64\Debug\gw2addon_gw2radial.pdb "C:\Program Files\Guild Wars 2\addons\gw2radial\"
```

**Expected output:**
```
Backup created: gw2addon_gw2radial_BACKUP.dll
DLL deployed successfully!
```

---

## 🎮 Step 3: Test In-Game

### 3.1 Launch GW2 and Check Logs

1. Launch Guild Wars 2
2. Wait for it to load completely
3. Check the addon loaded successfully:

```
C:\Program Files\Guild Wars 2\gw2radial.log
```

**Look for:**
```
[INFO] [TEST] Added 2-step action chain to Raptor: Build Template 1 -> Equipment Template 1
```

If you see this line, the test code is active! ✅

### 3.2 Set Up Templates

1. Make sure you have Build Template 1 and Equipment Template 1 configured
2. Switch to a **different** template (e.g., Template 2 or 3)
3. Note your current build/equipment so you can see when they change

### 3.3 Trigger the Action Chain

**Method A: Normal Mount Radial**
1. Press your mount radial keybind (e.g., Alt+M)
2. Move mouse toward **Raptor** icon (red/orange, usually at top)
3. Release the keybind

**Method B: Quick Keybind (if set)**
1. If you have Raptor bound to a quick keybind, press it

### 3.4 Observe the Results

**What should happen:**
1. Your Build switches to Template 1 (you'll see skill bar change)
2. ~75ms later
3. Your Equipment switches to Template 1 (you'll see gear stats change)
4. You'll see in-game notifications for both switches

**Timeline:**
```
Time 0ms:   You release mount radial on Raptor
Time 0ms:   Action chain queued
Time 0ms:   Ctrl+1 sent → Build Template 1 activates
Time 75ms:  Ctrl+Shift+1 sent → Equipment Template 1 activates
Time 150ms: Chain complete
```

---

## 📊 Step 4: Verify in Logs

After testing, check the detailed log:

```
C:\Program Files\Guild Wars 2\gw2radial.log
```

**Expected log sequence:**
```
[INFO] Starting action chain for 'Raptor' with 2 steps.
[INFO] Executing chain step 1/2 for 'Raptor'
[INFO] Executing chain step 2/2 for 'Raptor'
[INFO] Action chain completed for 'Raptor'
```

**Check timestamps** - they should be ~75ms apart:
```
[2025-12-17 12:00:00.000] [INFO] Executing chain step 1/2 for 'Raptor'
[2025-12-17 12:00:00.075] [INFO] Executing chain step 2/2 for 'Raptor'
[2025-12-17 12:00:00.150] [INFO] Action chain completed for 'Raptor'
```

---

## ✅ Success Criteria

The test is **successful** if:
- [ ] Build succeeded without errors
- [ ] DLL deployed to GW2
- [ ] GW2 loaded with addon (no crashes)
- [ ] Log shows test chain was added
- [ ] Selecting Raptor switches **both** templates
- [ ] Log shows chain execution with correct timing
- [ ] No crashes or hangs
- [ ] Templates switch in the correct order (Build → Equipment)

---

## 🐛 Troubleshooting

### Build Fails

**Error: Cannot find MountWheel.cpp**
- Make sure you're in the correct directory
- Run: `dir src\MountWheel.cpp`

**Error: Syntax errors in MountWheel.cpp**
- Check that the test code was added correctly
- Look at lines 45-63 in src/MountWheel.cpp

**Error: vcpkg dependencies missing**
- First build takes 10-30 minutes to compile dependencies
- Be patient and let it finish

### DLL Won't Load in GW2

**Check: gw2al_log.txt**
```
C:\Program Files\Guild Wars 2\gw2al_log.txt
```

Look for errors about loading gw2radial.

**Common issues:**
- GW2 was running when you copied the DLL → Close GW2 completely
- Addon loader not installed → Install GW2 Addon Loader
- Wrong DLL location → Should be in `addons\gw2radial\`

### Chain Doesn't Execute

**Log shows chain added but not executing:**
- Check that you actually selected Raptor (not another mount)
- Try selecting Raptor multiple times
- Check keybinds are correct in your GW2 settings

**Log shows "Starting action chain" but steps don't execute:**
- Check timing - wait a few frames
- Look for error messages in log
- Verify keybinds are set correctly (Ctrl+1, Ctrl+Shift+1)

### Templates Don't Switch

**Chain executes but templates don't change:**
- Verify keybinds match your GW2 settings exactly
- Check that templates are actually configured
- Try the keybinds manually (Ctrl+1) to verify they work
- Make sure you're on a different template before testing

### Focus Loss Cancels Chain

**Expected behavior:**
- Alt+Tab during chain execution → Chain cancels
- Log shows: "Canceling active action chain due to focus loss"
- This is intentional for safety!

---

## 📝 Notes

### About This Test

- This test code is **temporary**
- It modifies the Raptor mount only
- Other mounts are unaffected
- Can be easily removed after testing

### What Happens During Test

1. When addon loads, test code runs in MountWheel constructor
2. Raptor element gets 2-step chain attached
3. When you select Raptor, chain is detected
4. Chain is queued for execution
5. Every frame, OnUpdate() checks if it's time for next step
6. Steps execute with 75ms delays
7. Chain completes and resets

### Performance Impact

- Negligible - only adds ~10 bytes per wheel element
- Chain execution uses existing frame loop
- No additional threads or timers

---

## 🚀 Next Steps After Successful Test

Once the test succeeds:

1. **Remove test code** from MountWheel.cpp (lines 45-63)
2. **Implement Phase 2**: UI for configuring chains
3. **Add config persistence**: Save/load chains from config.ini
4. **Create real use case**: Build+Equipment combo wheel

Or continue testing with:
- Different mounts
- More than 2 steps
- Different delays
- Different keybinds

---

## 🎉 Expected Experience

**Before Action Chains:**
```
You: Press mount radial → Select Raptor → Get on Raptor
     Press build radial → Select template 1
     Press equipment radial → Select template 1
Total: 3 actions
```

**With Action Chains:**
```
You: Press mount radial → Select Raptor
     → Build switches to Template 1
     → Equipment switches to Template 1
     → Get on Raptor
Total: 1 action for everything!
```

This is the power of action chains! 🚀

---

## 📞 If You Need Help

**Check these files:**
- `C:\Program Files\Guild Wars 2\gw2radial.log` - Main log
- `build_detailed.log` - Build errors
- `ACTION_CHAIN_TESTING_GUIDE.md` - Detailed guide
- `FEATURE_IMPLEMENTATION_PLAN.md` - Full technical docs

**What to share if asking for help:**
- Last 50 lines of gw2radial.log
- Build errors (if build failed)
- What you expected vs what happened
- Screenshots/video of the issue

Good luck with testing! 🎮
