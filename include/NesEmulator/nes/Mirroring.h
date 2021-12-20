#ifndef OCFBNJ_NES_MIRRORING_H
#define OCFBNJ_NES_MIRRORING_H

#include <string_view>

enum class Mirroring {
    OneScreenLoBank,
    OneScreenUpBank,
    Vertical,
    Horizontal
};

std::string_view description(Mirroring mirroring);

#endif // OCFBNJ_NES_MIRRORING_H
