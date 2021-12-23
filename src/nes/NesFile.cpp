#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

#include "Cartridge.h"
#include "NesFile.h"
#include "literals.h"

// NesFileHeader represents an iNES file header.
// See https://wiki.nesdev.com/w/index.php/INES#iNES_file_format
struct NesFileHeader {
    static constexpr std::uint32_t Constant = 0x1A53454E;

    std::uint32_t constant;  // Constant $4E $45 $53 $1A ("NES" followed by MS-DOS end-of-file)
    std::uint8_t prgSize;    // Size of PRG ROM in 16 KB units
    std::uint8_t chrSize;    // Size of CHR ROM in 8 KB units (Value 0 means the board uses CHR RAM)
    std::uint8_t flag6;      // Mapper, mirroring, battery, trainer
    std::uint8_t flag7;      // Mapper, VS/Playchoice, NES 2.0
    std::uint8_t flag8;      // PRG-RAM size (rarely used extension)
    std::uint8_t flag9;      // TV system (rarely used extension)
    std::uint8_t flag10;     // TV system, PRG-RAM presence (unofficial, rarely used extension)
    std::uint8_t padding[5]; // Unused padding (should be filled with zero, but some rippers put their name across bytes 7-15)
};

static_assert(sizeof(NesFileHeader) == 16, "The header is not 16 bytes");

std::unique_ptr<Cartridge> loadNesFile(std::string_view path) {
    std::ifstream nesFile{path.data(), std::ifstream::in | std::ifstream::binary};
    if (!nesFile) {
        std::cerr << "Cannot open the nes file from " << path << "\n";
        return {};
    }

    // header
    NesFileHeader header{};
    nesFile.read(reinterpret_cast<char*>(&header), sizeof(NesFileHeader));
    if (!nesFile) {
        std::cerr << "Read the nes file header failed\n";
        return {};
    }

    if (header.constant != NesFileHeader::Constant) {
        std::cerr << "Not a valid .nes file: the constant is "
                  << header.constant << ", expect " << NesFileHeader::Constant << "\n";
        return {};
    }

    // mapper number
    std::uint8_t lowerMapper = (header.flag6 >> 4) & 0x0F;
    std::uint8_t upperMapper = (header.flag7 >> 4) & 0x0F;
    std::uint8_t mapperNum = (upperMapper << 4) | lowerMapper;
    std::cout << "The mapper number is " << +mapperNum << "\n";

    // mirroring type
    Mirroring mirroringType;

    if (header.flag6 & 1) {
        mirroringType = Mirroring::Vertical;
    } else {
        mirroringType = Mirroring::Horizontal;
    }
    std::cout << "The mirroring type is " << description(mirroringType) << "\n";

    // persistent memory
    if ((header.flag6 >> 1) & 1) {
        std::cout << "Cartridge contains battery-backed PRG RAM ($6000-7FFF) or other persistent memory\n";
    }

    // trainer, if present
    if ((header.flag6 >> 2) & 1) {
        std::cout << "Hava trainer\n";
        std::vector<std::uint8_t> trainer(512); // unused
        nesFile.read(reinterpret_cast<char*>(trainer.data()), trainer.size());
        if (!nesFile) {
            std::cerr << "Read the trainer failed\n";
            return {};
        }
        std::cout << "Has trainer";
    }

    // ignore mirroring control
    if ((header.flag6 >> 3) & 1) {
        std::cout << "Ignore mirroring control or above mirroring bit; instead provide four-screen VRAM\n";
    }

    // prg rom data
    std::vector<std::uint8_t> prgRom(header.prgSize * 16_kb);
    nesFile.read(reinterpret_cast<char*>(prgRom.data()), prgRom.size());
    if (!nesFile) {
        std::cerr << "Read the prg rom data failed\n";
        return {};
    }
    std::cout << "The PRG ROM size is " << prgRom.size() / 1024 << "KB\n";

    // chr rom data
    std::vector<std::uint8_t> chrRom;

    if (header.chrSize) {
        chrRom.resize(header.chrSize * 8_kb);
        nesFile.read(reinterpret_cast<char*>(chrRom.data()), chrRom.size());
        if (!nesFile) {
            std::cerr << "Read the chr rom data failed\n";
            return {};
        }
        std::cout << "The CHR ROM size is " << chrRom.size() / 1024 << "KB\n";
    } else {
        chrRom.resize(8_kb);
        std::cout << "The board uses CHR RAM\n";
    }

    return std::make_unique<Cartridge>(header.prgSize, header.chrSize, mapperNum, mirroringType, std::move(prgRom), std::move(chrRom));
}
