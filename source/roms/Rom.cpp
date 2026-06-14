#include "Rom.hpp"

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

static bool readHeader(std::ifstream& file, uint8_t header[16])
{
    file.read(reinterpret_cast<char*>(header), 16);
    return file.gcount() == 16
        && header[0] == 'N'
        && header[1] == 'E'
        && header[2] == 'S'
        && header[3] == 0x1A;
}

static uint16_t extractMapper(const uint8_t header[16], uint8_t version)
{
    uint16_t lo = header[6] >> 4;
    uint16_t hi = header[7] & 0xF0;
    if (version == 2)
        return static_cast<uint16_t>(lo | hi | ((header[8] & 0x0F) << 8));
    return static_cast<uint16_t>(lo | hi);
}

RomInfo Rom::parseHeader(const std::string& path)
{
    RomInfo info;
    info.path = path;
    info.name = fs::path(path).filename().string();

    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
    {
        info.error = "Cannot open file";
        return info;
    }

    uint8_t header[16];
    if (!readHeader(file, header))
    {
        info.error = "Invalid iNES header";
        return info;
    }

    info.nesVersion = ((header[7] & 0x0C) == 0x08) ? 2 : 1;
    info.mapper     = extractMapper(header, info.nesVersion);
    info.prgBanks   = header[4];
    info.chrBanks   = header[5];
    info.prgSizeKB  = info.prgBanks * 16;
    info.chrSizeKB  = info.chrBanks * 8;
    info.battery    = (header[6] & 0x02) != 0;
    info.trainer    = (header[6] & 0x04) != 0;
    info.fourScreen = (header[6] & 0x08) != 0;
    info.pal        = (info.nesVersion == 2)
                    ? (header[12] & 0x01) != 0
                    : (header[9]  & 0x01) != 0;
    info.valid      = true;
    return info;
}

bool Rom::load(const std::string& path)
{
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
        return false;

    uint8_t header[16];
    if (!readHeader(file, header))
        return false;

    const uint8_t nesVersion = ((header[7] & 0x0C) == 0x08) ? 2 : 1;

    prgBanks_ = header[4];
    chrBanks_ = header[5];
    mapper_   = extractMapper(header, nesVersion);

    if (header[6] & 0x04)
        file.seekg(512, std::ios::cur);

    prgData_.resize(prgBanks_ * 16384);
    file.read(reinterpret_cast<char*>(prgData_.data()),
              static_cast<std::streamsize>(prgData_.size()));

    if (!file)
        return false;

    chrData_.resize(chrBanks_ * 8192);
    if (!chrData_.empty())
        file.read(reinterpret_cast<char*>(chrData_.data()),
                  static_cast<std::streamsize>(chrData_.size()));

    return true;
}

const std::vector<uint8_t>& Rom::getPRG()      const noexcept { return prgData_; }
const std::vector<uint8_t>& Rom::getCHR()      const noexcept { return chrData_; }
uint16_t                    Rom::getMapper()   const noexcept { return mapper_;   }
uint8_t                     Rom::getPRGBanks() const noexcept { return prgBanks_; }
uint8_t                     Rom::getCHRBanks() const noexcept { return chrBanks_; }
