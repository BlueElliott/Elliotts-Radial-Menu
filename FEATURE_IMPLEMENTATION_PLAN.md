# Feature Implementation Plan: Multi-Step Action Chains

## 🎯 Goal
Allow wheel elements to trigger **multiple keybinds in sequence** instead of just one.

**Use Case:**
- Selector 1 → Switch to Build Template 1 + Equipment Template 1
- Selector 2 → Switch to Build Template 2 + Equipment Template 2
- Future: Complex sequences (mount → /sit → instrument, etc.)

---

## 📐 Architecture Design

### Phase 1: Basic Sequential Keybinds (Start Here)

#### **Changes Needed:**

**1. Update WheelElement.h**
```cpp
// Around line 170, add after existing keybind_
protected:
    Keybind keybind_;  // Existing single keybind

    // NEW: Action chain support
    struct KeybindStep {
        Keybind keybind;
        mstime delayAfterMs;  // Delay before next step (default: 75ms)

        KeybindStep(const std::string& nickname, const std::string& displayName,
                   const std::string& category, mstime delay = 75)
            : keybind(nickname, displayName, category)
            , delayAfterMs(delay) {}
    };

    std::vector<KeybindStep> keybindChain_;
    bool useActionChain_;  // If true, use chain; if false, use single keybind

public:
    // Accessors
    [[nodiscard]] bool hasActionChain() const {
        return useActionChain_ && !keybindChain_.empty();
    }

    [[nodiscard]] const std::vector<KeybindStep>& getChain() const {
        return keybindChain_;
    }

    void addChainStep(const KeybindStep& step) {
        keybindChain_.push_back(step);
        useActionChain_ = true;
    }

    void clearChain() {
        keybindChain_.clear();
        useActionChain_ = false;
    }
```

**2. Update Wheel.h**
```cpp
// Around line 115, add after OptKeybindWheelElement
protected:
    // Existing conditional delay for single actions
    struct ConditionalDelay { ... };
    ConditionalDelay conditionalDelay_;

    // NEW: Action chain execution state
    struct ActionChainState {
        std::vector<WheelElement::KeybindStep> steps;
        size_t currentStep = 0;
        mstime nextStepTime = 0;
        WheelElement* sourceElement = nullptr;

        bool isActive() const {
            return !steps.empty() && currentStep < steps.size();
        }

        void reset() {
            steps.clear();
            currentStep = 0;
            nextStepTime = 0;
            sourceElement = nullptr;
        }
    };

    ActionChainState actionChain_;
```

**3. Update Wheel.cpp - SendKeybindOrDelay()**

Around line 1101, modify the function:
```cpp
void Wheel::SendKeybindOrDelay(OptKeybindWheelElement kbwe, std::optional<Point> mousePos)
{
    // ... existing code ...

    // NEW: Check if this is a WheelElement with an action chain
    if (std::holds_alternative<WheelElement*>(kbwe)) {
        WheelElement* element = std::get<WheelElement*>(kbwe);

        if (element->hasActionChain()) {
            // Queue the entire action chain
            actionChain_.steps = element->getChain();
            actionChain_.currentStep = 0;
            actionChain_.nextStepTime = TimeInMilliseconds();
            actionChain_.sourceElement = element;

            if (mousePos) {
                Log::i().Print(Severity::Info,
                    "Starting action chain for '{}' with {} steps.",
                    element->displayName(), actionChain_.steps.size());
                Input::i().SendKeybind({}, mousePos);  // Reset cursor
            }

            return;  // Don't use normal single-keybind flow
        }
    }

    // ... rest of existing code for single keybinds ...
}
```

**4. Update Wheel.cpp - OnUpdate()**

Around line 900 (in OnUpdate function), add chain execution:
```cpp
void Wheel::OnUpdate()
{
    // ... existing conditional delay code ...

    // NEW: Execute action chain steps
    if (actionChain_.isActive()) {
        auto currentTime = TimeInMilliseconds();

        if (currentTime >= actionChain_.nextStepTime) {
            auto& step = actionChain_.steps[actionChain_.currentStep];

            // Send this step's keybind
            if (step.keybind.isSet()) {
                Log::i().Print(Severity::Debug,
                    "Executing chain step {}/{} for '{}'",
                    actionChain_.currentStep + 1,
                    actionChain_.steps.size(),
                    actionChain_.sourceElement ? actionChain_.sourceElement->displayName() : "unknown");

                Input::i().SendKeybind(step.keybind, {});
            }

            // Move to next step
            actionChain_.currentStep++;

            if (actionChain_.currentStep >= actionChain_.steps.size()) {
                // Chain complete!
                Log::i().Print(Severity::Info,
                    "Action chain completed for '{}'",
                    actionChain_.sourceElement ? actionChain_.sourceElement->displayName() : "unknown");
                actionChain_.reset();
            } else {
                // Schedule next step
                actionChain_.nextStepTime = currentTime + step.delayAfterMs;
            }
        }
    }
}
```

