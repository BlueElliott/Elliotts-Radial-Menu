#pragma once
#include <Main.h>
#include <Wheel.h>

namespace GW2Radial
{

class ChatWheel : public Wheel
{
public:
    ChatWheel(std::shared_ptr<Texture2D> bgTexture);

protected:
    void DrawMenu(Keybind** currentHover) override;
};

} // namespace GW2Radial
