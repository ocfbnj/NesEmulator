#ifndef PPU_H
#define PPU_H

#include <array>
#include <cstdint>
#include <functional>
#include <span>
#include <vector>

class Bus;

class PPU {
public:
    struct Pixel {
        std::uint8_t r;
        std::uint8_t g;
        std::uint8_t b;
        std::uint8_t a;
    };

    class Frame {
        static constexpr auto Width = 256;
        static constexpr auto Height = 240;

    public:
        [[nodiscard]] Pixel getPixel(int x, int y) const {
            assert(x >= 0 && x < Width);
            assert(y >= 0 && y < Height);

            y = Height - y - 1;
            return pixels[y * Width + x];
        }

        void setPixel(int x, int y, Pixel pixel) {
            assert(x >= 0 && x < Width);
            assert(y >= 0 && y < Height);

            y = Height - y - 1;
            pixels[y * Width + x] = pixel;
        }

    private:
        std::array<Pixel, Width * Height> pixels{};
    };

    explicit PPU(Bus& bus);

    void clock();

    [[nodiscard]] uint8_t readStatus();
    [[nodiscard]] uint8_t readOAMData() const;
    [[nodiscard]] uint8_t readData();
    [[nodiscard]] uint8_t readPalette(uint16_t addr) const;

    [[nodiscard]] bool showBackground() const;
    [[nodiscard]] bool showSprites() const;
    [[nodiscard]] bool renderingEnabled() const;

    [[nodiscard]] const Frame& getFrame() const;
    [[nodiscard]] bool isFrameComplete() const;

    [[nodiscard]] Pixel getColor(uint8_t palette, uint8_t pixel) const;

    void writeCtrl(uint8_t data);
    void writeMask(uint8_t data);
    void writeOAMAddr(uint8_t data);
    void writeOAMData(uint8_t data);
    void writeScroll(uint8_t data);
    void writeAddr(uint8_t data);
    void writeData(uint8_t data);
    void writeOAMDMA(const std::array<uint8_t, 256>& buffer);
    void writePalette(uint16_t addr, uint8_t data);

private:
    void incrementAddr();

    void incrementHorizontal();
    void incrementVertical();
    void transferHorizontalBits();
    void transferVerticalBits();

    void loadShifters();
    void updateShifters();

    void visibleFrameAndPreRender();
    void verticalBlanking();
    void renderFrame();
    void incrementCycle();

    void secondaryOamClearAndSpriteEvaluation();
    void calculateSpritesPatternAddr();

    // TODO After power/reset, writes to this register are ignored for about 30,000 cycles
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

        [[nodiscard]] uint8_t spriteHeight() const {
            return h ? 16 : 8;
        }

        [[nodiscard]] bool generateNMI() const {
            return v;
        }

        void write(uint8_t data) {
            reg = data;
        }

        union {
            struct {
                uint8_t n : 2; // Base nametable address (0 = $2000; 1 = $2400; 2 = $2800; 3 = $2C00)
                uint8_t i : 1; // VRAM address increment per CPU readData/writeData of PPUDATA (0: add 1, going across; 1: add 32, going down)
                uint8_t s : 1; // Sprite pattern table address for 8x8 sprites (0: $0000; 1: $1000; ignored in 8x16 mode)
                uint8_t b : 1; // Background pattern table address (0: $0000; 1: $1000)
                uint8_t h : 1; // Sprite size (0: 8x8 pixels; 1: 8x16 pixels)
                uint8_t p : 1; // TODO PPU master/slave select (0: read backdrop from EXT pins; 1: output color on EXT pins)
                uint8_t v : 1; // Generate an NMI at the start of the vertical blanking interval (0: off; 1: on)
            };

