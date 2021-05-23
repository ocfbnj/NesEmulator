#include <iomanip>
#include <iostream>

#include "Bus.h"
#include "Cpu.h"

bool isCrossed(uint16_t a, uint16_t b) {
    return (a & 0xFF00) != (b & 0xFF00);
}

std::array<Cpu::Operate, 256> Cpu::opTable{
    Operate{"BRK", Imp, &Cpu::BRK, 7, 0},
    Operate{"ORA", Izx, &Cpu::ORA, 6, 0},
    Operate{"???", Imp, &Cpu::NIL, 2, 0},
    Operate{"???", Izx, &Cpu::NIL, 8, 0},
    Operate{"NOP", Zp0, &Cpu::NOP, 3, 0},
    Operate{"ORA", Zp0, &Cpu::ORA, 3, 0},
    Operate{"ASL", Zp0, &Cpu::ASL, 5, 0},
    Operate{"???", Zp0, &Cpu::NIL, 5, 0},
    Operate{"PHP", Imp, &Cpu::PHP, 3, 0},
    Operate{"ORA", Imm, &Cpu::ORA, 2, 0},
    Operate{"ASL", Acc, &Cpu::ASL, 2, 0},
    Operate{"???", Imm, &Cpu::NIL, 2, 0},
    Operate{"NOP", Abs, &Cpu::NOP, 4, 0},
    Operate{"ORA", Abs, &Cpu::ORA, 4, 0},
    Operate{"ASL", Abs, &Cpu::ASL, 6, 0},
    Operate{"???", Abs, &Cpu::NIL, 6, 0},
    Operate{"BPL", Rel, &Cpu::BPL, 2, 1},
    Operate{"ORA", Izy, &Cpu::ORA, 5, 1},
    Operate{"???", Imp, &Cpu::NIL, 2, 0},
    Operate{"???", Izy, &Cpu::NIL, 8, 0},
    Operate{"NOP", Zpx, &Cpu::NOP, 4, 0},
    Operate{"ORA", Zpx, &Cpu::ORA, 4, 0},
    Operate{"ASL", Zpx, &Cpu::ASL, 6, 0},
    Operate{"???", Zpx, &Cpu::NIL, 6, 0},
    Operate{"CLC", Imp, &Cpu::CLC, 2, 0},
    Operate{"ORA", Aby, &Cpu::ORA, 4, 1},
    Operate{"NOP", Imp, &Cpu::NOP, 2, 0},
    Operate{"???", Aby, &Cpu::NIL, 7, 0},
    Operate{"NOP", Abx, &Cpu::NOP, 4, 1},
    Operate{"ORA", Abx, &Cpu::ORA, 4, 1},
    Operate{"ASL", Abx, &Cpu::ASL, 7, 0},
    Operate{"???", Abx, &Cpu::NIL, 7, 0},
    Operate{"JSR", Abs, &Cpu::JSR, 6, 0},
    Operate{"AND", Izx, &Cpu::AND, 6, 0},
    Operate{"???", Imp, &Cpu::NIL, 2, 0},
    Operate{"???", Izx, &Cpu::NIL, 8, 0},
    Operate{"BIT", Zp0, &Cpu::BIT, 3, 0},
    Operate{"AND", Zp0, &Cpu::AND, 3, 0},
    Operate{"ROL", Zp0, &Cpu::ROL, 5, 0},
    Operate{"???", Zp0, &Cpu::NIL, 5, 0},
    Operate{"PLP", Imp, &Cpu::PLP, 4, 0},
    Operate{"AND", Imm, &Cpu::AND, 2, 0},
    Operate{"ROL", Acc, &Cpu::ROL, 2, 0},
    Operate{"???", Imm, &Cpu::NIL, 2, 0},
    Operate{"BIT", Abs, &Cpu::BIT, 4, 0},
    Operate{"AND", Abs, &Cpu::AND, 4, 0},
    Operate{"ROL", Abs, &Cpu::ROL, 6, 0},
    Operate{"???", Abs, &Cpu::NIL, 6, 0},
    Operate{"BMI", Rel, &Cpu::BMI, 2, 1},
    Operate{"AND", Izy, &Cpu::AND, 5, 1},
    Operate{"???", Imp, &Cpu::NIL, 2, 0},
    Operate{"???", Izy, &Cpu::NIL, 8, 0},
    Operate{"NOP", Zpx, &Cpu::NOP, 4, 0},
    Operate{"AND", Zpx, &Cpu::AND, 4, 0},
    Operate{"ROL", Zpx, &Cpu::ROL, 6, 0},
    Operate{"???", Zpx, &Cpu::NIL, 6, 0},
    Operate{"SEC", Imp, &Cpu::SEC, 2, 0},
    Operate{"AND", Aby, &Cpu::AND, 4, 1},
    Operate{"NOP", Imp, &Cpu::NOP, 2, 0},
    Operate{"???", Aby, &Cpu::NIL, 7, 0},
    Operate{"NOP", Abx, &Cpu::NOP, 4, 1},
    Operate{"AND", Abx, &Cpu::AND, 4, 1},
    Operate{"ROL", Abx, &Cpu::ROL, 7, 0},
    Operate{"???", Abx, &Cpu::NIL, 7, 0},
    Operate{"RTI", Imp, &Cpu::RTI, 6, 0},
    Operate{"EOR", Izx, &Cpu::EOR, 6, 0},
    Operate{"???", Imp, &Cpu::NIL, 2, 0},
    Operate{"???", Izx, &Cpu::NIL, 8, 0},
    Operate{"NOP", Zp0, &Cpu::NOP, 3, 0},
    Operate{"EOR", Zp0, &Cpu::EOR, 3, 0},
    Operate{"LSR", Zp0, &Cpu::LSR, 5, 0},
    Operate{"???", Zp0, &Cpu::NIL, 5, 0},
    Operate{"PHA", Imp, &Cpu::PHA, 3, 0},
    Operate{"EOR", Imm, &Cpu::EOR, 2, 0},
    Operate{"LSR", Acc, &Cpu::LSR, 2, 0},
    Operate{"???", Imm, &Cpu::NIL, 2, 0},
    Operate{"JMP", Abs, &Cpu::JMP, 3, 0},
    Operate{"EOR", Abs, &Cpu::EOR, 4, 0},
    Operate{"LSR", Abs, &Cpu::LSR, 6, 0},
    Operate{"???", Abs, &Cpu::NIL, 6, 0},
    Operate{"BVC", Rel, &Cpu::BVC, 2, 1},
    Operate{"EOR", Izy, &Cpu::EOR, 5, 1},
    Operate{"???", Imp, &Cpu::NIL, 2, 0},
    Operate{"???", Izy, &Cpu::NIL, 8, 0},
    Operate{"NOP", Zpx, &Cpu::NOP, 4, 0},
    Operate{"EOR", Zpx, &Cpu::EOR, 4, 0},
    Operate{"LSR", Zpx, &Cpu::LSR, 6, 0},
    Operate{"???", Zpx, &Cpu::NIL, 6, 0},
    Operate{"CLI", Imp, &Cpu::CLI, 2, 0},
    Operate{"EOR", Aby, &Cpu::EOR, 4, 1},
    Operate{"NOP", Imp, &Cpu::NOP, 2, 0},
    Operate{"???", Aby, &Cpu::NIL, 7, 0},
    Operate{"NOP", Abx, &Cpu::NOP, 4, 1},
    Operate{"EOR", Abx, &Cpu::EOR, 4, 1},
    Operate{"LSR", Abx, &Cpu::LSR, 7, 0},
    Operate{"???", Abx, &Cpu::NIL, 7, 0},
    Operate{"RTS", Imp, &Cpu::RTS, 6, 0},
    Operate{"ADC", Izx, &Cpu::ADC, 6, 0},
    Operate{"???", Imp, &Cpu::NIL, 2, 0},
    Operate{"???", Izx, &Cpu::NIL, 8, 0},
    Operate{"NOP", Zp0, &Cpu::NOP, 3, 0},
    Operate{"ADC", Zp0, &Cpu::ADC, 3, 0},
    Operate{"ROR", Zp0, &Cpu::ROR, 5, 0},
    Operate{"???", Zp0, &Cpu::NIL, 5, 0},
    Operate{"PLA", Imp, &Cpu::PLA, 4, 0},
    Operate{"ADC", Imm, &Cpu::ADC, 2, 0},
    Operate{"ROR", Acc, &Cpu::ROR, 2, 0},
    Operate{"???", Imm, &Cpu::NIL, 2, 0},
    Operate{"JMP", Ind, &Cpu::JMP, 5, 0},
    Operate{"ADC", Abs, &Cpu::ADC, 4, 0},
    Operate{"ROR", Abs, &Cpu::ROR, 6, 0},
    Operate{"???", Abs, &Cpu::NIL, 6, 0},
    Operate{"BVS", Rel, &Cpu::BVS, 2, 1},
    Operate{"ADC", Izy, &Cpu::ADC, 5, 1},
    Operate{"???", Imp, &Cpu::NIL, 2, 0},
    Operate{"???", Izy, &Cpu::NIL, 8, 0},
    Operate{"NOP", Zpx, &Cpu::NOP, 4, 0},
    Operate{"ADC", Zpx, &Cpu::ADC, 4, 0},
    Operate{"ROR", Zpx, &Cpu::ROR, 6, 0},
    Operate{"???", Zpx, &Cpu::NIL, 6, 0},
    Operate{"SEI", Imp, &Cpu::SEI, 2, 0},
    Operate{"ADC", Aby, &Cpu::ADC, 4, 1},
    Operate{"NOP", Imp, &Cpu::NOP, 2, 0},
    Operate{"???", Aby, &Cpu::NIL, 7, 0},
    Operate{"NOP", Abx, &Cpu::NOP, 4, 1},
    Operate{"ADC", Abx, &Cpu::ADC, 4, 1},
    Operate{"ROR", Abx, &Cpu::ROR, 7, 0},
    Operate{"???", Abx, &Cpu::NIL, 7, 0},
    Operate{"NOP", Imm, &Cpu::NOP, 2, 0},
    Operate{"STA", Izx, &Cpu::STA, 6, 0},
    Operate{"NOP", Imm, &Cpu::NOP, 2, 0},
    Operate{"???", Izx, &Cpu::NIL, 6, 0},
    Operate{"STY", Zp0, &Cpu::STY, 3, 0},
    Operate{"STA", Zp0, &Cpu::STA, 3, 0},
    Operate{"STX", Zp0, &Cpu::STX, 3, 0},
    Operate{"???", Zp0, &Cpu::NIL, 3, 0},
    Operate{"DEY", Imp, &Cpu::DEY, 2, 0},
    Operate{"NOP", Imm, &Cpu::NOP, 2, 0},
    Operate{"TXA", Imp, &Cpu::TXA, 2, 0},
    Operate{"???", Imm, &Cpu::NIL, 2, 0},
    Operate{"STY", Abs, &Cpu::STY, 4, 0},
    Operate{"STA", Abs, &Cpu::STA, 4, 0},
    Operate{"STX", Abs, &Cpu::STX, 4, 0},
    Operate{"???", Abs, &Cpu::NIL, 4, 0},
    Operate{"BCC", Rel, &Cpu::BCC, 2, 1},
    Operate{"STA", Izy, &Cpu::STA, 6, 0},
    Operate{"???", Imp, &Cpu::NIL, 2, 0},
    Operate{"???", Izy, &Cpu::NIL, 6, 0},
    Operate{"STY", Zpx, &Cpu::STY, 4, 0},
    Operate{"STA", Zpx, &Cpu::STA, 4, 0},
    Operate{"STX", Zpy, &Cpu::STX, 4, 0},
    Operate{"???", Zpy, &Cpu::NIL, 4, 0},
    Operate{"TYA", Imp, &Cpu::TYA, 2, 0},
    Operate{"STA", Aby, &Cpu::STA, 5, 0},
    Operate{"TXS", Imp, &Cpu::TXS, 2, 0},
    Operate{"???", Aby, &Cpu::NIL, 5, 0},
    Operate{"???", Abx, &Cpu::NIL, 5, 0},
    Operate{"STA", Abx, &Cpu::STA, 5, 0},
    Operate{"???", Aby, &Cpu::NIL, 5, 0},
    Operate{"???", Aby, &Cpu::NIL, 5, 0},
    Operate{"LDY", Imm, &Cpu::LDY, 2, 0},
    Operate{"LDA", Izx, &Cpu::LDA, 6, 0},
    Operate{"LDX", Imm, &Cpu::LDX, 2, 0},
    Operate{"???", Izx, &Cpu::NIL, 6, 0},
    Operate{"LDY", Zp0, &Cpu::LDY, 3, 0},
    Operate{"LDA", Zp0, &Cpu::LDA, 3, 0},
    Operate{"LDX", Zp0, &Cpu::LDX, 3, 0},
    Operate{"???", Zp0, &Cpu::NIL, 3, 0},
    Operate{"TAY", Imp, &Cpu::TAY, 2, 0},
    Operate{"LDA", Imm, &Cpu::LDA, 2, 0},
    Operate{"TAX", Imp, &Cpu::TAX, 2, 0},
    Operate{"???", Imm, &Cpu::NIL, 2, 0},
    Operate{"LDY", Abs, &Cpu::LDY, 4, 0},
    Operate{"LDA", Abs, &Cpu::LDA, 4, 0},
    Operate{"LDX", Abs, &Cpu::LDX, 4, 0},
    Operate{"???", Abs, &Cpu::NIL, 4, 0},
    Operate{"BCS", Rel, &Cpu::BCS, 2, 1},
    Operate{"LDA", Izy, &Cpu::LDA, 5, 1},
    Operate{"???", Imp, &Cpu::NIL, 2, 0},
    Operate{"???", Izy, &Cpu::NIL, 5, 1},
    Operate{"LDY", Zpx, &Cpu::LDY, 4, 0},
    Operate{"LDA", Zpx, &Cpu::LDA, 4, 0},
    Operate{"LDX", Zpy, &Cpu::LDX, 4, 0},
    Operate{"???", Zpy, &Cpu::NIL, 4, 0},
    Operate{"CLV", Imp, &Cpu::CLV, 2, 0},
    Operate{"LDA", Aby, &Cpu::LDA, 4, 1},
    Operate{"TSX", Imp, &Cpu::TSX, 2, 0},
    Operate{"???", Aby, &Cpu::NIL, 4, 1},
    Operate{"LDY", Abx, &Cpu::LDY, 4, 1},
    Operate{"LDA", Abx, &Cpu::LDA, 4, 1},
    Operate{"LDX", Aby, &Cpu::LDX, 4, 1},
    Operate{"???", Aby, &Cpu::NIL, 4, 1},
    Operate{"CPY", Imm, &Cpu::CPY, 2, 0},
    Operate{"CMP", Izx, &Cpu::CMP, 6, 0},
    Operate{"NOP", Imm, &Cpu::NOP, 2, 0},
    Operate{"???", Izx, &Cpu::NIL, 8, 0},
    Operate{"CPY", Zp0, &Cpu::CPY, 3, 0},
    Operate{"CMP", Zp0, &Cpu::CMP, 3, 0},
    Operate{"DEC", Zp0, &Cpu::DEC, 5, 0},
    Operate{"???", Zp0, &Cpu::NIL, 5, 0},
    Operate{"INY", Imp, &Cpu::INY, 2, 0},
    Operate{"CMP", Imm, &Cpu::CMP, 2, 0},
    Operate{"DEX", Imp, &Cpu::DEX, 2, 0},
    Operate{"???", Imm, &Cpu::NIL, 2, 0},
    Operate{"CPY", Abs, &Cpu::CPY, 4, 0},
    Operate{"CMP", Abs, &Cpu::CMP, 4, 0},
    Operate{"DEC", Abs, &Cpu::DEC, 6, 0},
    Operate{"???", Abs, &Cpu::NIL, 6, 0},
    Operate{"BNE", Rel, &Cpu::BNE, 2, 1},
    Operate{"CMP", Izy, &Cpu::CMP, 5, 1},
    Operate{"???", Imp, &Cpu::NIL, 2, 0},
    Operate{"???", Izy, &Cpu::NIL, 8, 0},
    Operate{"NOP", Zpx, &Cpu::NOP, 4, 0},
    Operate{"CMP", Zpx, &Cpu::CMP, 4, 0},
    Operate{"DEC", Zpx, &Cpu::DEC, 6, 0},
    Operate{"???", Zpx, &Cpu::NIL, 6, 0},
    Operate{"CLD", Imp, &Cpu::CLD, 2, 0},
    Operate{"CMP", Aby, &Cpu::CMP, 4, 1},
    Operate{"NOP", Imp, &Cpu::NOP, 2, 0},
    Operate{"???", Aby, &Cpu::NIL, 7, 0},
    Operate{"NOP", Abx, &Cpu::NOP, 4, 1},
    Operate{"CMP", Abx, &Cpu::CMP, 4, 1},
    Operate{"DEC", Abx, &Cpu::DEC, 7, 0},
    Operate{"???", Abx, &Cpu::NIL, 7, 0},
    Operate{"CPX", Imm, &Cpu::CPX, 2, 0},
    Operate{"SBC", Izx, &Cpu::SBC, 6, 0},
    Operate{"NOP", Imm, &Cpu::NOP, 2, 0},
    Operate{"???", Izx, &Cpu::NIL, 8, 0},
    Operate{"CPX", Zp0, &Cpu::CPX, 3, 0},
    Operate{"SBC", Zp0, &Cpu::SBC, 3, 0},
    Operate{"INC", Zp0, &Cpu::INC, 5, 0},
    Operate{"???", Zp0, &Cpu::NIL, 5, 0},
    Operate{"INX", Imp, &Cpu::INX, 2, 0},
    Operate{"SBC", Imm, &Cpu::SBC, 2, 0},
    Operate{"NOP", Imp, &Cpu::NOP, 2, 0},
    Operate{"SBC", Imm, &Cpu::SBC, 2, 0},
    Operate{"CPX", Abs, &Cpu::CPX, 4, 0},
    Operate{"SBC", Abs, &Cpu::SBC, 4, 0},
    Operate{"INC", Abs, &Cpu::INC, 6, 0},
    Operate{"???", Abs, &Cpu::NIL, 6, 0},
    Operate{"BEQ", Rel, &Cpu::BEQ, 2, 1},
    Operate{"SBC", Izy, &Cpu::SBC, 5, 1},
    Operate{"???", Imp, &Cpu::NIL, 2, 0},
    Operate{"???", Izy, &Cpu::NIL, 8, 0},
    Operate{"NOP", Zpx, &Cpu::NOP, 4, 0},
    Operate{"SBC", Zpx, &Cpu::SBC, 4, 0},
    Operate{"INC", Zpx, &Cpu::INC, 6, 0},
    Operate{"???", Zpx, &Cpu::NIL, 6, 0},
    Operate{"SED", Imp, &Cpu::SED, 2, 0},
    Operate{"SBC", Aby, &Cpu::SBC, 4, 1},
    Operate{"NOP", Imp, &Cpu::NOP, 2, 0},
    Operate{"???", Aby, &Cpu::NIL, 7, 0},
    Operate{"NOP", Abx, &Cpu::NOP, 4, 1},
    Operate{"SBC", Abx, &Cpu::SBC, 4, 1},
    Operate{"INC", Abx, &Cpu::INC, 7, 0},
    Operate{"???", Abx, &Cpu::NIL, 7, 0},
};

