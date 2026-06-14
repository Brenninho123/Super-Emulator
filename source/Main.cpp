#include <algorithm>
#include <array>
#include <chrono>
#include <exception>
#include <filesystem>
#include <format>
#include <iostream>
#include <numeric>
#include <string>
#include <string_view>
#include <thread>

#include "controls/Controls.hpp"
#include "core/Terminal.hpp"
#include "menu/MainMenu.hpp"
#include "roms/Rom.hpp"

static constexpr std::string_view VERSION    = "1.0.0";
static constexpr std::string_view SYSTEM     = "NES";
static constexpr int              TARGET_FPS = 60;
static constexpr double           FRAME_TIME = 1.0 / TARGET_FPS;

// ─── Rolling frame timing window ─────────────────────────────────────────────
class FrameTimer
{
    static constexpr size_t WINDOW = 60;

    std::array<double, WINDOW> times_ = {};
    size_t idx_   = 0;
    size_t count_ = 0;

public:
    void record(double dt) noexcept
    {
        times_[idx_] = dt;
        idx_         = (idx_ + 1) % WINDOW;
        if (count_ < WINDOW) ++count_;
    }

    [[nodiscard]] double fps()   const noexcept { return avgDt() > 0.0 ? 1.0 / avgDt() : 0.0; }
    [[nodiscard]] double avgDt() const noexcept { return count_ > 0 ? sum() / count_ : 0.0; }
    [[nodiscard]] double minDt() const noexcept
    {
        if (count_ == 0) return 0.0;
        return *std::min_element(times_.begin(), times_.begin() + count_);
    }
    [[nodiscard]] double maxDt() const noexcept
    {
        if (count_ == 0) return 0.0;
        return *std::max_element(times_.begin(), times_.begin() + count_);
    }

private:
    [[nodiscard]] double sum() const noexcept
    {
        return std::accumulate(times_.begin(), times_.begin() + count_, 0.0);
    }
};

// ─── Emulator core ───────────────────────────────────────────────────────────
class Emulator
{
public:
    [[nodiscard]] bool initialize()
    {
        using namespace Terminal;
        std::cout << col(Color::PURPLE, "[Core]") << " Initializing emulator...\n";
        running_       = true;
        romLoaded_     = false;
        frameCount_    = 0;
        framesThisSec_ = 0;
        fpsTimer_      = std::chrono::steady_clock::now();
        return true;
    }

    [[nodiscard]] bool loadRom(const std::filesystem::path& path)
    {
        if (!rom_.load(path.string()))
            return false;

        romLoaded_ = true;

        using namespace Terminal;
        const RomInfo  info  = Rom::parseHeader(path.string());
        const std::string h  = repeat("=", 40);

        std::cout
            << col(Color::PURPLE, "+" + h + "+\n")
            << col(Color::PURPLE, "||")
            << col(Color::PURPLE_L, center(info.name, 40))
            << col(Color::PURPLE, "||\n")
            << col(Color::PURPLE, "+" + h + "+\n")
            << std::format("  Mapper    : {}\n",   info.mapper)
            << std::format("  PRG ROM   : {} banks  ({} KB)\n", info.prgBanks, info.prgSizeKB)
            << std::format("  CHR ROM   : {} banks  ({} KB)\n", info.chrBanks, info.chrSizeKB)
            << std::format("  Battery   : {}\n",   info.battery    ? "Yes" : "No")
            << std::format("  Trainer   : {}\n",   info.trainer    ? "Yes" : "No")
            << std::format("  4-Screen  : {}\n",   info.fourScreen ? "Yes" : "No")
            << std::format("  TV System : {}\n",   info.pal        ? "PAL" : "NTSC")
            << std::format("  iNES ver  : {}\n\n", info.nesVersion);

        return true;
    }

    void update(const InputState& input)
    {
        ++frameCount_;
        (void)input;
    }

    void render(const InputState& input, const FrameTimer& timer)
    {
        ++framesThisSec_;

        const auto   now   = std::chrono::steady_clock::now();
        const double delta = std::chrono::duration<double>(now - fpsTimer_).count();

        if (delta < 1.0) return;

        using namespace Terminal;
        using namespace Terminal::Color;

        auto btn = [&](Button b, std::string_view label) -> std::string
        {
            return input.pressed(b)
                ? col(PURPLE_L, std::string(BOLD) + std::string(label) + std::string(RESET))
                : col(GRAY, label);
        };

        std::cout
            << col(PURPLE, "[Frame]") << " "
            << col(WHITE,  std::format("{:>7}", frameCount_)) << "  "
            << col(PURPLE, "[FPS]")   << " "
            << col(PURPLE_L, std::format("{:>5.1f}", framesThisSec_ / delta)) << "  "
            << col(GRAY, std::format("avg {:.1f}ms  min {:.1f}ms  max {:.1f}ms",
                timer.avgDt() * 1000.0,
                timer.minDt() * 1000.0,
                timer.maxDt() * 1000.0))
            << "  " << col(GRAY, "| ")
            << btn(Button::Up,    "U") << btn(Button::Down,  "D")
            << btn(Button::Left,  "L") << btn(Button::Right, "R")
            << col(GRAY, " | ")
            << btn(Button::B, "B") << " " << btn(Button::A, "A")
            << col(GRAY, " | ")
            << btn(Button::Select, "Se") << " " << btn(Button::Start, "St")
            << '\n';

        framesThisSec_ = 0;
        fpsTimer_      = now;
    }

