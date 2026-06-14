#include "Controls.hpp"

#ifdef _WIN32
#include <windows.h>
#include <Xinput.h>
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Xinput.lib")
#endif

#include <cstring>

void Controls::poll()
{
    std::memset(state_.buttons, 0, sizeof(state_.buttons));
    quit_ = false;
    pollKeyboard();
    pollGamepad();
}

const InputState& Controls::state()        const noexcept { return state_; }
bool              Controls::quitRequested() const noexcept { return quit_; }

void Controls::pollKeyboard()
{
#ifdef _WIN32
    auto key = [](int vk) -> bool
    {
        return (GetAsyncKeyState(vk) & 0x8000) != 0;
    };

    state_.buttons[int(Button::Up)]     = key(VK_UP)    || key('W');
    state_.buttons[int(Button::Down)]   = key(VK_DOWN)  || key('S');
    state_.buttons[int(Button::Left)]   = key(VK_LEFT)  || key('A');
    state_.buttons[int(Button::Right)]  = key(VK_RIGHT) || key('D');
    state_.buttons[int(Button::A)]      = key('Z')      || key(VK_NUMPAD1);
    state_.buttons[int(Button::B)]      = key('X')      || key(VK_NUMPAD2);
    state_.buttons[int(Button::Start)]  = key(VK_RETURN);
    state_.buttons[int(Button::Select)] = key(VK_LSHIFT) || key(VK_RSHIFT);

    quit_ = key(VK_ESCAPE);
#endif
}

void Controls::pollGamepad()
{
#ifdef _WIN32
    XINPUT_STATE xstate{};
    if (XInputGetState(0, &xstate) != ERROR_SUCCESS)
        return;

    const WORD  btns = xstate.Gamepad.wButtons;
    const SHORT lx   = xstate.Gamepad.sThumbLX;
    const SHORT ly   = xstate.Gamepad.sThumbLY;

    constexpr SHORT DEAD = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;

    state_.buttons[int(Button::Up)]    = state_.buttons[int(Button::Up)]    || (btns & XINPUT_GAMEPAD_DPAD_UP)    != 0 || ly >  DEAD;
    state_.buttons[int(Button::Down)]  = state_.buttons[int(Button::Down)]  || (btns & XINPUT_GAMEPAD_DPAD_DOWN)  != 0 || ly < -DEAD;
    state_.buttons[int(Button::Left)]  = state_.buttons[int(Button::Left)]  || (btns & XINPUT_GAMEPAD_DPAD_LEFT)  != 0 || lx < -DEAD;
    state_.buttons[int(Button::Right)] = state_.buttons[int(Button::Right)] || (btns & XINPUT_GAMEPAD_DPAD_RIGHT) != 0 || lx >  DEAD;
    state_.buttons[int(Button::A)]     = state_.buttons[int(Button::A)]     || (btns & XINPUT_GAMEPAD_A)          != 0;
    state_.buttons[int(Button::B)]     = state_.buttons[int(Button::B)]     || (btns & XINPUT_GAMEPAD_B)          != 0;
    state_.buttons[int(Button::Start)] = state_.buttons[int(Button::Start)] || (btns & XINPUT_GAMEPAD_START)      != 0;
    state_.buttons[int(Button::Select)]= state_.buttons[int(Button::Select)]|| (btns & XINPUT_GAMEPAD_BACK)       != 0;
#endif
}
