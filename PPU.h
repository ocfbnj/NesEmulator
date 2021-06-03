#ifndef PPU_H
#define PPU_H

#include <array>
#include <cstdint>

class Bus;

class PPU {
public:
    explicit PPU(Bus& bus);

    uint8_t read();

    void writeAddr(uint8_t data);
    void writeCtrl(uint8_t data);

    void incrementAddr();

private:
    struct AddressRegister {
        uint16_t get() const {
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

        std::array<uint8_t, 2> addr;
        bool hiPtr = false;
    };

    struct ControlRegister {
        uint8_t addrIncrement() {
            return i ? 32 : 1;
        }

        void update(uint8_t data) {
            n = data & (0b0000'0011);
            i = (data >> 2) & 1;
            s = (data >> 3) & 1;
            b = (data >> 4) & 1;
            h = (data >> 5) & 1;
            p = (data >> 6) & 1;
            v = (data >> 7) & 1;
        }

        uint8_t n : 2; // Base nametable address
        uint8_t i : 1; // VRAM address increment per CPU read/write of PPUDATA
        uint8_t s : 1; // Sprite pattern table address for 8x8 sprites
        uint8_t b : 1; // Background pattern table address
        uint8_t h : 1; // Sprite size
        uint8_t p : 1; // PPU master/slave select
        uint8_t v : 1; // Generate an NMI at the start of the vertical blanking interval
    };
    static_assert(sizeof(ControlRegister) == 1, "The ControlRegister is not 1 bytes");

    Bus& bus;

    std::array<uint8_t, 32> palette;
    AddressRegister address;
    ControlRegister control;

    uint8_t internalDataBuf;
};

#endif