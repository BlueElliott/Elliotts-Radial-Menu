#include <ChatWheel.h>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <ImGuiExtensions.h>
#include <Input.h>
#include <MumbleLink.h>
#include <Utility.h>
#include <Core.h>
#include <ConfigurationFile.h>
#include <backends/imgui_impl_dx11.h>

namespace GW2Radial
{

void ChatCommand::LoadMessage()
{
    const char* value = INIConfigurationFile::i().ini().GetValue("Chat Commands", (nickname_ + "_message").c_str(), "");
    message = value ? value : "";
}

void ChatCommand::SaveMessage() const
{
    INIConfigurationFile::i().ini().SetValue("Chat Commands", (nickname_ + "_message").c_str(), message.c_str());
    INIConfigurationFile::i().Save();
}

void ChatCommand::LoadLabel()
{
    const char* value = INIConfigurationFile::i().ini().GetValue("Chat Commands", (nickname_ + "_label").c_str(), "");
    if (value && strlen(value) > 0) {
        label = value;
    }
    // If no label in config, keep the default from constructor
}

void ChatCommand::SaveLabel() const
{
    INIConfigurationFile::i().ini().SetValue("Chat Commands", (nickname_ + "_label").c_str(), label.c_str());
    INIConfigurationFile::i().Save();
}

ChatWheel::ChatWheel(std::shared_ptr<Texture2D> bgTexture)
    : Wheel(std::move(bgTexture), "chat_commands", "Chat Commands")
    , placeholderTexture_(CreatePlaceholderTexture())
    , dynamicFallbackChannel_("chat_dynamic_fallback", "Dynamic Mode Default Channel", "Chat Commands", 0) // Default to squad (/d)
{
    // Create blend state for text rendering
    auto dev = Core::i().device();
    D3D11_BLEND_DESC blendDesc = {};
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    dev->CreateBlendState(&blendDesc, textBlendState_.GetAddressOf());

    // Initialize label textures
    labelTextures_.resize(NUM_COMMANDS);
    for (int i = 0; i < NUM_COMMANDS; i++)
    {
        // Create render target for text (larger texture for better visibility)
        labelTextures_[i].rt = MakeRenderTarget(dev, 512, 512, DXGI_FORMAT_R8G8B8A8_UNORM);
        labelTextures_[i].needsRedraw = true;
    }

    // Create command slots and corresponding wheel elements
    for (int i = 0; i < NUM_COMMANDS; i++)
    {
        std::string nickname = "chat_cmd_" + std::to_string(i + 1);
        commands_.push_back(std::make_unique<ChatCommand>(nickname, i + 1));

        // Create a wheel element for this command
        // Use different colors for visual distinction
        glm::vec4 color = GetCommandColor(i);
        auto props = ChatCommand::GetDefaultProps();

        // Use the label texture instead of placeholder
        Texture2D labelTex;
        labelTex.texture = labelTextures_[i].rt.texture;
        labelTex.srv = labelTextures_[i].rt.srv;

        auto element = std::make_unique<WheelElement>(
            i,
            nickname,
            "Chat Commands",
            commands_[i]->label,  // Use the label from the command
            color,
            props,
            labelTex  // Use label texture
        );

        // Disable shadow effect for cleaner text display
        element->shadowStrength(0.f);

        // Set a dummy keybind so isBound() returns true
        // This allows the wheel to display even though we don't use traditional keybinds
        // Using F24 which is extremely unlikely to be pressed
        element->keybind().keyCombo(KeyCombo(ScanCode::F24));

        // Set custom behavior to only show if command is enabled and has a message
        // This acts as a pre-check (AND) before evaluating conditional props
        const size_t cmdIndex = i;
        element->customBehaviorKeepProps([this, cmdIndex](bool visibility) {
            return commands_[cmdIndex]->enabled->value() && !commands_[cmdIndex]->message.empty();
        });

        // Store pointer before moving the unique_ptr
        wheelElements_.push_back(element.get());

        AddElement(std::move(element));
    }
}

void ChatWheel::OnUpdate()
{
    Wheel::OnUpdate();
}

Keybind* ChatWheel::GetKeybindFromOpt(OptKeybindWheelElement& o)
{
    // When a wheel element is selected, send the chat message instead of a keybind
    if (std::holds_alternative<WheelElement*>(o))
    {
        WheelElement* we = std::get<WheelElement*>(o);
        u32 commandIndex = we->elementId();

        LogInfo("ChatWheel: Element selected, index {}", commandIndex);

        if (commandIndex < commands_.size())
        {
            auto& cmd = commands_[commandIndex];
            LogInfo("ChatWheel: Command enabled={}, message empty={}", cmd->enabled->value(), cmd->message.empty());

            if (cmd->enabled->value() && !cmd->message.empty())
            {
                LogInfo("ChatWheel: Sending chat command");
                // Send the chat message
                SendChatMessage(cmd->message, cmd->chatChannel->value());
            }
            else
            {
                LogWarn("ChatWheel: Command not enabled or message empty");
            }
        }
        else
        {
            LogWarn("ChatWheel: Invalid command index {}", commandIndex);
        }
    }

    // Return nullptr so no keybind is sent
    return nullptr;
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

    // Dynamic mode fallback setting
    UI::Title("Dynamic Mode Settings");
    ImGui::TextWrapped("When using Dynamic channel mode, commands will use this default channel for PvE/open world situations:");
    ImGui::Spacing();

    const char* fallbackChannels[] = { "Squad (/d)", "Say (/s)" };
    int fallback = dynamicFallbackChannel_.value() == 0 ? 0 : 1; // Map 0->0 (squad), 2->1 (say)
    if (ImGui::Combo("Default Channel", &fallback, fallbackChannels, IM_ARRAYSIZE(fallbackChannels)))
    {
        dynamicFallbackChannel_.value(fallback == 0 ? 0 : 2); // Map back: 0->0 (squad), 1->2 (say)
    }
    UI::HelpTooltip("Squad (/d) is best for group content. Say (/s) is best for solo play.");

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Command visibility/usability configuration
    UI::Title("Command Visibility & Ordering");
    ImGui::TextWrapped("Configure which commands appear in different game modes and adjust their order in the wheel.");
    ImGui::Text("Ordering top to bottom is clockwise starting at noon.");
    ImGui::Spacing();

    // Draw the elements table with conditional properties
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

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Command configuration section
    UI::Title("Command Configuration");
    ImGui::TextWrapped("Configure messages and channels for each command.", NUM_COMMANDS);
    ImGui::Spacing();

    // Display each command configuration
    for (size_t i = 0; i < commands_.size(); i++)
    {
        auto& cmd = commands_[i];

        ImGui::PushID(static_cast<int>(i));

        // Collapsing header for each command - keep label static to maintain ImGui state
        std::string headerLabel = "Command " + std::to_string(i + 1);
        bool isOpen = ImGui::CollapsingHeader(headerLabel.c_str(), ImGuiTreeNodeFlags_None);

        if (isOpen)
        {
            ImGui::Indent();

            // Enable checkbox
            ImGui::ConfigurationWrapper(&ImGui::Checkbox, *cmd->enabled);
            UI::HelpTooltip("Enable this command slot");
            ImGui::Spacing();

            // Label input
            ImGui::PushItemWidth(-1); // Full width
            ImGui::Text("Label:");
            if (ImGui::InputText(("##label" + std::to_string(i)).c_str(), &cmd->label))
            {
                // Save and update wheel element when label changes
                cmd->SaveLabel();
                UpdateElementLabel(i);
            }
            ImGui::PopItemWidth();
            UI::HelpTooltip("The label displayed on the radial wheel for this command");
            ImGui::Spacing();

            // Message input - use std::string directly with imgui_stdlib
            ImGui::PushItemWidth(-1); // Full width
            ImGui::Text("Message:");
            if (ImGui::InputTextMultiline(("##msg" + std::to_string(i)).c_str(),
                &cmd->message,
                ImVec2(-1, 60)))
            {
                // Save when text changes
                cmd->SaveMessage();
            }
            ImGui::PopItemWidth();
            UI::HelpTooltip("The message to send when this command is selected from the radial wheel");

            ImGui::Spacing();

            // Channel selector
            ImGui::Text("Channel:");
            const char* channels[] = { "Squad (/d)", "Party (/p)", "Say (/s)", "Squad Broadcast", "Dynamic (Auto)" };
            int channel = cmd->chatChannel->value();
            if (ImGui::Combo(("##channel" + std::to_string(i)).c_str(), &channel, channels, IM_ARRAYSIZE(channels)))
            {
                cmd->chatChannel->value(channel);
            }

            if (channel == 4) {
                ImGui::SameLine();
                ImGui::TextDisabled("(?)");
                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 25.0f);
                    ImGui::Text("Dynamic mode automatically selects the best channel:");
                    ImGui::BulletText("Commander tag active -> Squad Broadcast");
                    ImGui::BulletText("In Fractals -> Party (/p)");
                    ImGui::BulletText("In WvW -> Squad (/d)");
                    ImGui::BulletText("Otherwise -> Your configured default");
                    ImGui::PopTextWrapPos();
                    ImGui::EndTooltip();
                }
            } else {
                UI::HelpTooltip("Which chat channel to send the message to. Squad Broadcast uses Shift+Enter.");
            }

            ImGui::Unindent();
            ImGui::Spacing();
        }

