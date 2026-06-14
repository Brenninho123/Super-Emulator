#include <iostream>
#include <chrono>
#include <thread>
#include <exception>
#include <filesystem>
#include <vector>
#include <string>

#include "menu/MainMenu.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

class Emulator
{
public:
    bool initialize()
    {
        std::cout << "[Core] Initializing Emulator...\n";

        romLoaded = false;
        runningState = true;
        frameCount = 0;

        return true;
    }

    bool loadRom(const std::string& path)
    {
        loadedRom = path;
        romLoaded = true;

        std::cout << "\n[ROM] Loaded:\n";
        std::cout << loadedRom << "\n\n";

        return true;
    }

    void update()
    {
        frameCount++;
    }

    void render()
    {
        if (frameCount % 60 == 0)
        {
            std::cout
                << "[FPS] "
                << frameCount / 60
                << " second(s) elapsed\n";
        }
    }

    bool running() const
    {
        return runningState;
    }

    void stop()
    {
        runningState = false;
    }

private:
    bool runningState = false;
    bool romLoaded = false;

    unsigned long long frameCount = 0;

    std::string loadedRom;
};

static void printHeader()
{
    std::cout << "========================================\n";
    std::cout << "          SUPER EMULATOR\n";
    std::cout << "========================================\n";
    std::cout << "Version : 1.0.0\n";
    std::cout << "System  : NES\n";
    std::cout << "Target  : 60 FPS\n";
    std::cout << "========================================\n\n";
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
        {
            throw std::runtime_error(
                "Failed to initialize emulator."
            );
        }

        MainMenu menu;

        menu.scanRoms("roms");

        while (true)
        {
            menu.show();

            std::string selectedRom =
                menu.getSelectedRom();

            if (selectedRom.empty())
            {
                std::cout
                    << "\nNo ROM selected.\n";

                std::cout
                    << "Press ENTER to retry...";
                std::cin.ignore();
                std::cin.get();

                continue;
            }

            emulator.loadRom(selectedRom);

            break;
        }

        constexpr double targetFrameTime =
            1.0 / 60.0;

        while (emulator.running())
        {
            auto frameStart =
                std::chrono::high_resolution_clock::now();

            emulator.update();
            emulator.render();

            auto frameEnd =
                std::chrono::high_resolution_clock::now();

            std::chrono::duration<double> elapsed =
                frameEnd - frameStart;

            double sleepTime =
                targetFrameTime - elapsed.count();

            if (sleepTime > 0.0)
            {
                std::this_thread::sleep_for(
                    std::chrono::duration<double>(sleepTime)
                );
            }

            if (std::cin.rdbuf()->in_avail())
            {
                char c;
                std::cin >> c;

                if (c == 'q' || c == 'Q')
                {
                    emulator.stop();
                }
            }
        }

        std::cout
            << "\nEmulation Finished.\n";

        std::cout
            << "Press ENTER to exit...";

        std::cin.ignore();
        std::cin.get();

        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr
            << "\n[FATAL] "
            << e.what()
            << '\n';
    }
    catch (...)
    {
        std::cerr
            << "\n[FATAL] Unknown exception.\n";
    }

    std::cout
        << "\nPress ENTER to exit...";

    std::cin.get();

    return 1;
}