**5. Update Wheel.cpp - OnFocusLost()**

Around line 800, clear chains when focus lost:
```cpp
void Wheel::OnFocusLost()
{
    // ... existing code ...

    // NEW: Cancel any active action chains
    if (actionChain_.isActive()) {
        Log::i().Print(Severity::Warning, "Canceling active action chain due to focus loss");
        actionChain_.reset();
    }
}
```

---

### Phase 2: UI Configuration (Settings Menu)

**Create UI to configure chains:**

In your wheel's settings menu (e.g., for build templates):

```cpp
// In MenuSectionKeybinds() or similar
void Wheel::MenuSectionActionChains()
{
    ImGui::TextUnformatted("Action Chains");
    ImGui::Separator();

    for (auto& element : wheelElements_) {
        ImGui::PushID(element->elementId());

        ImGui::TextUnformatted(element->displayName().c_str());

        bool useChain = element->hasActionChain();
        if (ImGui::Checkbox("Use Action Chain", &useChain)) {
            if (!useChain) {
                element->clearChain();
            }
        }

        if (useChain) {
            if (ImGui::Button("Add Step")) {
                element->addChainStep(WheelElement::KeybindStep(
                    element->nickname() + "_step_" + std::to_string(element->getChain().size()),
                    "Chain Step",
                    element->nickname(),
                    75  // Default 75ms delay
                ));
            }

            // Show each step
            auto& chain = element->getChain();
            for (size_t i = 0; i < chain.size(); i++) {
                ImGui::PushID(i);
                auto& step = chain[i];

                ImGui::Text("Step %zu:", i + 1);
                ImGui::SameLine();

                // Keybind input
                step.keybind.DrawKeybindInput();

                ImGui::SameLine();
                ImGui::SetNextItemWidth(80);
                ImGui::InputScalar("Delay (ms)", ImGuiDataType_U64, &step.delayAfterMs);

                ImGui::SameLine();
                if (ImGui::Button("Remove")) {
                    element->getChain().erase(element->getChain().begin() + i);
                    i--;  // Adjust index
                }

                ImGui::PopID();
            }
        }

        ImGui::PopID();
        ImGui::Spacing();
    }
}
```

---

### Phase 3: Config Persistence

**Update config serialization:**

In WheelElement.cpp, add save/load for chains:

```cpp
// Save to config.ini
void WheelElement::SaveChainToConfig() {
    if (!useActionChain_ || keybindChain_.empty())
        return;

    std::string chainKey = nickname_ + "_chain_count";
    Config::i().Set(category_, chainKey, keybindChain_.size());

    for (size_t i = 0; i < keybindChain_.size(); i++) {
        std::string stepKey = nickname_ + "_chain_step_" + std::to_string(i);
        auto& step = keybindChain_[i];

        // Save keybind
        step.keybind.Save();

        // Save delay
        Config::i().Set(category_, stepKey + "_delay", step.delayAfterMs);
    }
}

// Load from config.ini
void WheelElement::LoadChainFromConfig() {
    std::string chainKey = nickname_ + "_chain_count";
    size_t count = Config::i().Get<size_t>(category_, chainKey, 0);

    if (count == 0)
        return;

    useActionChain_ = true;
    keybindChain_.clear();

    for (size_t i = 0; i < count; i++) {
        std::string stepKey = nickname_ + "_chain_step_" + std::to_string(i);

        KeybindStep step(
            stepKey,
            "Chain Step " + std::to_string(i + 1),
            category_,
            Config::i().Get<mstime>(category_, stepKey + "_delay", 75)
        );

        keybindChain_.push_back(step);
    }
}
```

---

## 🎮 Example Usage Scenarios

### Scenario 1: Build + Equipment Swap
```
Wheel Element: "PvE DPS Setup"
├─ Step 1: Send Build Template 1 keybind
├─ Delay 75ms
└─ Step 2: Send Equipment Template 1 keybind

Wheel Element: "WvW Roaming Setup"
├─ Step 1: Send Build Template 5 keybind
├─ Delay 75ms
└─ Step 2: Send Equipment Template 5 keybind
```

