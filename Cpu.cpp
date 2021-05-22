#include "Cpu.h"
#include "Bus.h"

bool isCrossed(uint16_t a, uint16_t b) {
    return (a & 0xFF00) != (b & 0xFF00);
}

std::array<Cpu::Operate, 256> Cpu::opTable{
    // TODO
};

Cpu::Cpu(Bus& bus) : bus(bus) {}

void Cpu::clock() {
    // execute the operation at last cycle
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
        push(getStatus());

        pc = bus.read16(0xFFFE);

        cycles = 7;
    }
}

void Cpu::nmi() {
    push16(pc);

    b = 0;
    u = 1;
    i = 1;
    push(getStatus());

    pc = bus.read16(0xFFFA);

    cycles = 8;
}

void Cpu::step() {
    uint8_t opcode = bus.read(pc++);
    Operate& op = opTable[opcode];

    uint16_t address = 0;
    bool pageCrossed = false;

    switch (op.addressing) {
    case Imp:
    case Acc:
        // ignore
        break;
    case Imm:
        address = pc++;
        break;
    case Zp0:
        address = bus.read(pc++) & 0x00FF;
        break;
    case Zpx:
        address = (bus.read(pc++) + x) & 0x00FF;
        break;
    case Zpy:
        address = (bus.read(pc++) + y) & 0x00FF;
        break;
    case Rel:
        // TODO
        address = bus.read(pc++);
        if (address & 0x80) {
            address |= 0xFF00;
        }
        break;
    case Abs:
        address = bus.read16(pc);
        pc += 2;
        break;
    case Abx:
        address = bus.read16(pc) + x;
        pc += 2;
        pageCrossed = isCrossed(address - x, address);
        break;
    case Aby:
        address = bus.read16(pc) + y;
        pc += 2;
        pageCrossed = isCrossed(address - y, address);
        break;
    case Ind: {
        uint16_t ptr = bus.read16(pc);
        pc += 2;

        // TODO
        if ((ptr & 0x00FF) == 0x00FF) {
            address = bus.read(ptr & 0x00FF) << 8 | bus.read(ptr);
        } else {
            address = bus.read(ptr + 1) << 8 | bus.read(ptr);
        }
    } break;
    case Izx: {
        uint16_t temp = bus.read(pc++);
        address = bus.read((temp + x + 1) & 0x00FF) << 8 | bus.read((temp + x) & 0x00FF);
    } break;
    case Izy: {
        uint16_t temp = bus.read(pc++);
        address = bus.read((temp + 1) & 0x00FF) << 8 | bus.read(temp & 0x00FF);
        address += y;
        pageCrossed = isCrossed(address - y, address);
    } break;
    }

    (this->*op.instruction)(address);
    cycles += op.cycle;

    if (pageCrossed) {
        cycles += op.pageCycle;
    }
}

void Cpu::push(uint8_t data) {
    bus.write(0x0100 + sp--, data);
}

void Cpu::push16(uint16_t data) {
    push((data >> 8) & 0x00FF);
    push(data & 0x00FF);
}

uint8_t Cpu::pop() {
    return bus.read(0x0100 + sp++);
}

uint16_t Cpu::pop16() {
    uint16_t l = pop();
    uint16_t h = pop();

    return (h << 8) | l;
}

uint8_t Cpu::getStatus() {
    return c << 0 |
           z << 1 |
           i << 2 |
           d << 3 |
           b << 4 |
           u << 5 |
           v << 6 |
           n << 7;
}
