#include <cassert>
#include <iomanip>

#include "Bus.h"
#include "CPU.h"

std::ostream& operator<<(std::ostream& os, const CPU& cpu) {
    os << std::hex << std::uppercase << std::right << std::setfill('0');
    os << std::setw(4) << cpu.pc - 1 << "  " << std::setw(2) << +cpu.opcode << " " << CPU::opTable[cpu.opcode].name << "         ";
    os << " A:" << std::setw(2) << +cpu.a
       << " X:" << std::setw(2) << +cpu.x
       << " Y:" << std::setw(2) << +cpu.y
       << " P:" << std::setw(2) << +cpu.status
       << " SP:" << std::setw(2) << +cpu.sp
       << " CYC:" << std::dec << cpu.totalCycles;

    return os;
}

namespace {
bool isCrossed(uint16_t a, uint16_t b) {
    return (a & 0xFF00) != (b & 0xFF00);
}
} // namespace

std::array<CPU::Operate, 256> CPU::opTable{
    Operate{"BRK", Addressing::Imp, &CPU::BRK, 7, 0},
    Operate{"ORA", Addressing::Izx, &CPU::ORA, 6, 0},
    Operate{"???", Addressing::Imp, &CPU::NIL, 2, 0},
    Operate{"???", Addressing::Izx, &CPU::NIL, 8, 0},
    Operate{"NOP", Addressing::Zp0, &CPU::NOP, 3, 0},
    Operate{"ORA", Addressing::Zp0, &CPU::ORA, 3, 0},
    Operate{"ASL", Addressing::Zp0, &CPU::ASL, 5, 0},
    Operate{"???", Addressing::Zp0, &CPU::NIL, 5, 0},
    Operate{"PHP", Addressing::Imp, &CPU::PHP, 3, 0},
    Operate{"ORA", Addressing::Imm, &CPU::ORA, 2, 0},
    Operate{"ASL", Addressing::Acc, &CPU::ASL, 2, 0},
    Operate{"???", Addressing::Imm, &CPU::NIL, 2, 0},
    Operate{"NOP", Addressing::Abs, &CPU::NOP, 4, 0},
    Operate{"ORA", Addressing::Abs, &CPU::ORA, 4, 0},
    Operate{"ASL", Addressing::Abs, &CPU::ASL, 6, 0},
    Operate{"???", Addressing::Abs, &CPU::NIL, 6, 0},
    Operate{"BPL", Addressing::Rel, &CPU::BPL, 2, 1},
    Operate{"ORA", Addressing::Izy, &CPU::ORA, 5, 1},
    Operate{"???", Addressing::Imp, &CPU::NIL, 2, 0},
    Operate{"???", Addressing::Izy, &CPU::NIL, 8, 0},
    Operate{"NOP", Addressing::Zpx, &CPU::NOP, 4, 0},
    Operate{"ORA", Addressing::Zpx, &CPU::ORA, 4, 0},
    Operate{"ASL", Addressing::Zpx, &CPU::ASL, 6, 0},
    Operate{"???", Addressing::Zpx, &CPU::NIL, 6, 0},
    Operate{"CLC", Addressing::Imp, &CPU::CLC, 2, 0},
    Operate{"ORA", Addressing::Aby, &CPU::ORA, 4, 1},
    Operate{"NOP", Addressing::Imp, &CPU::NOP, 2, 0},
    Operate{"???", Addressing::Aby, &CPU::NIL, 7, 0},
    Operate{"NOP", Addressing::Abx, &CPU::NOP, 4, 1},
    Operate{"ORA", Addressing::Abx, &CPU::ORA, 4, 1},
    Operate{"ASL", Addressing::Abx, &CPU::ASL, 7, 0},
    Operate{"???", Addressing::Abx, &CPU::NIL, 7, 0},
    Operate{"JSR", Addressing::Abs, &CPU::JSR, 6, 0},
    Operate{"AND", Addressing::Izx, &CPU::AND, 6, 0},
    Operate{"???", Addressing::Imp, &CPU::NIL, 2, 0},
    Operate{"???", Addressing::Izx, &CPU::NIL, 8, 0},
    Operate{"BIT", Addressing::Zp0, &CPU::BIT, 3, 0},
    Operate{"AND", Addressing::Zp0, &CPU::AND, 3, 0},
    Operate{"ROL", Addressing::Zp0, &CPU::ROL, 5, 0},
    Operate{"???", Addressing::Zp0, &CPU::NIL, 5, 0},
    Operate{"PLP", Addressing::Imp, &CPU::PLP, 4, 0},
    Operate{"AND", Addressing::Imm, &CPU::AND, 2, 0},
    Operate{"ROL", Addressing::Acc, &CPU::ROL, 2, 0},
    Operate{"???", Addressing::Imm, &CPU::NIL, 2, 0},
    Operate{"BIT", Addressing::Abs, &CPU::BIT, 4, 0},
    Operate{"AND", Addressing::Abs, &CPU::AND, 4, 0},
    Operate{"ROL", Addressing::Abs, &CPU::ROL, 6, 0},
    Operate{"???", Addressing::Abs, &CPU::NIL, 6, 0},
    Operate{"BMI", Addressing::Rel, &CPU::BMI, 2, 1},
    Operate{"AND", Addressing::Izy, &CPU::AND, 5, 1},
    Operate{"???", Addressing::Imp, &CPU::NIL, 2, 0},
    Operate{"???", Addressing::Izy, &CPU::NIL, 8, 0},
    Operate{"NOP", Addressing::Zpx, &CPU::NOP, 4, 0},
    Operate{"AND", Addressing::Zpx, &CPU::AND, 4, 0},
    Operate{"ROL", Addressing::Zpx, &CPU::ROL, 6, 0},
    Operate{"???", Addressing::Zpx, &CPU::NIL, 6, 0},
    Operate{"SEC", Addressing::Imp, &CPU::SEC, 2, 0},
    Operate{"AND", Addressing::Aby, &CPU::AND, 4, 1},
    Operate{"NOP", Addressing::Imp, &CPU::NOP, 2, 0},
    Operate{"???", Addressing::Aby, &CPU::NIL, 7, 0},
    Operate{"NOP", Addressing::Abx, &CPU::NOP, 4, 1},
    Operate{"AND", Addressing::Abx, &CPU::AND, 4, 1},
    Operate{"ROL", Addressing::Abx, &CPU::ROL, 7, 0},
    Operate{"???", Addressing::Abx, &CPU::NIL, 7, 0},
    Operate{"RTI", Addressing::Imp, &CPU::RTI, 6, 0},
    Operate{"EOR", Addressing::Izx, &CPU::EOR, 6, 0},
    Operate{"???", Addressing::Imp, &CPU::NIL, 2, 0},
    Operate{"???", Addressing::Izx, &CPU::NIL, 8, 0},
    Operate{"NOP", Addressing::Zp0, &CPU::NOP, 3, 0},
    Operate{"EOR", Addressing::Zp0, &CPU::EOR, 3, 0},
    Operate{"LSR", Addressing::Zp0, &CPU::LSR, 5, 0},
    Operate{"???", Addressing::Zp0, &CPU::NIL, 5, 0},
    Operate{"PHA", Addressing::Imp, &CPU::PHA, 3, 0},
    Operate{"EOR", Addressing::Imm, &CPU::EOR, 2, 0},
    Operate{"LSR", Addressing::Acc, &CPU::LSR, 2, 0},
    Operate{"???", Addressing::Imm, &CPU::NIL, 2, 0},
    Operate{"JMP", Addressing::Abs, &CPU::JMP, 3, 0},
    Operate{"EOR", Addressing::Abs, &CPU::EOR, 4, 0},
    Operate{"LSR", Addressing::Abs, &CPU::LSR, 6, 0},
    Operate{"???", Addressing::Abs, &CPU::NIL, 6, 0},
    Operate{"BVC", Addressing::Rel, &CPU::BVC, 2, 1},
    Operate{"EOR", Addressing::Izy, &CPU::EOR, 5, 1},
    Operate{"???", Addressing::Imp, &CPU::NIL, 2, 0},
    Operate{"???", Addressing::Izy, &CPU::NIL, 8, 0},
    Operate{"NOP", Addressing::Zpx, &CPU::NOP, 4, 0},
    Operate{"EOR", Addressing::Zpx, &CPU::EOR, 4, 0},
    Operate{"LSR", Addressing::Zpx, &CPU::LSR, 6, 0},
    Operate{"???", Addressing::Zpx, &CPU::NIL, 6, 0},
    Operate{"CLI", Addressing::Imp, &CPU::CLI, 2, 0},
    Operate{"EOR", Addressing::Aby, &CPU::EOR, 4, 1},
    Operate{"NOP", Addressing::Imp, &CPU::NOP, 2, 0},
    Operate{"???", Addressing::Aby, &CPU::NIL, 7, 0},
    Operate{"NOP", Addressing::Abx, &CPU::NOP, 4, 1},
    Operate{"EOR", Addressing::Abx, &CPU::EOR, 4, 1},
    Operate{"LSR", Addressing::Abx, &CPU::LSR, 7, 0},
    Operate{"???", Addressing::Abx, &CPU::NIL, 7, 0},
    Operate{"RTS", Addressing::Imp, &CPU::RTS, 6, 0},
    Operate{"ADC", Addressing::Izx, &CPU::ADC, 6, 0},
    Operate{"???", Addressing::Imp, &CPU::NIL, 2, 0},
    Operate{"???", Addressing::Izx, &CPU::NIL, 8, 0},
    Operate{"NOP", Addressing::Zp0, &CPU::NOP, 3, 0},
    Operate{"ADC", Addressing::Zp0, &CPU::ADC, 3, 0},
    Operate{"ROR", Addressing::Zp0, &CPU::ROR, 5, 0},
    Operate{"???", Addressing::Zp0, &CPU::NIL, 5, 0},
    Operate{"PLA", Addressing::Imp, &CPU::PLA, 4, 0},
    Operate{"ADC", Addressing::Imm, &CPU::ADC, 2, 0},
    Operate{"ROR", Addressing::Acc, &CPU::ROR, 2, 0},
    Operate{"???", Addressing::Imm, &CPU::NIL, 2, 0},
    Operate{"JMP", Addressing::Ind, &CPU::JMP, 5, 0},
    Operate{"ADC", Addressing::Abs, &CPU::ADC, 4, 0},
    Operate{"ROR", Addressing::Abs, &CPU::ROR, 6, 0},
    Operate{"???", Addressing::Abs, &CPU::NIL, 6, 0},
    Operate{"BVS", Addressing::Rel, &CPU::BVS, 2, 1},
    Operate{"ADC", Addressing::Izy, &CPU::ADC, 5, 1},
    Operate{"???", Addressing::Imp, &CPU::NIL, 2, 0},
    Operate{"???", Addressing::Izy, &CPU::NIL, 8, 0},
    Operate{"NOP", Addressing::Zpx, &CPU::NOP, 4, 0},
    Operate{"ADC", Addressing::Zpx, &CPU::ADC, 4, 0},
    Operate{"ROR", Addressing::Zpx, &CPU::ROR, 6, 0},
    Operate{"???", Addressing::Zpx, &CPU::NIL, 6, 0},
    Operate{"SEI", Addressing::Imp, &CPU::SEI, 2, 0},
    Operate{"ADC", Addressing::Aby, &CPU::ADC, 4, 1},
    Operate{"NOP", Addressing::Imp, &CPU::NOP, 2, 0},
    Operate{"???", Addressing::Aby, &CPU::NIL, 7, 0},
    Operate{"NOP", Addressing::Abx, &CPU::NOP, 4, 1},
    Operate{"ADC", Addressing::Abx, &CPU::ADC, 4, 1},
    Operate{"ROR", Addressing::Abx, &CPU::ROR, 7, 0},
    Operate{"???", Addressing::Abx, &CPU::NIL, 7, 0},
    Operate{"NOP", Addressing::Imm, &CPU::NOP, 2, 0},
    Operate{"STA", Addressing::Izx, &CPU::STA, 6, 0},
    Operate{"NOP", Addressing::Imm, &CPU::NOP, 2, 0},
    Operate{"???", Addressing::Izx, &CPU::NIL, 6, 0},
    Operate{"STY", Addressing::Zp0, &CPU::STY, 3, 0},
    Operate{"STA", Addressing::Zp0, &CPU::STA, 3, 0},
    Operate{"STX", Addressing::Zp0, &CPU::STX, 3, 0},
    Operate{"???", Addressing::Zp0, &CPU::NIL, 3, 0},
    Operate{"DEY", Addressing::Imp, &CPU::DEY, 2, 0},
    Operate{"NOP", Addressing::Imm, &CPU::NOP, 2, 0},
    Operate{"TXA", Addressing::Imp, &CPU::TXA, 2, 0},
    Operate{"???", Addressing::Imm, &CPU::NIL, 2, 0},
    Operate{"STY", Addressing::Abs, &CPU::STY, 4, 0},
    Operate{"STA", Addressing::Abs, &CPU::STA, 4, 0},
    Operate{"STX", Addressing::Abs, &CPU::STX, 4, 0},
    Operate{"???", Addressing::Abs, &CPU::NIL, 4, 0},
    Operate{"BCC", Addressing::Rel, &CPU::BCC, 2, 1},
    Operate{"STA", Addressing::Izy, &CPU::STA, 6, 0},
    Operate{"???", Addressing::Imp, &CPU::NIL, 2, 0},
    Operate{"???", Addressing::Izy, &CPU::NIL, 6, 0},
    Operate{"STY", Addressing::Zpx, &CPU::STY, 4, 0},
    Operate{"STA", Addressing::Zpx, &CPU::STA, 4, 0},
    Operate{"STX", Addressing::Zpy, &CPU::STX, 4, 0},
    Operate{"???", Addressing::Zpy, &CPU::NIL, 4, 0},
    Operate{"TYA", Addressing::Imp, &CPU::TYA, 2, 0},
    Operate{"STA", Addressing::Aby, &CPU::STA, 5, 0},
    Operate{"TXS", Addressing::Imp, &CPU::TXS, 2, 0},
    Operate{"???", Addressing::Aby, &CPU::NIL, 5, 0},
    Operate{"???", Addressing::Abx, &CPU::NIL, 5, 0},
    Operate{"STA", Addressing::Abx, &CPU::STA, 5, 0},
    Operate{"???", Addressing::Aby, &CPU::NIL, 5, 0},
    Operate{"???", Addressing::Aby, &CPU::NIL, 5, 0},
    Operate{"LDY", Addressing::Imm, &CPU::LDY, 2, 0},
    Operate{"LDA", Addressing::Izx, &CPU::LDA, 6, 0},
    Operate{"LDX", Addressing::Imm, &CPU::LDX, 2, 0},
    Operate{"???", Addressing::Izx, &CPU::NIL, 6, 0},
    Operate{"LDY", Addressing::Zp0, &CPU::LDY, 3, 0},
    Operate{"LDA", Addressing::Zp0, &CPU::LDA, 3, 0},
    Operate{"LDX", Addressing::Zp0, &CPU::LDX, 3, 0},
    Operate{"???", Addressing::Zp0, &CPU::NIL, 3, 0},
    Operate{"TAY", Addressing::Imp, &CPU::TAY, 2, 0},
    Operate{"LDA", Addressing::Imm, &CPU::LDA, 2, 0},
    Operate{"TAX", Addressing::Imp, &CPU::TAX, 2, 0},
    Operate{"???", Addressing::Imm, &CPU::NIL, 2, 0},
    Operate{"LDY", Addressing::Abs, &CPU::LDY, 4, 0},
    Operate{"LDA", Addressing::Abs, &CPU::LDA, 4, 0},
    Operate{"LDX", Addressing::Abs, &CPU::LDX, 4, 0},
    Operate{"???", Addressing::Abs, &CPU::NIL, 4, 0},
    Operate{"BCS", Addressing::Rel, &CPU::BCS, 2, 1},
    Operate{"LDA", Addressing::Izy, &CPU::LDA, 5, 1},
    Operate{"???", Addressing::Imp, &CPU::NIL, 2, 0},
    Operate{"???", Addressing::Izy, &CPU::NIL, 5, 1},
    Operate{"LDY", Addressing::Zpx, &CPU::LDY, 4, 0},
    Operate{"LDA", Addressing::Zpx, &CPU::LDA, 4, 0},
    Operate{"LDX", Addressing::Zpy, &CPU::LDX, 4, 0},
    Operate{"???", Addressing::Zpy, &CPU::NIL, 4, 0},
    Operate{"CLV", Addressing::Imp, &CPU::CLV, 2, 0},
    Operate{"LDA", Addressing::Aby, &CPU::LDA, 4, 1},
    Operate{"TSX", Addressing::Imp, &CPU::TSX, 2, 0},
    Operate{"???", Addressing::Aby, &CPU::NIL, 4, 1},
    Operate{"LDY", Addressing::Abx, &CPU::LDY, 4, 1},
    Operate{"LDA", Addressing::Abx, &CPU::LDA, 4, 1},
    Operate{"LDX", Addressing::Aby, &CPU::LDX, 4, 1},
    Operate{"???", Addressing::Aby, &CPU::NIL, 4, 1},
    Operate{"CPY", Addressing::Imm, &CPU::CPY, 2, 0},
    Operate{"CMP", Addressing::Izx, &CPU::CMP, 6, 0},
    Operate{"NOP", Addressing::Imm, &CPU::NOP, 2, 0},
    Operate{"???", Addressing::Izx, &CPU::NIL, 8, 0},
    Operate{"CPY", Addressing::Zp0, &CPU::CPY, 3, 0},
    Operate{"CMP", Addressing::Zp0, &CPU::CMP, 3, 0},
    Operate{"DEC", Addressing::Zp0, &CPU::DEC, 5, 0},
    Operate{"???", Addressing::Zp0, &CPU::NIL, 5, 0},
    Operate{"INY", Addressing::Imp, &CPU::INY, 2, 0},
    Operate{"CMP", Addressing::Imm, &CPU::CMP, 2, 0},
    Operate{"DEX", Addressing::Imp, &CPU::DEX, 2, 0},
    Operate{"???", Addressing::Imm, &CPU::NIL, 2, 0},
    Operate{"CPY", Addressing::Abs, &CPU::CPY, 4, 0},
    Operate{"CMP", Addressing::Abs, &CPU::CMP, 4, 0},
    Operate{"DEC", Addressing::Abs, &CPU::DEC, 6, 0},
    Operate{"???", Addressing::Abs, &CPU::NIL, 6, 0},
    Operate{"BNE", Addressing::Rel, &CPU::BNE, 2, 1},
    Operate{"CMP", Addressing::Izy, &CPU::CMP, 5, 1},
    Operate{"???", Addressing::Imp, &CPU::NIL, 2, 0},
    Operate{"???", Addressing::Izy, &CPU::NIL, 8, 0},
    Operate{"NOP", Addressing::Zpx, &CPU::NOP, 4, 0},
    Operate{"CMP", Addressing::Zpx, &CPU::CMP, 4, 0},
    Operate{"DEC", Addressing::Zpx, &CPU::DEC, 6, 0},
    Operate{"???", Addressing::Zpx, &CPU::NIL, 6, 0},
    Operate{"CLD", Addressing::Imp, &CPU::CLD, 2, 0},
    Operate{"CMP", Addressing::Aby, &CPU::CMP, 4, 1},
    Operate{"NOP", Addressing::Imp, &CPU::NOP, 2, 0},
    Operate{"???", Addressing::Aby, &CPU::NIL, 7, 0},
    Operate{"NOP", Addressing::Abx, &CPU::NOP, 4, 1},
    Operate{"CMP", Addressing::Abx, &CPU::CMP, 4, 1},
    Operate{"DEC", Addressing::Abx, &CPU::DEC, 7, 0},
    Operate{"???", Addressing::Abx, &CPU::NIL, 7, 0},
    Operate{"CPX", Addressing::Imm, &CPU::CPX, 2, 0},
    Operate{"SBC", Addressing::Izx, &CPU::SBC, 6, 0},
    Operate{"NOP", Addressing::Imm, &CPU::NOP, 2, 0},
    Operate{"???", Addressing::Izx, &CPU::NIL, 8, 0},
    Operate{"CPX", Addressing::Zp0, &CPU::CPX, 3, 0},
    Operate{"SBC", Addressing::Zp0, &CPU::SBC, 3, 0},
    Operate{"INC", Addressing::Zp0, &CPU::INC, 5, 0},
    Operate{"???", Addressing::Zp0, &CPU::NIL, 5, 0},
    Operate{"INX", Addressing::Imp, &CPU::INX, 2, 0},
    Operate{"SBC", Addressing::Imm, &CPU::SBC, 2, 0},
    Operate{"NOP", Addressing::Imp, &CPU::NOP, 2, 0},
    Operate{"SBC", Addressing::Imm, &CPU::SBC, 2, 0},
    Operate{"CPX", Addressing::Abs, &CPU::CPX, 4, 0},
    Operate{"SBC", Addressing::Abs, &CPU::SBC, 4, 0},
    Operate{"INC", Addressing::Abs, &CPU::INC, 6, 0},
    Operate{"???", Addressing::Abs, &CPU::NIL, 6, 0},
    Operate{"BEQ", Addressing::Rel, &CPU::BEQ, 2, 1},
    Operate{"SBC", Addressing::Izy, &CPU::SBC, 5, 1},
    Operate{"???", Addressing::Imp, &CPU::NIL, 2, 0},
    Operate{"???", Addressing::Izy, &CPU::NIL, 8, 0},
    Operate{"NOP", Addressing::Zpx, &CPU::NOP, 4, 0},
    Operate{"SBC", Addressing::Zpx, &CPU::SBC, 4, 0},
    Operate{"INC", Addressing::Zpx, &CPU::INC, 6, 0},
    Operate{"???", Addressing::Zpx, &CPU::NIL, 6, 0},
    Operate{"SED", Addressing::Imp, &CPU::SED, 2, 0},
    Operate{"SBC", Addressing::Aby, &CPU::SBC, 4, 1},
    Operate{"NOP", Addressing::Imp, &CPU::NOP, 2, 0},
    Operate{"???", Addressing::Aby, &CPU::NIL, 7, 0},
    Operate{"NOP", Addressing::Abx, &CPU::NOP, 4, 1},
    Operate{"SBC", Addressing::Abx, &CPU::SBC, 4, 1},
    Operate{"INC", Addressing::Abx, &CPU::INC, 7, 0},
    Operate{"???", Addressing::Abx, &CPU::NIL, 7, 0},
};

