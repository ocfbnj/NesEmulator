#include <cassert>

#include <nes/Mapper/Mapper2.h>

std::uint8_t Mapper2::cpuRead(std::uint16_t addr) {
    std::uint32_t mappedAddr = 0;

    if (addr >= 0x8000 && addr < 0xC000) {
        mappedAddr = bankSelect * 0x4000 + (addr & 0x3FFF);
    } else if (addr >= 0xC000 && addr <= 0xFFFF) {
        mappedAddr = (prgBanks() - 1) * 0x4000 + (addr & 0x3FFF);
    }

    assert(mappedAddr >= 0 && mappedAddr < cartridge.prgRom.size());
    return cartridge.prgRom[mappedAddr];
}

void Mapper2::cpuWrite(std::uint16_t addr, std::uint8_t data) {
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        bankSelect = data & 0b1111;
    }
}

std::uint8_t Mapper2::ppuRead(std::uint16_t addr) {
    assert(addr >= 0 && addr < 0x2000);
    return cartridge.chrRom[addr];
}

void Mapper2::ppuWrite(std::uint16_t addr, std::uint8_t data) {
    assert(addr >= 0 && addr < 0x2000);
    assert(chrBanks() == 0);
    cartridge.chrRom[addr] = data;
}

void Mapper2::reset() {
    bankSelect = 0;
}

void Mapper2::serialize(std::ostream& os) const {
    os.write(reinterpret_cast<const char*>(cartridge.chrRom.data()), cartridge.chrRom.size());
    os.write(reinterpret_cast<const char*>(&bankSelect), sizeof bankSelect);
}

void Mapper2::deserialize(std::istream& is) {
    is.read(reinterpret_cast<char*>(cartridge.chrRom.data()), cartridge.chrRom.size());
    is.read(reinterpret_cast<char*>(&bankSelect), sizeof bankSelect);
}
