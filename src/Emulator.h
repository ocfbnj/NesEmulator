#ifndef EMULATOR_H
#define EMULATOR_H

#include <sstream>
#include <string>
#include <string_view>

#include "NesEmulator/nes/Bus.h"
#include "NesEmulator/pixel_engine/PixelEngine.h"

class Emulator : public PixelEngine {
public:
    explicit Emulator(std::string_view nesFile);

    void onBegin() override;
    void onUpdate() override;

private:
    void renderFrame(const PPU::Frame& frame);
    void checkKeyboard();
    void checkReset();
    void checkSerialization();

    Bus nes;
    std::string nesFile;
};

#endif
