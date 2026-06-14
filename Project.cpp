#pragma once

#include <filesystem>
#include <vector>
#include <string>

namespace Project
{
    constexpr const char* SOURCE_PATH = "source";
    constexpr const char* MAIN_CLASS = "source/Main.cpp";

    inline std::vector<std::string> getSourceFiles()
    {
        std::vector<std::string> files;

        for (const auto& entry :
             std::filesystem::recursive_directory_iterator(SOURCE_PATH))
        {
            if (entry.path().extension() == ".cpp")
            {
                files.push_back(entry.path().string());
            }
        }

        return files;
    }
}
