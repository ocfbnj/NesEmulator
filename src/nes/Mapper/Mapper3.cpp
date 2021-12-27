#include <cassert>

#include <nes/Mapper/Mapper3.h>

std::uint8_t Mapper3::cpuRead(std::uint16_t addr) {
    std::uint32_t mappedAddr = 0;

    if (addr >= 0x8000 && addr <= 0xFFFF) {
        if (prgBanks() == 1) {
            mappedAddr = addr & 0x3FFF;
        } else if (prgBanks() == 2) {
            mappedAddr = addr & 0x7FFF;
        } else {
            assert(0);
        }
    }

    assert(mappedAddr >= 0 && mappedAddr < cartridge.prgRom.size());
    return cartridge.prgRom[mappedAddr];
}

void Mapper3::cpuWrite(std::uint16_t addr, std::uint8_t data) {
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        // CNROM only implements the lowest 2 bits,
        // capping it at 32 KiB CHR. Other boards may implement 4 or more bits for larger CHR.
        bankSelect = data & 0b11;
    }
}

std::uint8_t Mapper3::ppuRead(std::uint16_t addr) {
    assert(addr >= 0 && addr < 0x2000);

    std::uint32_t mappedAddr = 0;

    if (addr >= 0 && addr < 0x2000) {
        mappedAddr = bankSelect * 0x2000 + addr;
    }

    assert(mappedAddr >= 0 && mappedAddr < cartridge.chrRom.size());
    return cartridge.chrRom[mappedAddr];
}

void Mapper3::ppuWrite(std::uint16_t addr, std::uint8_t data) {
    assert(0);
}

void Mapper3::reset() {
    bankSelect = 0;
}

void Mapper3::serialize(std::ostream& os) const {
    os.write(reinterpret_cast<const char*>(&bankSelect), sizeof bankSelect);
}

void Mapper3::deserialize(std::istream& is) {
    is.read(reinterpret_cast<char*>(&bankSelect), sizeof bankSelect);
}
