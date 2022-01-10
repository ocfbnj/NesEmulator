#include <cassert>
#include <utility>

#include <nes/Bus.h>
#include <nes/Mirroring.h>

namespace {
std::uint16_t mirrorPaletteAddr(std::uint16_t addr) {
    assert(addr >= 0x3F00 && addr < 0x4000);

    // addresses $3F10/$3F14/$3F18/$3F1C are mirrors of $3F00/$3F04/$3F08/$3F0C
    if (addr == 0x3F10 || addr == 0x3F14 || addr == 0x3F18 || addr == 0x3F1C) {
        addr -= 0x10;
    }

    assert(addr >= 0x3F00 && addr < 0x3F20);
    return addr;
}

std::uint16_t mirrorVramAddr(std::uint16_t addr, Mirroring mirroring) {
    assert(addr >= 0x2000 && addr < 0x3F00);

    const std::uint16_t vramIndex = addr - 0x2000;
    assert(vramIndex >= 0 && vramIndex < 0x1000);

    const std::uint8_t nameTable = vramIndex / 0x0400;
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

void Bus::insert(Cartridge cartridge) {
    mapper = Mapper::create(std::move(cartridge));
    if (mapper == nullptr) {
        throw std::runtime_error{"No supported cartridge"};
    }
}

void Bus::powerUp() {
    cpu.connect(this);
    ppu.connect(this);
    apu.connect(this);

    reset();
}

void Bus::serialize(std::ostream& os) const {
    mapper->serialize(os);
    cpu.serialize(os);
    apu.serialize(os);
    ppu.serialize(os);
    os.write(reinterpret_cast<const char*>(cpuRam.data()), cpuRam.size());
    os.write(reinterpret_cast<const char*>(ppuRam.data()), ppuRam.size());
}

void Bus::deserialize(std::istream& is) {
    mapper->deserialize(is);
    cpu.deserialize(is);
    apu.deserialize(is);
    ppu.deserialize(is);
    is.read(reinterpret_cast<char*>(cpuRam.data()), cpuRam.size());
    is.read(reinterpret_cast<char*>(ppuRam.data()), ppuRam.size());
}

std::uint8_t Bus::cpuRead(std::uint16_t addr) {
    std::uint8_t data = 0;

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
            data = ppu.readOamData();
        } else if (addr == 0x2007) {
            // PPU Data Register
            data = ppu.readData();
        } else {
            // These are write-only registers. (some games do read these registers?)
            // assert(0);
        }
    } else if (addr >= 0x4000 && addr < 0x4018) {
        if (addr == 0x4015) {
            // APU Status Register
            data = apu.apuRead(addr);
        } else if (addr == 0x4016) {
            data = joypad1.read();
        } else if (addr == 0x4017) {
            data = joypad2.read();
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

std::uint16_t Bus::cpuRead16(std::uint16_t addr) {
    std::uint16_t lo = cpuRead(addr);
    std::uint16_t hi = cpuRead(addr + 1);

    return (hi << 8) | lo;
}

void Bus::cpuWrite(std::uint16_t addr, std::uint8_t data) {
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
            ppu.writeOamAddr(data);
        } else if (addr == 0x2004) {
            // PPU OAM Data Register
            ppu.writeOamData(data);
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
        if (addr >= 0x4000 && addr < 0x4009 || addr >= 0x400A && addr < 0x400D || addr >= 0x400E && addr < 0x4014 || addr == 0x4015 || addr == 0x4017) {
            // APU addresses
            apu.apuWrite(addr, data);
        } else if (addr == 0x4014) {
            // Writing $XX will upload 256 bytes of data from CPU page $XX00-$XXFF to the internal PPU OAM.
            std::array<std::uint8_t, 256> buffer{};
            std::uint16_t hi = std::uint16_t(data) << 8;
            for (int i = 0x00; i <= 0xFF; i++) {
                buffer[i] = cpuRead(hi | i);
            }

            ppu.writeOamDMA(buffer);
        } else if (addr == 0x4016) {
            joypad1.write(data);
            joypad2.write(data);
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

std::uint8_t Bus::ppuRead(std::uint16_t addr) {
    std::uint8_t data = 0;

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

void Bus::ppuWrite(std::uint16_t addr, std::uint8_t data) {
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
    static std::uint8_t i = 0;

    ppu.clock();

    if ((i % 3) == 0) {
        cpu.clock();
    }

    if ((i % 6) == 0) {
        apu.clock();
    }

    if (mapper->irqState()) {
        mapper->irqClear();
        cpu.irq();
    }

    if (++i == 6) {
        i = 0;
    }
}

void Bus::reset() {
    mapper->reset();
    cpu.reset();
    apu.reset();
    ppu.reset();
}

Mapper& Bus::getMapper() {
    return *mapper;
}

CPU& Bus::getCPU() {
    return cpu;
}

APU& Bus::getAPU() {
    return apu;
}

PPU& Bus::getPPU() {
    return ppu;
}

Joypad& Bus::getJoypad1() {
    return joypad1;
}

Joypad& Bus::getJoypad2() {
    return joypad2;
}
