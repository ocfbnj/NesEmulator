#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

#include "NesFile.h"
#include "literals.h"

// NesFileHeader represents an iNES file header.
// See https://wiki.nesdev.com/w/index.php/INES#iNES_file_format
struct NesFileHeader {
    static constexpr uint32_t Constant = 0x1A53454E;

    uint32_t constant;                   // Constant $4E $45 $53 $1A ("NES" followed by MS-DOS end-of-file)
    uint8_t prgSize;                     // Size of PRG ROM in 16 KB units
    uint8_t chrSize;                     // Size of CHR ROM in 8 KB units (Value 0 means the board uses CHR RAM)
    uint8_t flag6;                       // Mapper, mirroring, battery, trainer
    uint8_t flag7;                       // Mapper, VS/Playchoice, NES 2.0
    [[maybe_unused]] uint8_t flag8;      // PRG-RAM size (rarely used extension)
    [[maybe_unused]] uint8_t flag9;      // TV system (rarely used extension)
    [[maybe_unused]] uint8_t flag10;     // TV system, PRG-RAM presence (unofficial, rarely used extension)
    [[maybe_unused]] uint8_t padding[5]; // Unused padding (should be filled with zero, but some rippers put their name across bytes 7-15)
};

static_assert(sizeof(NesFileHeader) == 16, "The header is not 16 bytes");

std::unique_ptr<Cartridge> loadNesFile(std::string_view path) {
    std::ifstream nesFile{path.data(), std::ifstream::in | std::ifstream::binary};
    if (!nesFile) {
        std::clog << "Cannot open the nes file from " << path << "\n";
        return {};
    }

    // header
    NesFileHeader header{};
    nesFile.read(reinterpret_cast<char*>(&header), sizeof(NesFileHeader));
    if (!nesFile) {
        std::clog << "Read the nes file header failed\n";
        return {};
    }

    if (header.constant != NesFileHeader::Constant) {
        std::clog << "Not a valid .nes file: the constant is "
                  << header.constant << ", expect " << NesFileHeader::Constant << "\n";
        return {};
    }

    // mapper number
    uint8_t lowerMapper = header.flag6 >> 4;
    uint8_t upperMapper = header.flag7 >> 4;
    uint8_t mapperNum = (upperMapper << 4) | lowerMapper;
    std::clog << "The mapper number is " << int(mapperNum) << "\n";

    // mirroring type
    Mirroring mirroringType = Mirroring::Undefined;
    if ((header.flag6 >> 3) & 1) {
        mirroringType = Mirroring::FourScreen;
    } else {
        if (header.flag6 & 1) {
            mirroringType = Mirroring::Vertical;
        } else {
            mirroringType = Mirroring::Horizontal;
        }
    }
    assert(mirroringType != Mirroring::Undefined);

    // trainer, if present
    if (header.flag6 & (1u << 2)) {
        std::clog << "Hava trainer\n";
        std::vector<uint8_t> trainer(static_cast<const int>(512_b)); // unused
        nesFile.read(reinterpret_cast<char*>(trainer.data()), trainer.size());
        if (!nesFile) {
            std::clog << "Read the trainer failed\n";
            return {};
        }
    }

    // prg rom data
    std::vector<uint8_t> prgRom(static_cast<const int>(header.prgSize * 16_kb));
    nesFile.read(reinterpret_cast<char*>(prgRom.data()), prgRom.size());
    if (!nesFile) {
        std::clog << "Read the prg rom data failed\n";
        return {};
    }

    // chr rom data
    std::vector<uint8_t> chrRom(static_cast<const int>(header.chrSize * 8_kb));
    nesFile.read(reinterpret_cast<char*>(chrRom.data()), chrRom.size());
    if (!nesFile) {
        std::clog << "Read the chr rom data failed\n";
        return {};
    }

    return std::make_unique<Cartridge>(std::move(prgRom), std::move(chrRom), mapperNum, mirroringType);
}
