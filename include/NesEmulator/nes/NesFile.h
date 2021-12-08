#ifndef NES_EMULATOR_NES_FILE_H
#define NES_EMULATOR_NES_FILE_H

#include <memory>
#include <string_view>

#include "Cartridge.h"

std::unique_ptr<Cartridge> loadNesFile(std::string_view path);

#endif
