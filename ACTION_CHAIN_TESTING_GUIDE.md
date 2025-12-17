# Action Chain Feature - Testing Guide

## ✅ Implementation Complete!

The action chain system is now fully implemented in the codebase. Here's what was added:

### Changes Made:

**1. WheelElement.h** - Data structures
- Added `KeybindStep` struct (keybind + delay)
- Added `keybindChain_` vector to store chain steps
- Added methods: `hasActionChain()`, `getChain()`, `addChainStep()`, `clearChain()`

**2. Wheel.h** - Execution state
- Added `ActionChainState` struct to track execution
- Tracks current step, timing, and source element

**3. Wheel.cpp** - Logic implementation
- Modified `SendKeybindOrDelay()` to detect and queue chains
- Added chain execution in `OnUpdate()` (runs every frame)
- Added chain cancellation in `OnFocusLost()` for safety

## 🧪 How to Test

### Step 1: Build the Project

Open Developer Command Prompt for VS 2022:
```cmd
cd C:\Users\Aurora\Projects\GW2Radial
msbuild GW2Radial.sln /t:Build /p:Configuration=Debug /p:Platform=x64
```

Or use Visual Studio:
1. Open `GW2Radial.sln`
2. Select Debug / x64
3. Press F7 to build

Expected output: `bin\x64\Debug\gw2addon_gw2radial.dll`

### Step 2: Deploy to GW2

```cmd
deploy_to_gw2.bat
```

Or manually copy:
```cmd
copy bin\x64\Debug\gw2addon_gw2radial.dll "C:\Program Files\Guild Wars 2\addons\gw2radial\"
```

### Step 3: Create a Test Chain (Manual Method)

Since we haven't implemented the UI yet, you'll need to manually create a test chain by modifying the code temporarily.

**Option A: Test in Build Template Wheel**

Find where build templates are created (likely in a constructor or initialization), and add:

```cpp
// Example: In build template wheel initialization
auto* element1 = wheelElements_[0].get();  // First build template

// Create a 2-step chain: Build Template → Equipment Template
WheelElement::KeybindStep step1("test_step1", "Build Switch", "BuildTemplates", 75);
step1.keybind.keyCombo(ScanCode::KEY_2, Input::Modifier::Shift);  // Shift+2 for build template 1

WheelElement::KeybindStep step2("test_step2", "Equipment Switch", "BuildTemplates", 75);
step2.keybind.keyCombo(ScanCode::KEY_7, Input::Modifier::Shift);  // Shift+7 for equipment template 1

element1->addChainStep(step1);
element1->addChainStep(step2);
```

**Option B: Test with Simple Example**

Create a test wheel with a chain:

```cpp
// In some wheel initialization
auto testElement = std::make_unique<WheelElement>(
    999,  // ID
    "test_chain",  // nickname
    "Test",  // category
    "Test Chain",  // display name
    glm::vec4(1.0f, 0.5f, 0.0f, 1.0f),  // orange color
    ConditionalProperties::VisibleAll | ConditionalProperties::UsableAll
);

// Add two steps with a delay
WheelElement::KeybindStep step1("step1", "First Action", "Test", 100);
step1.keybind.keyCombo(ScanCode::KEY_1, Input::Modifier::None);

WheelElement::KeybindStep step2("step2", "Second Action", "Test", 100);
step2.keybind.keyCombo(ScanCode::KEY_2, Input::Modifier::None);

testElement->addChainStep(step1);
testElement->addChainStep(step2);

AddElement(std::move(testElement));
```

### Step 4: Test In-Game

1. Launch GW2
2. Trigger your test wheel element
3. Check the log file for chain execution messages:

```
C:\Program Files\Guild Wars 2\gw2radial.log
```

Expected log output:
```
[INFO] Starting action chain for 'Test Chain' with 2 steps.
[INFO] Executing chain step 1/2 for 'Test Chain'
[INFO] Executing chain step 2/2 for 'Test Chain'
[INFO] Action chain completed for 'Test Chain'
```

## 📊 Verification Checklist