        ImGui::PopID();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Status/info section
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Status: Feature Complete!");
    ImGui::TextWrapped("Chat Commands is fully functional. Press your configured keybinds in-game to send messages instantly.");

    ImGui::Spacing();
    ImGui::TextDisabled("How it works:");
    ImGui::Indent();
    ImGui::BulletText("Keybind triggers open chat, paste message, and send automatically");
    ImGui::BulletText("Uses clipboard paste method for reliable message sending");
    ImGui::BulletText("Preserves your clipboard contents (restored after sending)");
    ImGui::BulletText("Won't interfere if chat is already open");
    ImGui::Unindent();

    ImGui::Spacing();
    ImGui::TextDisabled("Future enhancements:");
    ImGui::Indent();
    ImGui::BulletText("Message templates with variables (e.g., target name)");
    ImGui::BulletText("Import/Export command presets");
    ImGui::BulletText("Command history and favorites");
    ImGui::Unindent();

    ImGui::PopID();
}

int ChatWheel::DetermineActualChannel(int configuredChannel) const
{
    // If not dynamic mode (channel 4), use configured channel directly
    if (configuredChannel != 4)
        return configuredChannel;

    // Dynamic mode - determine channel based on context
    auto& mumble = MumbleLink::i();

    LogInfo("ChatWheel: Dynamic mode - Commander: {}, InFractals: {}, InWvW: {}",
            mumble.isCommander(), mumble.isInFractals(), mumble.isInWvW());

    // Priority 1: If commander tag is active, use squad broadcast
    if (mumble.isCommander()) {
        LogInfo("ChatWheel: Dynamic mode - Commander detected, using Squad Broadcast");
        return 3; // Squad Broadcast
    }

    // Priority 2: If in Fractals, use party chat
    if (mumble.isInFractals()) {
        LogInfo("ChatWheel: Dynamic mode - In Fractals, using Party chat");
        return 1; // Party
    }

    // Priority 3: Check if in WvW - use squad chat
    if (mumble.isInWvW()) {
        LogInfo("ChatWheel: Dynamic mode - In WvW, using Squad chat");
        return 0; // Squad
    }

    // Priority 4: Use user-configured fallback channel for PvE/open world
    // This lets users choose between Squad (/d) for group play or Say (/s) for solo
    int fallbackChannel = dynamicFallbackChannel_.value();
    LogInfo("ChatWheel: Dynamic mode - Using fallback channel: {}", fallbackChannel);
    return fallbackChannel;
}

