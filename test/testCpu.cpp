#include <algorithm>
#include <fstream>
#include <iterator>
#include <sstream>

#include <gtest/gtest.h>

#include "Bus.h"
#include "Cpu.h"
#include "NesFile.h"

std::ostringstream oss;

GTEST_TEST(Nes, Cpu) {
    auto cartridge = loadNesFile("nestest.nes");
    ASSERT_TRUE(cartridge != nullptr);

    auto mapper = Mapper::create(std::move(cartridge));
    ASSERT_TRUE(mapper != nullptr);

    Bus bus{std::move(mapper)};
    Cpu cpu{bus};

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
