#include "Cpu.h"
#include "Bus.h"

Cpu::Cpu(Bus& bus) : bus(bus) {}

void Cpu::clock() {
    if (cycles == 0) {
        step();
    }

    cycles--;
}

void Cpu::reset() {
    pc = bus.read16(0xFFFC);
    a = 0;
    x = 0;
    y = 0;
    sp = 0xFD;

    c = 0;
    z = 0;
    i = 0;
    d = 0;
    b = 0;
    u = 0;
    v = 0;
    n = 0;

    cycles = 8;
}

void Cpu::irq() {
    if (i == 0) {
        push16(pc);

        b = 0;
        u = 1;
        i = 1;

        uint8_t status = (c << 0) |
                         (z << 1) |
                         (i << 2) |
                         (d << 3) |
                         (b << 4) |
                         (u << 5) |
                         (v << 6) |
                         (n << 7);
        push(status);

        pc = bus.read16(0xFFFE);

        cycles = 7;
    }
}

void Cpu::nmi() {
    push16(pc);

    b = 0;
    u = 1;
    i = 1;

    uint8_t status = (c << 0) |
                     (z << 1) |
                     (i << 2) |
                     (d << 3) |
                     (b << 4) |
                     (u << 5) |
                     (v << 6) |
                     (n << 7);
    push(status);

    pc = bus.read16(0xFFFA);

    cycles = 8;
}

void Cpu::step() {
    uint8_t opcode = bus.read(pc++);
}

void Cpu::push(uint8_t data) {
    bus.write(0x0100 + sp--, data);
}

void Cpu::push16(uint16_t data) {
    push((data >> 8) & 0xFF);
    push(data & 0xFF);
}

uint8_t Cpu::pop() {
    return bus.read(0x0100 + sp++);
}

uint16_t Cpu::pop16() {
    uint16_t l = pop();
    uint16_t h = pop();

    return (h << 8) | l;
}
