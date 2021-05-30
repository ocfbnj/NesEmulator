#include <iostream>

#include "Mapper.h"
#include "Mapper0.h"

std::unique_ptr<Mapper> Mapper::create(std::unique_ptr<Cartridge> cartridge) {
    switch (cartridge->mapperNum) {
    case 0:
        return std::make_unique<Mapper0>(std::move(cartridge));
    default:
        std::clog << "No mapper available\n";
        return {};
    }
}

Mapper::Mapper(std::unique_ptr<Cartridge> cartridge) : cartridge(std::move(cartridge)) {}
