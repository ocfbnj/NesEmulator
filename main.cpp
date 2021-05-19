#include <cassert>
#include <iostream>

#include "Mapper.h"
#include "NesFile.h"
#include "Bus.h"

int main() {
    auto cartridge = loadNesFile(R"(C:\Users\ocfbnj\Downloads\Super Mario Bros (E).nes)");
    assert(cartridge != nullptr);

    auto mapper = Mapper::create(std::move(cartridge));
    assert(mapper != nullptr);

    Bus bus{std::move(mapper)};

    std::cout << std::hex << +bus.read(0x8000) << "\n";
    std::cout << std::hex << +bus.read(0x8001) << "\n";
    std::cout << std::hex << +bus.read16(0x8000) << "\n";

    bus.write16(0x9000, 0x1234);
    assert(bus.read16(0x9000) == 0x1234);
}