- [ ] **Build succeeds** without errors
- [ ] **DLL loads** in GW2 (check gw2radial.log for "loaded successfully")
- [ ] **Chain triggers** when wheel element is selected
- [ ] **Steps execute in order** with proper delays (check log timestamps)
- [ ] **Chain completes** successfully
- [ ] **Chain cancels** when you Alt+Tab out of game
- [ ] **No crashes** or hangs

## 🐛 Troubleshooting

### Build Errors

**Error: `KeybindStep` not found**
- Make sure WheelElement.h changes are saved
- Clean and rebuild: `msbuild /t:Clean && msbuild /t:Build`

**Error: `actionChain_` not found**
- Make sure Wheel.h changes are saved
- Rebuild the project

### Runtime Errors

**Chain doesn't execute**
- Check log: "Starting action chain" should appear
- Verify `hasActionChain()` returns true
- Check that keybinds are set on chain steps

**Keybinds don't send**
- Verify ScanCodes are correct
- Check Input::i().SendKeybind() is being called
- Test with simple keys first (1, 2, 3)

**Chain executes too fast/slow**
- Adjust `delayAfterMs` in KeybindStep (default: 75ms)
- Minimum recommended: 50ms
- Maximum safe: 500ms

## 🎯 Example Use Case: Build + Equipment Switch

Once tested, this is how it should work:

### Current Problem:
- Press keybind → Build Template wheel appears → Select template 1
- Press different keybind → Equipment Template wheel appears → Select template 1
- Total: 2 actions, 2 wheel activations

### With Action Chains:
- Press ONE keybind → Select "PvE DPS Setup"
- Chain executes: Build Template 1 (75ms delay) Equipment Template 1
- Total: 1 action, both templates switched!

## 📝 Next Steps

### Phase 2: UI Configuration (Future Work)

To make this user-friendly, we need to add:

1. **Settings Menu Section**
   - "Enable Action Chain" checkbox
   - "Add Step" button
   - List of chain steps with keybind inputs
   - "Delay" input for each step
   - "Remove Step" button

2. **Config Persistence**
   - Save chains to config.ini
   - Load chains on startup
   - Per-character or global setting

3. **Visual Feedback**
   - Progress indicator (Step 2/4)
   - Remaining time display
   - Animation at skill bar

See `FEATURE_IMPLEMENTATION_PLAN.md` for full Phase 2 details.

## 🚀 Current Status

✅ **Phase 1 Complete**: Core functionality is fully implemented and ready to test!

**What works:**
- Chain data structures
- Chain queuing
- Chain execution with delays
- Chain cancellation on focus loss
- Logging for debugging

**What's missing:**
- UI to configure chains (Phase 2)
- Config persistence (Phase 2)
- Visual feedback (Phase 3)

**You can test the feature by:**
1. Manually adding chains in code (as shown above)
2. Building and deploying
3. Testing in-game
4. Checking logs to verify execution

Once Phase 1 is confirmed working, we can move to Phase 2 (UI) to make it user-friendly!

## 💡 Quick Test Script

Here's a complete test you can add to any wheel's constructor:

```cpp
// TEST CODE - Remove after testing
if (wheelElements_.size() > 0)
{
    auto* firstElement = wheelElements_[0].get();

    WheelElement::KeybindStep testStep1("test1", "Test Step 1", nickname_, 75);
    testStep1.keybind.keyCombo(ScanCode::KEY_1, Input::Modifier::None);

    WheelElement::KeybindStep testStep2("test2", "Test Step 2", nickname_, 75);
    testStep2.keybind.keyCombo(ScanCode::KEY_2, Input::Modifier::None);

    firstElement->addChainStep(testStep1);
    firstElement->addChainStep(testStep2);

    Log::i().Print(Severity::Info, "TEST: Added 2-step chain to first element of {}", displayName_);
}
// END TEST CODE
```

Add this to a wheel constructor, build, deploy, and test. You should see keys "1" and "2" pressed in sequence when you activate that wheel element!

## 🎉 Success Criteria

The feature is working correctly if:
1. Log shows "Starting action chain"
2. Log shows each step executing
3. In-game actions occur in sequence
4. Delays are respected (check timestamps in log)
5. Alt+Tab cancels the chain gracefully
6. No crashes or errors

Happy testing! 🚀
