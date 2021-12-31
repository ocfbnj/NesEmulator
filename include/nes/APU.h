#ifndef OCFBNJ_NES_APU_H
#define OCFBNJ_NES_APU_H

#include <cstdint>
#include <functional>
#include <istream>
#include <ostream>

class APU {
public:
    using SampleCallback = std::function<void(double sample)>;

    void clock();
    void reset();

    std::uint8_t apuRead(std::uint16_t addr);
    void apuWrite(std::uint16_t addr, std::uint8_t data);

    void setSampleRate(int rate);
    void setSampleCallback(SampleCallback callback);

    void serialize(std::ostream& os) const;
    void deserialize(std::istream& is);

private:
    static void defaultSampleCallback(double) {}

    void stepTimer();
    void stepFrameCounter();
    void sendSample();

    double getOutputSample();

    struct Timer {
        bool step() {
            if (--value == 0) {
                value = reloadValue;
                return true;
            }

            return false;
        }

        std::uint16_t value;
        std::uint16_t reloadValue;
    };

    struct Pulse {
        void stepTimer() {
            if (timer.step()) {
                dutyValue++;
            }
        }

        std::uint8_t output() const {
            constexpr std::array<std::uint8_t, 4> DutyCycleSequences{
                0b0100'0000, // 12.5%
                0b0110'0000, // 25%
                0b0111'1000, // 50%
                0b1001'1111, // 25% negated
            };

            return ((DutyCycleSequences[dutyCycle] >> (7 - dutyValue)) & 1) ? (reg & period) : 0;
        }

        void writeControl(std::uint8_t data) {
            reg = data;
        }

        void writeTimerLo(std::uint8_t data) {
            timer.reloadValue = (timer.reloadValue & 0xFF00) | static_cast<std::uint16_t>(data);
        }

        void writeTimerHi(std::uint8_t data) {
            timer.reloadValue = (timer.reloadValue & 0x00FF) | ((static_cast<std::uint16_t>(data) & 0b111) << 8);

            dutyValue = 0;
        }

        union {
            struct {
                std::uint8_t period : 4;
                std::uint8_t envelopeEnabled : 1;
                std::uint8_t lengthCounterHalt : 1;
                std::uint8_t dutyCycle : 2;
            };

            std::uint8_t reg;
        };

        Timer timer;
        std::uint8_t dutyValue = 0;
    };

    struct StatusRegister {
        bool pulse1Enabled() const {
            return p1;
        }

        bool pulse2Enabled() const {
            return p2;
        }

        bool triangleEnabled() const {
            return t;
        }

        bool noiseEnabled() const {
            return n;
        }

        bool dmcEnabled() const {
            return d;
        }

        union {
            struct {
                std::uint8_t p1 : 1;
                std::uint8_t p2 : 1;
                std::uint8_t t : 1;
                std::uint8_t n : 1;
                std::uint8_t d : 1;
                std::uint8_t unused : 3;
            };

            std::uint8_t reg;
        };
    };

    std::uint16_t frameCounter;
    std::uint8_t frameCounterMode;

    StatusRegister status{.reg = 0};
    Pulse pulse1{.reg = 0};
    Pulse pulse2{.reg = 0};

    int sampleRate = 44100;
    SampleCallback sampleCallback = &APU::defaultSampleCallback;
};

#endif