Cpu::Cpu(Bus& bus) : bus(bus) {
    reset();
    // debug
    pc = 0xC000;
}

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

    // P(flags): 0x24
    c = 0;
    z = 0;
    i = 1;
    d = 0;
    b = 0;
    u = 1;
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

    // debug
    std::cout << std::hex << std::uppercase << std::right << std::setfill('0');
    std::cout << pc - 1 << " " << std::setw(2) << +opcode << " " << op.name << "         ";
    std::cout << " A:" << std::setw(2) << +a
              << " X:" << std::setw(2) << +x
              << " Y:" << std::setw(2) << +y
              << " P:" << std::setw(2) << +getStatus()
              << " SP:" << std::setw(2) << +sp
              << "\n";

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
    return bus.read(0x0100 + ++sp);
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

void Cpu::setStatus(uint8_t status) {
    c = (status >> 0) & 1;
    z = (status >> 1) & 1;
    i = (status >> 2) & 1;
    d = (status >> 3) & 1;
    b = (status >> 4) & 1;
    u = (status >> 5) & 1;
    v = (status >> 6) & 1;
    n = (status >> 7) & 1;
}

void Cpu::ADC(uint16_t address) {
    uint16_t sum = uint16_t(a) + bus.read(address) + c;
    a = uint8_t(sum);

    c = (sum > 0xFF ? 1 : 0);
    z = (a == 0 ? 1 : 0);
    v = (((sum >> 7) & 1) != ((a >> 7) & 1) ? 1 : 0);
    n = (((a >> 7) & 1) == 1 ? 1 : 0);
}

