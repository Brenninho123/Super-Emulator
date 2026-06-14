#pragma once

#include <iostream>
#include <string>
#include <string_view>

#ifdef _WIN32
#include <windows.h>
#endif

namespace Terminal
{
    namespace Color
    {
        constexpr std::string_view RESET    = "\033[0m";
        constexpr std::string_view BOLD     = "\033[1m";
        constexpr std::string_view DIM      = "\033[2m";
        constexpr std::string_view PURPLE   = "\033[38;2;148;0;211m";
        constexpr std::string_view PURPLE_L = "\033[38;2;210;140;255m";
        constexpr std::string_view GRAY     = "\033[90m";
        constexpr std::string_view WHITE    = "\033[97m";
    }

    inline void enableAnsi()
    {
#ifdef _WIN32
        HANDLE out  = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD  mode = 0;
        if (GetConsoleMode(out, &mode))
            SetConsoleMode(out, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING
                                     | ENABLE_PROCESSED_OUTPUT);
#endif
    }

    inline void clear()
    {
        std::cout << "\033[2J\033[H" << std::flush;
    }

    inline std::string repeat(std::string_view ch, size_t n)
    {
        std::string s;
        s.reserve(ch.size() * n);
        for (size_t i = 0; i < n; ++i) s += ch;
        return s;
    }

    inline std::string center(std::string_view text, size_t width)
    {
        if (text.size() >= width)
            return std::string(text.substr(0, width));
        size_t lpad = (width - text.size()) / 2;
        size_t rpad = width - text.size() - lpad;
        return std::string(lpad, ' ') + std::string(text) + std::string(rpad, ' ');
    }

    inline std::string padRight(std::string s, size_t width)
    {
        if (s.size() < width)
            s.append(width - s.size(), ' ');
        else if (s.size() > width)
            s = s.substr(0, width - 3) + "...";
        return s;
    }
}
