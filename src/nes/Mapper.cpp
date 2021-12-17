#include <iostream>

#include "Mapper.h"
#include "Mapper0.h"
#include "Mapper1.h"
#include "Mapper2.h"
#include "Mapper3.h"

std::unique_ptr<Mapper> Mapper::create(std::unique_ptr<Cartridge> cartridge) {
    switch (cartridge->mapperNum) {
    case 0:
        return std::make_unique<Mapper0>(std::move(cartridge));
    case 1:
        return std::make_unique<Mapper1>(std::move(cartridge));
    case 2:
        return std::make_unique<Mapper2>(std::move(cartridge));
    case 3:
        return std::make_unique<Mapper3>(std::move(cartridge));
    default:
        std::cerr << "No mapper available\n";
        return {};
    }
}

Mapper::Mapper(std::unique_ptr<Cartridge> cartridge) : cartridge(std::move(cartridge)) {}

Mirroring Mapper::mirroring() const {
    return cartridge->mirroring;
}

uint8_t Mapper::prgBanks() const {
    return cartridge->prgRom.size() / (16 * 1024);
}

uint8_t Mapper::chrBanks() const {
    return cartridge->chrRom.size() / (8 * 1024);
}
