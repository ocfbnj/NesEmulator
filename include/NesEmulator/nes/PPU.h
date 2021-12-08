#ifndef PPU_H
#define PPU_H

#include <array>
#include <cstdint>
#include <vector>
#include <functional>

class Bus;

class PPU {
public:
    explicit PPU(Bus& bus);

    void clock();

    [[nodiscard]] uint16_t spritePatternAddr() const;
    [[nodiscard]] uint16_t backgroundPatternAddr() const;

    [[nodiscard]] uint8_t readStatus();
    [[nodiscard]] uint8_t readOAMData() const;
    [[nodiscard]] uint8_t readData();

    [[nodiscard]] std::array<uint8_t, 4> backgroundPaletteFor(int tileX, int tileY) const;

    void writeCtrl(uint8_t data);
    void writeMask(uint8_t data);
    void writeOAMAddr(uint8_t data);
    void writeOAMData(uint8_t data);
    void writeScroll(uint8_t data);
    void writeAddr(uint8_t data);
    void writeData(uint8_t data);
    void writeOAMDMA(const std::array<uint8_t, 256>& buffer);

    std::function<void()> vblankCallback;

private:
    struct ControlRegister {
        [[nodiscard]] uint16_t baseNameTableAddr() const {
            return 0x2000 + 0x400 * n;
        }

        [[nodiscard]] uint8_t addrIncrement() const {
            return i ? 32 : 1;
        }

        [[nodiscard]] uint16_t spritePatternAddr() const {
            return s ? 0x1000 : 0x0000;
        }

        [[nodiscard]] uint16_t backgroundPatternAddr() const {
            return b ? 0x1000 : 0x0000;
        }

        [[nodiscard]] bool isGenerateVblankNMI() const {
            return v;
        }

        void write(uint8_t data) {
            n = data & (0b0000'0011);
            i = (data >> 2) & 1;
            s = (data >> 3) & 1;
            b = (data >> 4) & 1;
            h = (data >> 5) & 1;
            p = (data >> 6) & 1;
            v = (data >> 7) & 1;
        }

        uint8_t n : 2 {}; // Base nametable address
        uint8_t i : 1 {}; // VRAM address increment per CPU readData/writeData of PPUDATA
        uint8_t s : 1 {}; // Sprite pattern table address for 8x8 sprites
        uint8_t b : 1 {}; // Background pattern table address
        uint8_t h : 1 {}; // Sprite size
        uint8_t p : 1 {}; // PPU master/slave select
        uint8_t v : 1 {}; // Generate an NMI at the start of the vertical blanking interval
    };
    static_assert(sizeof(ControlRegister) == 1, "The ControlRegister is not 1 byte");

    struct MaskRegister {
        [[nodiscard]] bool isGreyScale() const {
            return g;
        }

        [[nodiscard]] bool showLeftmostBackground() const {
            return m;
        }

        [[nodiscard]] bool showLeftmostSprites() const {
            return M;
        }

        [[nodiscard]] bool showBackground() const {
            return b;
        }

        [[nodiscard]] bool showSprites() const {
            return s;
        }

        void write(uint8_t data) {
            g = (data >> 0) & 1;
            m = (data >> 1) & 1;
            M = (data >> 2) & 1;
            b = (data >> 3) & 1;
            s = (data >> 4) & 1;
            R = (data >> 5) & 1;
            G = (data >> 6) & 1;
            B = (data >> 7) & 1;
        }

        uint8_t g : 1 {}; // Greyscale (0: normal color, 1: produce a greyscale display)
        uint8_t m : 1 {}; // 1: Show background in leftmost 8 pixels of screen, 0: Hide
        uint8_t M : 1 {}; // 1: Show sprites in leftmost 8 pixels of screen, 0: Hide
        uint8_t b : 1 {}; // 1: Show background
        uint8_t s : 1 {}; // 1: Show sprites
        uint8_t R : 1 {}; // Emphasize red (green on PAL/Dendy)
        uint8_t G : 1 {}; // Emphasize green (red on PAL/Dendy)
        uint8_t B : 1 {}; // Emphasize blue
    };
    static_assert(sizeof(MaskRegister) == 1, "The MaskRegister is not 1 byte");

    struct StatusRegister {
        [[nodiscard]] uint8_t read() const {
            return _0 << 0 |
                   _1 << 1 |
                   _2 << 2 |
                   _3 << 3 |
                   _4 << 4 |
                   o << 5 |
                   s << 6 |
                   v << 7;
        }

        [[nodiscard]] bool isInVblank() const {
            return v;
        }

        void setVblank() {
            v = 1;
        }

        void resetVblank() {
            v = 0;
        }

        // unused (0 ~ 4)
        uint8_t _0 : 1 {};
        uint8_t _1 : 1 {};
        uint8_t _2 : 1 {};
        uint8_t _3 : 1 {};
        uint8_t _4 : 1 {};

        uint8_t o : 1 {}; // Sprite overflow
        uint8_t s : 1 {}; // Sprite 0 Hit
        uint8_t v : 1 {}; // Vertical blank has started (0: not in vblank; 1: in vblank)
    };
    static_assert(sizeof(StatusRegister) == 1, "The StatusRegister is not 1 byte");

    struct ScrollRegister {
        void write(uint8_t data) {
            if (latch) {
                y = data;
            } else {
                x = data;
            }
        }

        void resetLatch() {
            latch = false;
        }

        uint8_t x{};
        uint8_t y{};
        bool latch{};
    };

    struct AddressRegister {
        [[nodiscard]] uint16_t get() const {
            return (addr[0] << 8) | addr[1];
        }

        void set(uint16_t data) {
            addr[0] = (data >> 8) & 0x00FF;
            addr[1] = data & 0x00FF;
        }

        void update(uint8_t data) {
            if (hiPtr) {
                addr[0] = data & 0x00FF;
            } else {
                addr[1] = data & 0x00FF;
            }
            hiPtr = !hiPtr;

            set(get() & 0x3FFF);
        }

        void increment(uint8_t step) {
            uint8_t lo = addr[1];
            addr[1] += step;
            if (lo > addr[1]) {
                addr[0]++;
            }

            set(get() & 0x3FFF);
        }

        void resetLatch() {
            hiPtr = true;
        }

        std::array<uint8_t, 2> addr{}; // [hi, lo]
        bool hiPtr = false;
    };

    void incrementAddr();

    Bus& bus;

    std::array<uint8_t, 32> paletteTable{};

    ControlRegister control;
    MaskRegister mask;
    StatusRegister status;
    uint8_t oamAddr{};
    std::array<uint8_t, 256> oamData{};
    ScrollRegister scroll;
    AddressRegister address;

    uint8_t internalReadBuf{};

    uint16_t scanline = 0;
    uint32_t cycles = 0;
};

#endif
