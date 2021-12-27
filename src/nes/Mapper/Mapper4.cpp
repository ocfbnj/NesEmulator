#include <cassert>

#include <nes/Mapper/Mapper4.h>

std::uint8_t Mapper4::cpuRead(std::uint16_t addr) {
    if (addr >= 0x6000 && addr < 0x8000) {
        return prgRam[addr & 0x1FFF];
    }

    std::uint32_t baseAddr = 0;
    std::uint8_t d6 = (bankSelect >> 6) & 1;

    if (addr >= 0x8000 && addr < 0xA000) {
        if (d6) {
            baseAddr = (prgBanks() * 2 - 2) * 0x2000;
        } else {
            baseAddr = (bankRegister[6] & (prgBanks() * 2 - 1)) * 0x2000;
        }
    } else if (addr >= 0xA000 && addr < 0xC000) {
        baseAddr = (bankRegister[7] & (prgBanks() * 2 - 1)) * 0x2000;
    } else if (addr >= 0xC000 && addr < 0xE000) {
        if (d6) {
            baseAddr = (bankRegister[6] & (prgBanks() * 2 - 1)) * 0x2000;
        } else {
            baseAddr = (prgBanks() * 2 - 2) * 0x2000;
        }
    } else if (addr >= 0xE000 && addr <= 0xFFFF) {
        baseAddr = (prgBanks() * 2 - 1) * 0x2000;
    }

    std::uint32_t mappedAddr = baseAddr + (addr & 0x1FFF);
    assert(mappedAddr >= 0 && mappedAddr < cartridge.prgRom.size());

    return cartridge.prgRom[mappedAddr];
}

void Mapper4::cpuWrite(std::uint16_t addr, std::uint8_t data) {
    if (addr >= 0x6000 && addr < 0x8000) {
        prgRam[addr & 0x1FFF] = data;
        return;
    }

    if (addr >= 0x8000 && addr < 0xA000) {
        if (addr & 1) {
            // Bank data ($8001-$9FFF, odd)
            bankRegister[bankSelect & 0b111] = data;
        } else {
            // Bank select ($8000-$9FFE, even)
            bankSelect = data;
        }
    } else if (addr >= 0xA000 && addr < 0xC000) {
        if (addr & 1) {
            // PRG RAM protect ($A001-$BFFF, odd)
            prgRamProtect = data;
        } else {
            // Mirroring ($A000-$BFFE, even)
            mirror = data;
        }
    } else if (addr >= 0xC000 && addr < 0xE000) {
        if (addr & 1) {
            // IRQ reload ($C001-$DFFF, odd)
            irqCounter = 0;
        } else {
            // IRQ latch ($C000-$DFFE, even)
            irqReload = data;
        }
    } else if (addr >= 0xE000 && addr <= 0xFFFF) {
        if (addr & 1) {
            // IRQ enable ($E001-$FFFF, odd)
            irqEnable = true;
        } else {
            // IRQ disable ($E000-$FFFE, even)
            irqEnable = false;
            irqActive = false;
        }
    }
}

