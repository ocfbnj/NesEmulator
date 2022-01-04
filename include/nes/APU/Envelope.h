#ifndef OCFBNJ_NES_ENVELOP_H
#define OCFBNJ_NES_ENVELOP_H

#include <cstdint>

class Envelope {
public:
    std::uint8_t getVolume() const;

    void write(std::uint8_t value);
    void setStartFlag();

    void step();

private:
    union {
        struct {
            std::uint8_t v : 4; // Used as the volume in constant volume (C set) mode. Also used as the `reload` value for the envelope's divider (the divider becomes V + 1 quarter frames)
            std::uint8_t c : 1; // Constant volume flag (0: use volume from envelope; 1: use constant volume)
            std::uint8_t l : 1; // APU Length Counter halt flag/envelope loop flag
            std::uint8_t unused : 2;
        };

        std::uint8_t data = 0;
    };

    bool start = false;
    std::uint8_t decayLevel = 0;
    std::uint8_t counter = 0;
};

#endif // OCFBNJ_NES_ENVELOP_H
