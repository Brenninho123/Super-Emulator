#pragma once

#include <string>
#include <vector>

class Project
{
public:
    static inline const std::string NAME = "Super Emulator";
    static inline const std::string VERSION = "1.0.0";
    static inline const std::string COMPANY = "Brenninho123";
    static inline const std::string WINDOW_TITLE = "Super Emulator";
    static inline const int WINDOW_WIDTH = 1280;
    static inline const int WINDOW_HEIGHT = 720;
    static inline const int FPS = 60;

    static inline const bool ENABLE_AUDIO = true;
    static inline const bool ENABLE_VSYNC = true;
    static inline const bool ENABLE_DEBUGGER = false;

    static inline const std::vector<std::string> SUPPORTED_FORMATS =
    {
        ".nes",
        ".gb",
        ".gbc",
        ".sms"
    };

    static std::string getInfo()
    {
        return NAME + " v" + VERSION;
    }
};
