#include <iostream>

#include <nes/Mapper.h>
#include <nes/Mapper/Mapper0.h>
#include <nes/Mapper/Mapper1.h>
#include <nes/Mapper/Mapper2.h>
#include <nes/Mapper/Mapper3.h>
#include <nes/Mapper/Mapper4.h>

std::unique_ptr<Mapper> Mapper::create(Cartridge cartridge) {
    switch (cartridge.mapperNum) {
    case 0:
        return std::make_unique<Mapper0>(std::move(cartridge));
    case 1:
        return std::make_unique<Mapper1>(std::move(cartridge));
    case 2:
        return std::make_unique<Mapper2>(std::move(cartridge));
    case 3:
        return std::make_unique<Mapper3>(std::move(cartridge));
    case 4:
        return std::make_unique<Mapper4>(std::move(cartridge));
    default:
        std::cerr << "No mapper available\n";
        return {};
    }
}

Mapper::Mapper(Cartridge cartridge) : cartridge(std::move(cartridge)) {}

void Mapper::reset() {
    // do nothing
}

void Mapper::serialize(std::ostream& os) const {
    // do nothing
}

void Mapper::deserialize(std::istream& is) {
    // do nothing
}

Mirroring Mapper::mirroring() const {
    return cartridge.mirroring;
}

std::uint8_t Mapper::prgBanks() const {
    return cartridge.prgBanks;
}

std::uint8_t Mapper::chrBanks() const {
    return cartridge.chrBanks;
}

bool Mapper::irqState() const {
    return false;
}

void Mapper::irqClear() {
    // do nothing
}

void Mapper::scanline() {
    // do nothing
}
