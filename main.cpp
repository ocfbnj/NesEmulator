#include <cassert>

#include "Bus.h"
#include "Cpu.h"
#include "NesFile.h"

int main() {
    auto cartridge = loadNesFile(R"(C:\Users\13911\Downloads\nestest.nes)");
    assert(cartridge != nullptr);

    auto mapper = Mapper::create(std::move(cartridge));
    assert(mapper != nullptr);

    Bus bus{std::move(mapper)};
    Cpu cpu{bus};

    // debug
    while (true) {
        cpu.clock();
    }
}