void Cpu::AND(uint16_t address) {
    a = a & bus.read(address);
    z = (a == 0 ? 1 : 0);
    n = (((a >> 7) & 1) == 1 ? 1 : 0);
}

void Cpu::ASL(uint16_t address) {
    // TODO
}

void Cpu::BCC(uint16_t address) {
    if (!c) {
        pc += address;
        if (isCrossed(pc, pc - address)) {
            cycles += 2;
        } else {
            cycles += 1;
        }
    }
}

void Cpu::BCS(uint16_t address) {
    if (c) {
        pc += address;
        if (isCrossed(pc, pc - address)) {
            cycles += 2;
        } else {
            cycles += 1;
        }
    }
}

void Cpu::BEQ(uint16_t address) {
    if (z) {
        pc += address;
        if (isCrossed(pc, pc - address)) {
            cycles += 2;
        } else {
            cycles += 1;
        }
    }
}

void Cpu::BIT(uint16_t address) {
    uint8_t m = bus.read(address);
    z = (m & a) == 0;
    v = (m >> 6) & 1;
    n = (m >> 7) & 1;
}

void Cpu::BMI(uint16_t address) {
    if (n) {
        pc += address;
        cycles += (isCrossed(pc, pc - address) ? 2 : 1);
    }
}

void Cpu::BNE(uint16_t address) {
    if (!z) {
        pc += address;
        cycles += (isCrossed(pc, pc - address) ? 2 : 1);
    }
}

