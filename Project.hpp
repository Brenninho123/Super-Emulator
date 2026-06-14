#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>

namespace Project
{
    inline constexpr const char* NAME = "Super Emulator";
    inline constexpr const char* VERSION = "1.0.0";
    inline constexpr const char* COMPANY = "Brenninho123";

    inline constexpr const char* MAIN_CLASS = "Main";
    inline constexpr const char* SOURCE_PATH = "source";

    inline constexpr int WINDOW_WIDTH = 1280;
    inline constexpr int WINDOW_HEIGHT = 720;
    inline constexpr int TARGET_FPS = 60;

    inline constexpr bool ENABLE_AUDIO = true;
    inline constexpr bool ENABLE_VIDEO = true;
    inline constexpr bool ENABLE_DEBUG = false;
    inline constexpr bool ENABLE_VSYNC = true;

    enum class BuildTarget
    {
        Native,
        HTML5,
        Android,
        Linux,
        Windows,
        MacOS
    };

    inline BuildTarget CURRENT_TARGET =
    #ifdef __EMSCRIPTEN__
        BuildTarget::HTML5;
    #elif defined(_WIN32)
        BuildTarget::Windows;
    #elif defined(__ANDROID__)
        BuildTarget::Android;
    #elif defined(__APPLE__)
        BuildTarget::MacOS;
    #elif defined(__linux__)
        BuildTarget::Linux;
    #else
        BuildTarget::Native;
    #endif

    inline std::vector<std::string> getSourceFiles()
    {
        std::vector<std::string> files;

        if (!std::filesystem::exists(SOURCE_PATH))
            return files;

        for (const auto& entry :
             std::filesystem::recursive_directory_iterator(SOURCE_PATH))
        {
            if (entry.path().extension() == ".cpp" ||
                entry.path().extension() == ".hpp" ||
                entry.path().extension() == ".h")
            {
                files.push_back(entry.path().string());
            }
        }

        return files;
    }

    inline std::string getTargetName()
    {
        switch (CURRENT_TARGET)
        {
            case BuildTarget::HTML5:   return "HTML5";
            case BuildTarget::Windows: return "Windows";
            case BuildTarget::Linux:   return "Linux";
            case BuildTarget::Android: return "Android";
            case BuildTarget::MacOS:   return "MacOS";
            default:                   return "Native";
        }
    }

    inline std::unordered_map<std::string, std::string> Properties =
    {
        {"Renderer", "OpenGL"},
        {"Language", "C++20"},
        {"Engine", "Super Emulator"},
        {"Build", "Release"}
    };

    inline std::string getProperty(const std::string& key)
    {
        auto it = Properties.find(key);
        return it != Properties.end() ? it->second : "";
    }

    inline void setProperty(const std::string& key, const std::string& value)
    {
        Properties[key] = value;
    }
}
