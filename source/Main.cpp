#include <iostream>
#include <chrono>
#include <thread>
#include <exception>
#include <filesystem>
#include <format>
#include <string>
#include <string_view>

#include "menu/MainMenu.hpp"
#include "roms/Rom.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

static constexpr std::string_view VERSION    = "1.0.0";
static constexpr std::string_view SYSTEM     = "NES";
static constexpr int              TARGET_FPS = 60;
static constexpr double           FRAME_TIME = 1.0 / TARGET_FPS;

class Emulator
{
public:
    [[nodiscard]] bool initialize()
    {
        std::cout << "[Core] Initializing emulator...\n";
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

        std::cout << std::format(
            "\n[ROM] {}\n"
            "      PRG Banks : {}\n"
            "      CHR Banks : {}\n"
            "      Mapper    : {}\n\n",
            path.filename().string(),
            rom_.getPRGBanks(),
            rom_.getCHRBanks(),
            rom_.getMapper()
        );

        return true;
    }

    void update()
    {
        ++frameCount_;
    }

    void render()
    {
        ++framesThisSec_;

        auto   now   = std::chrono::steady_clock::now();
        double delta = std::chrono::duration<double>(now - fpsTimer_).count();

        if (delta >= 1.0)
        {
            std::cout << std::format("[FPS] {:.1f}\n", framesThisSec_ / delta);
            framesThisSec_ = 0;
            fpsTimer_      = now;
        }
    }

    [[nodiscard]] bool running() const { return running_; }
    void               stop()         { running_ = false; }

private:
    Rom  rom_;
    bool running_  = false;
    bool romLoaded_ = false;

    unsigned long long frameCount_    = 0;
    unsigned int       framesThisSec_ = 0;

    std::chrono::steady_clock::time_point fpsTimer_ =
        std::chrono::steady_clock::now();
};

static void printHeader()
{
    std::cout << std::format(
        "========================================\n"
        "          SUPER EMULATOR\n"
        "========================================\n"
        "Version : {}\n"
        "System  : {}\n"
        "Target  : {} FPS\n"
        "========================================\n\n",
        VERSION, SYSTEM, TARGET_FPS
    );
}

int main()
{
    try
    {
#ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
#endif
        printHeader();

        Emulator emulator;

        if (!emulator.initialize())
            throw std::runtime_error("Failed to initialize emulator.");

        MainMenu menu;
        menu.scanRoms("roms");

        while (true)
        {
            menu.show();

            const std::string selected = menu.getSelectedRom();

            if (selected.empty())
            {
                std::cout << "\nNo ROM selected. Press ENTER to retry...";
                std::cin.ignore();
                std::cin.get();
                continue;
            }

            if (!emulator.loadRom(selected))
            {
                std::cout << "\nFailed to load ROM. Press ENTER to retry...";
                std::cin.ignore();
                std::cin.get();
                continue;
            }

            break;
        }

        while (emulator.running())
        {
            auto frameStart = std::chrono::steady_clock::now();

            emulator.update();
            emulator.render();

            auto elapsed  = std::chrono::duration_cast<std::chrono::duration<double>>(
                std::chrono::steady_clock::now() - frameStart
            );
            auto sleepFor = std::chrono::duration<double>(FRAME_TIME) - elapsed;

            if (sleepFor > std::chrono::duration<double>::zero())
                std::this_thread::sleep_for(sleepFor);

            if (std::cin.rdbuf()->in_avail())
            {
                char c;
                std::cin >> c;
                if (c == 'q' || c == 'Q')
                    emulator.stop();
            }
        }

        std::cout << "\nEmulation finished.\nPress ENTER to exit...";
        std::cin.ignore();
        std::cin.get();

        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << std::format("\n[FATAL] {}\n", e.what());
    }
    catch (...)
    {
        std::cerr << "\n[FATAL] Unknown exception.\n";
    }

    std::cout << "\nPress ENTER to exit...";
    std::cin.get();

    return 1;
}
