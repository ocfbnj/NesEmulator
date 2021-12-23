#ifndef OCFBNJ_NES_NES_FILE_H
#define OCFBNJ_NES_NES_FILE_H

#include <memory>
#include <optional>
#include <string_view>

#include "Cartridge.h"

std::optional<Cartridge> loadNesFile(std::string_view path);

#endif // OCFBNJ_NES_NES_FILE_H