CPU::CPU(Bus& bus) : bus(&bus) {
    reset();
}

void CPU::clock() {
    // execute the operation at last cycle
    if (cycles == 0) {
        step();
    }

    cycles--;
}

void CPU::reset() {
    pc = read16(0xFFFC);
    a = 0;
    x = 0;
    y = 0;
    sp = 0xFD;

    status = 0x24;

    cycles = 8;
}

void CPU::irq() {
    if (i == 0) {
        push16(pc);
        push(status);

        i = 1;

        pc = read16(0xFFFE);

        cycles = 7;
    }
}

void CPU::nmi() {
    push16(pc);
    push(status);

    i = 1;

    pc = read16(0xFFFA);

    cycles = 8;
}

void CPU::testCPU(std::ostream* os, uint16_t pc, uint32_t totalCycles) {
    this->os = os;
    this->pc = pc;
    this->totalCycles = totalCycles;
}

uint8_t CPU::read(uint16_t addr) {
    assert(bus != nullptr);
    return bus->cpuRead(addr);
}

uint16_t CPU::read16(uint16_t addr) {
    assert(bus != nullptr);
    return bus->cpuRead16(addr);
}

void CPU::write(uint16_t addr, uint8_t data) {
    assert(bus != nullptr);
    bus->cpuWrite(addr, data);
}

