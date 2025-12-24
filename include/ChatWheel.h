#pragma once
#include <Main.h>
#include <Wheel.h>

namespace GW2Radial
{

// Holds configuration for one chat command
struct ChatCommand
{
    std::string message; // The chat message to send
    std::unique_ptr<ConfigurationOption<int>> chatChannel; // 0=squad, 1=party, 2=say
    std::unique_ptr<ConfigurationOption<bool>> enabled;
    std::unique_ptr<Keybind> keybind;

    ChatCommand(const std::string& nickname, int index)
        : message("")
        , chatChannel(std::make_unique<ConfigurationOption<int>>(
            nickname + "_channel", "Command " + std::to_string(index) + " Channel", "Chat Commands", 0))
        , enabled(std::make_unique<ConfigurationOption<bool>>(
            nickname + "_enabled", "Command " + std::to_string(index) + " Enabled", "Chat Commands", false))
        , keybind(std::make_unique<Keybind>(
            nickname + "_keybind", "Command " + std::to_string(index), "Chat Commands"))
    {
    }
};

class ChatWheel : public Wheel
{
public:
    ChatWheel(std::shared_ptr<Texture2D> bgTexture);

protected:
    void DrawMenu(Keybind** currentHover) override;
    void OnUpdate() override;

private:
    static constexpr int NUM_COMMANDS = 8; // Number of configurable commands
    std::vector<std::unique_ptr<ChatCommand>> commands_;
};

} // namespace GW2Radial
