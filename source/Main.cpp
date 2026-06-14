#include <iostream>
#include "../project.cpp"

int main()
{
    std::cout << "Super Emulator" << std::endl;
    std::cout << "Main Class: " << Project::MAIN_CLASS << std::endl;
    std::cout << "Source Path: " << Project::SOURCE_PATH << std::endl;

    auto sourceFiles = Project::getSourceFiles();

    std::cout << "\nLoaded Source Files:\n";

    for (const auto& file : sourceFiles)
    {
        std::cout << file << std::endl;
    }

    return 0;
}
