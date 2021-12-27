#ifndef OCFBNJ_NES_MAPPER_H
#define OCFBNJ_NES_MAPPER_H

#include <cstdint>
#include <istream>
#include <memory>
#include <ostream>

#include <nes/Cartridge.h>

// Mapper is an interface that provide access to extended ROM memory(both CHR ROM and PRG ROM).
// See https://bugzmanov.github.io/nes_ebook/chapter_5.html
class Mapper {
public:
    static std::unique_ptr<Mapper> create(Cartridge cartridge);

    explicit Mapper(Cartridge cartridge);
    virtual ~Mapper() = default;

    virtual std::uint8_t cpuRead(std::uint16_t addr) = 0;
    virtual void cpuWrite(std::uint16_t addr, std::uint8_t data) = 0;

    virtual std::uint8_t ppuRead(std::uint16_t addr) = 0;
    virtual void ppuWrite(std::uint16_t addr, std::uint8_t data) = 0;

    virtual void reset();

    virtual void serialize(std::ostream& os) const;
    virtual void deserialize(std::istream& is);

    virtual Mirroring mirroring() const;

    virtual bool irqState() const;
    virtual void irqClear();

    virtual void scanline();

protected:
    std::uint8_t prgBanks() const;
    std::uint8_t chrBanks() const;

    Cartridge cartridge;
};

#endif // OCFBNJ_NES_MAPPER_H
