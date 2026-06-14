#include <iostream>
#include "../Project.hpp"

int main()
{
    std::cout << Project::NAME << '\n';
    std::cout << Project::VERSION << '\n';
    std::cout << Project::getTargetName() << '\n';

    for (const auto& file : Project::getSourceFiles())
        std::cout << file << '\n';

    return 0;
}
