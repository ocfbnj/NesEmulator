#ifndef OCFBNJ_NES_NES_FILE_H
#define OCFBNJ_NES_NES_FILE_H

#include <memory>
#include <string_view>

#include "Cartridge.h"

std::unique_ptr<Cartridge> loadNesFile(std::string_view path);

#endif
