#ifndef OCFBNJ_NES_TIMER_H
#define OCFBNJ_NES_TIMER_H

#include <cstdint>

struct Timer {
    bool step();

    std::uint16_t counter = 0;
    std::uint16_t period = 0;
};

#endif // OCFBNJ_NES_TIMER_H