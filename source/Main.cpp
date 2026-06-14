#include <chrono>
#include <exception>
#include <filesystem>
#include <format>
#include <iostream>
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

class Emulator
{
public:
    [[nodiscard]] bool initialize()
    {
        using namespace Terminal::Color;
        std::cout << PURPLE << "[Core]" << RESET << " Initializing emulator...\n";
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

        using namespace Terminal::Color;
        std::cout << '\n'
                  << PURPLE << "[ROM] " << RESET
                  << PURPLE_L << path.filename().string() << RESET << '\n'
                  << "      PRG Banks : " << static_cast<int>(rom_.getPRGBanks()) << '\n'
                  << "      CHR Banks : " << static_cast<int>(rom_.getCHRBanks()) << '\n'
                  << "      Mapper    : " << static_cast<int>(rom_.getMapper())   << "\n\n";
        return true;
    }

    void update(const InputState& input)
    {
        ++frameCount_;
        (void)input;
    }

    void render(const InputState& input)
    {
        ++framesThisSec_;

        auto   now   = std::chrono::steady_clock::now();
        double delta = std::chrono::duration<double>(now - fpsTimer_).count();

        if (delta < 1.0)
            return;

        using namespace Terminal::Color;

        auto btn = [&](Button b, std::string_view label) -> std::string
        {
            return input.pressed(b)
                ? std::string(PURPLE_L) + std::string(BOLD) + std::string(label) + std::string(RESET)
                : std::string(GRAY)     + std::string(label) + std::string(RESET);
        };

        std::cout
            << PURPLE << "[FPS]" << RESET
            << std::format(" {:5.1f}  ", framesThisSec_ / delta)
            << GRAY << "| " << RESET
            << btn(Button::Up,    "U") << btn(Button::Down, "D")
            << btn(Button::Left,  "L") << btn(Button::Right,"R")
            << GRAY << " | " << RESET
            << btn(Button::B, "B") << " " << btn(Button::A, "A")
            << GRAY << " | " << RESET
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

static void printHeader()
{
    using namespace Terminal::Color;

    Terminal::clear();

    const std::string hline = Terminal::repeat("=", 46);

    std::cout << PURPLE << BOLD
              << "+" << hline << "+\n"
              << "||" << Terminal::center("SUPER EMULATOR  v" + std::string(VERSION), 46) << "||\n"
              << "||" << Terminal::center(std::format("System: {}  |  {} FPS", SYSTEM, TARGET_FPS), 46) << "||\n"
              << "+" << hline << "+\n"
              << RESET << '\n';
}

static void printControls()
{
    using namespace Terminal::Color;

    const std::string hline = Terminal::repeat("-", 40);

    std::cout << PURPLE << BOLD << "+" << hline << "+\n" << RESET
              << PURPLE << BOLD << "||" << RESET
              << Terminal::center("CONTROLS", 40)
              << PURPLE << BOLD << "||\n" << RESET
              << PURPLE << BOLD << "+" << hline << "+\n" << RESET;

    auto row = [&](std::string_view input, std::string_view action)
    {
        std::cout << PURPLE << BOLD << "|| " << RESET
                  << PURPLE_L << Terminal::padRight(std::string(input), 18) << RESET
                  << GRAY << "->  " << RESET
                  << Terminal::padRight(std::string(action), 16)
                  << PURPLE << BOLD << " ||\n" << RESET;
    };

    std::cout << PURPLE << BOLD << "||" << RESET
              << GRAY << Terminal::center("-- Keyboard --", 40) << RESET
              << PURPLE << BOLD << "||\n" << RESET;

    row("Arrows / WASD",   "D-Pad");
    row("Z  / Numpad 1",   "A Button");
    row("X  / Numpad 2",   "B Button");
    row("Enter",           "Start");
    row("Shift",           "Select");
    row("Escape",          "Quit");

    std::cout << PURPLE << BOLD << "||" << RESET
              << GRAY << Terminal::center("-- Gamepad --", 40) << RESET
              << PURPLE << BOLD << "||\n" << RESET;

    row("D-Pad",           "D-Pad");
    row("A",               "A Button");
    row("B",               "B Button");
    row("Start",           "Start");
    row("Back",            "Select");
    row("Left Stick",      "D-Pad");

    std::cout << PURPLE << BOLD << "+" << hline << "+\n" << RESET << '\n';
}

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

        Emulator emulator;
        Controls controls;

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
                using namespace Terminal::Color;
                std::cout << '\n' << PURPLE_L << "Goodbye." << RESET << '\n';
                return 0;
            }

            const std::string selected = menu.getSelectedRom();
            if (selected.empty())
                continue;

            if (!emulator.loadRom(selected))
            {
                using namespace Terminal::Color;
                std::cerr << PURPLE << "[ROM]" << RESET
                          << " Failed to load ROM. Press ENTER to retry...";
                std::cin.ignore();
                std::cin.get();
                continue;
            }

            break;
        }

        while (emulator.running())
        {
            auto frameStart = std::chrono::steady_clock::now();

            controls.poll();

            if (controls.quitRequested())
                emulator.stop();

            emulator.update(controls.state());
            emulator.render(controls.state());

            auto elapsed  = std::chrono::duration_cast<std::chrono::duration<double>>(
                std::chrono::steady_clock::now() - frameStart
            );
            auto sleepFor = std::chrono::duration<double>(FRAME_TIME) - elapsed;

            if (sleepFor > std::chrono::duration<double>::zero())
                std::this_thread::sleep_for(sleepFor);
        }

        using namespace Terminal::Color;
        std::cout << '\n' << PURPLE_L << "Emulation finished." << RESET
                  << "\nPress ENTER to exit...";
        std::cin.ignore();
        std::cin.get();

        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << Terminal::Color::PURPLE << "\n[FATAL] "
                  << Terminal::Color::RESET  << e.what() << '\n';
    }
    catch (...)
    {
        std::cerr << Terminal::Color::PURPLE << "\n[FATAL] "
                  << Terminal::Color::RESET  << "Unknown exception.\n";
    }

    std::cout << "\nPress ENTER to exit...";
    std::cin.get();

    return 1;
}
