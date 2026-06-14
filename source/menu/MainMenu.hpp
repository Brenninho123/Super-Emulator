#pragma once

#include <string>
#include <vector>

class MainMenu
{
public:
    MainMenu();

    void scanRoms(const std::string& romFolder);
    void show();
    std::string getSelectedRom() const;

private:
    std::vector<std::string> roms;
    int selectedIndex = -1;
};
