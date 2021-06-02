#ifdef NES_DEBUG
#include <iomanip>
#include <sstream>

extern std::ostringstream oss;
#endif

#include "Bus.h"
#include "CPU.h"

bool isCrossed(uint16_t a, uint16_t b) {
    return (a & 0xFF00) != (b & 0xFF00);
}

std::array<CPU::Operate, 256> CPU::opTable{
    Operate{"BRK", Imp, &CPU::BRK, 7, 0},
    Operate{"ORA", Izx, &CPU::ORA, 6, 0},
    Operate{"???", Imp, &CPU::NIL, 2, 0},
    Operate{"???", Izx, &CPU::NIL, 8, 0},
    Operate{"NOP", Zp0, &CPU::NOP, 3, 0},
    Operate{"ORA", Zp0, &CPU::ORA, 3, 0},
    Operate{"ASL", Zp0, &CPU::ASL, 5, 0},
    Operate{"???", Zp0, &CPU::NIL, 5, 0},
    Operate{"PHP", Imp, &CPU::PHP, 3, 0},
    Operate{"ORA", Imm, &CPU::ORA, 2, 0},
    Operate{"ASL", Acc, &CPU::ASL, 2, 0},
    Operate{"???", Imm, &CPU::NIL, 2, 0},
    Operate{"NOP", Abs, &CPU::NOP, 4, 0},
    Operate{"ORA", Abs, &CPU::ORA, 4, 0},
    Operate{"ASL", Abs, &CPU::ASL, 6, 0},
    Operate{"???", Abs, &CPU::NIL, 6, 0},
    Operate{"BPL", Rel, &CPU::BPL, 2, 1},
    Operate{"ORA", Izy, &CPU::ORA, 5, 1},
    Operate{"???", Imp, &CPU::NIL, 2, 0},
    Operate{"???", Izy, &CPU::NIL, 8, 0},
    Operate{"NOP", Zpx, &CPU::NOP, 4, 0},
    Operate{"ORA", Zpx, &CPU::ORA, 4, 0},
    Operate{"ASL", Zpx, &CPU::ASL, 6, 0},
    Operate{"???", Zpx, &CPU::NIL, 6, 0},
    Operate{"CLC", Imp, &CPU::CLC, 2, 0},
    Operate{"ORA", Aby, &CPU::ORA, 4, 1},
    Operate{"NOP", Imp, &CPU::NOP, 2, 0},
    Operate{"???", Aby, &CPU::NIL, 7, 0},
    Operate{"NOP", Abx, &CPU::NOP, 4, 1},
    Operate{"ORA", Abx, &CPU::ORA, 4, 1},
    Operate{"ASL", Abx, &CPU::ASL, 7, 0},
    Operate{"???", Abx, &CPU::NIL, 7, 0},
    Operate{"JSR", Abs, &CPU::JSR, 6, 0},
    Operate{"AND", Izx, &CPU::AND, 6, 0},
    Operate{"???", Imp, &CPU::NIL, 2, 0},
    Operate{"???", Izx, &CPU::NIL, 8, 0},
    Operate{"BIT", Zp0, &CPU::BIT, 3, 0},
    Operate{"AND", Zp0, &CPU::AND, 3, 0},
    Operate{"ROL", Zp0, &CPU::ROL, 5, 0},
    Operate{"???", Zp0, &CPU::NIL, 5, 0},
    Operate{"PLP", Imp, &CPU::PLP, 4, 0},
    Operate{"AND", Imm, &CPU::AND, 2, 0},
    Operate{"ROL", Acc, &CPU::ROL, 2, 0},
    Operate{"???", Imm, &CPU::NIL, 2, 0},
    Operate{"BIT", Abs, &CPU::BIT, 4, 0},
    Operate{"AND", Abs, &CPU::AND, 4, 0},
    Operate{"ROL", Abs, &CPU::ROL, 6, 0},
    Operate{"???", Abs, &CPU::NIL, 6, 0},
    Operate{"BMI", Rel, &CPU::BMI, 2, 1},
    Operate{"AND", Izy, &CPU::AND, 5, 1},
    Operate{"???", Imp, &CPU::NIL, 2, 0},
    Operate{"???", Izy, &CPU::NIL, 8, 0},
    Operate{"NOP", Zpx, &CPU::NOP, 4, 0},
    Operate{"AND", Zpx, &CPU::AND, 4, 0},
    Operate{"ROL", Zpx, &CPU::ROL, 6, 0},
    Operate{"???", Zpx, &CPU::NIL, 6, 0},
    Operate{"SEC", Imp, &CPU::SEC, 2, 0},
    Operate{"AND", Aby, &CPU::AND, 4, 1},
    Operate{"NOP", Imp, &CPU::NOP, 2, 0},
    Operate{"???", Aby, &CPU::NIL, 7, 0},
    Operate{"NOP", Abx, &CPU::NOP, 4, 1},
    Operate{"AND", Abx, &CPU::AND, 4, 1},
    Operate{"ROL", Abx, &CPU::ROL, 7, 0},
    Operate{"???", Abx, &CPU::NIL, 7, 0},
    Operate{"RTI", Imp, &CPU::RTI, 6, 0},
    Operate{"EOR", Izx, &CPU::EOR, 6, 0},
    Operate{"???", Imp, &CPU::NIL, 2, 0},
    Operate{"???", Izx, &CPU::NIL, 8, 0},
    Operate{"NOP", Zp0, &CPU::NOP, 3, 0},
    Operate{"EOR", Zp0, &CPU::EOR, 3, 0},
    Operate{"LSR", Zp0, &CPU::LSR, 5, 0},
    Operate{"???", Zp0, &CPU::NIL, 5, 0},
    Operate{"PHA", Imp, &CPU::PHA, 3, 0},
    Operate{"EOR", Imm, &CPU::EOR, 2, 0},
    Operate{"LSR", Acc, &CPU::LSR, 2, 0},
    Operate{"???", Imm, &CPU::NIL, 2, 0},
    Operate{"JMP", Abs, &CPU::JMP, 3, 0},
    Operate{"EOR", Abs, &CPU::EOR, 4, 0},
    Operate{"LSR", Abs, &CPU::LSR, 6, 0},
    Operate{"???", Abs, &CPU::NIL, 6, 0},
    Operate{"BVC", Rel, &CPU::BVC, 2, 1},
    Operate{"EOR", Izy, &CPU::EOR, 5, 1},
    Operate{"???", Imp, &CPU::NIL, 2, 0},
    Operate{"???", Izy, &CPU::NIL, 8, 0},
    Operate{"NOP", Zpx, &CPU::NOP, 4, 0},
    Operate{"EOR", Zpx, &CPU::EOR, 4, 0},
    Operate{"LSR", Zpx, &CPU::LSR, 6, 0},
    Operate{"???", Zpx, &CPU::NIL, 6, 0},
    Operate{"CLI", Imp, &CPU::CLI, 2, 0},
    Operate{"EOR", Aby, &CPU::EOR, 4, 1},
    Operate{"NOP", Imp, &CPU::NOP, 2, 0},
    Operate{"???", Aby, &CPU::NIL, 7, 0},
    Operate{"NOP", Abx, &CPU::NOP, 4, 1},
    Operate{"EOR", Abx, &CPU::EOR, 4, 1},
    Operate{"LSR", Abx, &CPU::LSR, 7, 0},
    Operate{"???", Abx, &CPU::NIL, 7, 0},
    Operate{"RTS", Imp, &CPU::RTS, 6, 0},
    Operate{"ADC", Izx, &CPU::ADC, 6, 0},
    Operate{"???", Imp, &CPU::NIL, 2, 0},
    Operate{"???", Izx, &CPU::NIL, 8, 0},
    Operate{"NOP", Zp0, &CPU::NOP, 3, 0},
    Operate{"ADC", Zp0, &CPU::ADC, 3, 0},
    Operate{"ROR", Zp0, &CPU::ROR, 5, 0},
    Operate{"???", Zp0, &CPU::NIL, 5, 0},
    Operate{"PLA", Imp, &CPU::PLA, 4, 0},
    Operate{"ADC", Imm, &CPU::ADC, 2, 0},
    Operate{"ROR", Acc, &CPU::ROR, 2, 0},
    Operate{"???", Imm, &CPU::NIL, 2, 0},
    Operate{"JMP", Ind, &CPU::JMP, 5, 0},
    Operate{"ADC", Abs, &CPU::ADC, 4, 0},
    Operate{"ROR", Abs, &CPU::ROR, 6, 0},
    Operate{"???", Abs, &CPU::NIL, 6, 0},
    Operate{"BVS", Rel, &CPU::BVS, 2, 1},
    Operate{"ADC", Izy, &CPU::ADC, 5, 1},
    Operate{"???", Imp, &CPU::NIL, 2, 0},
    Operate{"???", Izy, &CPU::NIL, 8, 0},
    Operate{"NOP", Zpx, &CPU::NOP, 4, 0},
    Operate{"ADC", Zpx, &CPU::ADC, 4, 0},
    Operate{"ROR", Zpx, &CPU::ROR, 6, 0},
    Operate{"???", Zpx, &CPU::NIL, 6, 0},
    Operate{"SEI", Imp, &CPU::SEI, 2, 0},
    Operate{"ADC", Aby, &CPU::ADC, 4, 1},
    Operate{"NOP", Imp, &CPU::NOP, 2, 0},
    Operate{"???", Aby, &CPU::NIL, 7, 0},
    Operate{"NOP", Abx, &CPU::NOP, 4, 1},
    Operate{"ADC", Abx, &CPU::ADC, 4, 1},
    Operate{"ROR", Abx, &CPU::ROR, 7, 0},
    Operate{"???", Abx, &CPU::NIL, 7, 0},
    Operate{"NOP", Imm, &CPU::NOP, 2, 0},
    Operate{"STA", Izx, &CPU::STA, 6, 0},
    Operate{"NOP", Imm, &CPU::NOP, 2, 0},
    Operate{"???", Izx, &CPU::NIL, 6, 0},
    Operate{"STY", Zp0, &CPU::STY, 3, 0},
    Operate{"STA", Zp0, &CPU::STA, 3, 0},
    Operate{"STX", Zp0, &CPU::STX, 3, 0},
    Operate{"???", Zp0, &CPU::NIL, 3, 0},
    Operate{"DEY", Imp, &CPU::DEY, 2, 0},
    Operate{"NOP", Imm, &CPU::NOP, 2, 0},
    Operate{"TXA", Imp, &CPU::TXA, 2, 0},
    Operate{"???", Imm, &CPU::NIL, 2, 0},
    Operate{"STY", Abs, &CPU::STY, 4, 0},
    Operate{"STA", Abs, &CPU::STA, 4, 0},
    Operate{"STX", Abs, &CPU::STX, 4, 0},
    Operate{"???", Abs, &CPU::NIL, 4, 0},
    Operate{"BCC", Rel, &CPU::BCC, 2, 1},
    Operate{"STA", Izy, &CPU::STA, 6, 0},
    Operate{"???", Imp, &CPU::NIL, 2, 0},
    Operate{"???", Izy, &CPU::NIL, 6, 0},
    Operate{"STY", Zpx, &CPU::STY, 4, 0},
    Operate{"STA", Zpx, &CPU::STA, 4, 0},
    Operate{"STX", Zpy, &CPU::STX, 4, 0},
    Operate{"???", Zpy, &CPU::NIL, 4, 0},
    Operate{"TYA", Imp, &CPU::TYA, 2, 0},
    Operate{"STA", Aby, &CPU::STA, 5, 0},
    Operate{"TXS", Imp, &CPU::TXS, 2, 0},
    Operate{"???", Aby, &CPU::NIL, 5, 0},
    Operate{"???", Abx, &CPU::NIL, 5, 0},
    Operate{"STA", Abx, &CPU::STA, 5, 0},
    Operate{"???", Aby, &CPU::NIL, 5, 0},
    Operate{"???", Aby, &CPU::NIL, 5, 0},
    Operate{"LDY", Imm, &CPU::LDY, 2, 0},
    Operate{"LDA", Izx, &CPU::LDA, 6, 0},
    Operate{"LDX", Imm, &CPU::LDX, 2, 0},
    Operate{"???", Izx, &CPU::NIL, 6, 0},
    Operate{"LDY", Zp0, &CPU::LDY, 3, 0},
    Operate{"LDA", Zp0, &CPU::LDA, 3, 0},
    Operate{"LDX", Zp0, &CPU::LDX, 3, 0},
    Operate{"???", Zp0, &CPU::NIL, 3, 0},
    Operate{"TAY", Imp, &CPU::TAY, 2, 0},
    Operate{"LDA", Imm, &CPU::LDA, 2, 0},
    Operate{"TAX", Imp, &CPU::TAX, 2, 0},
    Operate{"???", Imm, &CPU::NIL, 2, 0},
    Operate{"LDY", Abs, &CPU::LDY, 4, 0},
    Operate{"LDA", Abs, &CPU::LDA, 4, 0},
    Operate{"LDX", Abs, &CPU::LDX, 4, 0},
    Operate{"???", Abs, &CPU::NIL, 4, 0},
    Operate{"BCS", Rel, &CPU::BCS, 2, 1},
    Operate{"LDA", Izy, &CPU::LDA, 5, 1},
    Operate{"???", Imp, &CPU::NIL, 2, 0},
    Operate{"???", Izy, &CPU::NIL, 5, 1},
    Operate{"LDY", Zpx, &CPU::LDY, 4, 0},
    Operate{"LDA", Zpx, &CPU::LDA, 4, 0},
    Operate{"LDX", Zpy, &CPU::LDX, 4, 0},
    Operate{"???", Zpy, &CPU::NIL, 4, 0},
    Operate{"CLV", Imp, &CPU::CLV, 2, 0},
    Operate{"LDA", Aby, &CPU::LDA, 4, 1},
    Operate{"TSX", Imp, &CPU::TSX, 2, 0},
    Operate{"???", Aby, &CPU::NIL, 4, 1},
    Operate{"LDY", Abx, &CPU::LDY, 4, 1},
    Operate{"LDA", Abx, &CPU::LDA, 4, 1},
    Operate{"LDX", Aby, &CPU::LDX, 4, 1},
    Operate{"???", Aby, &CPU::NIL, 4, 1},
    Operate{"CPY", Imm, &CPU::CPY, 2, 0},
    Operate{"CMP", Izx, &CPU::CMP, 6, 0},
    Operate{"NOP", Imm, &CPU::NOP, 2, 0},
    Operate{"???", Izx, &CPU::NIL, 8, 0},
    Operate{"CPY", Zp0, &CPU::CPY, 3, 0},
    Operate{"CMP", Zp0, &CPU::CMP, 3, 0},
    Operate{"DEC", Zp0, &CPU::DEC, 5, 0},
    Operate{"???", Zp0, &CPU::NIL, 5, 0},
    Operate{"INY", Imp, &CPU::INY, 2, 0},
    Operate{"CMP", Imm, &CPU::CMP, 2, 0},
    Operate{"DEX", Imp, &CPU::DEX, 2, 0},
    Operate{"???", Imm, &CPU::NIL, 2, 0},
    Operate{"CPY", Abs, &CPU::CPY, 4, 0},
    Operate{"CMP", Abs, &CPU::CMP, 4, 0},
    Operate{"DEC", Abs, &CPU::DEC, 6, 0},
    Operate{"???", Abs, &CPU::NIL, 6, 0},
    Operate{"BNE", Rel, &CPU::BNE, 2, 1},
    Operate{"CMP", Izy, &CPU::CMP, 5, 1},
    Operate{"???", Imp, &CPU::NIL, 2, 0},
    Operate{"???", Izy, &CPU::NIL, 8, 0},
    Operate{"NOP", Zpx, &CPU::NOP, 4, 0},
    Operate{"CMP", Zpx, &CPU::CMP, 4, 0},
    Operate{"DEC", Zpx, &CPU::DEC, 6, 0},
    Operate{"???", Zpx, &CPU::NIL, 6, 0},
    Operate{"CLD", Imp, &CPU::CLD, 2, 0},
    Operate{"CMP", Aby, &CPU::CMP, 4, 1},
    Operate{"NOP", Imp, &CPU::NOP, 2, 0},
    Operate{"???", Aby, &CPU::NIL, 7, 0},
    Operate{"NOP", Abx, &CPU::NOP, 4, 1},
    Operate{"CMP", Abx, &CPU::CMP, 4, 1},
    Operate{"DEC", Abx, &CPU::DEC, 7, 0},
    Operate{"???", Abx, &CPU::NIL, 7, 0},
    Operate{"CPX", Imm, &CPU::CPX, 2, 0},
    Operate{"SBC", Izx, &CPU::SBC, 6, 0},
    Operate{"NOP", Imm, &CPU::NOP, 2, 0},
    Operate{"???", Izx, &CPU::NIL, 8, 0},
    Operate{"CPX", Zp0, &CPU::CPX, 3, 0},
    Operate{"SBC", Zp0, &CPU::SBC, 3, 0},
    Operate{"INC", Zp0, &CPU::INC, 5, 0},
    Operate{"???", Zp0, &CPU::NIL, 5, 0},
    Operate{"INX", Imp, &CPU::INX, 2, 0},
    Operate{"SBC", Imm, &CPU::SBC, 2, 0},
    Operate{"NOP", Imp, &CPU::NOP, 2, 0},
    Operate{"SBC", Imm, &CPU::SBC, 2, 0},
    Operate{"CPX", Abs, &CPU::CPX, 4, 0},
    Operate{"SBC", Abs, &CPU::SBC, 4, 0},
    Operate{"INC", Abs, &CPU::INC, 6, 0},
    Operate{"???", Abs, &CPU::NIL, 6, 0},
    Operate{"BEQ", Rel, &CPU::BEQ, 2, 1},
    Operate{"SBC", Izy, &CPU::SBC, 5, 1},
    Operate{"???", Imp, &CPU::NIL, 2, 0},
    Operate{"???", Izy, &CPU::NIL, 8, 0},
    Operate{"NOP", Zpx, &CPU::NOP, 4, 0},
    Operate{"SBC", Zpx, &CPU::SBC, 4, 0},
    Operate{"INC", Zpx, &CPU::INC, 6, 0},
    Operate{"???", Zpx, &CPU::NIL, 6, 0},
    Operate{"SED", Imp, &CPU::SED, 2, 0},
    Operate{"SBC", Aby, &CPU::SBC, 4, 1},
    Operate{"NOP", Imp, &CPU::NOP, 2, 0},
    Operate{"???", Aby, &CPU::NIL, 7, 0},
    Operate{"NOP", Abx, &CPU::NOP, 4, 1},
    Operate{"SBC", Abx, &CPU::SBC, 4, 1},
    Operate{"INC", Abx, &CPU::INC, 7, 0},
    Operate{"???", Abx, &CPU::NIL, 7, 0},
};