void Cpu::BPL(uint16_t address) {
    if (!n) {
        pc += address;
        cycles += (isCrossed(pc, pc - address) ? 2 : 1);
    }
}

void Cpu::BRK(uint16_t address) {
    // TODO
}

void Cpu::BVC(uint16_t address) {
    if (!v) {
        pc += address;
        if (isCrossed(pc, pc - address)) {
            cycles += 2;
        } else {
            cycles += 1;
        }
    }
}

void Cpu::BVS(uint16_t address) {
    if (v) {
        pc += address;
        if (isCrossed(pc, pc - address)) {
            cycles += 2;
        } else {
            cycles += 1;
        }
    }
}

void Cpu::CLC(uint16_t address) {
    c = 0;
}

void Cpu::CLD(uint16_t address) {
    d = 0;
}

void Cpu::CLI(uint16_t address) {
    i = 0;
}

void Cpu::CLV(uint16_t address) {
    v = 0;
}

void Cpu::CMP(uint16_t address) {
    uint8_t m = bus.read(address);
    c = (a >= m ? 1 : 0);
    z = (a == m ? 1 : 0);
    n = ((((a - m) >> 7) & 1) == 1 ? 1 : 0);
}

void Cpu::CPX(uint16_t address) {
    // TODO
}

void Cpu::CPY(uint16_t address) {
    // TODO
}

