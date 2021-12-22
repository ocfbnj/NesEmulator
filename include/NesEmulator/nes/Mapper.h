#ifndef OCFBNJ_NES_MAPPER_H
#define OCFBNJ_NES_MAPPER_H

#include <cstdint>
#include <istream>
#include <memory>
#include <ostream>

#include "Cartridge.h"

// Mapper is an interface that provide access to extended ROM memory(both CHR ROM and PRG ROM).
// See https://bugzmanov.github.io/nes_ebook/chapter_5.html
class Mapper {
public:
    static std::unique_ptr<Mapper> create(std::unique_ptr<Cartridge> cartridge);

    explicit Mapper(std::unique_ptr<Cartridge> cartridge);
    virtual ~Mapper() = default;

    virtual void serialize(std::ostream& os);
    virtual void deserialize(std::istream& is);

    virtual uint8_t cpuRead(uint16_t addr) = 0;
    virtual void cpuWrite(uint16_t addr, uint8_t data) = 0;

    virtual uint8_t ppuRead(uint16_t addr) = 0;
    virtual void ppuWrite(uint16_t addr, uint8_t data) = 0;

    virtual void reset();

    [[nodiscard]] virtual Mirroring mirroring() const;

    [[nodiscard]] virtual bool irqState() const;
    virtual void irqClear();

    virtual void scanline();

protected:
    [[nodiscard]] uint8_t prgBanks() const;
    [[nodiscard]] uint8_t chrBanks() const;

    std::unique_ptr<Cartridge> cartridge;
};

#endif // OCFBNJ_NES_MAPPER_H