CPU::CPU(Bus& bus) : bus(bus) {
    reset();

#ifdef NES_DEBUG
    pc = 0xC000;
#endif
}

void CPU::clock() {
    // execute the operation at last cycle
    if (cycles == 0) {
        step();
    }

    cycles--;
}

void CPU::reset() {
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

void CPU::irq() {
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

void CPU::nmi() {
    push16(pc);

    b = 0;
    u = 1;
    i = 1;
    push(getStatus());

    pc = bus.read16(0xFFFA);

    cycles = 8;
}

void CPU::step() {
    uint8_t opcode = bus.read(pc++);
    Operate& op = opTable[opcode];

#ifdef NES_DEBUG
    oss << std::hex << std::uppercase << std::right << std::setfill('0');
    oss << std::setw(4) << pc - 1 << "  " << std::setw(2) << +opcode << " " << op.name << "         ";
    oss << " A:" << std::setw(2) << +a
        << " X:" << std::setw(2) << +x
        << " Y:" << std::setw(2) << +y
        << " P:" << std::setw(2) << +getStatus()
        << " SP:" << std::setw(2) << +sp
        << " CYC:" << std::dec << totalCycles
        << "\n";
#endif

    uint16_t address = 0;
    bool pageCrossed = false;

    addressingMode = op.addressing;
    switch (addressingMode) {
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

        if ((ptr & 0x00FF) == 0x00FF) {
            address = bus.read(ptr & 0xFF00) << 8 | bus.read(ptr);
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

#ifdef NES_DEBUG
    totalCycles += cycles;
#endif
}

void CPU::push(uint8_t data) {
    bus.write(0x0100 + sp--, data);
}

void CPU::push16(uint16_t data) {
    push((data >> 8) & 0x00FF);
    push(data & 0x00FF);
}

uint8_t CPU::pop() {
    return bus.read(0x0100 + ++sp);
}

uint16_t CPU::pop16() {
    uint16_t l = pop();
    uint16_t h = pop();

    return (h << 8) | l;
}

uint8_t CPU::getStatus() const {
    return c << 0 |
           z << 1 |
           i << 2 |
           d << 3 |
           b << 4 |
           u << 5 |
           v << 6 |
           n << 7;
}

void CPU::setStatus(uint8_t status) {
    c = (status >> 0) & 1;
    z = (status >> 1) & 1;
    i = (status >> 2) & 1;
    d = (status >> 3) & 1;
    b = (status >> 4) & 1;
    u = (status >> 5) & 1;
    v = (status >> 6) & 1;
    n = (status >> 7) & 1;
}

void CPU::ADC(uint16_t address) {
    uint8_t m = bus.read(address);
    uint16_t sum = uint16_t(a) + m + c;

    c = (sum > 0xFF ? 1 : 0);
    v = ((a ^ sum) & (m ^ sum) & 0x80 ? 1 : 0);

    a = uint8_t(sum);
    z = (a == 0 ? 1 : 0);
    n = (((a >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::AND(uint16_t address) {
    a = a & bus.read(address);
    z = (a == 0 ? 1 : 0);
    n = (((a >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::ASL(uint16_t address) {
    if (addressingMode == Acc) {
        c = (a >> 7) & 1;
        a <<= 1;
        z = (a == 0 ? 1 : 0);
        n = (((a >> 7) & 1) == 1 ? 1 : 0);
    } else {
        uint8_t m = bus.read(address);
        c = (m >> 7) & 1;
        m <<= 1;
        bus.write(address, m);
        z = (m == 0 ? 1 : 0);
        n = (((m >> 7) & 1) == 1 ? 1 : 0);
    }
}

void CPU::BCC(uint16_t address) {
    if (!c) {
        pc += address;
        if (isCrossed(pc, pc - address)) {
            cycles += 2;
        } else {
            cycles += 1;
        }
    }
}

void CPU::BCS(uint16_t address) {
    if (c) {
        pc += address;
        if (isCrossed(pc, pc - address)) {
            cycles += 2;
        } else {
            cycles += 1;
        }
    }
}

void CPU::BEQ(uint16_t address) {
    if (z) {
        pc += address;
        if (isCrossed(pc, pc - address)) {
            cycles += 2;
        } else {
            cycles += 1;
        }
    }
}

void CPU::BIT(uint16_t address) {
    uint8_t m = bus.read(address);
    z = (m & a) == 0;
    v = (m >> 6) & 1;
    n = (m >> 7) & 1;
}

void CPU::BMI(uint16_t address) {
    if (n) {
        pc += address;
        cycles += (isCrossed(pc, pc - address) ? 2 : 1);
    }
}

void CPU::BNE(uint16_t address) {
    if (!z) {
        pc += address;
        cycles += (isCrossed(pc, pc - address) ? 2 : 1);
    }
}

void CPU::BPL(uint16_t address) {
    if (!n) {
        pc += address;
        cycles += (isCrossed(pc, pc - address) ? 2 : 1);
    }
}

void CPU::BRK(uint16_t address) {
    push16(pc);
    PHP(address);
    b = 1;
    pc = bus.read16(0xFFFE);
}

void CPU::BVC(uint16_t address) {
    if (!v) {
        pc += address;
        if (isCrossed(pc, pc - address)) {
            cycles += 2;
        } else {
            cycles += 1;
        }
    }
}

void CPU::BVS(uint16_t address) {
    if (v) {
        pc += address;
        if (isCrossed(pc, pc - address)) {
            cycles += 2;
        } else {
            cycles += 1;
        }
    }
}

void CPU::CLC(uint16_t) {
    c = 0;
}

void CPU::CLD(uint16_t) {
    d = 0;
}

void CPU::CLI(uint16_t) {
    i = 0;
}

void CPU::CLV(uint16_t) {
    v = 0;
}

void CPU::CMP(uint16_t address) {
    uint8_t m = bus.read(address);
    c = (a >= m ? 1 : 0);
    z = (a == m ? 1 : 0);
    n = ((((a - m) >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::CPX(uint16_t address) {
    uint8_t m = bus.read(address);
    c = (x >= m ? 1 : 0);
    z = (x == m ? 1 : 0);
    n = ((((x - m) >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::CPY(uint16_t address) {
    uint8_t m = bus.read(address);
    c = (y >= m ? 1 : 0);
    z = (y == m ? 1 : 0);
    n = ((((y - m) >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::DEC(uint16_t address) {
    uint8_t m = bus.read(address);
    m--;
    bus.write(address, m);

    z = (m == 0 ? 1 : 0);
    n = (((m >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::DEX(uint16_t) {
    x--;
    z = (x == 0 ? 1 : 0);
    n = (((x >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::DEY(uint16_t) {
    y--;
    z = (y == 0 ? 1 : 0);
    n = (((y >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::EOR(uint16_t address) {
    a = a ^ bus.read(address);
    z = (a == 0 ? 1 : 0);
    n = (((a >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::INC(uint16_t address) {
    uint8_t m = bus.read(address);
    m++;
    bus.write(address, m);

    z = (m == 0 ? 1 : 0);
    n = (((m >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::INX(uint16_t) {
    x++;
    z = (x == 0 ? 1 : 0);
    n = (((x >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::INY(uint16_t) {
    y++;
    z = (y == 0 ? 1 : 0);
    n = (((y >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::JMP(uint16_t address) {
    pc = address;
}

void CPU::JSR(uint16_t address) {
    push16(pc - 1);
    pc = address;
}

void CPU::LDA(uint16_t address) {
    a = bus.read(address);
    z = (a == 0 ? 1 : 0);
    n = (((a >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::LDX(uint16_t address) {
    x = bus.read(address);
    z = (x == 0 ? 1 : 0);
    n = (((x >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::LDY(uint16_t address) {
    y = bus.read(address);
    z = (y == 0 ? 1 : 0);
    n = (((y >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::LSR(uint16_t address) {
    if (addressingMode == Acc) {
        c = a & 1;
        a >>= 1;
        z = (a == 0 ? 1 : 0);
        n = (((a >> 7) & 1) == 1 ? 1 : 0);
    } else {
        uint8_t m = bus.read(address);
        c = m & 1;
        m >>= 1;
        bus.write(address, m);
        z = (m == 0 ? 1 : 0);
        n = (((m >> 7) & 1) == 1 ? 1 : 0);
    }
}

void CPU::NOP(uint16_t) {
    // do nothing
}

void CPU::ORA(uint16_t address) {
    a = a | bus.read(address);
    z = (a == 0 ? 1 : 0);
    n = (((a >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::PHA(uint16_t) {
    push(a);
}

void CPU::PHP(uint16_t) {
    push(getStatus() | 0x10);
}

void CPU::PLA(uint16_t) {
    a = pop();
    z = (a == 0 ? 1 : 0);
    n = (((a >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::PLP(uint16_t) {
    setStatus(pop() & 0xEF | 0x20);
}

void CPU::ROL(uint16_t address) {
    if (addressingMode == Acc) {
        uint8_t oldC = c;
        c = (a >> 7) & 1;
        a = (a << 1) | oldC;
        z = (a == 0 ? 1 : 0);
        n = (((a >> 7) & 1) == 1 ? 1 : 0);
    } else {
        uint8_t m = bus.read(address);
        uint8_t oldC = c;
        c = (m >> 7) & 1;
        m = (m << 1) | oldC;
        bus.write(address, m);
        z = (m == 0 ? 1 : 0);
        n = (((m >> 7) & 1) == 1 ? 1 : 0);
    }
}

void CPU::ROR(uint16_t address) {
    if (addressingMode == Acc) {
        uint8_t oldC = c;
        c = a & 1;
        a = (a >> 1) | (oldC << 7);
        z = (a == 0 ? 1 : 0);
        n = (((a >> 7) & 1) == 1 ? 1 : 0);
    } else {
        uint8_t m = bus.read(address);
        uint8_t oldC = c;
        c = m & 1;
        m = (m >> 1) | (oldC << 7);
        bus.write(address, m);
        z = (m == 0 ? 1 : 0);
        n = (((m >> 7) & 1) == 1 ? 1 : 0);
    }
}

void CPU::RTI(uint16_t) {
    setStatus(pop() & 0xEF | 0x20);
    pc = pop16();
}

void CPU::RTS(uint16_t) {
    pc = pop16() + 1;
}

void CPU::SBC(uint16_t address) {
    uint8_t m = bus.read(address);
    uint16_t diff = uint16_t(a) - m - (1 - c);

    c = (diff > 0xFF ? 0 : 1);
    v = ((a ^ diff) & (~m ^ diff) & 0x80 ? 1 : 0);

    a = uint8_t(diff);
    z = (a == 0 ? 1 : 0);
    n = (((a >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::SEC(uint16_t) {
    c = 1;
}

void CPU::SED(uint16_t) {
    d = 1;
}

void CPU::SEI(uint16_t) {
    i = 1;
}

void CPU::STA(uint16_t address) {
    bus.write(address, a);
}

void CPU::STX(uint16_t address) {
    bus.write(address, x);
}

void CPU::STY(uint16_t address) {
    bus.write(address, y);
}

void CPU::TAX(uint16_t) {
    x = a;
    z = (x == 0 ? 1 : 0);
    n = (((x >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::TAY(uint16_t) {
    y = a;
    z = (y == 0 ? 1 : 0);
    n = (((y >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::TSX(uint16_t) {
    x = sp;
    z = (x == 0 ? 1 : 0);
    n = (((x >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::TXA(uint16_t) {
    a = x;
    z = (a == 0 ? 1 : 0);
    n = (((a >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::TXS(uint16_t) {
    sp = x;
}

void CPU::TYA(uint16_t) {
    a = y;
    z = (a == 0 ? 1 : 0);
    n = (((a >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::NIL(uint16_t) {
    // unimplemented
}