void CPU::step() {
    opcode = read(pc++);

    if (os) {
        *os << *this << "\n";
    }

    uint16_t address = 0;
    bool pageCrossed = false;

    Operate op = opTable[opcode];
    addressingMode = op.addressing;
    switch (addressingMode) {
    case Addressing::Imp:
    case Addressing::Acc:
        // ignore
        break;
    case Addressing::Imm:
        address = pc++;
        break;
    case Addressing::Zp0:
        address = read(pc++) & 0x00FF;
        break;
    case Addressing::Zpx:
        address = (read(pc++) + x) & 0x00FF;
        break;
    case Addressing::Zpy:
        address = (read(pc++) + y) & 0x00FF;
        break;
    case Addressing::Rel:
        address = read(pc++);
        if (address & 0x80) {
            address |= 0xFF00;
        }
        break;
    case Addressing::Abs:
        address = read16(pc);
        pc += 2;
        break;
    case Addressing::Abx:
        address = read16(pc) + x;
        pc += 2;
        pageCrossed = isCrossed(address - x, address);
        break;
    case Addressing::Aby:
        address = read16(pc) + y;
        pc += 2;
        pageCrossed = isCrossed(address - y, address);
        break;
    case Addressing::Ind: {
        uint16_t ptr = read16(pc);
        pc += 2;

        if ((ptr & 0x00FF) == 0x00FF) {
            address = read(ptr & 0xFF00) << 8 | read(ptr);
        } else {
            address = read(ptr + 1) << 8 | read(ptr);
        }
    } break;
    case Addressing::Izx: {
        uint16_t temp = read(pc++);
        address = read((temp + x + 1) & 0x00FF) << 8 | read((temp + x) & 0x00FF);
    } break;
    case Addressing::Izy: {
        uint16_t temp = read(pc++);
        address = read((temp + 1) & 0x00FF) << 8 | read(temp & 0x00FF);
        address += y;
        pageCrossed = isCrossed(address - y, address);
    } break;
    default:
        assert(0);
    }

    (this->*op.instruction)(address);
    cycles += op.cycle;

    if (pageCrossed) {
        cycles += op.pageCycle;
    }

    totalCycles += cycles;
}

