#pragma once

#include <string>
#include <vector>

#include "../roms/Rom.hpp"

class MainMenu
{
public:
    void scanRoms(const std::string& romFolder);
    void show();

    std::string getSelectedRom() const;
    bool        wantsQuit()      const noexcept;

private:
    std::vector<RomInfo> roms_;
    int  selectedIndex_ = -1;
    bool quit_          = false;

    void printRow(size_t idx, const RomInfo& info) const;
};