            uint8_t reg;
        };
    };
    static_assert(sizeof(ControlRegister) == 1, "The ControlRegister is not 1 byte");

    struct MaskRegister {
        [[nodiscard]] bool isGreyscale() const {
            return g;
        }

        [[nodiscard]] bool showBackgroundLeft() const {
            return m;
        }

        [[nodiscard]] bool showSpritesLeft() const {
            return M;
        }

        [[nodiscard]] bool showBackground() const {
            return b;
        }

        [[nodiscard]] bool showSprites() const {
            return s;
        }

        void write(uint8_t data) {
            reg = data;
        }

        union {
            struct {
                uint8_t g : 1; // Greyscale (0: normal color, 1: produce a greyscale display)
                uint8_t m : 1; // 1: Show background in leftmost 8 pixels of screen, 0: Hide
                uint8_t M : 1; // 1: Show sprites in leftmost 8 pixels of screen, 0: Hide
                uint8_t b : 1; // 1: Show background
                uint8_t s : 1; // 1: Show sprites
                uint8_t R : 1; // TODO Emphasize red (green on PAL/Dendy)
                uint8_t G : 1; // TODO Emphasize green (red on PAL/Dendy)
                uint8_t B : 1; // TODO Emphasize blue
            };

            uint8_t reg;
        };
    };
    static_assert(sizeof(MaskRegister) == 1, "The MaskRegister is not 1 byte");

    struct StatusRegister {
        [[nodiscard]] uint8_t read() const {
            return reg;
        }

        [[nodiscard]] bool isInVblank() const {
            return v;
        }

        void setSpriteOverflow() {
            o = 1;
        }

        void resetSpriteOverflow() {
            o = 0;
        }

        void setSprite0Hit() {
            s = 1;
        }

        void resetSprite0Hit() {
            s = 0;
        }

        void setVblank() {
            v = 1;
        }

        void resetVblank() {
            v = 0;
        }

        union {
            struct {
                uint8_t unused : 5; // TODO Least significant bits previously written into a PPU register
                uint8_t o : 1;      // Sprite overflow
                uint8_t s : 1;      // Sprite 0 Hit
                uint8_t v : 1;      // Vertical blank has started (0: not in vblank; 1: in vblank)
            };

            uint8_t reg;
        };
    };
    static_assert(sizeof(StatusRegister) == 1, "The StatusRegister is not 1 byte");

    struct LoopyRegister {
        union {
            struct {
                uint16_t coarseX : 5;
                uint16_t coarseY : 5;
                uint16_t nametableX : 1;
                uint16_t nametableY : 1;
                uint16_t fineY : 3;
                [[maybe_unused]] uint16_t unused : 1;
            };

            uint16_t reg;
        };
    };

    static std::array<PPU::Pixel, 64> DefaultPalette;

    Bus& bus;

    std::array<uint8_t, 32> paletteTable{};

    ControlRegister control{};
    MaskRegister mask{};
    StatusRegister status{};
    uint8_t oamAddr{};
    std::array<uint8_t, 256> primaryOamData{};
    uint8_t internalReadBuf{};

    // PPU internal registers
    // See https://wiki.nesdev.org/w/index.php?title=PPU_scrolling#PPU_internal_registers
    LoopyRegister vramAddr{}; // current VRAM address (15 bits)
    LoopyRegister tramAddr{}; // temporary VRAM address (15 bits)
    uint8_t fineX{};          // fine X scroll (3 bits)
    uint8_t latch{};          // first or second write toggle

    // background rendering
    uint8_t bgNtByte{};     // name table
    uint8_t bgAtByte{};     // attribute table
    uint8_t bgTileByteLo{}; // pattern low byte
    uint8_t bgTileByteHi{}; // pattern high byte
    uint16_t bgPatternShifterLo{};
    uint16_t bgPatternShifterHi{};
    uint16_t bgAttributeShifterLo{};
    uint16_t bgAttributeShifterHi{};

    // foreground rendering
    std::array<uint8_t, 32> secondaryOamData{};
    std::array<uint8_t, 8> spritePatternShifterLo{};
    std::array<uint8_t, 8> spritePatternShifterHi{};
    uint8_t spriteCount = 0;
    bool sprite0HitPossible = false;

    int16_t scanline = 0;
    int16_t cycle = 0;

    bool frameComplete = false;
    Frame frame;
};

#endif
