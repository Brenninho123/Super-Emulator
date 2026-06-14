#pragma once

#include <cstdint>

enum class Button : uint8_t
{
    Up     = 0,
    Down   = 1,
    Left   = 2,
    Right  = 3,
    A      = 4,
    B      = 5,
    Select = 6,
    Start  = 7,
    COUNT  = 8
};

struct InputState
{
    bool buttons[static_cast<int>(Button::COUNT)] = {};

    [[nodiscard]] bool pressed(Button b) const noexcept
    {
        return buttons[static_cast<int>(b)];
    }

    [[nodiscard]] bool any() const noexcept
    {
        for (bool b : buttons) if (b) return true;
        return false;
    }
};

class Controls
{
public:
    void poll();

    [[nodiscard]] const InputState& state()        const noexcept;
    [[nodiscard]] bool              quitRequested() const noexcept;

private:
    InputState state_;
    bool       quit_ = false;

    void pollKeyboard();
    void pollGamepad();
};
