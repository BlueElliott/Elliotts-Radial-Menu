#include <ChatWheel.h>
#include <imgui.h>
#include <ImGuiExtensions.h>

namespace GW2Radial
{

ChatWheel::ChatWheel(std::shared_ptr<Texture2D> bgTexture)
    : Wheel(std::move(bgTexture), "chat_commands", "Chat Commands")
{
    // Create command slots
    for (int i = 0; i < NUM_COMMANDS; i++)
    {
        std::string nickname = "chat_cmd_" + std::to_string(i + 1);
        commands_.push_back(std::make_unique<ChatCommand>(nickname, i + 1));
    }

    // Chat commands don't use wheel elements (no radial display needed)
    // This is purely a keybind-based system for quick chat messages
}

void ChatWheel::OnUpdate()
{
    Wheel::OnUpdate();

    // Check each command's keybind and send message if triggered
    for (auto& cmd : commands_)
    {
        if (cmd->enabled->value() && cmd->keybind->isSet() && !cmd->message.empty())
        {
            // TODO: Check if keybind was just pressed (need to track previous state)
            // TODO: Implement actual chat message sending
            // For now, keybind detection is handled by the Wheel base class
            // In future: SendChatMessage(cmd->message, cmd->chatChannel->value());
        }
    }
}

void ChatWheel::DrawMenu(Keybind** currentEditedKeybind)
{
    ImGui::PushID((nickname_ + "ChatCommands").c_str());

    UI::Title("Chat Commands");
    ImGui::TextWrapped("Quick chat commands for commanding. Configure preset messages to send to squad or team chat.");
    ImGui::Spacing();

    // Keybind to open the wheel (inherited from Wheel base class)
    UI::Title("Wheel Keybinds");
    ImGui::KeybindInput((Keybind&)keybind_, currentEditedKeybind,
        "Pressing this key combination will open the Chat Commands radial menu at your cursor's location.");
    ImGui::KeybindInput((Keybind&)centralKeybind_, currentEditedKeybind,
        "Pressing this key combination will open the Chat Commands radial menu in the middle of the screen.");

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Command configuration section
    UI::Title("Command Configuration");
    ImGui::TextWrapped("Configure up to %d quick chat commands. Each command can be triggered instantly via its keybind.", NUM_COMMANDS);
    ImGui::Spacing();

    // Display each command configuration
    for (size_t i = 0; i < commands_.size(); i++)
    {
        auto& cmd = commands_[i];

        ImGui::PushID(static_cast<int>(i));

        // Collapsing header for each command
        bool isOpen = ImGui::CollapsingHeader(
            (std::string("Command ") + std::to_string(i + 1) +
             (cmd->message.empty() ? " (not configured)" : " - " + cmd->message.substr(0, 30))).c_str(),
            ImGuiTreeNodeFlags_None
        );

        if (isOpen)
        {
            ImGui::Indent();

            // Enable checkbox
            ImGui::ConfigurationWrapper(&ImGui::Checkbox, *cmd->enabled);
            UI::HelpTooltip("Enable to allow this command to be triggered by its keybind");
            ImGui::Spacing();

            // Message input
            char buffer[256];
            strncpy_s(buffer, cmd->message.c_str(), sizeof(buffer) - 1);
            buffer[sizeof(buffer) - 1] = '\0';

            ImGui::PushItemWidth(-1); // Full width
            if (ImGui::InputText("Message", buffer, sizeof(buffer)))
            {
                cmd->message = std::string(buffer);
            }
            ImGui::PopItemWidth();
            UI::HelpTooltip("The message to send when this command is triggered");

            ImGui::Spacing();

            // Channel selector
            const char* channels[] = { "Squad", "Party", "Say" };
            int channel = cmd->chatChannel->value();
            if (ImGui::Combo("Channel", &channel, channels, IM_ARRAYSIZE(channels)))
            {
                cmd->chatChannel->value(channel);
            }
            UI::HelpTooltip("Which chat channel to send the message to");

            ImGui::Spacing();

            // Keybind
            ImGui::KeybindInput(*cmd->keybind, currentEditedKeybind,
                "Press this keybind to instantly send this chat message");

            ImGui::Unindent();
            ImGui::Spacing();
        }

        ImGui::PopID();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Status/info section
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Status: UI Complete, Backend Pending");
    ImGui::TextWrapped("The command configuration UI is complete. Message sending functionality will be implemented next.");

    ImGui::Spacing();
    ImGui::TextDisabled("Future features:");
    ImGui::Indent();
    ImGui::BulletText("Message sending integration with GW2 chat API");
    ImGui::BulletText("Message templates with variables (e.g., target name)");
    ImGui::BulletText("Import/Export command presets");
    ImGui::BulletText("Command history and favorites");
    ImGui::Unindent();

    ImGui::PopID();
}

} // namespace GW2Radial