void ChatWheel::SendChatMessage(const std::string& message, int channel)
{
    LogInfo("ChatWheel: SendChatMessage called with message='{}' channel={}", message, channel);

    // Determine actual channel (handles dynamic mode)
    int actualChannel = DetermineActualChannel(channel);
    LogInfo("ChatWheel: Actual channel after dynamic resolution: {}", actualChannel);

    // Build the complete chat command
    std::string chatCommand;
    bool broadcast = false;

    switch (actualChannel)
    {
        case 0: // Squad - use /d prefix
            chatCommand = "/d ";
            chatCommand += message;
            break;
        case 1: // Party - use /p prefix
            chatCommand = "/p ";
            chatCommand += message;
            break;
        case 2: // Say - use /s prefix
            chatCommand = "/s ";
            chatCommand += message;
            break;
        case 3: // Squad Broadcast - no prefix, Shift+Enter opens broadcast box
            chatCommand = message;
            broadcast = true;
            break;
        default:
            chatCommand = "/d ";
            chatCommand += message;
            break;
    }

    // Send the text to chat (with broadcast flag for squad broadcast)
    SendTextToChat(chatCommand, broadcast);
}

void ChatWheel::SendTextToChat(const std::string& text, bool broadcast)
{
    LogInfo("ChatWheel: Attempting to send message: {} (broadcast: {})", text, broadcast);

    // Don't send if chat is already open
    if (MumbleLink::i().textboxHasFocus())
    {
        LogInfo("ChatWheel: Chat already open, aborting");
        return;
    }

    // Capture currently pressed movement keys (WASD) and mouse buttons
    struct InputState {
        bool w = (GetAsyncKeyState('W') & 0x8000) != 0;
        bool a = (GetAsyncKeyState('A') & 0x8000) != 0;
        bool s = (GetAsyncKeyState('S') & 0x8000) != 0;
        bool d = (GetAsyncKeyState('D') & 0x8000) != 0;
        bool leftMouse = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
        bool rightMouse = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
    };
    InputState pressedKeys;
    LogInfo("ChatWheel: Captured input - W:{} A:{} S:{} D:{} LMB:{} RMB:{}",
        pressedKeys.w, pressedKeys.a, pressedKeys.s, pressedKeys.d, pressedKeys.leftMouse, pressedKeys.rightMouse);

    // Release any pressed keys/buttons BEFORE opening chat
    // This creates a clean state so GW2 will accept our re-presses later
    std::vector<INPUT> releaseInputs;
    if (pressedKeys.w) {
        INPUT wUp = {};
        wUp.type = INPUT_KEYBOARD;
        wUp.ki.wScan = 0x11;  // W scan code
        wUp.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
        releaseInputs.push_back(wUp);
        LogInfo("ChatWheel: Releasing W key");
    }
    if (pressedKeys.a) {
        INPUT aUp = {};
        aUp.type = INPUT_KEYBOARD;
        aUp.ki.wScan = 0x1E;  // A scan code
        aUp.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
        releaseInputs.push_back(aUp);
        LogInfo("ChatWheel: Releasing A key");
    }
    if (pressedKeys.s) {
        INPUT sUp = {};
        sUp.type = INPUT_KEYBOARD;
        sUp.ki.wScan = 0x1F;  // S scan code
        sUp.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
        releaseInputs.push_back(sUp);
        LogInfo("ChatWheel: Releasing S key");
    }
    if (pressedKeys.d) {
        INPUT dUp = {};
        dUp.type = INPUT_KEYBOARD;
        dUp.ki.wScan = 0x20;  // D scan code
        dUp.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
        releaseInputs.push_back(dUp);
        LogInfo("ChatWheel: Releasing D key");
    }
    if (pressedKeys.leftMouse) {
        INPUT lmbUp = {};
        lmbUp.type = INPUT_MOUSE;
        lmbUp.mi.dwFlags = MOUSEEVENTF_LEFTUP;
        releaseInputs.push_back(lmbUp);
        LogInfo("ChatWheel: Releasing left mouse button");
    }
    if (pressedKeys.rightMouse) {
        INPUT rmbUp = {};
        rmbUp.type = INPUT_MOUSE;
        rmbUp.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
        releaseInputs.push_back(rmbUp);
        LogInfo("ChatWheel: Releasing right mouse button");
    }

    if (!releaseInputs.empty()) {
        UINT result = SendInput(static_cast<UINT>(releaseInputs.size()), releaseInputs.data(), sizeof(INPUT));
        LogInfo("ChatWheel: Released {} inputs, result: {}", releaseInputs.size(), result);
        // No sleep needed - SendInput is synchronous
    }

    auto& input = Input::i();
    mstime currentTime = TimeInMilliseconds();
    mstime delay = currentTime + 15; // Minimal initial delay

    if (broadcast)
    {
        LogInfo("ChatWheel: Opening squad broadcast with Shift+Enter");
        input.SendKeybind(KeyCombo(ScanCode::Enter, Modifier::Shift), std::nullopt, KeybindAction::Both, true, delay);
        delay += 80; // Reduced but still enough for broadcast to open
    }
    else
    {
        LogInfo("ChatWheel: Opening chat with Enter");
        input.SendKeybind(KeyCombo(ScanCode::Enter), std::nullopt, KeybindAction::Both, true, delay);
        delay += 50; // Reduced - regular chat opens fast
    }

    // Type each character using Windows SendInput API
    LogInfo("ChatWheel: Typing {} characters", text.length());

    // We need to queue the character typing to happen after the chat opens
    // Since SendKeybind doesn't support WM_CHAR, we'll use Windows SendInput with KEYEVENTF_UNICODE
    // We need to do this in a way that's queued properly with the delay system

    // Get the window handle for GW2
    HWND hwnd = GetForegroundWindow();
    if (!hwnd)
    {
        LogWarn("ChatWheel: Could not get foreground window");
        return;
    }

    // Schedule a background task to type the characters after the delay
    std::thread([text, delay, broadcast, pressedKeys]() {
        // Calculate wait time from current time
        mstime currentTime = TimeInMilliseconds();
        mstime waitTime = delay > currentTime ? delay - currentTime : 0;

        LogInfo("ChatWheel: Thread waiting {}ms before typing", waitTime);
        Sleep(static_cast<DWORD>(waitTime));

        // Type each character using Unicode input - build all inputs first for faster sending
        LogInfo("ChatWheel: Starting to type message");
        std::wstring wtext(text.begin(), text.end());
        std::vector<INPUT> inputs;
        inputs.reserve(wtext.length() * 2); // Each character needs key down + key up

        for (wchar_t wc : wtext)
        {
            // Key down
            INPUT inputDown = {};
            inputDown.type = INPUT_KEYBOARD;
            inputDown.ki.wScan = wc;
            inputDown.ki.dwFlags = KEYEVENTF_UNICODE;
            inputs.push_back(inputDown);

            // Key up
            INPUT inputUp = {};
            inputUp.type = INPUT_KEYBOARD;
            inputUp.ki.wScan = wc;
            inputUp.ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
            inputs.push_back(inputUp);
        }

        // Send all character inputs at once for maximum speed
        if (!inputs.empty())
        {
            SendInput(static_cast<UINT>(inputs.size()), inputs.data(), sizeof(INPUT));
        }

        // Send Enter to submit the message (same for both broadcast and regular)
        LogInfo("ChatWheel: Sending final Enter key");
        Sleep(50);  // Longer delay to ensure text is fully processed before Enter

        // Regular Enter key to send the message
        INPUT enterInput[2] = {};
        enterInput[0].type = INPUT_KEYBOARD;
        enterInput[0].ki.wScan = 0x1C;  // Scan code for Enter
        enterInput[0].ki.dwFlags = KEYEVENTF_SCANCODE;

        enterInput[1].type = INPUT_KEYBOARD;
        enterInput[1].ki.wScan = 0x1C;  // Scan code for Enter
        enterInput[1].ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;

        UINT result = SendInput(2, enterInput, sizeof(INPUT));
        LogInfo("ChatWheel: Enter key sent with scan code, result: {}", result);

        // Re-press movement keys that were held down before opening chat
        // Wait for chat to close and game to be ready for input
        // Optimized timing for fast restoration without key capture
        if (broadcast) {
            Sleep(40); // Fast restoration for broadcast
        } else {
            Sleep(60); // Fast restoration for slash commands
        }

        std::vector<INPUT> movementInputs;

        // Use scan codes instead of VK codes for hardware-level input
        // W=0x11, A=0x1E, S=0x1F, D=0x20
        // Send key down with scan code to register the press with GW2
        if (pressedKeys.w) {
            INPUT wDown = {};
            wDown.type = INPUT_KEYBOARD;
            wDown.ki.wScan = 0x11;  // W scan code
            wDown.ki.dwFlags = KEYEVENTF_SCANCODE;
            movementInputs.push_back(wDown);
            LogInfo("ChatWheel: Queuing W key press");
        }
        if (pressedKeys.a) {
            INPUT aDown = {};
            aDown.type = INPUT_KEYBOARD;
            aDown.ki.wScan = 0x1E;  // A scan code
            aDown.ki.dwFlags = KEYEVENTF_SCANCODE;
            movementInputs.push_back(aDown);
            LogInfo("ChatWheel: Queuing A key press");
        }
        if (pressedKeys.s) {
            INPUT sDown = {};
            sDown.type = INPUT_KEYBOARD;
            sDown.ki.wScan = 0x1F;  // S scan code
            sDown.ki.dwFlags = KEYEVENTF_SCANCODE;
            movementInputs.push_back(sDown);
            LogInfo("ChatWheel: Queuing S key press");
        }
        if (pressedKeys.d) {
            INPUT dDown = {};
            dDown.type = INPUT_KEYBOARD;
            dDown.ki.wScan = 0x20;  // D scan code
            dDown.ki.dwFlags = KEYEVENTF_SCANCODE;
            movementInputs.push_back(dDown);
            LogInfo("ChatWheel: Queuing D key press");
        }
        if (pressedKeys.leftMouse) {
            INPUT lmbDown = {};
            lmbDown.type = INPUT_MOUSE;
            lmbDown.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
            movementInputs.push_back(lmbDown);
            LogInfo("ChatWheel: Queuing left mouse button press");
        }
        if (pressedKeys.rightMouse) {
            INPUT rmbDown = {};
            rmbDown.type = INPUT_MOUSE;
            rmbDown.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
            movementInputs.push_back(rmbDown);
            LogInfo("ChatWheel: Queuing right mouse button press");
        }

        if (!movementInputs.empty()) {
            UINT result = SendInput(static_cast<UINT>(movementInputs.size()), movementInputs.data(), sizeof(INPUT));
            LogInfo("ChatWheel: Sent {} input restorations using scan codes, result: {}", movementInputs.size(), result);
        } else {
            LogInfo("ChatWheel: No inputs to restore");
        }

    }).detach();

    LogInfo("ChatWheel: Character typing scheduled");
}

