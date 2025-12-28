#pragma once
#include <Main.h>
#include <Wheel.h>

namespace GW2Radial
{

// Holds configuration for one chat command
struct ChatCommand
{
    std::string message; // The chat message to send
    std::string label;   // The label to display on the wheel
    std::string nickname_; // Nickname for config storage
    std::unique_ptr<ConfigurationOption<int>> chatChannel; // 0=squad, 1=party, 2=say, 3=broadcast, 4=dynamic
    std::unique_ptr<ConfigurationOption<bool>> enabled;

    ChatCommand(const std::string& nickname, int index)
        : message("")
        , label("Command " + std::to_string(index))
        , nickname_(nickname)
        , chatChannel(std::make_unique<ConfigurationOption<int>>(
            nickname + "_channel", "Command " + std::to_string(index) + " Channel", "Chat Commands", 4)) // Default to dynamic
        , enabled(std::make_unique<ConfigurationOption<bool>>(
            nickname + "_enabled", "Command " + std::to_string(index) + " Enabled", "Chat Commands", false))
    {
        LoadMessage();
        LoadLabel();
    }

    void LoadMessage();
    void SaveMessage() const;
    void LoadLabel();
    void SaveLabel() const;

    // Default conditional properties for commands
    static ConditionalProperties GetDefaultProps()
    {
        // By default, commands are visible and usable everywhere
        // We include all states but only expose Default and WvW controls in the UI
        return ConditionalProperties::VisibleAll | ConditionalProperties::UsableAll;
    }
};

class ChatWheel : public Wheel
{
public:
    ChatWheel(std::shared_ptr<Texture2D> bgTexture);

    void DrawOffscreen(ID3D11DeviceContext* ctx);

protected:
    void DrawMenu(Keybind** currentHover) override;
    void OnUpdate() override;
    Keybind* GetKeybindFromOpt(OptKeybindWheelElement& o) override;

private:
    static constexpr int NUM_COMMANDS = 8; // Number of configurable commands
    std::vector<std::unique_ptr<ChatCommand>> commands_;
    std::vector<WheelElement*> wheelElements_; // Pointers to wheel elements for updating labels
    Texture2D placeholderTexture_; // Shared placeholder texture for all command elements

    // Dynamic mode fallback channel: 0=squad (/d), 2=say (/s)
    ConfigurationOption<int> dynamicFallbackChannel_;

    // Text rendering for labels
    struct LabelTexture {
        RenderTarget rt;
        std::wstring text;
        bool needsRedraw = true;
    };
    std::vector<LabelTexture> labelTextures_;
    ComPtr<ID3D11BlendState> textBlendState_;

    void SendChatMessage(const std::string& message, int channel);
    void SendTextToChat(const std::string& text, bool broadcast = false);
    void UpdateElementLabel(size_t index);
    void RegenerateTexture(size_t index, ID3D11DeviceContext* ctx);
    int DetermineActualChannel(int configuredChannel) const;

    static glm::vec4 GetCommandColor(int index);
    Texture2D CreatePlaceholderTexture();
};

} // namespace GW2Radial