### Scenario 2: Mount + Emote Combo
```
Wheel Element: "AFK Pose"
├─ Step 1: Send Raptor mount keybind
├─ Delay 200ms (wait for mount)
├─ Step 2: Send /sit keybind
├─ Delay 100ms
└─ Step 3: Send Musical Instrument keybind
```

### Scenario 3: Quick Template Test
```
Wheel Element: "Test All Builds"
├─ Step 1: Build Template 1
├─ Delay 500ms
├─ Step 2: Build Template 2
├─ Delay 500ms
└─ Step 3: Build Template 3
```

---

## ⚠️ Important Considerations

### 1. **Game Rate Limiting**
GW2 may have input rate limits. Keep delays >= 50ms between steps.

### 2. **Combat Interruption**
If combat starts mid-chain, should it:
- ✅ Complete the chain (recommended)
- ❌ Cancel immediately
- ⏸️ Pause and resume after combat

**Recommendation:** Complete the chain since you intentionally triggered it.

### 3. **Queuing with Chains**
Should action chains support conditional delays like mounts?
- **Phase 1:** No queuing, chains execute immediately
- **Phase 2:** Add queuing: "Wait until out of combat, then start chain"

### 4. **UI Feedback**
Show chain progress:
- Progress bar (Step 2/4)
- Remaining time indicator
- Animation at skill bar (like mount queue indicator)

### 5. **Error Handling**
What if a step fails?
- Continue to next step (resilient)
- Stop entire chain (fail-fast)

**Recommendation:** Continue execution. Most failures are timing issues.

---

## 🧪 Testing Plan

1. **Basic Chain Test**
   - Create 2-step chain: Build Template 1 → Equipment Template 1
   - Verify both keybinds sent in order with correct delay

2. **Long Chain Test**
   - Create 5+ step chain
   - Verify all steps execute
   - Check timing accuracy

3. **Interruption Test**
   - Start chain
   - Lose focus (Alt+Tab)
   - Verify chain cancels gracefully

4. **Config Persistence**
   - Create chain
   - Save and reload addon
   - Verify chain persists

5. **Performance Test**
   - Multiple wheels with chains
   - Verify no frame drops
   - Check memory usage

---

## 📅 Implementation Timeline

**Phase 1: Core Functionality (2-3 days)**
- Day 1: Data structures (WheelElement, Wheel changes)
- Day 2: Chain execution logic (OnUpdate, SendKeybindOrDelay)
- Day 3: Testing and debugging

**Phase 2: UI & Config (1-2 days)**
- Day 4: Settings menu UI
- Day 5: Config save/load, polish

**Phase 3: Polish & Extras (1-2 days)**
- Day 6: Visual feedback (progress indicator)
- Day 7: Documentation, edge cases

**Total: ~1 week of development time**

---

## 🚀 Quick Start Implementation

Want to start **right now**? Here's the minimal viable implementation:

### Step 1: Add to WheelElement.h (line 170)
```cpp
std::vector<Keybind> keybindChain_;  // Just store multiple keybinds
```

### Step 2: Modify Wheel::SendKeybindOrDelay() (line 1101)
```cpp
// If element has chain, queue it
if (element has keybindChain_ with items) {
    for (size_t i = 0; i < keybindChain_.size(); i++) {
        PostDelayed([kb = keybindChain_[i]]() {
            Input::i().SendKeybind(kb, {});
        }, i * 75);  // 75ms between each
    }
}
```

This bare-bones version will work immediately without complex state management!

---

## 💡 Future Enhancements

Once Phase 1-3 are working:

- **Conditional Steps**: "Only send step 2 if underwater"
- **Mouse Actions**: "Move mouse to position X,Y"
- **Wait Conditions**: "Wait until mount animation completes"
- **Sub-Wheels**: "Open another wheel as a step"
- **Import/Export**: Share action chains with friends
- **Macro Recording**: Record your actions, replay as chain

---

## ✅ Summary

**Your feature is absolutely implementable!**

The architecture already supports it with minimal changes:
- ✅ Keybind system works
- ✅ Queuing system exists
- ✅ Timing system in place
- ✅ Config persistence ready

You just need to:
1. Store multiple keybinds per element
2. Execute them sequentially with delays
3. Add UI to configure them

**This is a high-value feature that will significantly improve the addon!**

Want me to help you implement Phase 1 right now?