void CPU::push(uint8_t data) {
    write(0x0100 + sp--, data);
}

void CPU::push16(uint16_t data) {
    push((data >> 8) & 0x00FF);
    push(data & 0x00FF);
}

uint8_t CPU::pop() {
    return read(0x0100 + ++sp);
}

uint16_t CPU::pop16() {
    uint16_t l = pop();
    uint16_t h = pop();

    return (h << 8) | l;
}

void CPU::ADC(uint16_t address) {
    uint8_t m = read(address);
    uint16_t sum = uint16_t(a) + m + c;

    c = (sum > 0xFF ? 1 : 0);
    v = ((a ^ sum) & (m ^ sum) & 0x80 ? 1 : 0);

    a = uint8_t(sum);
    z = (a == 0 ? 1 : 0);
    n = (((a >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::AND(uint16_t address) {
    a = a & read(address);
    z = (a == 0 ? 1 : 0);
    n = (((a >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::ASL(uint16_t address) {
    if (addressingMode == Addressing::Acc) {
        c = (a >> 7) & 1;
        a <<= 1;
        z = (a == 0 ? 1 : 0);
        n = (((a >> 7) & 1) == 1 ? 1 : 0);
    } else {
        uint8_t m = read(address);
        c = (m >> 7) & 1;
        m <<= 1;
        write(address, m);
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
    uint8_t m = read(address);
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
    pc = read16(0xFFFE);
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
    uint8_t m = read(address);
    c = (a >= m ? 1 : 0);
    z = (a == m ? 1 : 0);
    n = ((((a - m) >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::CPX(uint16_t address) {
    uint8_t m = read(address);
    c = (x >= m ? 1 : 0);
    z = (x == m ? 1 : 0);
    n = ((((x - m) >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::CPY(uint16_t address) {
    uint8_t m = read(address);
    c = (y >= m ? 1 : 0);
    z = (y == m ? 1 : 0);
    n = ((((y - m) >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::DEC(uint16_t address) {
    uint8_t m = read(address);
    m--;
    write(address, m);

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
    a = a ^ read(address);
    z = (a == 0 ? 1 : 0);
    n = (((a >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::INC(uint16_t address) {
    uint8_t m = read(address);
    m++;
    write(address, m);

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
    a = read(address);
    z = (a == 0 ? 1 : 0);
    n = (((a >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::LDX(uint16_t address) {
    x = read(address);
    z = (x == 0 ? 1 : 0);
    n = (((x >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::LDY(uint16_t address) {
    y = read(address);
    z = (y == 0 ? 1 : 0);
    n = (((y >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::LSR(uint16_t address) {
    if (addressingMode == Addressing::Acc) {
        c = a & 1;
        a >>= 1;
        z = (a == 0 ? 1 : 0);
        n = (((a >> 7) & 1) == 1 ? 1 : 0);
    } else {
        uint8_t m = read(address);
        c = m & 1;
        m >>= 1;
        write(address, m);
        z = (m == 0 ? 1 : 0);
        n = (((m >> 7) & 1) == 1 ? 1 : 0);
    }
}

void CPU::NOP(uint16_t) {
    // do nothing
}

void CPU::ORA(uint16_t address) {
    a = a | read(address);
    z = (a == 0 ? 1 : 0);
    n = (((a >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::PHA(uint16_t) {
    push(a);
}

void CPU::PHP(uint16_t) {
    push(status | 0x10);
}

void CPU::PLA(uint16_t) {
    a = pop();
    z = (a == 0 ? 1 : 0);
    n = (((a >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::PLP(uint16_t) {
    status = pop() & 0xEF | 0x20;
}

void CPU::ROL(uint16_t address) {
    if (addressingMode == Addressing::Acc) {
        uint8_t oldC = c;
        c = (a >> 7) & 1;
        a = (a << 1) | oldC;
        z = (a == 0 ? 1 : 0);
        n = (((a >> 7) & 1) == 1 ? 1 : 0);
    } else {
        uint8_t m = read(address);
        uint8_t oldC = c;
        c = (m >> 7) & 1;
        m = (m << 1) | oldC;
        write(address, m);
        z = (m == 0 ? 1 : 0);
        n = (((m >> 7) & 1) == 1 ? 1 : 0);
    }
}

void CPU::ROR(uint16_t address) {
    if (addressingMode == Addressing::Acc) {
        uint8_t oldC = c;
        c = a & 1;
        a = (a >> 1) | (oldC << 7);
        z = (a == 0 ? 1 : 0);
        n = (((a >> 7) & 1) == 1 ? 1 : 0);
    } else {
        uint8_t m = read(address);
        uint8_t oldC = c;
        c = m & 1;
        m = (m >> 1) | (oldC << 7);
        write(address, m);
        z = (m == 0 ? 1 : 0);
        n = (((m >> 7) & 1) == 1 ? 1 : 0);
    }
}

void CPU::RTI(uint16_t) {
    status = pop() & 0xEF | 0x20;
    pc = pop16();
}

void CPU::RTS(uint16_t) {
    pc = pop16() + 1;
}

void CPU::SBC(uint16_t address) {
    uint8_t m = read(address);
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
    write(address, a);
}

void CPU::STX(uint16_t address) {
    write(address, x);
}

void CPU::STY(uint16_t address) {
    write(address, y);
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
