#ifndef EMULATOR_H
#define EMULATOR_H

#include <sstream>
#include <string>
#include <string_view>

#include "NesEmulator/nes/Bus.h"

class Emulator {
public:
    explicit Emulator(std::string_view nesFile);

    void run();

private:
    Bus nes;
};

#endif
