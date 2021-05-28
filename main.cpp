#include "EmulatorWindow.h"

int main() {
    EmulatorWindow emulator;

    while (emulator.isOpen()) {
        emulator.handleEvent();
        emulator.draw();
        emulator.display();
    }

    return 0;
}
