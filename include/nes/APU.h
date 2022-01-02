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

    std::uint8_t readStatus() const;

    void writeStatus(std::uint8_t data);
    void writeFrameCounter(std::uint8_t data);

    void stepTimer();
    void stepLengthCounter();
    void stepEnvelope();
    void stepSweep();
    void stepFrameCounter();

    void sendSample();

    double getOutputSample() const;

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

        std::uint8_t value;
    };

    struct Envelope {
        void step() {
            if (start) {
                start = false;
                volume = 15;
                value = divider;
            } else if (value > 0) {
                value--;
            } else {
                value = divider;

                if (volume > 0) {
                    volume--;
                } else {
                    if (loop) {
                        volume = 15;
                    }
                }
            }
        }

        bool start;
        std::uint8_t divider;
        std::uint8_t volume;
        std::uint8_t value;
        bool loop;
    };

    struct Sweep {
        union {
            struct {
                std::uint8_t s : 3; // Shift count (number of bits)
                std::uint8_t n : 1; // Negate flag; 0: add to period, sweeping toward lower frequencies; 1: subtract from period, sweeping toward higher frequencies
                std::uint8_t p : 3; // The divider's period is P + 1 half-frames
                std::uint8_t e : 1; // Enabled flag
            };

            std::uint8_t reg;
        };

        bool reload;
        std::uint8_t value;
    };

    struct Pulse {
        void stepTimer() {
            if (timer.step()) {
                dutyValue = (dutyValue + 1) % 8;
            }
        }

        void stepLengthCounter() {
            if (!l) {
                lengthCounter.step();
            }
        }

        void stepEnvelope() {
            envelope.step();
        }

        void stepSweep() {
            if (sweep.reload) {
                sweep.reload = false;

                if (sweep.e && sweep.value == 0) {
                    performSweep();
                }

                sweep.value = sweep.p;
            } else if (sweep.value > 0) {
                sweep.value--;
            } else {
                sweep.value = sweep.p;

                if (sweep.e) {
                    performSweep();
                }
            }
        }

        void performSweep() {
            std::uint16_t delta = timer.reloadValue >> sweep.s;

            if (sweep.n) {
                timer.reloadValue -= delta;
            } else {
                timer.reloadValue += delta;
            }
        }

        std::uint8_t output() const {
            if (lengthCounter.value == 0) {
                return 0;
            }

            std::uint8_t volume = c ? v : envelope.volume;
            return ((DutyCycleSequences[d] >> (7 - dutyValue)) & 1) ? volume : 0;
        }

        void writeControl(std::uint8_t data) {
            reg = data;

            envelope.divider = v;
            envelope.loop = l;
            envelope.start = true;
        }

        void writeSweep(std::uint8_t data) {
            sweep.reg = data;
        }

        void writeTimerLo(std::uint8_t data) {
            timer.reloadValue = (timer.reloadValue & 0xFF00) | static_cast<std::uint16_t>(data);
        }

        void writeTimerHi(std::uint8_t data) {
            timer.reloadValue = (timer.reloadValue & 0x00FF) | ((static_cast<std::uint16_t>(data) & 0b111) << 8);
            lengthCounter.value = LengthTable[data >> 3];
            envelope.start = true;

            dutyValue = 0;
        }

        union {
            struct {
                std::uint8_t v : 4; // Used as the volume in constant volume (C set) mode. Also used as the `reload` value for the envelope's divider (the divider becomes V + 1 quarter frames).
                std::uint8_t c : 1; // Constant volume flag (0: use volume from envelope; 1: use constant volume)
                std::uint8_t l : 1; // APU Length Counter halt flag/envelope loop flag
                std::uint8_t d : 2; // Duty cycle
            };

            std::uint8_t reg;
        };

        std::uint8_t dutyValue = 0;
        Timer timer;
        LengthCounter lengthCounter;
        Envelope envelope;
        Sweep sweep;
    };

    struct Noise {
        void stepTimer() {
            if (timer.step()) {
                // When the timer clocks the shift register, the following actions occur in order:
                // 1. Feedback is calculated as the exclusive-OR of bit 0 and one other bit: bit 6 if Mode flag is set, otherwise bit 1.
                // 2. The shift register is shifted right by one bit.
                // 3. Bit 14, the leftmost bit, is set to the feedback calculated earlier.
                std::uint8_t shift = mode ? 6 : 1;

                std::uint16_t b1 = shiftRegister & 1;
                std::uint16_t b2 = (shiftRegister >> shift) & 1;

                shiftRegister >>= 1;
                shiftRegister |= (b1 ^ b2) << 14;
            }
        }

        void stepLengthCounter() {
            if (!l) {
                lengthCounter.step();
            }
        }

        void stepEnvelope() {
            envelope.step();
        }

        std::uint8_t output() const {
            // The mixer receives the current envelope volume except when
            // - Bit 0 of the shift register is set, or
            // - The length counter is zero
            if (shiftRegister & 1) {
                return 0;
            }

            if (lengthCounter.value == 0) {
                return 0;
            }

            std::uint8_t volume = c ? v : envelope.volume;
            return volume;
        }

        void writeControl(std::uint8_t data) {
            reg = data;

            envelope.divider = v;
            envelope.loop = l;
            envelope.start = true;
        }

        void writePeriod(std::uint8_t data) {
            mode = (data >> 7) & 1;
            timer.reloadValue = NoiseTable[data & 0x0F];
        }

        void writeLengthCounter(std::uint8_t data) {
            lengthCounter.value = LengthTable[data >> 3];
            envelope.start = true;
        }

        union {
            struct {
                std::uint8_t v : 4; // Used as the volume in constant volume (C set) mode. Also used as the `reload` value for the envelope's divider (the divider becomes V + 1 quarter frames).
                std::uint8_t c : 1; // Constant volume flag (0: use volume from envelope; 1: use constant volume)
                std::uint8_t l : 1; // APU Length Counter halt flag/envelope loop flag
                std::uint8_t unused : 2;
            };

            std::uint8_t reg;
        };

        std::uint8_t mode;
        std::uint16_t shiftRegister = 0x7F;
        Timer timer;
        LengthCounter lengthCounter;
        Envelope envelope;
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
    Noise noise{.reg = 0};

    int sampleRate = SampleRate;
    SampleCallback sampleCallback = &APU::defaultSampleCallback;
};

#endif
