#include "MainMenu.hpp"
#include "../core/Terminal.hpp"

#include <algorithm>
#include <filesystem>
#include <format>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

static constexpr size_t BOX_W = 46;

static void borderRow(std::string_view content)
{
    using namespace Terminal::Color;
    std::cout << PURPLE << BOLD << "||" << RESET
              << content
              << PURPLE << BOLD << "||\n" << RESET;
}

void MainMenu::scanRoms(const std::string& romFolder)
{
    roms_.clear();

    if (!fs::exists(romFolder))
    {
        fs::create_directories(romFolder);
        return;
    }

    for (const auto& entry : fs::directory_iterator(romFolder))
        if (entry.path().extension() == ".nes")
            roms_.push_back(entry.path().string());

    std::sort(roms_.begin(), roms_.end(), [](const std::string& a, const std::string& b) {
        return fs::path(a).filename() < fs::path(b).filename();
    });
}

void MainMenu::show()
{
    using namespace Terminal::Color;

    Terminal::clear();

    const std::string hline = Terminal::repeat("=", BOX_W);
    const std::string blank(BOX_W, ' ');

    // Top border
    std::cout << PURPLE << BOLD << "+" << hline << "+\n" << RESET;

    // Title
    {
        std::string title = Terminal::center("SUPER EMULATOR - ROM LIBRARY", BOX_W);
        std::cout << PURPLE << BOLD << "||" << RESET
                  << PURPLE_L << BOLD << title << RESET
                  << PURPLE << BOLD << "||\n" << RESET;
    }

    // Divider
    std::cout << PURPLE << BOLD << "+" << hline << "+\n" << RESET;

    // Empty row
    borderRow(blank);

    if (roms_.empty())
    {
        std::string msg = Terminal::padRight("  No NES ROMs found in /roms/", BOX_W);
        borderRow(std::string(GRAY) + msg + std::string(RESET));
    }
    else
    {
        for (size_t i = 0; i < roms_.size(); ++i)
        {
            std::string name  = fs::path(roms_[i]).filename().string();
            std::string entry = Terminal::padRight(std::format("  [{:2}]  {}", i, name), BOX_W);
            borderRow(std::string(PURPLE_L) + entry + std::string(RESET));
        }
    }

    // Empty row
    borderRow(blank);

    // Controls row
    std::cout << PURPLE << BOLD << "+" << hline << "+\n" << RESET;
    {
        std::string hint = Terminal::padRight("  [0-N] Select ROM    [Q] Quit", BOX_W);
        borderRow(std::string(GRAY) + hint + std::string(RESET));
    }

    // Bottom border
    std::cout << PURPLE << BOLD << "+" << hline << "+\n" << RESET;

    // Prompt
    std::cout << "\n" << PURPLE_L << BOLD << " > " << RESET;

    std::string input;
    std::cin >> input;

    quit_          = false;
    selectedIndex_ = -1;

    if (input == "q" || input == "Q")
    {
        quit_ = true;
        return;
    }

    try   { selectedIndex_ = std::stoi(input); }
    catch (...) {}
}

std::string MainMenu::getSelectedRom() const
{
    if (selectedIndex_ < 0 || selectedIndex_ >= static_cast<int>(roms_.size()))
        return "";
    return roms_[selectedIndex_];
}

bool MainMenu::wantsQuit() const { return quit_; }
