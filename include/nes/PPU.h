#ifndef OCFBNJ_NES_PPU_H
#define OCFBNJ_NES_PPU_H

#include <array>
#include <cassert>
#include <cstdint>
#include <functional>
#include <istream>
#include <ostream>
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
    public:
        std::span<const std::uint8_t> getRawPixels() const {
            return std::span{reinterpret_cast<const std::uint8_t*>(pixels.data()), Width * Height * sizeof(Pixel)};
        }

        Pixel getPixel(int x, int y) const {
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
        static constexpr auto Width = 256;
        static constexpr auto Height = 240;

        std::array<Pixel, Width * Height> pixels{};
    };

    PPU() = default;
    PPU(const PPU&) = delete;
    PPU& operator=(const PPU&) = delete;
    PPU(PPU&&) = default;
    PPU& operator=(PPU&&) = default;

    void connect(Bus* bus);

    void clock();
    void reset();

    std::uint8_t readStatus();
    std::uint8_t readOamData() const;
    std::uint8_t readData();
    std::uint8_t readPalette(std::uint16_t addr) const;

    void writeCtrl(std::uint8_t data);
    void writeMask(std::uint8_t data);
    void writeOamAddr(std::uint8_t data);
    void writeOamData(std::uint8_t data);
    void writeScroll(std::uint8_t data);
    void writeAddr(std::uint8_t data);
    void writeData(std::uint8_t data);
    void writeOamDMA(std::span<std::uint8_t, 256> buffer);
    void writePalette(std::uint16_t addr, std::uint8_t data);

    void serialize(std::ostream& os) const;
    void deserialize(std::istream& is);

    const Frame& getFrame() const;
    bool isFrameComplete() const;

    Pixel getColor(std::uint8_t palette, std::uint8_t pixel);

