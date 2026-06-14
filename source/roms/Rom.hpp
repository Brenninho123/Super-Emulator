#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct RomInfo
{
    std::string path;
    std::string name;
    uint16_t    mapper      = 0;
    uint8_t     prgBanks    = 0;
    uint8_t     chrBanks    = 0;
    uint32_t    prgSizeKB   = 0;
    uint32_t    chrSizeKB   = 0;
    bool        battery     = false;
    bool        trainer     = false;
    bool        fourScreen  = false;
    bool        pal         = false;
    uint8_t     nesVersion  = 1;
    bool        valid       = false;
    std::string error;
};

class Rom
{
public:
    [[nodiscard]] bool load(const std::string& path);

    static RomInfo parseHeader(const std::string& path);

    [[nodiscard]] const std::vector<uint8_t>& getPRG()      const noexcept;
    [[nodiscard]] const std::vector<uint8_t>& getCHR()      const noexcept;
    [[nodiscard]] uint16_t                    getMapper()   const noexcept;
    [[nodiscard]] uint8_t                     getPRGBanks() const noexcept;
    [[nodiscard]] uint8_t                     getCHRBanks() const noexcept;

private:
    std::vector<uint8_t> prgData_;
    std::vector<uint8_t> chrData_;
    uint16_t mapper_   = 0;
    uint8_t  prgBanks_ = 0;
    uint8_t  chrBanks_ = 0;
};
