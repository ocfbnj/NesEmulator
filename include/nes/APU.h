#ifndef OCFBNJ_NES_APU_H
#define OCFBNJ_NES_APU_H

#include <cstdint>
#include <functional>
#include <istream>
#include <ostream>

#include <nes/APU/DMC.h>
#include <nes/APU/Noise.h>
#include <nes/APU/Pulse.h>
#include <nes/APU/StatusRegister.h>
#include <nes/APU/Triangle.h>

class Bus;

class APU {
public:
    using SampleCallback = std::function<void(double sample)>;

    void connect(Bus* bus);

    void clock();
    void reset();

    std::uint8_t apuRead(std::uint16_t addr);
    void apuWrite(std::uint16_t addr, std::uint8_t data);

    void setSampleRate(int rate);
    void setSampleCallback(SampleCallback callback);

    void serialize(std::ostream& os) const;
    void deserialize(std::istream& is);

private:
    std::uint8_t readStatus() const;

    void writeStatus(std::uint8_t data);
    void writeFrameCounter(std::uint8_t data);

    void stepTimer();
    void stepLengthCounter();
    void stepEnvelopeAndLinearCounter();
    void stepSweep();
    void stepFrameCounter();

    void sendSample();

    double getOutputSample() const;

    // APU Components Begin
    std::uint16_t frameCounter = 0;
    std::uint8_t frameCounterMode = 4;
    bool irqInhibit = false;

    Pulse pulse1;
    Pulse pulse2;
    Triangle triangle;
    Noise noise;
    DMC dmc;

    StatusRegister status;

    // With 64-bit integer and 44'100 sample rate, it can be stable for about 27 hours.
    std::uint64_t i = 0;
    // APU Components End

    Bus* bus = nullptr;

    int sampleRate = 44100;
    SampleCallback sampleCallback;
};

#endif
