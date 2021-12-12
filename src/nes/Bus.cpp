#include <cassert>
#include <utility>

#include "Bus.h"

Bus::Bus(std::unique_ptr<Mapper> mapper)
    : mapper(std::move(mapper)),
      cpu(std::make_unique<CPU>(*this)),
      ppu(std::make_unique<PPU>(*this)) {}

uint8_t Bus::read(uint16_t addr) const {
    uint8_t data = 0;

    if (addr >= 0x0000 && addr < 0x2000) {
        // CPU RAM
        data = cpuRam[addr & 0x07FF];
    } else if (addr >= 0x2000 && addr < 0x4000) {
        addr &= 0x2007;

        // The PPU exposes eight memory-mapped registers to the CPU
        if (addr == 0x2002) {
            // PPU Status Register
            data = ppu->readStatus();
        } else if (addr == 0x2004) {
            // PPU OAM Data Register
            data = ppu->readOAMData();
        } else if (addr == 0x2007) {
            // PPU Data Register
            data = ppu->readData();
        } else {
            // These are write-only registers
            assert(0);
        }
    } else if (addr >= 0x4000 && addr < 0x4018) {
        // TODO NES APU and I/O registers
        if (addr == 0x4016) {
            return joypad.read();
        }
    } else if (addr >= 0x4018 && addr < 0x4020) {
        // APU and I/O functionality that is normally disabled.
        // See https://wiki.nesdev.org/w/index.php?title=CPU_Test_Mode
        assert(0);
    } else {
        // Cartridge space: PRG ROM, PRG RAM, and mapper registers.
        // See https://wiki.nesdev.org/w/index.php?title=CPU_memory_map
        return mapper->readPrgRom(addr);
    }

    return data;
}

void Bus::write(uint16_t addr, uint8_t data) {
    if (addr >= 0x0000 && addr < 0x2000) {
        // CPU RAM
        cpuRam[addr & 0x07FF] = data;
    } else if (addr >= 0x2000 && addr < 0x4000) {
        addr &= 0x2007;

        // The PPU exposes eight memory-mapped registers to the CPU
        if (addr == 0x2000) {
            // PPU Controller Register
            ppu->writeCtrl(data);
        } else if (addr == 0x2001) {
            // PPU Mask Register
            ppu->writeMask(data);
        } else if (addr == 0x2003) {
            // PPU OAM Address Register
            ppu->writeOAMAddr(data);
        } else if (addr == 0x2004) {
            // PPU OAM Data Register
            ppu->writeOAMData(data);
        } else if (addr == 0x2005) {
            // PPU Scroll Data Register
            ppu->writeScroll(data);
        } else if (addr == 0x2006) {
            // PPU Address Register
            ppu->writeAddr(data);
        } else if (addr == 0x2007) {
            // PPU Data Register
            ppu->writeData(data);
        } else {
            // PPU Status Register is read-only
            assert(0);
        }
    } else if (addr >= 0x4000 && addr < 0x4018) {
        // TODO	NES APU and I/O registers
        if (addr == 0x4014) {
            // Writing $XX will upload 256 bytes of data from CPU page $XX00-$XXFF to the internal PPU OAM
            std::array<uint8_t, 256> buffer{};
            uint16_t hi = uint16_t(data) << 8;
            for (int i = 0x00; i <= 0xFF; i++) {
                buffer[i] = read(hi | i);
            }

            ppu->writeOAMDMA(buffer);
        } else if (addr == 0x4016) {
            joypad.write(data);
        }
    } else if (addr >= 0x4018 && addr < 0x4020) {
        // APU and I/O functionality that is normally disabled.
        // See https://wiki.nesdev.org/w/index.php?title=CPU_Test_Mode
        assert(0);
    } else {
        // Save RAM and PRG ROM that stored in cartridge
        mapper->writePrgRom(addr, data);
    }
}

uint16_t Bus::read16(uint16_t addr) const {
    uint16_t lo = read(addr);
    uint16_t hi = read(addr + 1);

    return (hi << 8) | lo;
}

void Bus::write16(uint16_t addr, uint16_t data) {
    write(addr, data & 0xFF);
    write(addr + 1, (data >> 8) & 0xFF);
}

uint8_t Bus::ppuRead(uint16_t addr) const {
    uint8_t data;

    if (addr >= 0x0000 && addr < 0x2000) {
        // CHR ROM (aka pattern table)
        data = mapper->readChrRom(addr);
    } else if (addr >= 0x2000 && addr < 0x3F00) {
        addr &= 0x2FFF;

        // PPU RAM (aka name table)
        data = ppuRam[mirrorVramAddr(addr) - 0x2000];
    } else if (addr >= 0x3F00 && addr < 0x4000) {
        addr &= 0x3F1F;

        // palette RAM indexes
        data = ppu->readPalette(mirrorPaletteAddr(addr));
    } else {
        assert(0);
    }

    return data;
}

void Bus::ppuWrite(uint16_t addr, uint8_t data) {
    if (addr >= 0x0000 && addr < 0x2000) {
        // CHR ROM (aka pattern table)
        mapper->writeChrRom(addr, data);
    } else if (addr >= 0x2000 && addr < 0x3F00) {
        addr &= 0x2FFF;

        // PPU RAM (aka name table)
        ppuRam[mirrorVramAddr(addr) - 0x2000] = data;
    } else if (addr >= 0x3F00 && addr < 0x4000) {
        addr &= 0x3F1F;

        // palette RAM indexes
        ppu->writePalette(mirrorPaletteAddr(addr), data);
    } else {
        assert(0);
    }
}

void Bus::clock() {
    static size_t i = 0;

    ppu->clock();

    if (i % 3) {
        cpu->clock();
    }

    i++;
}

CPU& Bus::getCPU() {
    return *cpu;
}

PPU& Bus::getPPU() {
    return *ppu;
}

Joypad& Bus::getJoypad() {
    return joypad;
}

const std::vector<uint8_t>& Bus::chrRom() const {
    return mapper->chrRom();
}

uint16_t Bus::mirrorPaletteAddr(uint16_t addr) const {
    // addresses $3F10/$3F14/$3F18/$3F1C are mirrors of $3F00/$3F04/$3F08/$3F0C
    if (addr == 0x3F10 || addr == 0x3F14 || addr == 0x3F18 || addr == 0x3F1C) {
        addr -= 0x10;
    }

    assert(addr >= 0x3F00 && addr < 0x3F20);
    return addr;
}

uint16_t Bus::mirrorVramAddr(uint16_t addr) const {
    const uint16_t vramIndex = addr - 0x2000;
    assert(vramIndex >= 0 && vramIndex < 0x1000);

    const uint8_t nameTable = vramIndex / 0x0400;
    assert(nameTable >= 0 && nameTable < 4);

    const Mirroring mirror = mapper->mirroring();

    if (mirror == Mirroring::Vertical) {
        if (nameTable == 0 || nameTable == 2) {
            addr = addr & 0x23FF;
        } else if (nameTable == 1 || nameTable == 3) {
            addr = 0x2400 + (addr & 0x03FF);
        }
    } else if (mirror == Mirroring::Horizontal) {
        if (nameTable == 0 || nameTable == 1) {
            addr = addr & 0x23FF;
        } else if (nameTable == 2 || nameTable == 3) {
            addr = 0x2400 + (addr & 0x03FF);
        }
    } else {
        assert(0);
    }

    assert(addr >= 0x2000 && addr < 0x2800);
    return addr;
}