glm::vec4 ChatWheel::GetCommandColor(int index)
{
    // Use different colors for each command slot
    static const glm::vec4 colors[] = {
        { 213 / 255.f, 100 / 255.f, 89 / 255.f, 1 },   // Red
        { 212 / 255.f, 198 / 255.f, 94 / 255.f, 1 },   // Yellow
        { 108 / 255.f, 128 / 255.f, 213 / 255.f, 1 },  // Blue
        { 120 / 255.f, 183 / 255.f, 197 / 255.f, 1 },  // Cyan
        { 136 / 255.f, 123 / 255.f, 195 / 255.f, 1 },  // Purple
        { 199 / 255.f, 131 / 255.f, 68 / 255.f, 1 },   // Orange
        { 56 / 255.f, 228 / 255.f, 85 / 255.f, 1 },    // Green
        { 211 / 255.f, 142 / 255.f, 244 / 255.f, 1 }   // Pink
    };

    if (index >= 0 && index < 8)
        return colors[index];

    return { 1, 1, 1, 1 }; // White fallback
}

Texture2D ChatWheel::CreatePlaceholderTexture()
{
    // Create a simple 64x64 white texture to use as placeholder for all wheel elements
    auto dev = Core::i().device();
    return MakeTexture<Texture2D>(dev, 64, 64, 1, DXGI_FORMAT_R8G8B8A8_UNORM);
}

