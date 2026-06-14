#pragma once

#include <string>
#include <vector>
#include <cstdint>

class Rom
{
public:
    bool load(const std::string& path);

    const std::vector<uint8_t>& getPRG() const;
    const std::vector<uint8_t>& getCHR() const;

    uint8_t getMapper() const;
    uint8_t getPRGBanks() const;
    uint8_t getCHRBanks() const;

private:
    std::vector<uint8_t> prgData;
    std::vector<uint8_t> chrData;

    uint8_t mapper = 0;
    uint8_t prgBanks = 0;
    uint8_t chrBanks = 0;
};
