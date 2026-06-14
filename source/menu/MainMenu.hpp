#pragma once

#include <string>
#include <vector>

class MainMenu
{
public:
    void scanRoms(const std::string& romFolder);
    void show();

    std::string getSelectedRom() const;
    bool        wantsQuit()      const;

private:
    std::vector<std::string> roms_;
    int  selectedIndex_ = -1;
    bool quit_          = false;
};
