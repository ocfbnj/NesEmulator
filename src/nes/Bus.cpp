#include <cassert>
#include <utility>

#include "Bus.h"
#include "Mirroring.h"

namespace {
uint16_t mirrorPaletteAddr(uint16_t addr) {
    assert(addr >= 0x3F00 && addr < 0x4000);

    // addresses $3F10/$3F14/$3F18/$3F1C are mirrors of $3F00/$3F04/$3F08/$3F0C
    if (addr == 0x3F10 || addr == 0x3F14 || addr == 0x3F18 || addr == 0x3F1C) {
        addr -= 0x10;
    }

    assert(addr >= 0x3F00 && addr < 0x3F20);
    return addr;
}

uint16_t mirrorVramAddr(uint16_t addr, Mirroring mirroring) {
    assert(addr >= 0x2000 && addr < 0x3F00);

    const uint16_t vramIndex = addr - 0x2000;
    assert(vramIndex >= 0 && vramIndex < 0x1000);

    const uint8_t nameTable = vramIndex / 0x0400;
    assert(nameTable >= 0 && nameTable < 4);

    switch (mirroring) {
    case Mirroring::OneScreenLoBank:
        addr = addr & 0x23FF;
        break;
    case Mirroring::OneScreenUpBank:
        addr = 0x2400 + (addr & 0x03FF);
        break;
    case Mirroring::Vertical:
        if (nameTable == 0 || nameTable == 2) {
            addr = addr & 0x23FF;
        } else if (nameTable == 1 || nameTable == 3) {
            addr = 0x2400 + (addr & 0x03FF);
        }
        break;
    case Mirroring::Horizontal:
        if (nameTable == 0 || nameTable == 1) {
            addr = addr & 0x23FF;
        } else if (nameTable == 2 || nameTable == 3) {
            addr = 0x2400 + (addr & 0x03FF);
        }
        break;
    default:
        assert(0);
    }

    assert(addr >= 0x2000 && addr < 0x2800);
    return addr;
}
} // namespace

Bus::Bus(std::unique_ptr<Mapper> mapper) : mapper(std::move(mapper)), cpu(*this), ppu(*this) {}

void Bus::serialize(std::ostream& os) {
    mapper->serialize(os);
    cpu.serialize(os);
    ppu.serialize(os);
    os.write((char*)cpuRam.data(), cpuRam.size());
    os.write((char*)ppuRam.data(), ppuRam.size());
}

void Bus::deserialize(std::istream& is) {
    mapper->deserialize(is);
    cpu.deserialize(is);
    ppu.deserialize(is);
    is.read((char*)cpuRam.data(), cpuRam.size());
    is.read((char*)ppuRam.data(), ppuRam.size());
}

uint8_t Bus::cpuRead(uint16_t addr) {
    uint8_t data = 0;

    if (addr >= 0x0000 && addr < 0x2000) {
        // CPU RAM
        data = cpuRam[addr & 0x07FF];
    } else if (addr >= 0x2000 && addr < 0x4000) {
        addr &= 0x2007;

        // The PPU exposes eight memory-mapped registers to the CPU.
        if (addr == 0x2002) {
            // PPU Status Register
            data = ppu.readStatus();
        } else if (addr == 0x2004) {
            // PPU OAM Data Register
            data = ppu.readOAMData();
        } else if (addr == 0x2007) {
            // PPU Data Register
            data = ppu.readData();
        } else {
            // These are write-only registers. (some games do read these registers?)
            // assert(0);
        }
    } else if (addr >= 0x4000 && addr < 0x4018) {
        if (addr == 0x4016) {
            data = joypad.read();
        }
    } else if (addr >= 0x4018 && addr < 0x4020) {
        // APU and I/O functionality that is normally disabled.
        // See https://wiki.nesdev.org/w/index.php?title=CPU_Test_Mode
        assert(0);
    } else {
        // Cartridge space: PRG ROM, PRG RAM, and mapper registers.
        // See https://wiki.nesdev.org/w/index.php?title=CPU_memory_map
        data = mapper->cpuRead(addr);
    }

    return data;
}

uint16_t Bus::cpuRead16(uint16_t addr) {
    uint16_t lo = cpuRead(addr);
    uint16_t hi = cpuRead(addr + 1);

    return (hi << 8) | lo;
}

