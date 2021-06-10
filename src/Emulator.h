#ifndef EMULATOR_H
#define EMULATOR_H

#include <string_view>

#include "NesEmulator/nes/Bus.h"

#include "EmulatorWindow.h"

class Emulator {
public:
    explicit Emulator(std::string_view nesFile);
    void run();

private:
    EmulatorWindow window;
    Bus bus;
};

#endif
