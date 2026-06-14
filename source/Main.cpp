#include <iostream>
#include "../project.cpp"

int main()
{
    std::cout << "=========================\n";
    std::cout << Project::NAME << '\n';
    std::cout << "Version: " << Project::VERSION << '\n';
    std::cout << "Main Class: " << Project::MAIN_CLASS << '\n';
    std::cout << "=========================\n\n";

    auto sourceFiles = Project::getSourceFiles();

    std::cout << "Loaded source files:\n";

    for (const auto& file : sourceFiles)
    {
        std::cout << "- " << file << '\n';
    }

    std::cout << "\nSuper Emulator started successfully.\n";

    return 0;
}