void Cpu::DEC(uint16_t address) {
    // TODO
}

void Cpu::DEX(uint16_t address) {
    // TODO
}

void Cpu::DEY(uint16_t address) {
    // TODO
}

void Cpu::EOR(uint16_t address) {
    // TODO
}

void Cpu::INC(uint16_t address) {
    // TODO
}

void Cpu::INX(uint16_t address) {
    // TODO
}

void Cpu::INY(uint16_t address) {
    // TODO
}

void Cpu::JMP(uint16_t address) {
    pc = address;
}

void Cpu::JSR(uint16_t address) {
    push16(pc - 1);
    pc = address;
}

void Cpu::LDA(uint16_t address) {
    a = bus.read(address);
    z = (a == 0 ? 1 : 0);
    n = (((a >> 7) & 1) == 1 ? 1 : 0);
}

void Cpu::LDX(uint16_t address) {
    x = bus.read(address);
    z = (x == 0 ? 1 : 0);
    n = (((x >> 7) & 1) == 1 ? 1 : 0);
}

void Cpu::LDY(uint16_t address) {
    y = bus.read(address);
    z = (y == 0 ? 1 : 0);
    n = (((y >> 7) & 1) == 1 ? 1 : 0);
}

void Cpu::LSR(uint16_t address) {
    // TODO
}

