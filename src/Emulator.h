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
    struct Rect {
        int x1; // left
        int y1; // top
        int x2; // right
        int y2; // bottom

        [[nodiscard]] bool isInside(int x, int y) const {
            return x >= x1 && x < x2 && y >= y1 && y < y2;
        }
    };

    void render();
    void renderBackground();
    void renderSprites();

    void renderNametable(int n,
                         Rect viewport = {.x1 = 0, .y1 = 0, .x2 = 256, .y2 = 240},
                         int shiftX = 0,
                         int shiftY = 0);

    void checkKeyboard();

    Bus bus;

    float freeTime = 0.0f;
};

#endif