    [[nodiscard]] bool running() const noexcept { return running_; }
    void               stop()         noexcept  { running_ = false; }

private:
    Rom  rom_;
    bool running_   = false;
    bool romLoaded_ = false;

    unsigned long long frameCount_    = 0;
    unsigned int       framesThisSec_ = 0;

    std::chrono::steady_clock::time_point fpsTimer_ =
        std::chrono::steady_clock::now();
};

// ─── Startup panels ──────────────────────────────────────────────────────────
static void printHeader()
{
    using namespace Terminal;
    using namespace Terminal::Color;

    Terminal::clear();

    const std::string h = repeat("=", 50);
    std::cout
        << col(PURPLE, "+" + h + "+\n")
        << col(PURPLE, "||") << col(PURPLE_L, bold(center("SUPER EMULATOR", 50))) << col(PURPLE, "||\n")
        << col(PURPLE, "||") << col(GRAY,     center("v" + std::string(VERSION) +
                                                      "   System: " + std::string(SYSTEM) +
                                                      "   Target: " + std::to_string(TARGET_FPS) + " FPS", 50))
                             << col(PURPLE, "||\n")
        << col(PURPLE, "||") << col(GRAY,     center("iNES 1.0 / 2.0  +  Keyboard  +  XInput Gamepad", 50)) << col(PURPLE, "||\n")
        << col(PURPLE, "+" + h + "+\n\n");
}

static void printControls()
{
    using namespace Terminal;
    using namespace Terminal::Color;

    const std::string h = repeat("=", 42);

    auto row = [&](std::string_view input, std::string_view action)
    {
        std::cout
            << col(PURPLE, "|| ")
            << col(PURPLE_L, padRight(std::string(input), 18))
            << col(GRAY,   " -> ")
            << padRight(std::string(action), 17)
            << col(PURPLE, " ||\n");
    };

    auto sect = [&](std::string_view title)
    {
        std::cout << col(PURPLE, "||") << col(GRAY, center(title, 42)) << col(PURPLE, "||\n");
    };

    std::cout << col(PURPLE, "+" + h + "+\n");
    std::cout << col(PURPLE, "||") << col(WHITE, center("CONTROLS", 42)) << col(PURPLE, "||\n");
    std::cout << col(PURPLE, "+" + h + "+\n");

    sect("-- Keyboard --");
    row("Arrows / WASD",   "D-Pad");
    row("Z  / Numpad 1",   "A Button");
    row("X  / Numpad 2",   "B Button");
    row("Enter",           "Start");
    row("Shift",           "Select");
    row("Escape",          "Quit emulation");

    sect("-- Gamepad (XInput) --");
    row("D-Pad",           "D-Pad");
    row("Left Stick",      "D-Pad (analog)");
    row("A",               "A Button");
    row("B",               "B Button");
    row("Start",           "Start");
    row("Back",            "Select");

    std::cout << col(PURPLE, "+" + h + "+\n\n");
}

// ─── Entry point ─────────────────────────────────────────────────────────────
int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    Terminal::enableAnsi();

    try
    {
        printHeader();
        printControls();

        Emulator   emulator;
        Controls   controls;
        FrameTimer timer;

        if (!emulator.initialize())
            throw std::runtime_error("Failed to initialize emulator.");

        std::cout << '\n';

        MainMenu menu;
        menu.scanRoms("roms");

        while (true)
        {
            menu.show();

            if (menu.wantsQuit())
            {
                std::cout << '\n' << Terminal::col(Terminal::Color::PURPLE_L, "Goodbye.") << '\n';
                return 0;
            }

            const std::string selected = menu.getSelectedRom();
            if (selected.empty())
                continue;

            if (!emulator.loadRom(selected))
            {
                std::cerr << Terminal::col(Terminal::Color::PURPLE, "[ROM]")
                          << " Failed to load ROM. Press ENTER to retry...";
                std::cin.ignore();
                std::cin.get();
                continue;
            }

            break;
        }

        while (emulator.running())
        {
            const auto frameStart = std::chrono::steady_clock::now();

            controls.poll();

            if (controls.quitRequested())
                emulator.stop();

            emulator.update(controls.state());
            emulator.render(controls.state(), timer);

            const auto elapsed  = std::chrono::duration_cast<std::chrono::duration<double>>(
                std::chrono::steady_clock::now() - frameStart
            );
            const auto sleepFor = std::chrono::duration<double>(FRAME_TIME) - elapsed;

            if (sleepFor > std::chrono::duration<double>::zero())
                std::this_thread::sleep_for(sleepFor);

            timer.record(
                std::chrono::duration<double>(
                    std::chrono::steady_clock::now() - frameStart
                ).count()
            );
        }

        std::cout << '\n'
                  << Terminal::col(Terminal::Color::PURPLE_L, "Emulation finished.")
                  << "\nPress ENTER to exit...";
        std::cin.ignore();
        std::cin.get();
        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << Terminal::col(Terminal::Color::PURPLE, "\n[FATAL] ") << e.what() << '\n';
    }
    catch (...)
    {
        std::cerr << Terminal::col(Terminal::Color::PURPLE, "\n[FATAL] ") << "Unknown exception.\n";
    }

    std::cout << "\nPress ENTER to exit...";
    std::cin.get();
    return 1;
}
