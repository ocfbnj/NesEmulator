#ifndef EMULATOR_H
#define EMULATOR_H

#include <string_view>

#include "NesEmulator/nes/Bus.h"
#include "NesEmulator/pixel_engine/PixelEngine.h"

class Emulator : public PixelEngine {
public:
    explicit Emulator(std::string_view nesFile);

    void onUpdate(float elapsedTime) override;

private:
    void render();
    void renderBackground();
    void renderTile(uint16_t bank, uint8_t n, int tileX, int tileY);

    Bus bus;
};

#endif
