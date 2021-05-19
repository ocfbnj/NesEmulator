#include <cassert>

#include "NesFile.h"

int main() {
    auto res = loadNesFile(R"(C:\Users\ocfbnj\Downloads\Super Mario Bros (E).nes)");
    assert(res != nullptr);
}
