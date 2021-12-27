#include <cassert>

#include <nes/Mapper/Mapper1.h>

std::uint8_t Mapper1::cpuRead(std::uint16_t addr) {
    if (addr >= 0x6000 && addr < 0x8000) {
        return prgRam[addr - 0x6000];
    }

    std::uint32_t mappedAddr = 0;
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        // PRG ROM bank mode
        std::uint8_t mode = (controlRegister >> 2) & 0b11;

        if (mode == 0 || mode == 1) {
            // 0, 1: switch 32 KB at $8000, ignoring low bit of bank number
            std::uint16_t selectedPrgBank = (prgBank >> 1) & 0b111;
            mappedAddr = (selectedPrgBank * 0x8000) + (addr & 0x7FFF);
        } else if (mode == 2) {
            // 2: fix first bank at $8000 and switch 16 KB bank at $C000
            if (addr >= 0x8000 && addr < 0xC000) {
                mappedAddr = addr & 0x3FFF;
            } else if (addr >= 0xC000 && addr <= 0xFFFF) {
                std::uint16_t selectedPrgBank = prgBank & 0b1111;
                mappedAddr = (selectedPrgBank * 0x4000) + (addr & 0x3FFF);
            }
        } else if (mode == 3) {
            // 3: fix last bank at $C000 and switch 16 KB bank at $8000
            if (addr >= 0x8000 && addr < 0xC000) {
                std::uint16_t selectedPrgBank = prgBank & 0b1111;
                mappedAddr = (selectedPrgBank * 0x4000) + (addr & 0x3FFF);
            } else if (addr >= 0xC000 && addr <= 0xFFFF) {
                mappedAddr = ((prgBanks() - 1) * 0x4000) + (addr & 0x3FFF);
            }
        } else {
            assert(0);
        }
    }

    return cartridge.prgRom[mappedAddr];
}

void Mapper1::cpuWrite(std::uint16_t addr, std::uint8_t data) {
    if (addr >= 0x6000 && addr < 0x8000) {
        prgRam[addr - 0x6000] = data;
        return;
    }

    if (addr >= 0x8000 && addr <= 0xFFFF) {
        static std::uint8_t count = 0;

        // Load Register
        if (data & 0x80) {
            // Reset shift register and write Control with (Control OR $0C),
            // locking PRG ROM at $C000-$FFFF to the last bank.
            loadRegister = 0;
            controlRegister |= 0x0C;
            count = 0;
        } else {
            loadRegister >>= 1;
            loadRegister |= (data & 0b1) << 4;
            count++;

            if (count == 5) {
                count = 0;

                std::uint8_t targetRegister = (addr >> 13) & 0x03;
                if (targetRegister == 0) {
                    controlRegister = loadRegister & 0b1'1111;
                } else if (targetRegister == 1) {
                    chrBank0 = loadRegister & 0b1'1111;
                } else if (targetRegister == 2) {
                    chrBank1 = loadRegister & 0b1'1111;
                } else if (targetRegister == 3) {
                    prgBank = loadRegister & 0b1'1111;
                } else {
                    assert(0);
                }

                loadRegister = 0;
            }
        }
    }
}

std::uint8_t Mapper1::ppuRead(std::uint16_t addr) {
    assert(addr >= 0 && addr < 0x2000);

    if (chrBanks() == 0) {
        return cartridge.chrRom[addr];
    }

    std::uint32_t mappedAddr = 0;
    if (addr >= 0 && addr < 0x2000) {
        // CHR ROM bank mode
        std::uint8_t mode = (controlRegister >> 4) & 0b1;

        if (mode == 0) {
            // 0: switch 8 KB at a time
            std::uint8_t selectedChrBank = (chrBank0 >> 1) & 0b1111;
            mappedAddr = (selectedChrBank * 0x2000) + (addr & 0x1FFF);
        } else if (mode == 1) {
            // 1: switch two separate 4 KB banks
            if (addr >= 0x0000 && addr < 0x1000) {
                std::uint8_t selectedChrBank = chrBank0 & 0b1'1111;
                mappedAddr = (selectedChrBank * 0x1000) + (addr & 0x0FFF);
            } else if (addr >= 0x1000 && addr < 0x2000) {
                std::uint8_t selectedChrBank = chrBank1 & 0b1'1111;
                mappedAddr = (selectedChrBank * 0x1000) + (addr & 0x0FFF);
            }
        } else {
            assert(0);
        }
    }

    assert(mappedAddr >= 0 && mappedAddr < cartridge.chrRom.size());
    return cartridge.chrRom[mappedAddr];
}

void Mapper1::ppuWrite(std::uint16_t addr, std::uint8_t data) {
    assert(addr >= 0 && addr < 0x2000);

    if (chrBanks() == 0) {
        cartridge.chrRom[addr] = data;
    } else {
        assert(0);
    }
}

void Mapper1::reset() {
    loadRegister = 0;
    controlRegister = 0x1C;

    chrBank0 = 0;
    chrBank1 = 0;
    prgBank = 0;
}

void Mapper1::serialize(std::ostream& os) const {
    os.write(reinterpret_cast<const char*>(prgRam.data()), prgRam.size());

    if (chrBanks() == 0) {
        os.write(reinterpret_cast<const char*>(cartridge.chrRom.data()), cartridge.chrRom.size());
    }

    os.write(reinterpret_cast<const char*>(&loadRegister), sizeof loadRegister);
    os.write(reinterpret_cast<const char*>(&controlRegister), sizeof controlRegister);
    os.write(reinterpret_cast<const char*>(&chrBank0), sizeof chrBank0);
    os.write(reinterpret_cast<const char*>(&chrBank1), sizeof chrBank1);
    os.write(reinterpret_cast<const char*>(&prgBank), sizeof prgBank);
}

void Mapper1::deserialize(std::istream& is) {
    is.read(reinterpret_cast<char*>(prgRam.data()), prgRam.size());

    if (chrBanks() == 0) {
        is.read(reinterpret_cast<char*>(cartridge.chrRom.data()), cartridge.chrRom.size());
    }

    is.read(reinterpret_cast<char*>(&loadRegister), sizeof loadRegister);
    is.read(reinterpret_cast<char*>(&controlRegister), sizeof controlRegister);
    is.read(reinterpret_cast<char*>(&chrBank0), sizeof chrBank0);
    is.read(reinterpret_cast<char*>(&chrBank1), sizeof chrBank1);
    is.read(reinterpret_cast<char*>(&prgBank), sizeof prgBank);
}

Mirroring Mapper1::mirroring() const {
    Mirroring mirroringMode{};

    switch (controlRegister & 0b11) {
    case 0:
        mirroringMode = Mirroring::OneScreenLoBank;
        break;
    case 1:
        mirroringMode = Mirroring::OneScreenUpBank;
        break;
    case 2:
        mirroringMode = Mirroring::Vertical;
        break;
    case 3:
        mirroringMode = Mirroring::Horizontal;
        break;
    default:
        assert(0);
    }

    return mirroringMode;
}
