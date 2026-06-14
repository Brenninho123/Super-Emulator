#include "MainMenu.hpp"
#include "../core/Terminal.hpp"

#include <algorithm>
#include <filesystem>
#include <format>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

// ─── column cell widths (content + 1-space padding each side) ────────────────
static constexpr size_t W_IDX    =  4;  //  2 content
static constexpr size_t W_NAME   = 34;  // 32 content
static constexpr size_t W_MAPPER =  6;  //  4 content
static constexpr size_t W_PRG    =  4;  //  2 content
static constexpr size_t W_CHR    =  4;  //  2 content
static constexpr size_t W_TV     =  4;  //  2 content
// 5 inner ║ separators
static constexpr size_t BOX_INNER =
    W_IDX + 1 + W_NAME + 1 + W_MAPPER + 1 + W_PRG + 1 + W_CHR + 1 + W_TV;
// BOX_INNER = 4+1+34+1+6+1+4+1+4+1+4 = 61
// Full display width = ║(1) + 61 + ║(1) = 63 chars

// ─── separator row builders ──────────────────────────────────────────────────
static std::string makeSep(std::string_view l,
                           std::string_view mid,
                           std::string_view r)
{
    using namespace Terminal;
    return col(Color::PURPLE,
        std::string(l) +
        repeat("=", W_IDX)    + std::string(mid) +
        repeat("=", W_NAME)   + std::string(mid) +
        repeat("=", W_MAPPER) + std::string(mid) +
        repeat("=", W_PRG)    + std::string(mid) +
        repeat("=", W_CHR)    + std::string(mid) +
        repeat("=", W_TV)     + std::string(r)
    ) + "\n";
}

static std::string makeFullSep(std::string_view l, std::string_view r)
{
    using namespace Terminal;
    return col(Color::PURPLE,
        std::string(l) + repeat("=", BOX_INNER) + std::string(r)
    ) + "\n";
}

// ─── full-width text row ─────────────────────────────────────────────────────
static void printWide(std::string_view text,
                      std::string_view textColor = Terminal::Color::WHITE)
{
    using namespace Terminal;
    std::cout << col(Color::PURPLE, "||")
              << col(textColor, center(text, BOX_INNER))
              << col(Color::PURPLE, "||\n");
}

// ─── data row ────────────────────────────────────────────────────────────────
void MainMenu::printRow(size_t idx, const RomInfo& info) const
{
    using namespace Terminal::Color;
    using namespace Terminal;

    const std::string name = info.name.size() > 32
        ? info.name.substr(0, 29) + "..."
        : info.name;

    const std::string_view nameColor = info.valid ? PURPLE_L : GRAY;
    const std::string      mpr  = info.valid ? std::format("{:>4}", info.mapper)   : "----";
    const std::string      prg  = info.valid ? std::format("{:>2}", info.prgBanks) : "--";
    const std::string      chr_ = info.valid ? std::format("{:>2}", info.chrBanks) : "--";
    const std::string      tv   = info.valid ? (info.pal ? "PA" : "NT")            : "--";

    std::cout
        << col(PURPLE, "|| ") << col(WHITE,  std::format("{:>2}", idx))
        << col(PURPLE, " || ") << col(nameColor, std::format("{:<32}", name))
        << col(PURPLE, " || ") << col(GRAY,  mpr)
        << col(PURPLE, " || ") << col(GRAY,  prg)
        << col(PURPLE, " || ") << col(GRAY,  chr_)
        << col(PURPLE, " || ") << col(GRAY,  tv)
        << col(PURPLE, " ||\n");
}

// ─── scan ────────────────────────────────────────────────────────────────────
void MainMenu::scanRoms(const std::string& romFolder)
{
    roms_.clear();

    if (!fs::exists(romFolder))
    {
        fs::create_directories(romFolder);
        return;
    }

    for (const auto& entry : fs::directory_iterator(romFolder))
    {
        const auto& ext = entry.path().extension();
        if (ext == ".nes" || ext == ".NES")
            roms_.push_back(Rom::parseHeader(entry.path().string()));
    }

    std::sort(roms_.begin(), roms_.end(), [](const RomInfo& a, const RomInfo& b) {
        return a.name < b.name;
    });
}

// ─── show ────────────────────────────────────────────────────────────────────
void MainMenu::show()
{
    using namespace Terminal;
    using namespace Terminal::Color;

    Terminal::clear();

    const size_t total  = roms_.size();
    const size_t valid  = static_cast<size_t>(
        std::count_if(roms_.begin(), roms_.end(), [](const RomInfo& r){ return r.valid; })
    );

    // Top border
    std::cout << makeFullSep("+", "+");

    // Title
    printWide("SUPER EMULATOR  -  ROM LIBRARY", PURPLE_L);
    printWide(std::format("{} ROM(s) found  ({} valid)", total, valid), GRAY);

    // Column header top separator
    std::cout << makeSep("+", "+", "+");

    // Column header row
    std::cout
        << col(PURPLE, "|| ") << col(WHITE, " #")
        << col(PURPLE, " || ") << col(WHITE, padRight("Name", 32))
        << col(PURPLE, " || ") << col(WHITE, " Mpr")
        << col(PURPLE, " || ") << col(WHITE, "Pr")
        << col(PURPLE, " || ") << col(WHITE, "Ch")
        << col(PURPLE, " || ") << col(WHITE, "TV")
        << col(PURPLE, " ||\n");

    // Column header bottom separator
    std::cout << makeSep("+", "+", "+");

    // ROM rows
    if (roms_.empty())
    {
        printWide("  No .nes files found in /roms/", GRAY);
        printWide("  Place NES ROM files there and restart.", GRAY);
    }
    else
    {
        for (size_t i = 0; i < roms_.size(); ++i)
            printRow(i, roms_[i]);
    }

    // Footer separator
    std::cout << makeSep("+", "+", "+");

    // Controls hint
    {
        std::string hint = "[0-N] Select";
        if (!roms_.empty())
        {
            const size_t bat = static_cast<size_t>(
                std::count_if(roms_.begin(), roms_.end(),
                    [](const RomInfo& r){ return r.valid && r.battery; })
            );
            if (bat > 0)
                hint += std::format("  [*={} battery-backed]", bat);
        }
        hint += "  [Q] Quit";
        printWide(hint, GRAY);
    }

    // Bottom border
    std::cout << makeFullSep("+", "+");

    // Prompt
    std::cout << "\n" << col(PURPLE_L, " > ") << col(Color::RESET, "");

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
    return roms_[selectedIndex_].path;
}

bool MainMenu::wantsQuit() const noexcept { return quit_; }
