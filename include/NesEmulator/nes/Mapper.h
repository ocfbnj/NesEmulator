#ifndef MAPPER_H
#define MAPPER_H

#include <cstdint>
#include <memory>

#include "Cartridge.h"

// Mapper is an interface that provide access to extended ROM memory(both CHR ROM and PRG ROM).
// See https://bugzmanov.github.io/nes_ebook/chapter_5.html
class Mapper {
public:
    static std::unique_ptr<Mapper> create(std::unique_ptr<Cartridge> cartridge);

    explicit Mapper(std::unique_ptr<Cartridge> cartridge);
    virtual ~Mapper() = default;

    virtual uint8_t readChrRom(uint16_t addr) = 0;
    virtual void writeChrRom(uint16_t addr, uint8_t data) = 0;

    virtual uint8_t readPrgRom(uint16_t addr) = 0;
    virtual void writePrgRom(uint16_t addr, uint8_t data) = 0;

    [[nodiscard]] const std::vector<uint8_t>& chrRom() const;
    [[nodiscard]] Mirroring mirroring() const;

protected:
    std::unique_ptr<Cartridge> cartridge;
};

#endif