std::uint8_t Mapper4::ppuRead(std::uint16_t addr) {
    assert(addr >= 0 && addr < 0x2000);

    std::uint32_t baseAddr = 0;
    std::uint8_t d7 = (bankSelect >> 7) & 1;

    if (addr >= 0x0000 && addr < 0x0400) {
        if (d7) {
            baseAddr = bankRegister[2] * 0x0400;
        } else {
            baseAddr = bankRegister[0] * 0x0400;
        }
    } else if (addr >= 0x0400 && addr < 0x0800) {
        if (d7) {
            baseAddr = bankRegister[3] * 0x0400;
        } else {
            baseAddr = bankRegister[0] * 0x0400 + 0x0400;
        }
    } else if (addr >= 0x0800 && addr < 0x0C00) {
        if (d7) {
            baseAddr = bankRegister[4] * 0x0400;
        } else {
            baseAddr = bankRegister[1] * 0x0400;
        }
    } else if (addr >= 0x0C00 && addr < 0x1000) {
        if (d7) {
            baseAddr = bankRegister[5] * 0x0400;
        } else {
            baseAddr = bankRegister[1] * 0x0400 + 0x0400;
        }
    } else if (addr >= 0x1000 && addr < 0x1400) {
        if (d7) {
            baseAddr = bankRegister[0] * 0x0400;
        } else {
            baseAddr = bankRegister[2] * 0x0400;
        }
    } else if (addr >= 0x1400 && addr < 0x1800) {
        if (d7) {
            baseAddr = bankRegister[0] * 0x0400 + 0x0400;
        } else {
            baseAddr = bankRegister[3] * 0x0400;
        }
    } else if (addr >= 0x1800 && addr < 0x1C00) {
        if (d7) {
            baseAddr = bankRegister[1] * 0x0400;
        } else {
            baseAddr = bankRegister[4] * 0x0400;
        }
    } else if (addr >= 0x1C00 && addr < 0x2000) {
        if (d7) {
            baseAddr = bankRegister[1] * 0x0400 + 0x0400;
        } else {
            baseAddr = bankRegister[5] * 0x0400;
        }
    }

    std::uint32_t mappedAddr = baseAddr + (addr & 0x03FF);
    assert(mappedAddr >= 0 && mappedAddr < cartridge.chrRom.size());

    return cartridge.chrRom[mappedAddr];
}

void Mapper4::ppuWrite(std::uint16_t addr, std::uint8_t data) {
    assert(0);
}

void Mapper4::reset() {
    bankSelect = 0;
    bankRegister.fill(0);

    mirror = 0;
    prgRamProtect = 0;

    irqReload = 0;
    irqCounter = 0;

    irqEnable = false;
    irqActive = false;
}

void Mapper4::serialize(std::ostream& os) const {
    os.write(reinterpret_cast<const char*>(prgRam.data()), prgRam.size());
    os.write(reinterpret_cast<const char*>(&bankSelect), sizeof bankSelect);
    os.write(reinterpret_cast<const char*>(bankRegister.data()), bankRegister.size());
    os.write(reinterpret_cast<const char*>(&mirror), sizeof mirror);
    os.write(reinterpret_cast<const char*>(&prgRamProtect), sizeof prgRamProtect);
    os.write(reinterpret_cast<const char*>(&irqReload), sizeof irqReload);
    os.write(reinterpret_cast<const char*>(&irqCounter), sizeof irqCounter);
    os.write(reinterpret_cast<const char*>(&irqEnable), sizeof irqEnable);
    os.write(reinterpret_cast<const char*>(&irqActive), sizeof irqActive);
}

void Mapper4::deserialize(std::istream& is) {
    is.read(reinterpret_cast<char*>(prgRam.data()), prgRam.size());
    is.read(reinterpret_cast<char*>(&bankSelect), sizeof bankSelect);
    is.read(reinterpret_cast<char*>(bankRegister.data()), bankRegister.size());
    is.read(reinterpret_cast<char*>(&mirror), sizeof mirror);
    is.read(reinterpret_cast<char*>(&prgRamProtect), sizeof prgRamProtect);
    is.read(reinterpret_cast<char*>(&irqReload), sizeof irqReload);
    is.read(reinterpret_cast<char*>(&irqCounter), sizeof irqCounter);
    is.read(reinterpret_cast<char*>(&irqEnable), sizeof irqEnable);
    is.read(reinterpret_cast<char*>(&irqActive), sizeof irqActive);
}

Mirroring Mapper4::mirroring() const {
    return (mirror & 0b1) ? Mirroring::Horizontal : Mirroring::Vertical;
}

bool Mapper4::irqState() const {
    return irqActive;
}

void Mapper4::irqClear() {
    irqActive = false;
}

void Mapper4::scanline() {
    if (irqCounter == 0) {
        irqCounter = irqReload;
    } else {
        irqCounter--;
    }

    if (irqCounter == 0 && irqEnable) {
        irqActive = true;
    }
}
