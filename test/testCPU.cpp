#include <algorithm>
#include <fstream>
#include <iterator>
#include <string>

#include <gtest/gtest.h>

#include <nes/Bus.h>
#include <nes/CPU.h>
#include <nes/NesFile.h>

GTEST_TEST(Nes, CPU) {
    auto cartridge = loadNesFile("nestest.nes");
    ASSERT_TRUE(cartridge.has_value());

    Bus bus;
    bus.insert(std::move(cartridge.value()));
    bus.powerUp();

    CPU& cpu = bus.getCPU();
    cpu.setPc(0xC000);

    int cycles = 15274 + 1;
    std::string str;

    while (cycles--) {
        cpu.clock();
        str.append(cpu.debugStr());
    }

    std::string expect;
    std::ifstream file{"nestest.txt", std::ifstream::in};
    file >> std::noskipws;
    std::copy(std::istream_iterator<char>{file}, std::istream_iterator<char>{}, std::back_inserter(expect));

    ASSERT_TRUE(expect == str);
}
