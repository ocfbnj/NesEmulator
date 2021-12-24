#include <iostream>

#include "Emulator.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <nes file>\n";
        return -1;
    }

    Emulator emulator{argv[1]};
    emulator.run();
}
