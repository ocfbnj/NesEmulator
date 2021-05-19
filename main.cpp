#include <cassert>
#include <iostream>

#include "Mapper.h"
#include "NesFile.h"

int main() {
    auto cartridge = loadNesFile(R"(C:\Users\ocfbnj\Downloads\Super Mario Bros (E).nes)");
    assert(cartridge != nullptr);

    auto mapper = Mapper::create(std::move(cartridge));
    assert(mapper != nullptr);

    std::cout << std::hex << +mapper->read(0x8000) << "\n";
}
