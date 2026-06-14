#include "MainMenu.hpp"

#include <iostream>
#include <filesystem>

MainMenu::MainMenu()
{
}

void MainMenu::scanRoms(const std::string& romFolder)
{
    roms.clear();

    if (!std::filesystem::exists(romFolder))
    {
        std::filesystem::create_directories(romFolder);
        return;
    }

    for (const auto& entry :
         std::filesystem::directory_iterator(romFolder))
    {
        if (entry.path().extension() == ".nes")
        {
            roms.push_back(entry.path().string());
        }
    }
}

void MainMenu::show()
{
    std::cout << "\n=== SUPER EMULATOR ===\n\n";

    if (roms.empty())
    {
        std::cout << "No NES ROMs found.\n";
        return;
    }

    for (size_t i = 0; i < roms.size(); i++)
    {
        std::cout << "[" << i << "] "
                  << std::filesystem::path(roms[i]).filename().string()
                  << '\n';
    }

    std::cout << "\nSelect a ROM: ";
    std::cin >> selectedIndex;
}

std::string MainMenu::getSelectedRom() const
{
    if (selectedIndex < 0 ||
        selectedIndex >= static_cast<int>(roms.size()))
    {
        return "";
    }

    return roms[selectedIndex];
}
