#pragma once
#include <Main.h>
#include <Wheel.h>

namespace GW2Radial
{

// Holds keybind configuration for one template combo slot
struct TemplateComboKeybinds
{
    std::unique_ptr<Keybind> buildTemplateKeybind;
    std::unique_ptr<Keybind> equipTemplateKeybind;

    TemplateComboKeybinds(const std::string& nickname, const std::string& displayName)
        : buildTemplateKeybind(std::make_unique<Keybind>(nickname + "_build", displayName + " Build", "Templates"))
        , equipTemplateKeybind(std::make_unique<Keybind>(nickname + "_equip", displayName + " Equipment", "Templates"))
    {
    }
};

class TemplateWheel : public Wheel
{
public:
    TemplateWheel(std::shared_ptr<Texture2D> bgTexture);

protected:
    void OnUpdate() override;
    void DrawMenu(Keybind** currentHover) override;
    void MenuSectionKeybinds(Keybind** keybindInEdit) override;

private:
    void UpdateActionChain(size_t elementIndex);

    // Number of template combos to create (1-9)
    static constexpr int NUM_TEMPLATE_COMBOS = 9;

    // Keybind storage for each combo slot
    std::vector<std::unique_ptr<TemplateComboKeybinds>> comboKeybinds_;
};

} // namespace GW2Radial
