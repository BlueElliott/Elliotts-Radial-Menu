#include <ChatWheel.h>
#include <imgui.h>
#include <ImGuiExtensions.h>

namespace GW2Radial
{

ChatWheel::ChatWheel(std::shared_ptr<Texture2D> bgTexture)
    : Wheel(std::move(bgTexture), "chat_commands", "Chat Commands")
{
    // Chat commands don't need any wheel elements for now - just settings UI
    // This is a placeholder wheel that will be expanded with actual chat command functionality
}

void ChatWheel::DrawMenu(Keybind** currentEditedKeybind)
{
    ImGui::PushID((nickname_ + "ChatCommands").c_str());

    UI::Title("Chat Commands");
    ImGui::TextWrapped("Quick chat commands for commanding. Configure preset messages to send to squad or team chat.");
    ImGui::Spacing();

    // Placeholder for future chat command configuration
    ImGui::TextDisabled("Chat command configuration will be added here.");
    ImGui::Spacing();

    ImGui::TextDisabled("Features planned:");
    ImGui::Indent();
    ImGui::BulletText("Preset squad commands");
    ImGui::BulletText("Quick target calls");
    ImGui::BulletText("Keybind shortcuts for common messages");
    ImGui::BulletText("Custom message templates");
    ImGui::Unindent();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::TextWrapped("This feature is under development. The UI will be expanded to allow configuring chat commands that can be triggered via keybinds for quick communication during squad commanding.");

    ImGui::PopID();
}

} // namespace GW2Radial
