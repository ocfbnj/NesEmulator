#ifndef OCFBNJ_NES_STATUS_REGISTER_H
#define OCFBNJ_NES_STATUS_REGISTER_H

#include <cstdint>

class StatusRegister {
public:
    bool pulse1Enabled() const;
    bool pulse2Enabled() const;
    bool triangleEnabled() const;
    bool noiseEnabled() const;
    bool dmcEnabled() const;

    void write(std::uint8_t data);

private:
    union {
        struct {
            std::uint8_t p1 : 1;
            std::uint8_t p2 : 1;
            std::uint8_t t : 1;
            std::uint8_t n : 1;
            std::uint8_t d : 1;
            std::uint8_t unused : 3;
        };

        std::uint8_t reg = 0;
    };
};

#endif // OCFBNJ_NES_STATUS_REGISTER_H
