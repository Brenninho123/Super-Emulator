#include "Rom.hpp"

#include <fstream>
#include <iostream>

bool Rom::load(const std::string& path)
{
    std::ifstream file(path, std::ios::binary);

    if (!file.is_open())
    {
        std::cout << "Failed to open ROM: " << path << '\n';
        return false;
    }

    uint8_t header[16];
    file.read(reinterpret_cast<char*>(header), 16);

    if (
        header[0] != 'N' ||
        header[1] != 'E' ||
        header[2] != 'S' ||
        header[3] != 0x1A
    )
    {
        std::cout << "Invalid NES ROM\n";
        return false;
    }

    prgBanks = header[4];
    chrBanks = header[5];

    mapper =
        ((header[7] & 0xF0)) |
        ((header[6] >> 4));

    prgData.resize(prgBanks * 16384);
    file.read(
        reinterpret_cast<char*>(prgData.data()),
        prgData.size()
    );

    chrData.resize(chrBanks * 8192);

    if (!chrData.empty())
    {
        file.read(
            reinterpret_cast<char*>(chrData.data()),
            chrData.size()
        );
    }

    std::cout << "ROM Loaded\n";
    std::cout << "PRG Banks: " << (int)prgBanks << '\n';
    std::cout << "CHR Banks: " << (int)chrBanks << '\n';
    std::cout << "Mapper: " << (int)mapper << '\n';

    return true;
}

const std::vector<uint8_t>& Rom::getPRG() const
{
    return prgData;
}

const std::vector<uint8_t>& Rom::getCHR() const
{
    return chrData;
}

uint8_t Rom::getMapper() const
{
    return mapper;
}

uint8_t Rom::getPRGBanks() const
{
    return prgBanks;
}

uint8_t Rom::getCHRBanks() const
{
    return chrBanks;
}