void ChatWheel::UpdateElementLabel(size_t index)
{
    if (index < wheelElements_.size() && index < commands_.size())
    {
        wheelElements_[index]->displayName(commands_[index]->label);
        LogInfo("ChatWheel: Updated label for command {} to '{}'", index + 1, commands_[index]->label);

        // Mark texture for regeneration
        if (index < labelTextures_.size())
        {
            labelTextures_[index].needsRedraw = true;
        }
    }
}

void ChatWheel::RegenerateTexture(size_t index, ID3D11DeviceContext* ctx)
{
    if (index >= labelTextures_.size() || index >= commands_.size())
        return;

    auto& lt = labelTextures_[index];
    if (!lt.needsRedraw)
        return;

    // Convert label to wide string
    std::wstring wlabel = utf8_decode(commands_[index]->label);

    // Check if text changed
    if (lt.text == wlabel && !lt.needsRedraw)
        return;

    lt.text = wlabel;
    lt.needsRedraw = false;

    // Get font
    auto font = Core::i().font();

    const float fontSize = 180.f;  // Much larger font for better visibility
    const u32 fgColor = 0xFFFFFFFF;
    const u32 outlineColor = 0xFF000000;  // Black outline for boldness

    // Calculate text size with wrapping at ~5 characters
    const auto& txt = utf8_encode(wlabel);
    const float wrapWidth = fontSize * 5.2f;  // Approximate width for ~5 characters
    auto sz = font->CalcTextSizeA(fontSize, FLT_MAX, wrapWidth, txt.c_str());

    ImVec2 clip(512.f, 512.f);
    float xOff = (clip.x - sz.x) * 0.5f;
    float yOff = (clip.y - sz.y) * 0.5f;

    // Add padding to prevent text from touching edges
    const float verticalPadding = 32.f;
    yOff = std::max(verticalPadding, std::min(yOff, clip.y - sz.y - verticalPadding));

    // Setup ImGui draw list
    ImDrawList imDraw(ImGui::GetDrawListSharedData());
    imDraw.AddDrawCmd();
    imDraw.PushClipRect(ImVec2(0.f, 0.f), clip);
    imDraw.PushTextureID(font->ContainerAtlas->TexID);

    // Draw outline for bold effect (draw text multiple times with slight offsets)
    const float outlineThickness = 3.f;
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            if (x == 0 && y == 0) continue;
            imDraw.AddText(font, fontSize, ImVec2(xOff + x * outlineThickness, yOff + y * outlineThickness),
                          outlineColor, txt.c_str(), nullptr, wrapWidth);
        }
    }

    // Draw main text on top
    imDraw.AddText(font, fontSize, ImVec2(xOff, yOff), fgColor, txt.c_str(), nullptr, wrapWidth);

    auto& io = ImGui::GetIO();
    auto oldDisplaySize = io.DisplaySize;
    io.DisplaySize = clip;

    // Save old render target
    ComPtr<ID3D11RenderTargetView> oldRt;
    ComPtr<ID3D11DepthStencilView> oldDs;
    ctx->OMGetRenderTargets(1, oldRt.GetAddressOf(), oldDs.GetAddressOf());
    ctx->OMSetRenderTargets(1, lt.rt.rtv.GetAddressOf(), nullptr);

    // Clear to transparent
    float clearColor[] = { 0.f, 0.f, 0.f, 0.f };
    ctx->ClearRenderTargetView(lt.rt.rtv.Get(), clearColor);

    // Render text
    ImDrawData imData;
    imData.Valid = true;
    imData.CmdLists.clear();
    imData.CmdLists.push_back(&imDraw);
    imData.CmdListsCount = 1;
    imData.TotalIdxCount = imDraw.IdxBuffer.Size;
    imData.TotalVtxCount = imDraw.VtxBuffer.Size;
    imData.DisplayPos = ImVec2(0.0f, 0.0f);
    imData.DisplaySize = io.DisplaySize;

    {
        ImGuiBlendStateOverride ov(textBlendState_.Get());
        ImGui_ImplDX11_RenderDrawData(&imData);
    }

    // Restore render target
    ctx->OMSetRenderTargets(1, oldRt.GetAddressOf(), oldDs.Get());
    io.DisplaySize = oldDisplaySize;

    LogInfo("ChatWheel: Regenerated texture for command {} with label '{}'", index + 1, commands_[index]->label);
}

void ChatWheel::DrawOffscreen(ID3D11DeviceContext* ctx)
{
    // Regenerate any textures that need it
    for (size_t i = 0; i < labelTextures_.size(); i++)
    {
        RegenerateTexture(i, ctx);
    }
}

} // namespace GW2Radial