void Bus::cpuWrite(uint16_t addr, uint8_t data) {
    if (addr >= 0x0000 && addr < 0x2000) {
        // CPU RAM
        cpuRam[addr & 0x07FF] = data;
    } else if (addr >= 0x2000 && addr < 0x4000) {
        addr &= 0x2007;

        // The PPU exposes eight memory-mapped registers to the CPU.
        if (addr == 0x2000) {
            // PPU Controller Register
            ppu.writeCtrl(data);
        } else if (addr == 0x2001) {
            // PPU Mask Register
            ppu.writeMask(data);
        } else if (addr == 0x2003) {
            // PPU OAM Address Register
            ppu.writeOAMAddr(data);
        } else if (addr == 0x2004) {
            // PPU OAM Data Register
            ppu.writeOAMData(data);
        } else if (addr == 0x2005) {
            // PPU Scroll Data Register
            ppu.writeScroll(data);
        } else if (addr == 0x2006) {
            // PPU Address Register
            ppu.writeAddr(data);
        } else if (addr == 0x2007) {
            // PPU Data Register
            ppu.writeData(data);
        } else {
            // PPU Status Register is read-only. (some games do write these registers?)
            // assert(0);
        }
    } else if (addr >= 0x4000 && addr < 0x4018) {
        if (addr == 0x4014) {
            // Writing $XX will upload 256 bytes of data from CPU page $XX00-$XXFF to the internal PPU OAM.
            std::array<uint8_t, 256> buffer{};
            uint16_t hi = uint16_t(data) << 8;
            for (int i = 0x00; i <= 0xFF; i++) {
                buffer[i] = cpuRead(hi | i);
            }

            ppu.writeOAMDMA(buffer);
        } else if (addr == 0x4016) {
            joypad.write(data);
        }
    } else if (addr >= 0x4018 && addr < 0x4020) {
        // APU and I/O functionality that is normally disabled.
        // See https://wiki.nesdev.org/w/index.php?title=CPU_Test_Mode
        assert(0);
    } else {
        // Save RAM and PRG ROM that stored in cartridge.
        mapper->cpuWrite(addr, data);
    }
}

uint8_t Bus::ppuRead(uint16_t addr) {
    uint8_t data = 0;

    if (addr >= 0x0000 && addr < 0x2000) {
        // CHR ROM (aka pattern table)
        data = mapper->ppuRead(addr);
    } else if (addr >= 0x2000 && addr < 0x3F00) {
        addr &= 0x2FFF;

        // PPU RAM (aka name table)
        data = ppuRam[mirrorVramAddr(addr, mapper->mirroring()) - 0x2000];
    } else if (addr >= 0x3F00 && addr < 0x4000) {
        addr &= 0x3F1F;

        // palette RAM indexes
        data = ppu.readPalette(mirrorPaletteAddr(addr));
    } else {
        assert(0);
    }

    return data;
}

void Bus::ppuWrite(uint16_t addr, uint8_t data) {
    if (addr >= 0x0000 && addr < 0x2000) {
        // CHR ROM (aka pattern table)
        mapper->ppuWrite(addr, data);
    } else if (addr >= 0x2000 && addr < 0x3F00) {
        addr &= 0x2FFF;

        // PPU RAM (aka name table)
        ppuRam[mirrorVramAddr(addr, mapper->mirroring()) - 0x2000] = data;
    } else if (addr >= 0x3F00 && addr < 0x4000) {
        addr &= 0x3F1F;

        // palette RAM indexes
        ppu.writePalette(mirrorPaletteAddr(addr), data);
    } else {
        assert(0);
    }
}

void Bus::clock() {
    static uint8_t i = 0;

    ppu.clock();

    if ((i % 3) == 0) {
        cpu.clock();
    }

    if (mapper->irqState()) {
        mapper->irqClear();
        cpu.irq();
    }

    i++;
}

void Bus::reset() {
    mapper->reset();
    cpu.reset();
    ppu.reset();
}

Mapper& Bus::getMapper() {
    return *mapper;
}

CPU& Bus::getCPU() {
    return cpu;
}

PPU& Bus::getPPU() {
    return ppu;
}

Joypad& Bus::getJoypad() {
    return joypad;
}
