#include "Emulator.h"

int main() {
    Emulator emulator{"test/nestest.nes"};
    emulator.run();
}