void Cpu::NOP(uint16_t address) {
    // do nothing
}

void Cpu::ORA(uint16_t address) {
    a = a | bus.read(address);
    z = (a == 0 ? 1 : 0);
    n = (((a >> 7) & 1) == 1 ? 1 : 0);
}

void Cpu::PHA(uint16_t address) {
    push(a);
}

void Cpu::PHP(uint16_t address) {
    push(getStatus() | 0x10);
}

void Cpu::PLA(uint16_t address) {
    a = pop();
    z = (a == 0 ? 1 : 0);
    n = (((a >> 7) & 1) == 1 ? 1 : 0);
}

void Cpu::PLP(uint16_t address) {
    setStatus(pop() & 0xEF | 0x20);
}

void Cpu::ROL(uint16_t address) {
    // TODO
}

void Cpu::ROR(uint16_t address) {
    // TODO
}

void Cpu::RTI(uint16_t address) {
    // TODO
}

void Cpu::RTS(uint16_t address) {
    pc = pop16() + 1;
}

void Cpu::SBC(uint16_t address) {
    // TODO
}

void Cpu::SEC(uint16_t address) {
    c = 1;
}

void Cpu::SED(uint16_t address) {
    d = 1;
}

void Cpu::SEI(uint16_t address) {
    i = 1;
}

void Cpu::STA(uint16_t address) {
    bus.write(address, a);
}

void Cpu::STX(uint16_t address) {
    bus.write(address, x);
}

void Cpu::STY(uint16_t address) {
    bus.write(address, y);
}

void Cpu::TAX(uint16_t address) {
    // TODO
}

void Cpu::TAY(uint16_t address) {
    // TODO
}

void Cpu::TSX(uint16_t address) {
    // TODO
}

void Cpu::TXA(uint16_t address) {
    // TODO
}

void Cpu::TXS(uint16_t address) {
    // TODO
}

void Cpu::TYA(uint16_t address) {
    // TODO
}

void Cpu::NIL(uint16_t address) {
    // unimplemented
}
