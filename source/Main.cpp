#include <iostream>
#include <chrono>
#include <thread>
#include <exception>

#ifdef _WIN32
#include <windows.h>
#endif

class Emulator
{
public:
    bool initialize()
    {
        std::cout << "Initializing Super Emulator...\n";

        initialized = true;

        return true;
    }

    void update()
    {
        frames++;
    }

    void render()
    {
        if (frames % 60 == 0)
        {
            std::cout << "Frames: " << frames << '\n';
        }
    }

    bool running() const
    {
        return frames < 600;
    }

private:
    bool initialized = false;
    unsigned long long frames = 0;
};

int main()
{
    try
    {
#ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
#endif

        std::cout << "=====================================\n";
        std::cout << "         SUPER EMULATOR\n";
        std::cout << "=====================================\n";
        std::cout << "Version: 1.0.0\n";
        std::cout << "Target FPS: 60\n";
        std::cout << "=====================================\n\n";

        Emulator emulator;

        if (!emulator.initialize())
        {
            std::cerr << "Failed to initialize emulator.\n";
            std::cin.get();
            return 1;
        }

        constexpr double targetFrameTime = 1.0 / 60.0;

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
        }

        std::cout << "\nEmulator stopped successfully.\n";
        std::cout << "Press ENTER to exit...";
        std::cin.get();

        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "\nFatal Error: " << e.what() << '\n';
    }
    catch (...)
    {
        std::cerr << "\nUnknown Fatal Error.\n";
    }

    std::cout << "\nPress ENTER to exit...";
    std::cin.get();

    return 1;
}
