#ifndef OCFBNJ_NES_APU_H
#define OCFBNJ_NES_APU_H

#include <cstdint>
#include <functional>
#include <istream>
#include <ostream>

#include <nes/literals.h>

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

    std::uint8_t readStatus();

    void writeStatus(std::uint8_t data);
    void writeFrameCounter(std::uint8_t data);

    void stepTimer();
    void stepLengthCounter();
    void stepEnvelope();
    void stepSweep();
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

    struct LengthCounter {
        void step() {
            if (value > 0) {
                value--;
            }
        }

        bool isZero() const {
            return value == 0;
        }

        std::uint8_t value;
    };

    struct Pulse {
        void stepTimer() {
            if (timer.step()) {
                dutyValue = (dutyValue + 1) % 8;
            }
        }

        void stepLengthCounter() {
            if (!lengthCounterHalt) {
                lengthCounter.step();
            }
        }

        std::uint8_t output() const {
            if (lengthCounter.isZero()) {
                return 0;
            }

            return ((DutyCycleSequences[dutyCycle] >> (7 - dutyValue)) & 1) ? period : 0;
        }

        void writeControl(std::uint8_t data) {
            reg = data;
        }

        void writeSweep(std::uint8_t data) {
        }

        void writeTimerLo(std::uint8_t data) {
            timer.reloadValue = (timer.reloadValue & 0xFF00) | static_cast<std::uint16_t>(data);
        }

        void writeTimerHi(std::uint8_t data) {
            timer.reloadValue = (timer.reloadValue & 0x00FF) | ((static_cast<std::uint16_t>(data) & 0b111) << 8);
            lengthCounter.value = LengthTable[data >> 3];

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

        std::uint8_t dutyValue = 0;
        Timer timer;
        LengthCounter lengthCounter;
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

    std::uint16_t frameCounter = 0;
    std::uint8_t frameCounterMode = 4;
    bool irqInhibit = false;

    StatusRegister status{.reg = 0};
    Pulse pulse1{.reg = 0};
    Pulse pulse2{.reg = 0};

    int sampleRate = 44100;
    SampleCallback sampleCallback = &APU::defaultSampleCallback;
};

#endif
