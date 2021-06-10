#include "Emulator.h"

int main() {
    Emulator emulator{"nestest.nes"};
    emulator.run();
}