private:
    std::uint8_t read(std::uint16_t addr);
    void write(std::uint16_t addr, std::uint8_t data);

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

    void processMapper();

    struct ControlRegister {
        std::uint16_t baseNameTableAddr() const {
            return 0x2000 + 0x400 * n;
        }

        std::uint8_t addrIncrement() const {
            return i ? 32 : 1;
        }

        std::uint16_t spritePatternAddr() const {
            return s ? 0x1000 : 0x0000;
        }

        std::uint16_t backgroundPatternAddr() const {
            return b ? 0x1000 : 0x0000;
        }

        std::uint8_t spriteHeight() const {
            return h ? 16 : 8;
        }

        bool generateNMI() const {
            return v;
        }

        void write(std::uint8_t data) {
            reg = data;
        }

        union {
            struct {
                std::uint8_t n : 2; // Base nametable address (0 = $2000; 1 = $2400; 2 = $2800; 3 = $2C00)
                std::uint8_t i : 1; // VRAM address increment per CPU readData/writeData of PPUDATA (0: add 1, going across; 1: add 32, going down)
                std::uint8_t s : 1; // Sprite pattern table address for 8x8 sprites (0: $0000; 1: $1000; ignored in 8x16 mode)
                std::uint8_t b : 1; // Background pattern table address (0: $0000; 1: $1000)
                std::uint8_t h : 1; // Sprite size (0: 8x8 pixels; 1: 8x16 pixels)
                std::uint8_t p : 1; // TODO PPU master/slave select (0: read backdrop from EXT pins; 1: output color on EXT pins)
                std::uint8_t v : 1; // Generate an NMI at the start of the vertical blanking interval (0: off; 1: on)
            };

            std::uint8_t reg;
        };
    };

    struct MaskRegister {
        bool isGreyscale() const {
            return g;
        }

        bool showBackgroundLeft() const {
            return m;
        }

        bool showSpritesLeft() const {
            return M;
        }

        bool showBackground() const {
            return b;
        }

        bool showSprites() const {
            return s;
        }

        bool renderingEnabled() const {
            return showBackground() || showSprites();
        }

        void write(std::uint8_t data) {
            reg = data;
        }

        union {
            struct {
                std::uint8_t g : 1; // Greyscale (0: normal color, 1: produce a greyscale display)
                std::uint8_t m : 1; // 1: Show background in leftmost 8 pixels of screen, 0: Hide
                std::uint8_t M : 1; // 1: Show sprites in leftmost 8 pixels of screen, 0: Hide
                std::uint8_t b : 1; // 1: Show background
                std::uint8_t s : 1; // 1: Show sprites
                std::uint8_t R : 1; // TODO Emphasize red (green on PAL/Dendy)
                std::uint8_t G : 1; // TODO Emphasize green (red on PAL/Dendy)
                std::uint8_t B : 1; // TODO Emphasize blue
            };

            std::uint8_t reg;
        };
    };

    struct StatusRegister {
        std::uint8_t read() const {
            return reg;
        }

        bool isInVblank() const {
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
                std::uint8_t unused : 5; // TODO Least significant bits previously written into a PPU register
                std::uint8_t o : 1;      // Sprite overflow
                std::uint8_t s : 1;      // Sprite 0 Hit
                std::uint8_t v : 1;      // Vertical blank has started (0: not in vblank; 1: in vblank)
            };

            std::uint8_t reg;
        };
    };

    struct LoopyRegister {
        union {
            struct {
                std::uint16_t coarseX : 5;
                std::uint16_t coarseY : 5;
                std::uint16_t nametableX : 1;
                std::uint16_t nametableY : 1;
                std::uint16_t fineY : 3;
                std::uint16_t unused : 1;
            };

            std::uint16_t reg;
        };
    };

    // Initially the maximum value is 8, doubled to eliminate flicker.
    constexpr static auto MaximumSpriteCount = 8 * 2;

    static std::array<PPU::Pixel, 64> defaultPalette;

    // The following member variables are PPU components, they can be used to serialize and deserialize.

    // PPU Components Begin
    ControlRegister control{.reg = 0};
    MaskRegister mask{.reg = 0};
    StatusRegister status{.reg = 0};
    std::uint8_t oamAddr = 0;
    std::uint8_t internalReadBuf = 0;

    std::array<std::uint8_t, 32> paletteTable{};
    std::array<std::uint8_t, 256> primaryOamData{};

    // PPU internal registers
    // See https://wiki.nesdev.org/w/index.php?title=PPU_scrolling#PPU_internal_registers
    LoopyRegister vramAddr{.reg = 0}; // current VRAM address (15 bits)
    LoopyRegister tramAddr{.reg = 0}; // temporary VRAM address (15 bits)
    std::uint8_t fineX = 0;           // fine X scroll (3 bits)
    std::uint8_t latch = 0;           // first or second write toggle

    // for frame rendering
    // See https://wiki.nesdev.org/w/images/4/4f/Ppu.svg
    std::int16_t scanline = -1;
    std::int16_t cycle = 0;

    // for background rendering
    std::uint8_t bgNtByte = 0;     // name table
    std::uint8_t bgAtByte = 0;     // attribute table
    std::uint8_t bgTileByteLo = 0; // pattern low byte
    std::uint8_t bgTileByteHi = 0; // pattern high byte
    std::uint16_t bgPatternShifterLo = 0;
    std::uint16_t bgPatternShifterHi = 0;
    std::uint16_t bgAttributeShifterLo = 0;
    std::uint16_t bgAttributeShifterHi = 0;

    // for foreground rendering
    std::array<std::uint8_t, MaximumSpriteCount * 4> secondaryOamData{};
    std::array<std::uint8_t, MaximumSpriteCount> spritePatternShifterLo{};
    std::array<std::uint8_t, MaximumSpriteCount> spritePatternShifterHi{};
    std::uint8_t spriteCount = 0;
    bool sprite0HitPossible = false;
    // PPU Components End

    Bus* bus = nullptr;

    bool frameComplete = false;
    Frame frame;
};

#endif // OCFBNJ_NES_PPU_H
