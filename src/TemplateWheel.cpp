#include <TemplateWheel.h>
#include <imgui.h>
#include <Input.h>
#include <MumbleLink.h>
#include <UpdateCheck.h>
#include <ImGuiExtensions.h>
#include <Resource.h>

namespace GW2Radial
{

TemplateWheel::TemplateWheel(std::shared_ptr<Texture2D> bgTexture)
    : Wheel(std::move(bgTexture), "templates", "Build + Equipment Templates")
{
    // Enable queuing - templates will retry if in combat/underwater/etc.
    clearConditionalDelayOnSend_ = false;

    // Templates should queue when in combat (like mounts do), so we use UsableDefault (not UsableInCombat)
    // but VisibleInCombat so they remain visible when queued
    auto props = ConditionalProperties::UsableDefault | ConditionalProperties::VisibleDefault | ConditionalProperties::VisibleInCombat;

    // Create 9 template combo elements
    for (int i = 1; i <= NUM_TEMPLATE_COMBOS; i++)
    {
        std::string nickname = "template_combo" + std::to_string(i);
        std::string displayName = "Template Combo " + std::to_string(i);

        // Create keybind storage for this combo
        comboKeybinds_.push_back(std::make_unique<TemplateComboKeybinds>(nickname, displayName));

        // Create different colors for each combo
        float hue = (i - 1) * (360.0f / NUM_TEMPLATE_COMBOS);
        float r, g, b;
        ImGui::ColorConvertHSVtoRGB(hue / 360.0f, 0.7f, 0.9f, r, g, b);

        // Use resource ID for template icon (IDR_TEMPLATE1 + i - 1)
        auto element = std::make_unique<WheelElement>(
            IDR_TEMPLATE1 + (i - 1),
            nickname,
            "Templates",
            displayName,
            glm::vec4(r, g, b, 1.0f),
            props
        );

        // Use customBehaviorKeepProps to check if keybinds are configured
        // This acts as a pre-check (AND) before evaluating props
        size_t index = i - 1;
        element->customBehaviorKeepProps([this, index](bool visibility) {
            // Check if any keybinds are configured for this combo
            if (index >= comboKeybinds_.size())
                return false;

            bool hasKeybind = comboKeybinds_[index]->buildTemplateKeybind->isSet() ||
                              comboKeybinds_[index]->equipTemplateKeybind->isSet();
            return hasKeybind;
        });

        AddElement(std::move(element));

        // Initialize the action chain immediately after adding the element
        // This sets the dummy keybind so isBound() works from the start
        UpdateActionChain(index);
    }
}

void TemplateWheel::UpdateActionChain(size_t elementIndex)
{
    if (elementIndex >= wheelElements_.size() || elementIndex >= comboKeybinds_.size())
        return;

    auto& element = wheelElements_[elementIndex];
    auto& keybinds = comboKeybinds_[elementIndex];

    // Clear existing chain
    element->clearChain();

    // Check if any keybinds are configured
    bool hasAnyKeybind = keybinds->buildTemplateKeybind->isSet() || keybinds->equipTemplateKeybind->isSet();

    // Set a dummy keybind so isBound() returns true (required for isVisible/isUsable logic)
    if (hasAnyKeybind)
    {
        // Set the first configured keybind as the element's keybind (just for isBound() check)
        if (keybinds->buildTemplateKeybind->isSet())
            element->keybind().keyCombo(keybinds->buildTemplateKeybind->keyCombo());
        else
            element->keybind().keyCombo(keybinds->equipTemplateKeybind->keyCombo());
    }
    else
    {
        // No keybinds configured, clear the element's keybind
        element->keybind().keyCombo(KeyCombo());
        return;
    }

    // Step 1: Build Template keybind (if set)
    if (keybinds->buildTemplateKeybind->isSet())
    {
        WheelElement::KeybindStep buildStep(
            keybinds->buildTemplateKeybind->keyCombo(),
            75  // 75ms delay before equipment switch
        );
        element->addChainStep(buildStep);
    }

    // Step 2: Equipment Template keybind (if set)
    if (keybinds->equipTemplateKeybind->isSet())
    {
        WheelElement::KeybindStep equipStep(
            keybinds->equipTemplateKeybind->keyCombo(),
            75
        );
        element->addChainStep(equipStep);
    }
}

void TemplateWheel::OnUpdate()
{
    Wheel::OnUpdate();

    // Ensure props are correct for all template elements
    // Templates should NEVER be usable in combat (this enables queuing)
    static bool propsFixed = false;
    if (!propsFixed)
    {
        for (auto& element : wheelElements_)
        {
            auto currentProps = element->props();
            // Remove UsableInCombat if it was set
            if (NotNone(currentProps & ConditionalProperties::UsableInCombat))
            {
                currentProps = ConditionalProperties(ToUnderlying(currentProps) & ~ToUnderlying(ConditionalProperties::UsableInCombat));
                element->props(currentProps);
            }
        }
        propsFixed = true;
    }

    // Update action chains on first update and whenever keybinds change
    static std::vector<std::pair<KeyCombo, KeyCombo>> lastKeyCombos;

    if (lastKeyCombos.empty())
    {
        lastKeyCombos.resize(comboKeybinds_.size());
        for (size_t i = 0; i < comboKeybinds_.size(); i++)
        {
            lastKeyCombos[i] = {
                comboKeybinds_[i]->buildTemplateKeybind->keyCombo(),
                comboKeybinds_[i]->equipTemplateKeybind->keyCombo()
            };
            UpdateActionChain(i);
        }
    }
    else
    {
        // Check if any keybinds have changed
        for (size_t i = 0; i < comboKeybinds_.size(); i++)
        {
            auto currentBuild = comboKeybinds_[i]->buildTemplateKeybind->keyCombo();
            auto currentEquip = comboKeybinds_[i]->equipTemplateKeybind->keyCombo();

            if (!(currentBuild == lastKeyCombos[i].first) || !(currentEquip == lastKeyCombos[i].second))
            {
                lastKeyCombos[i] = { currentBuild, currentEquip };
                UpdateActionChain(i);
            }
        }
    }
}

void TemplateWheel::DrawMenu(Keybind** currentEditedKeybind)
{
    ImGui::PushID((nickname_ + "Elements").c_str());

    // Skip the "In-game Keybinds" section (lines 170-175 in base Wheel::DrawMenu)
    // We don't want individual slot keybinds

    UI::Title("Keybinds");

    ImGui::KeybindInput((Keybind&)keybind_, currentEditedKeybind, "Pressing this key combination will open the radial menu at your cursor's current location.");
    ImGui::KeybindInput((Keybind&)centralKeybind_, currentEditedKeybind,
                        "Pressing this key combination will open the radial menu in the middle of the screen. Your cursor will be moved to the middle of the screen and moved back "
                        "after you have selected an option.");

    MenuSectionKeybinds(currentEditedKeybind);

    if (ImGui::ConfigurationWrapper(&ImGui::Checkbox, enableConditionsOption_) && conditions_)
        conditions_->enable(enableConditionsOption_.value());

    {
        UI::Scoped::Disable disable(!conditions_ || !enableConditionsOption_.value());

        ImGui::Indent();

        conditions_->DrawMenu();

        ImGui::Unindent();
    }

    UI::Title("Display Options");

    ImGui::ConfigurationWrapper(&ImGui::SliderInt, opacityMultiplierOption_, 0, 100, "%d %%", ImGuiSliderFlags_AlwaysClamp);
    UI::HelpTooltip("Transparency of the entire overlay. Setting to 0% hides it entirely.");

    ImGui::ConfigurationWrapper(&ImGui::SliderInt, animationTimeOption_, 0, 2000, "%d ms", ImGuiSliderFlags_AlwaysClamp);
    UI::HelpTooltip("Amount of time, in milliseconds, for the radial menu to fade in.");

    ImGui::ConfigurationWrapper(&ImGui::SliderFloat, animationScale_, 0.f, 1.f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
    UI::HelpTooltip("Intensity of the 3D animations.");

    ImGui::ConfigurationWrapper(&ImGui::SliderFloat, scaleOption_, 0.25f, 4.f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
    UI::HelpTooltip("Scale factor for the size of the whole radial menu.");

    ImGui::ConfigurationWrapper(&ImGui::SliderFloat, centerScaleOption_, 0.05f, 0.5f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
    UI::HelpTooltip("Scale factor for the size of just the central region of the radial menu.");

    ImGui::ConfigurationWrapper(&ImGui::SliderInt, displayDelayOption_, 0, 3000, "%d ms", ImGuiSliderFlags_AlwaysClamp);
    UI::HelpTooltip("Amount of time, in milliseconds, to wait before displaying the radial menu. The input bound to the central region can still be sent by releasing the key, "
                    "even before the menu is visible.");

    ImGui::ConfigurationWrapper(&ImGui::Checkbox, showOverGameUIOption_);
    UI::HelpTooltip("Either show the radial menu over or under the game's UI.");

    MenuSectionDisplay();

    UI::Title("Interaction Options");

    {
        UI::Scoped::Disable disable(clickSelectOption_.value());
        if (clickSelectOption_.value())
            noHoldOption_.value() = false;

        ImGui::ConfigurationWrapper(&ImGui::Checkbox, noHoldOption_);
        UI::HelpTooltip("This option will activate whichever option is hovered first. Any key pressed to activate the menu can be released immediately without dismissing it. "
                        "Mutually exclusive with \"click to select\".");
    }

    {
        UI::Scoped::Disable disable(noHoldOption_.value());
        if (noHoldOption_.value())
            clickSelectOption_.value() = false;

        ImGui::ConfigurationWrapper(&ImGui::Checkbox, clickSelectOption_);
        UI::HelpTooltip("This option will only activate an option when it is clicked on. Any key pressed to activate the menu can be released immediately without dismissing it. "
                        "Mutually exclusive with \"hover to select\".");
    }

    ImGui::ConfigurationWrapper(&ImGui::Checkbox, resetCursorOnLockedKeybindOption_);
    UI::HelpTooltip("Moves the cursor to the center of the screen when the \"show in center\" keybind is used.");

    ImGui::ConfigurationWrapper(&ImGui::Checkbox, lockCameraWhenOverlayedOption_);
    UI::HelpTooltip("Prevents the camera from being affected by mouse movements while the menu is displayed.");

    if (!alwaysResetCursorPositionBeforeKeyPress_)
    {
        ImGui::ConfigurationWrapper(&ImGui::Checkbox, resetCursorAfterKeybindOption_);
        UI::HelpTooltip("Once the menu is dismissed, moves the cursor to where it was on screen before the menu was displayed.");
    }

    MenuSectionInteraction();

    UI::Title("Queuing Options");

    ImGui::ConfigurationWrapper(&ImGui::Checkbox, enableQueuingOption_);
    UI::HelpTooltip("If sending a keybind now would be ignored by the game (e.g., switching templates while in combat), enabling queuing will \"queue\" the input until all necessary "
                    "conditions are satisfied.");

    {
        UI::Scoped::Disable disable(!enableQueuingOption_.value());

        ImGui::PushItemWidth(0.33f * ImGui::GetWindowWidth());
        ImGui::ConfigurationWrapper(&ImGui::InputIntFormat, conditionalDelayDelayOption_, "%d ms", 1, 100, 0);
        UI::HelpTooltip("Time, in milliseconds, to wait before sending a queued input once all conditions pass. Increasing this value can make activation more reliable.");
        ImGui::ConfigurationWrapper(&ImGui::InputIntFormat, maximumConditionalWaitTimeOption_, "%d s", 1, 10, 0);
        UI::HelpTooltip("Maximum amount of time, in seconds, to wait with a queued input before dismissing it.");
        ImGui::PopItemWidth();

        {
            UI::Scoped::Disable disable2(noHoldOption_.value());
            ImGui::ConfigurationWrapper(&ImGui::Checkbox, centerCancelDelayedInputOption_);
            UI::HelpTooltip("If the center region has no bound behavior, this option makes it cancel any queued input.");
        }

        MenuSectionQueuing();
    }

    UI::Title("Visibility & Ordering");

    ImGui::Text("Ordering top to bottom is clockwise starting at noon.");

    ImGui::PushStyleColor(ImGuiCol_TableRowBg, 0);
    ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, (ImGui::GetColorU32(ImGuiCol_FrameBg) & 0xFFFFFF) | 0x33000000);
    if (ImGui::BeginTable("##OrderingTable", 3, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_RowBg))
    {
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Show/Use Conditions", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("##UpDown", ImGuiTableColumnFlags_WidthFixed);

        ImGui::TableHeadersRow();

        for (auto it = sortedWheelElements_.begin(); it != sortedWheelElements_.end(); ++it)
        {
            const auto extremum = it == sortedWheelElements_.begin() ? 1 : it == std::prev(sortedWheelElements_.end()) ? -1 : 0;
            auto&      e        = *it;
            if (const auto dir = e->DrawPriority(extremum); dir != 0)
            {
                if (dir == 1 && e == sortedWheelElements_.front() || dir == -1 && e == sortedWheelElements_.back())
                    continue;

                auto& eOther = dir == 1 ? *std::prev(it) : *std::next(it);
                std::swap(e, eOther);
                const auto tempPriority = eOther->sortingPriority();
                eOther->sortingPriority(e->sortingPriority());
                e->sortingPriority(tempPriority);
            }
        }

        ImGui::EndTable();
    }
    ImGui::PopStyleColor(2);

    MenuSectionMisc();

    ImGui::PopID();
}

void TemplateWheel::MenuSectionKeybinds(Keybind** keybindInEdit)
{
    ImGui::Spacing();
    UI::Title("Template Combinations");

    ImGui::TextWrapped("Configure which Build and Equipment templates each combo slot should trigger. Match these to your in-game keybinds (F11, Control Options). Slots with at least one keybind configured will appear on the wheel.");
    ImGui::Spacing();

    // Display keybind inputs for each combo
    for (size_t i = 0; i < comboKeybinds_.size(); i++)
    {
        auto& element = wheelElements_[i];
        auto& keybinds = comboKeybinds_[i];

        ImGui::PushID(static_cast<int>(i));

        // Combo slot header
        ImGui::TextUnformatted(element->displayName().c_str());
        ImGui::Indent();

        // Build template keybind
        ImGui::KeybindInput(*keybinds->buildTemplateKeybind, keybindInEdit, "The in-game keybind for this Build Template.");

        // Equipment template keybind
        ImGui::KeybindInput(*keybinds->equipTemplateKeybind, keybindInEdit, "The in-game keybind for this Equipment Template.");

        ImGui::Unindent();
        ImGui::Spacing();

        ImGui::PopID();
    }
}

} // namespace GW2Radial
