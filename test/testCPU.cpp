#include <algorithm>
#include <fstream>
#include <iterator>
#include <sstream>

#include <gtest/gtest.h>

#include "Bus.h"
#include "CPU.h"
#include "Mapper.h"
#include "NesFile.h"
#include "PPU.h"

std::ostringstream oss;

GTEST_TEST(Nes, CPU) {
    auto cartridge = loadNesFile("nestest.nes");
    ASSERT_TRUE(cartridge != nullptr);

    auto mapper = Mapper::create(std::move(cartridge));
    ASSERT_TRUE(mapper != nullptr);

    Bus bus{std::move(mapper)};
    CPU& cpu{bus.getCPU()};

    int cycles = 15274 + 3;

    while (cycles--) {
        cpu.clock();
    }

    oss << std::flush;

    std::string expect;
    std::ifstream file{"nestest.txt", std::ifstream::in};
    file >> std::noskipws;
    std::copy(std::istream_iterator<char>{file}, std::istream_iterator<char>{},
              std::back_inserter(expect));

    ASSERT_TRUE(expect == oss.str());
}
