#include <iostream>

#include "NesEmulator/nes/NesFile.h"
#include "NesEmulator/pixel_engine/PixelEngine.h"

class ShowTiles : public PixelEngine {
public:
    explicit ShowTiles(std::string_view nesFile) : PixelEngine(256, 240, "Show Tiles") {
        auto cartridge = loadNesFile(nesFile);
        chrRom = std::move(cartridge->chrRom);
    }

    void showTile(int n) {
        auto begin = std::next(chrRom.begin(), n * 16);
        auto end = std::next(begin, 16);
        std::vector<uint8_t> tile{begin, end};

        for (int y = 0; y != 8; y++) {
            uint8_t hi = tile[y];
            uint8_t lo = tile[y + 8];

            for (int x = 0; x != 8; x++) {
                uint8_t index = (((hi >> (7 - x)) & 1) << 1) | ((lo >> (7 - x)) & 1);
                Pixel pixel{.r = 255, .g = 255, .b = 255, .a = 255};

                switch (index) {
                case 0:
                    pixel = SystemPalette[0x01];
                    break;
                case 1:
                    pixel = SystemPalette[0x23];
                    break;
                case 2:
                    pixel = SystemPalette[0x27];
                    break;
                case 3:
                    pixel = SystemPalette[0x30];
                    break;
                default:
                    break;
                }

                int showX = x + 10 * (n % 25);
                int showY = y + 10 * (n / 25);
                drawPixel(showX, showY, pixel);
            }
        }
    }

    void onUpdate() override {
        for (int i = 0; i != 512; i++) {
            showTile(i);
        }
    }

private:
    std::vector<uint8_t> chrRom;
};

int main(int argc, char* argv[]) {
    std::string nesFile;

    if (argc == 2) {
        nesFile = argv[1];
    } else {
        std::cout << "Please input the NES file path: ";
        std::getline(std::cin, nesFile);
    }

    ShowTiles showTiles{nesFile};
    showTiles.run();
}
