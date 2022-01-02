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
            if (counter == 0) {
                counter = period;
                return true;
            } else if (counter > 0) {
                counter--;
            }

            return false;
        }

        std::uint16_t counter = 0;
        std::uint16_t period = 0;
    };

    class LengthCounter {
    public:
        bool isZero() const {
            return counter == 0;
        }

        bool greaterZero() const {
            return counter > 0;
        }

        void setCounter(std::uint8_t value) {
            counter = value;
        }

        void setHalt(bool b) {
            halt = b;
        }

        void step() {
            if (counter > 0 && !halt) {
                counter--;
            }
        }

    private:
        std::uint8_t counter = 0;
        bool halt = false;
    };

    class Envelope {
    public:
        std::uint8_t getVolume() const {
            return c ? v : decayLevel;
        }

        void write(std::uint8_t value) {
            data = value;
        }

        void setStartFlag() {
            start = true;
        }

        void step() {
            if (start) {
                start = false;
                decayLevel = 15;
                counter = v;
            } else {
                if (counter == 0) {
                    counter = v;
                    if (decayLevel == 0 && l) {
                        decayLevel = 15;
                    } else if (decayLevel > 0) {
                        decayLevel--;
                    }
                } else if (counter > 0) {
                    counter--;
                }
            }
        }

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

    class Sweep {
    public:
        std::uint16_t getPeriodChangeAmount(std::uint16_t rawTimerPeriod) const {
            std::uint16_t delta = rawTimerPeriod >> s;

            return n ? -delta : delta;
        }

        void write(std::uint8_t value) {
            data = value;
        }

        void setReloadFlag() {
            reload = true;
        }

        void step(std::uint16_t& timerPeriod, bool muting) {
            if (counter == 0 && e && !muting) {
                // The pulse's period is adjusted.
                sweep(timerPeriod);
            }

            if (counter == 0 || reload) {
                counter = p + 1;
                if (reload) {
                    reload = false;
                }
            } else if (counter > 0) {
                counter--;
            }
        }

    private:
        void sweep(std::uint16_t& timerPeriod) {
            std::uint16_t delta = timerPeriod >> s;
            if (n) {
                timerPeriod -= delta;
            } else {
                timerPeriod += delta;
            }
        }

        union {
            struct {
                std::uint8_t s : 3; // Shift count (number of bits)
                std::uint8_t n : 1; // Negate flag; 0: add to period, sweeping toward lower frequencies; 1: subtract from period, sweeping toward higher frequencies
                std::uint8_t p : 3; // The divider's period is P + 1 half-frames
                std::uint8_t e : 1; // Enabled flag
            };

            std::uint8_t data = 0;
        };

        bool reload = false;
        std::uint8_t counter = 0;
    };

    class Pulse {
    public:
        bool lengthGreaterThanZero() const {
            return lengthCounter.greaterZero();
        }

        std::uint8_t output() const {
            if (muting()) {
                return 0;
            }

            if (((DutyCycleSequences[dutyCycle] >> (7 - dutyValue)) & 1) == 0) {
                return 0;
            }

            if (lengthCounter.isZero()) {
                return 0;
            }

            return envelope.getVolume();
        }

        void resetLengthCounter() {
            lengthCounter.setCounter(0);
        }

        void stepTimer() {
            if (timer.step()) {
                dutyValue = (dutyValue + 1) % 8;
            }
        }

        void stepLengthCounter() {
            lengthCounter.step();
        }

        void stepEnvelope() {
            envelope.step();
        }

        void stepSweep() {
            sweep.step(timer.period, muting());
        }

        void writeControl(std::uint8_t data) {
            dutyCycle = (data >> 6) & 0b11;

            lengthCounter.setHalt((data >> 5) & 1);

            envelope.write(data);
            envelope.setStartFlag();
        }

        void writeSweep(std::uint8_t data) {
            sweep.write(data);
            sweep.setReloadFlag();
        }

        void writeTimerLo(std::uint8_t data) {
            timer.period = (timer.period & 0xFF00) | static_cast<std::uint16_t>(data);
        }

        void writeTimerHi(std::uint8_t data) {
            timer.period = (timer.period & 0x00FF) | ((static_cast<std::uint16_t>(data) & 0b111) << 8);
            lengthCounter.setCounter(LengthTable[data >> 3]);
            envelope.setStartFlag();

            dutyValue = 0;
        }

    private:
        bool muting() const {
            return timer.period < 8 || timer.period > 0x7FF;
        }

        std::uint8_t dutyCycle = 0;
        std::uint8_t dutyValue = 0;

        Timer timer;
        LengthCounter lengthCounter;
        Envelope envelope;
        Sweep sweep;
    };

    class Noise {
    public:
        bool lengthGreaterThanZero() const {
            return lengthCounter.greaterZero();
        }

        std::uint8_t output() const {
            // The mixer receives the current envelope volume except when
            // - Bit 0 of the shift register is set, or
            // - The length counter is zero
            if (shiftRegister & 1) {
                return 0;
            }

            if (lengthCounter.isZero()) {
                return 0;
            }

            return envelope.getVolume();
        }

        void resetLengthCounter() {
            lengthCounter.setCounter(0);
        }

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
            lengthCounter.step();
        }

        void stepEnvelope() {
            envelope.step();
        }

        void writeControl(std::uint8_t data) {
            lengthCounter.setHalt((data >> 5) & 1);

            envelope.write(data);
            envelope.setStartFlag();
        }

        void writePeriod(std::uint8_t data) {
            mode = (data >> 7) & 1;
            timer.period = NoiseTable[data & 0x0F];
        }

        void writeLengthCounter(std::uint8_t data) {
            lengthCounter.setCounter(LengthTable[data >> 3]);
            envelope.setStartFlag();
        }

    private:
        std::uint8_t mode = 0;
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
    Pulse pulse1;
    Pulse pulse2;
    Noise noise;

    int sampleRate = SampleRate;
    SampleCallback sampleCallback = &APU::defaultSampleCallback;
};

#endif
