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
bool isCrossed(std::uint16_t a, std::uint16_t b) {
    return (a & 0xFF00) != (b & 0xFF00);
}
} // namespace

std::array<CPU::Operation, 256> CPU::opTable{
    Operation{.name = "BRK", .addressing = Addressing::Imp, .instruction = &CPU::BRK, .cycle = 7, .pageCycle = 0},
    Operation{.name = "ORA", .addressing = Addressing::Izx, .instruction = &CPU::ORA, .cycle = 6, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Imp, .instruction = &CPU::NIL, .cycle = 2, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Izx, .instruction = &CPU::NIL, .cycle = 8, .pageCycle = 0},
    Operation{.name = "NOP", .addressing = Addressing::Zp0, .instruction = &CPU::NOP, .cycle = 3, .pageCycle = 0},
    Operation{.name = "ORA", .addressing = Addressing::Zp0, .instruction = &CPU::ORA, .cycle = 3, .pageCycle = 0},
    Operation{.name = "ASL", .addressing = Addressing::Zp0, .instruction = &CPU::ASL, .cycle = 5, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Zp0, .instruction = &CPU::NIL, .cycle = 5, .pageCycle = 0},
    Operation{.name = "PHP", .addressing = Addressing::Imp, .instruction = &CPU::PHP, .cycle = 3, .pageCycle = 0},
    Operation{.name = "ORA", .addressing = Addressing::Imm, .instruction = &CPU::ORA, .cycle = 2, .pageCycle = 0},
    Operation{.name = "ASL", .addressing = Addressing::Acc, .instruction = &CPU::ASL, .cycle = 2, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Imm, .instruction = &CPU::NIL, .cycle = 2, .pageCycle = 0},
    Operation{.name = "NOP", .addressing = Addressing::Abs, .instruction = &CPU::NOP, .cycle = 4, .pageCycle = 0},
    Operation{.name = "ORA", .addressing = Addressing::Abs, .instruction = &CPU::ORA, .cycle = 4, .pageCycle = 0},
    Operation{.name = "ASL", .addressing = Addressing::Abs, .instruction = &CPU::ASL, .cycle = 6, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Abs, .instruction = &CPU::NIL, .cycle = 6, .pageCycle = 0},
    Operation{.name = "BPL", .addressing = Addressing::Rel, .instruction = &CPU::BPL, .cycle = 2, .pageCycle = 1},
    Operation{.name = "ORA", .addressing = Addressing::Izy, .instruction = &CPU::ORA, .cycle = 5, .pageCycle = 1},
    Operation{.name = "???", .addressing = Addressing::Imp, .instruction = &CPU::NIL, .cycle = 2, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Izy, .instruction = &CPU::NIL, .cycle = 8, .pageCycle = 0},
    Operation{.name = "NOP", .addressing = Addressing::Zpx, .instruction = &CPU::NOP, .cycle = 4, .pageCycle = 0},
    Operation{.name = "ORA", .addressing = Addressing::Zpx, .instruction = &CPU::ORA, .cycle = 4, .pageCycle = 0},
    Operation{.name = "ASL", .addressing = Addressing::Zpx, .instruction = &CPU::ASL, .cycle = 6, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Zpx, .instruction = &CPU::NIL, .cycle = 6, .pageCycle = 0},
    Operation{.name = "CLC", .addressing = Addressing::Imp, .instruction = &CPU::CLC, .cycle = 2, .pageCycle = 0},
    Operation{.name = "ORA", .addressing = Addressing::Aby, .instruction = &CPU::ORA, .cycle = 4, .pageCycle = 1},
    Operation{.name = "NOP", .addressing = Addressing::Imp, .instruction = &CPU::NOP, .cycle = 2, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Aby, .instruction = &CPU::NIL, .cycle = 7, .pageCycle = 0},
    Operation{.name = "NOP", .addressing = Addressing::Abx, .instruction = &CPU::NOP, .cycle = 4, .pageCycle = 1},
    Operation{.name = "ORA", .addressing = Addressing::Abx, .instruction = &CPU::ORA, .cycle = 4, .pageCycle = 1},
    Operation{.name = "ASL", .addressing = Addressing::Abx, .instruction = &CPU::ASL, .cycle = 7, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Abx, .instruction = &CPU::NIL, .cycle = 7, .pageCycle = 0},
    Operation{.name = "JSR", .addressing = Addressing::Abs, .instruction = &CPU::JSR, .cycle = 6, .pageCycle = 0},
    Operation{.name = "AND", .addressing = Addressing::Izx, .instruction = &CPU::AND, .cycle = 6, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Imp, .instruction = &CPU::NIL, .cycle = 2, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Izx, .instruction = &CPU::NIL, .cycle = 8, .pageCycle = 0},
    Operation{.name = "BIT", .addressing = Addressing::Zp0, .instruction = &CPU::BIT, .cycle = 3, .pageCycle = 0},
    Operation{.name = "AND", .addressing = Addressing::Zp0, .instruction = &CPU::AND, .cycle = 3, .pageCycle = 0},
    Operation{.name = "ROL", .addressing = Addressing::Zp0, .instruction = &CPU::ROL, .cycle = 5, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Zp0, .instruction = &CPU::NIL, .cycle = 5, .pageCycle = 0},
    Operation{.name = "PLP", .addressing = Addressing::Imp, .instruction = &CPU::PLP, .cycle = 4, .pageCycle = 0},
    Operation{.name = "AND", .addressing = Addressing::Imm, .instruction = &CPU::AND, .cycle = 2, .pageCycle = 0},
    Operation{.name = "ROL", .addressing = Addressing::Acc, .instruction = &CPU::ROL, .cycle = 2, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Imm, .instruction = &CPU::NIL, .cycle = 2, .pageCycle = 0},
    Operation{.name = "BIT", .addressing = Addressing::Abs, .instruction = &CPU::BIT, .cycle = 4, .pageCycle = 0},
    Operation{.name = "AND", .addressing = Addressing::Abs, .instruction = &CPU::AND, .cycle = 4, .pageCycle = 0},
    Operation{.name = "ROL", .addressing = Addressing::Abs, .instruction = &CPU::ROL, .cycle = 6, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Abs, .instruction = &CPU::NIL, .cycle = 6, .pageCycle = 0},
    Operation{.name = "BMI", .addressing = Addressing::Rel, .instruction = &CPU::BMI, .cycle = 2, .pageCycle = 1},
    Operation{.name = "AND", .addressing = Addressing::Izy, .instruction = &CPU::AND, .cycle = 5, .pageCycle = 1},
    Operation{.name = "???", .addressing = Addressing::Imp, .instruction = &CPU::NIL, .cycle = 2, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Izy, .instruction = &CPU::NIL, .cycle = 8, .pageCycle = 0},
    Operation{.name = "NOP", .addressing = Addressing::Zpx, .instruction = &CPU::NOP, .cycle = 4, .pageCycle = 0},
    Operation{.name = "AND", .addressing = Addressing::Zpx, .instruction = &CPU::AND, .cycle = 4, .pageCycle = 0},
    Operation{.name = "ROL", .addressing = Addressing::Zpx, .instruction = &CPU::ROL, .cycle = 6, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Zpx, .instruction = &CPU::NIL, .cycle = 6, .pageCycle = 0},
    Operation{.name = "SEC", .addressing = Addressing::Imp, .instruction = &CPU::SEC, .cycle = 2, .pageCycle = 0},
    Operation{.name = "AND", .addressing = Addressing::Aby, .instruction = &CPU::AND, .cycle = 4, .pageCycle = 1},
    Operation{.name = "NOP", .addressing = Addressing::Imp, .instruction = &CPU::NOP, .cycle = 2, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Aby, .instruction = &CPU::NIL, .cycle = 7, .pageCycle = 0},
    Operation{.name = "NOP", .addressing = Addressing::Abx, .instruction = &CPU::NOP, .cycle = 4, .pageCycle = 1},
    Operation{.name = "AND", .addressing = Addressing::Abx, .instruction = &CPU::AND, .cycle = 4, .pageCycle = 1},
    Operation{.name = "ROL", .addressing = Addressing::Abx, .instruction = &CPU::ROL, .cycle = 7, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Abx, .instruction = &CPU::NIL, .cycle = 7, .pageCycle = 0},
    Operation{.name = "RTI", .addressing = Addressing::Imp, .instruction = &CPU::RTI, .cycle = 6, .pageCycle = 0},
    Operation{.name = "EOR", .addressing = Addressing::Izx, .instruction = &CPU::EOR, .cycle = 6, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Imp, .instruction = &CPU::NIL, .cycle = 2, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Izx, .instruction = &CPU::NIL, .cycle = 8, .pageCycle = 0},
    Operation{.name = "NOP", .addressing = Addressing::Zp0, .instruction = &CPU::NOP, .cycle = 3, .pageCycle = 0},
    Operation{.name = "EOR", .addressing = Addressing::Zp0, .instruction = &CPU::EOR, .cycle = 3, .pageCycle = 0},
    Operation{.name = "LSR", .addressing = Addressing::Zp0, .instruction = &CPU::LSR, .cycle = 5, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Zp0, .instruction = &CPU::NIL, .cycle = 5, .pageCycle = 0},
    Operation{.name = "PHA", .addressing = Addressing::Imp, .instruction = &CPU::PHA, .cycle = 3, .pageCycle = 0},
    Operation{.name = "EOR", .addressing = Addressing::Imm, .instruction = &CPU::EOR, .cycle = 2, .pageCycle = 0},
    Operation{.name = "LSR", .addressing = Addressing::Acc, .instruction = &CPU::LSR, .cycle = 2, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Imm, .instruction = &CPU::NIL, .cycle = 2, .pageCycle = 0},
    Operation{.name = "JMP", .addressing = Addressing::Abs, .instruction = &CPU::JMP, .cycle = 3, .pageCycle = 0},
    Operation{.name = "EOR", .addressing = Addressing::Abs, .instruction = &CPU::EOR, .cycle = 4, .pageCycle = 0},
    Operation{.name = "LSR", .addressing = Addressing::Abs, .instruction = &CPU::LSR, .cycle = 6, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Abs, .instruction = &CPU::NIL, .cycle = 6, .pageCycle = 0},
    Operation{.name = "BVC", .addressing = Addressing::Rel, .instruction = &CPU::BVC, .cycle = 2, .pageCycle = 1},
    Operation{.name = "EOR", .addressing = Addressing::Izy, .instruction = &CPU::EOR, .cycle = 5, .pageCycle = 1},
    Operation{.name = "???", .addressing = Addressing::Imp, .instruction = &CPU::NIL, .cycle = 2, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Izy, .instruction = &CPU::NIL, .cycle = 8, .pageCycle = 0},
    Operation{.name = "NOP", .addressing = Addressing::Zpx, .instruction = &CPU::NOP, .cycle = 4, .pageCycle = 0},
    Operation{.name = "EOR", .addressing = Addressing::Zpx, .instruction = &CPU::EOR, .cycle = 4, .pageCycle = 0},
    Operation{.name = "LSR", .addressing = Addressing::Zpx, .instruction = &CPU::LSR, .cycle = 6, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Zpx, .instruction = &CPU::NIL, .cycle = 6, .pageCycle = 0},
    Operation{.name = "CLI", .addressing = Addressing::Imp, .instruction = &CPU::CLI, .cycle = 2, .pageCycle = 0},
    Operation{.name = "EOR", .addressing = Addressing::Aby, .instruction = &CPU::EOR, .cycle = 4, .pageCycle = 1},
    Operation{.name = "NOP", .addressing = Addressing::Imp, .instruction = &CPU::NOP, .cycle = 2, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Aby, .instruction = &CPU::NIL, .cycle = 7, .pageCycle = 0},
    Operation{.name = "NOP", .addressing = Addressing::Abx, .instruction = &CPU::NOP, .cycle = 4, .pageCycle = 1},
    Operation{.name = "EOR", .addressing = Addressing::Abx, .instruction = &CPU::EOR, .cycle = 4, .pageCycle = 1},
    Operation{.name = "LSR", .addressing = Addressing::Abx, .instruction = &CPU::LSR, .cycle = 7, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Abx, .instruction = &CPU::NIL, .cycle = 7, .pageCycle = 0},
    Operation{.name = "RTS", .addressing = Addressing::Imp, .instruction = &CPU::RTS, .cycle = 6, .pageCycle = 0},
    Operation{.name = "ADC", .addressing = Addressing::Izx, .instruction = &CPU::ADC, .cycle = 6, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Imp, .instruction = &CPU::NIL, .cycle = 2, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Izx, .instruction = &CPU::NIL, .cycle = 8, .pageCycle = 0},
    Operation{.name = "NOP", .addressing = Addressing::Zp0, .instruction = &CPU::NOP, .cycle = 3, .pageCycle = 0},
    Operation{.name = "ADC", .addressing = Addressing::Zp0, .instruction = &CPU::ADC, .cycle = 3, .pageCycle = 0},
    Operation{.name = "ROR", .addressing = Addressing::Zp0, .instruction = &CPU::ROR, .cycle = 5, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Zp0, .instruction = &CPU::NIL, .cycle = 5, .pageCycle = 0},
    Operation{.name = "PLA", .addressing = Addressing::Imp, .instruction = &CPU::PLA, .cycle = 4, .pageCycle = 0},
    Operation{.name = "ADC", .addressing = Addressing::Imm, .instruction = &CPU::ADC, .cycle = 2, .pageCycle = 0},
    Operation{.name = "ROR", .addressing = Addressing::Acc, .instruction = &CPU::ROR, .cycle = 2, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Imm, .instruction = &CPU::NIL, .cycle = 2, .pageCycle = 0},
    Operation{.name = "JMP", .addressing = Addressing::Ind, .instruction = &CPU::JMP, .cycle = 5, .pageCycle = 0},
    Operation{.name = "ADC", .addressing = Addressing::Abs, .instruction = &CPU::ADC, .cycle = 4, .pageCycle = 0},
    Operation{.name = "ROR", .addressing = Addressing::Abs, .instruction = &CPU::ROR, .cycle = 6, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Abs, .instruction = &CPU::NIL, .cycle = 6, .pageCycle = 0},
    Operation{.name = "BVS", .addressing = Addressing::Rel, .instruction = &CPU::BVS, .cycle = 2, .pageCycle = 1},
    Operation{.name = "ADC", .addressing = Addressing::Izy, .instruction = &CPU::ADC, .cycle = 5, .pageCycle = 1},
    Operation{.name = "???", .addressing = Addressing::Imp, .instruction = &CPU::NIL, .cycle = 2, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Izy, .instruction = &CPU::NIL, .cycle = 8, .pageCycle = 0},
    Operation{.name = "NOP", .addressing = Addressing::Zpx, .instruction = &CPU::NOP, .cycle = 4, .pageCycle = 0},
    Operation{.name = "ADC", .addressing = Addressing::Zpx, .instruction = &CPU::ADC, .cycle = 4, .pageCycle = 0},
    Operation{.name = "ROR", .addressing = Addressing::Zpx, .instruction = &CPU::ROR, .cycle = 6, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Zpx, .instruction = &CPU::NIL, .cycle = 6, .pageCycle = 0},
    Operation{.name = "SEI", .addressing = Addressing::Imp, .instruction = &CPU::SEI, .cycle = 2, .pageCycle = 0},
    Operation{.name = "ADC", .addressing = Addressing::Aby, .instruction = &CPU::ADC, .cycle = 4, .pageCycle = 1},
    Operation{.name = "NOP", .addressing = Addressing::Imp, .instruction = &CPU::NOP, .cycle = 2, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Aby, .instruction = &CPU::NIL, .cycle = 7, .pageCycle = 0},
    Operation{.name = "NOP", .addressing = Addressing::Abx, .instruction = &CPU::NOP, .cycle = 4, .pageCycle = 1},
    Operation{.name = "ADC", .addressing = Addressing::Abx, .instruction = &CPU::ADC, .cycle = 4, .pageCycle = 1},
    Operation{.name = "ROR", .addressing = Addressing::Abx, .instruction = &CPU::ROR, .cycle = 7, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Abx, .instruction = &CPU::NIL, .cycle = 7, .pageCycle = 0},
    Operation{.name = "NOP", .addressing = Addressing::Imm, .instruction = &CPU::NOP, .cycle = 2, .pageCycle = 0},
    Operation{.name = "STA", .addressing = Addressing::Izx, .instruction = &CPU::STA, .cycle = 6, .pageCycle = 0},
    Operation{.name = "NOP", .addressing = Addressing::Imm, .instruction = &CPU::NOP, .cycle = 2, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Izx, .instruction = &CPU::NIL, .cycle = 6, .pageCycle = 0},
    Operation{.name = "STY", .addressing = Addressing::Zp0, .instruction = &CPU::STY, .cycle = 3, .pageCycle = 0},
    Operation{.name = "STA", .addressing = Addressing::Zp0, .instruction = &CPU::STA, .cycle = 3, .pageCycle = 0},
    Operation{.name = "STX", .addressing = Addressing::Zp0, .instruction = &CPU::STX, .cycle = 3, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Zp0, .instruction = &CPU::NIL, .cycle = 3, .pageCycle = 0},
    Operation{.name = "DEY", .addressing = Addressing::Imp, .instruction = &CPU::DEY, .cycle = 2, .pageCycle = 0},
    Operation{.name = "NOP", .addressing = Addressing::Imm, .instruction = &CPU::NOP, .cycle = 2, .pageCycle = 0},
    Operation{.name = "TXA", .addressing = Addressing::Imp, .instruction = &CPU::TXA, .cycle = 2, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Imm, .instruction = &CPU::NIL, .cycle = 2, .pageCycle = 0},
    Operation{.name = "STY", .addressing = Addressing::Abs, .instruction = &CPU::STY, .cycle = 4, .pageCycle = 0},
    Operation{.name = "STA", .addressing = Addressing::Abs, .instruction = &CPU::STA, .cycle = 4, .pageCycle = 0},
    Operation{.name = "STX", .addressing = Addressing::Abs, .instruction = &CPU::STX, .cycle = 4, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Abs, .instruction = &CPU::NIL, .cycle = 4, .pageCycle = 0},
    Operation{.name = "BCC", .addressing = Addressing::Rel, .instruction = &CPU::BCC, .cycle = 2, .pageCycle = 1},
    Operation{.name = "STA", .addressing = Addressing::Izy, .instruction = &CPU::STA, .cycle = 6, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Imp, .instruction = &CPU::NIL, .cycle = 2, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Izy, .instruction = &CPU::NIL, .cycle = 6, .pageCycle = 0},
    Operation{.name = "STY", .addressing = Addressing::Zpx, .instruction = &CPU::STY, .cycle = 4, .pageCycle = 0},
    Operation{.name = "STA", .addressing = Addressing::Zpx, .instruction = &CPU::STA, .cycle = 4, .pageCycle = 0},
    Operation{.name = "STX", .addressing = Addressing::Zpy, .instruction = &CPU::STX, .cycle = 4, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Zpy, .instruction = &CPU::NIL, .cycle = 4, .pageCycle = 0},
    Operation{.name = "TYA", .addressing = Addressing::Imp, .instruction = &CPU::TYA, .cycle = 2, .pageCycle = 0},
    Operation{.name = "STA", .addressing = Addressing::Aby, .instruction = &CPU::STA, .cycle = 5, .pageCycle = 0},
    Operation{.name = "TXS", .addressing = Addressing::Imp, .instruction = &CPU::TXS, .cycle = 2, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Aby, .instruction = &CPU::NIL, .cycle = 5, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Abx, .instruction = &CPU::NIL, .cycle = 5, .pageCycle = 0},
    Operation{.name = "STA", .addressing = Addressing::Abx, .instruction = &CPU::STA, .cycle = 5, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Aby, .instruction = &CPU::NIL, .cycle = 5, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Aby, .instruction = &CPU::NIL, .cycle = 5, .pageCycle = 0},
    Operation{.name = "LDY", .addressing = Addressing::Imm, .instruction = &CPU::LDY, .cycle = 2, .pageCycle = 0},
    Operation{.name = "LDA", .addressing = Addressing::Izx, .instruction = &CPU::LDA, .cycle = 6, .pageCycle = 0},
    Operation{.name = "LDX", .addressing = Addressing::Imm, .instruction = &CPU::LDX, .cycle = 2, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Izx, .instruction = &CPU::NIL, .cycle = 6, .pageCycle = 0},
    Operation{.name = "LDY", .addressing = Addressing::Zp0, .instruction = &CPU::LDY, .cycle = 3, .pageCycle = 0},
    Operation{.name = "LDA", .addressing = Addressing::Zp0, .instruction = &CPU::LDA, .cycle = 3, .pageCycle = 0},
    Operation{.name = "LDX", .addressing = Addressing::Zp0, .instruction = &CPU::LDX, .cycle = 3, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Zp0, .instruction = &CPU::NIL, .cycle = 3, .pageCycle = 0},
    Operation{.name = "TAY", .addressing = Addressing::Imp, .instruction = &CPU::TAY, .cycle = 2, .pageCycle = 0},
    Operation{.name = "LDA", .addressing = Addressing::Imm, .instruction = &CPU::LDA, .cycle = 2, .pageCycle = 0},
    Operation{.name = "TAX", .addressing = Addressing::Imp, .instruction = &CPU::TAX, .cycle = 2, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Imm, .instruction = &CPU::NIL, .cycle = 2, .pageCycle = 0},
    Operation{.name = "LDY", .addressing = Addressing::Abs, .instruction = &CPU::LDY, .cycle = 4, .pageCycle = 0},
    Operation{.name = "LDA", .addressing = Addressing::Abs, .instruction = &CPU::LDA, .cycle = 4, .pageCycle = 0},
    Operation{.name = "LDX", .addressing = Addressing::Abs, .instruction = &CPU::LDX, .cycle = 4, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Abs, .instruction = &CPU::NIL, .cycle = 4, .pageCycle = 0},
    Operation{.name = "BCS", .addressing = Addressing::Rel, .instruction = &CPU::BCS, .cycle = 2, .pageCycle = 1},
    Operation{.name = "LDA", .addressing = Addressing::Izy, .instruction = &CPU::LDA, .cycle = 5, .pageCycle = 1},
    Operation{.name = "???", .addressing = Addressing::Imp, .instruction = &CPU::NIL, .cycle = 2, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Izy, .instruction = &CPU::NIL, .cycle = 5, .pageCycle = 1},
    Operation{.name = "LDY", .addressing = Addressing::Zpx, .instruction = &CPU::LDY, .cycle = 4, .pageCycle = 0},
    Operation{.name = "LDA", .addressing = Addressing::Zpx, .instruction = &CPU::LDA, .cycle = 4, .pageCycle = 0},
    Operation{.name = "LDX", .addressing = Addressing::Zpy, .instruction = &CPU::LDX, .cycle = 4, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Zpy, .instruction = &CPU::NIL, .cycle = 4, .pageCycle = 0},
    Operation{.name = "CLV", .addressing = Addressing::Imp, .instruction = &CPU::CLV, .cycle = 2, .pageCycle = 0},
    Operation{.name = "LDA", .addressing = Addressing::Aby, .instruction = &CPU::LDA, .cycle = 4, .pageCycle = 1},
    Operation{.name = "TSX", .addressing = Addressing::Imp, .instruction = &CPU::TSX, .cycle = 2, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Aby, .instruction = &CPU::NIL, .cycle = 4, .pageCycle = 1},
    Operation{.name = "LDY", .addressing = Addressing::Abx, .instruction = &CPU::LDY, .cycle = 4, .pageCycle = 1},
    Operation{.name = "LDA", .addressing = Addressing::Abx, .instruction = &CPU::LDA, .cycle = 4, .pageCycle = 1},
    Operation{.name = "LDX", .addressing = Addressing::Aby, .instruction = &CPU::LDX, .cycle = 4, .pageCycle = 1},
    Operation{.name = "???", .addressing = Addressing::Aby, .instruction = &CPU::NIL, .cycle = 4, .pageCycle = 1},
    Operation{.name = "CPY", .addressing = Addressing::Imm, .instruction = &CPU::CPY, .cycle = 2, .pageCycle = 0},
    Operation{.name = "CMP", .addressing = Addressing::Izx, .instruction = &CPU::CMP, .cycle = 6, .pageCycle = 0},
    Operation{.name = "NOP", .addressing = Addressing::Imm, .instruction = &CPU::NOP, .cycle = 2, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Izx, .instruction = &CPU::NIL, .cycle = 8, .pageCycle = 0},
    Operation{.name = "CPY", .addressing = Addressing::Zp0, .instruction = &CPU::CPY, .cycle = 3, .pageCycle = 0},
    Operation{.name = "CMP", .addressing = Addressing::Zp0, .instruction = &CPU::CMP, .cycle = 3, .pageCycle = 0},
    Operation{.name = "DEC", .addressing = Addressing::Zp0, .instruction = &CPU::DEC, .cycle = 5, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Zp0, .instruction = &CPU::NIL, .cycle = 5, .pageCycle = 0},
    Operation{.name = "INY", .addressing = Addressing::Imp, .instruction = &CPU::INY, .cycle = 2, .pageCycle = 0},
    Operation{.name = "CMP", .addressing = Addressing::Imm, .instruction = &CPU::CMP, .cycle = 2, .pageCycle = 0},
    Operation{.name = "DEX", .addressing = Addressing::Imp, .instruction = &CPU::DEX, .cycle = 2, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Imm, .instruction = &CPU::NIL, .cycle = 2, .pageCycle = 0},
    Operation{.name = "CPY", .addressing = Addressing::Abs, .instruction = &CPU::CPY, .cycle = 4, .pageCycle = 0},
    Operation{.name = "CMP", .addressing = Addressing::Abs, .instruction = &CPU::CMP, .cycle = 4, .pageCycle = 0},
    Operation{.name = "DEC", .addressing = Addressing::Abs, .instruction = &CPU::DEC, .cycle = 6, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Abs, .instruction = &CPU::NIL, .cycle = 6, .pageCycle = 0},
    Operation{.name = "BNE", .addressing = Addressing::Rel, .instruction = &CPU::BNE, .cycle = 2, .pageCycle = 1},
    Operation{.name = "CMP", .addressing = Addressing::Izy, .instruction = &CPU::CMP, .cycle = 5, .pageCycle = 1},
    Operation{.name = "???", .addressing = Addressing::Imp, .instruction = &CPU::NIL, .cycle = 2, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Izy, .instruction = &CPU::NIL, .cycle = 8, .pageCycle = 0},
    Operation{.name = "NOP", .addressing = Addressing::Zpx, .instruction = &CPU::NOP, .cycle = 4, .pageCycle = 0},
    Operation{.name = "CMP", .addressing = Addressing::Zpx, .instruction = &CPU::CMP, .cycle = 4, .pageCycle = 0},
    Operation{.name = "DEC", .addressing = Addressing::Zpx, .instruction = &CPU::DEC, .cycle = 6, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Zpx, .instruction = &CPU::NIL, .cycle = 6, .pageCycle = 0},
    Operation{.name = "CLD", .addressing = Addressing::Imp, .instruction = &CPU::CLD, .cycle = 2, .pageCycle = 0},
    Operation{.name = "CMP", .addressing = Addressing::Aby, .instruction = &CPU::CMP, .cycle = 4, .pageCycle = 1},
    Operation{.name = "NOP", .addressing = Addressing::Imp, .instruction = &CPU::NOP, .cycle = 2, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Aby, .instruction = &CPU::NIL, .cycle = 7, .pageCycle = 0},
    Operation{.name = "NOP", .addressing = Addressing::Abx, .instruction = &CPU::NOP, .cycle = 4, .pageCycle = 1},
    Operation{.name = "CMP", .addressing = Addressing::Abx, .instruction = &CPU::CMP, .cycle = 4, .pageCycle = 1},
    Operation{.name = "DEC", .addressing = Addressing::Abx, .instruction = &CPU::DEC, .cycle = 7, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Abx, .instruction = &CPU::NIL, .cycle = 7, .pageCycle = 0},
    Operation{.name = "CPX", .addressing = Addressing::Imm, .instruction = &CPU::CPX, .cycle = 2, .pageCycle = 0},
    Operation{.name = "SBC", .addressing = Addressing::Izx, .instruction = &CPU::SBC, .cycle = 6, .pageCycle = 0},
    Operation{.name = "NOP", .addressing = Addressing::Imm, .instruction = &CPU::NOP, .cycle = 2, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Izx, .instruction = &CPU::NIL, .cycle = 8, .pageCycle = 0},
    Operation{.name = "CPX", .addressing = Addressing::Zp0, .instruction = &CPU::CPX, .cycle = 3, .pageCycle = 0},
    Operation{.name = "SBC", .addressing = Addressing::Zp0, .instruction = &CPU::SBC, .cycle = 3, .pageCycle = 0},
    Operation{.name = "INC", .addressing = Addressing::Zp0, .instruction = &CPU::INC, .cycle = 5, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Zp0, .instruction = &CPU::NIL, .cycle = 5, .pageCycle = 0},
    Operation{.name = "INX", .addressing = Addressing::Imp, .instruction = &CPU::INX, .cycle = 2, .pageCycle = 0},
    Operation{.name = "SBC", .addressing = Addressing::Imm, .instruction = &CPU::SBC, .cycle = 2, .pageCycle = 0},
    Operation{.name = "NOP", .addressing = Addressing::Imp, .instruction = &CPU::NOP, .cycle = 2, .pageCycle = 0},
    Operation{.name = "SBC", .addressing = Addressing::Imm, .instruction = &CPU::SBC, .cycle = 2, .pageCycle = 0},
    Operation{.name = "CPX", .addressing = Addressing::Abs, .instruction = &CPU::CPX, .cycle = 4, .pageCycle = 0},
    Operation{.name = "SBC", .addressing = Addressing::Abs, .instruction = &CPU::SBC, .cycle = 4, .pageCycle = 0},
    Operation{.name = "INC", .addressing = Addressing::Abs, .instruction = &CPU::INC, .cycle = 6, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Abs, .instruction = &CPU::NIL, .cycle = 6, .pageCycle = 0},
    Operation{.name = "BEQ", .addressing = Addressing::Rel, .instruction = &CPU::BEQ, .cycle = 2, .pageCycle = 1},
    Operation{.name = "SBC", .addressing = Addressing::Izy, .instruction = &CPU::SBC, .cycle = 5, .pageCycle = 1},
    Operation{.name = "???", .addressing = Addressing::Imp, .instruction = &CPU::NIL, .cycle = 2, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Izy, .instruction = &CPU::NIL, .cycle = 8, .pageCycle = 0},
    Operation{.name = "NOP", .addressing = Addressing::Zpx, .instruction = &CPU::NOP, .cycle = 4, .pageCycle = 0},
    Operation{.name = "SBC", .addressing = Addressing::Zpx, .instruction = &CPU::SBC, .cycle = 4, .pageCycle = 0},
    Operation{.name = "INC", .addressing = Addressing::Zpx, .instruction = &CPU::INC, .cycle = 6, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Zpx, .instruction = &CPU::NIL, .cycle = 6, .pageCycle = 0},
    Operation{.name = "SED", .addressing = Addressing::Imp, .instruction = &CPU::SED, .cycle = 2, .pageCycle = 0},
    Operation{.name = "SBC", .addressing = Addressing::Aby, .instruction = &CPU::SBC, .cycle = 4, .pageCycle = 1},
    Operation{.name = "NOP", .addressing = Addressing::Imp, .instruction = &CPU::NOP, .cycle = 2, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Aby, .instruction = &CPU::NIL, .cycle = 7, .pageCycle = 0},
    Operation{.name = "NOP", .addressing = Addressing::Abx, .instruction = &CPU::NOP, .cycle = 4, .pageCycle = 1},
    Operation{.name = "SBC", .addressing = Addressing::Abx, .instruction = &CPU::SBC, .cycle = 4, .pageCycle = 1},
    Operation{.name = "INC", .addressing = Addressing::Abx, .instruction = &CPU::INC, .cycle = 7, .pageCycle = 0},
    Operation{.name = "???", .addressing = Addressing::Abx, .instruction = &CPU::NIL, .cycle = 7, .pageCycle = 0},
};

CPU::CPU(Bus& bus) : bus(&bus) {
    assert(this->bus != nullptr);
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

void CPU::nmi() {
    push16(pc);
    push(status);

    i = 1;

    pc = read16(0xFFFA);

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

void CPU::serialize(std::ostream& os) const {
    auto begin = reinterpret_cast<const char*>(this) + offsetof(CPU, pc);
    auto end = reinterpret_cast<const char*>(this) + offsetof(CPU, bus);
    os.write(begin, end - begin);
}

void CPU::deserialize(std::istream& is) {
    auto begin = reinterpret_cast<char*>(this) + offsetof(CPU, pc);
    auto end = reinterpret_cast<char*>(this) + offsetof(CPU, bus);
    is.read(begin, end - begin);
}

void CPU::testCPU(std::ostream* os, std::uint16_t pc, std::uint32_t totalCycles) {
    this->os = os;
    this->pc = pc;
    this->totalCycles = totalCycles;
}

void CPU::step() {
    opcode = read(pc++);

    if (os) {
        *os << *this << "\n";
    }

    std::uint16_t address = 0;
    bool pageCrossed = false;

    const Operation& op = opTable[opcode];
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
        std::uint16_t ptr = read16(pc);
        pc += 2;

        if ((ptr & 0x00FF) == 0x00FF) {
            address = read(ptr & 0xFF00) << 8 | read(ptr);
        } else {
            address = read(ptr + 1) << 8 | read(ptr);
        }
    } break;
    case Addressing::Izx: {
        std::uint16_t temp = read(pc++);
        address = read((temp + x + 1) & 0x00FF) << 8 | read((temp + x) & 0x00FF);
    } break;
    case Addressing::Izy: {
        std::uint16_t temp = read(pc++);
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

std::uint8_t CPU::read(std::uint16_t addr) {
    assert(bus != nullptr);
    return bus->cpuRead(addr);
}

std::uint16_t CPU::read16(std::uint16_t addr) {
    assert(bus != nullptr);
    return bus->cpuRead16(addr);
}

void CPU::write(std::uint16_t addr, std::uint8_t data) {
    assert(bus != nullptr);
    bus->cpuWrite(addr, data);
}

void CPU::push(std::uint8_t data) {
    write(0x0100 + sp--, data);
}

void CPU::push16(std::uint16_t data) {
    push((data >> 8) & 0x00FF);
    push(data & 0x00FF);
}

std::uint8_t CPU::pop() {
    return read(0x0100 + ++sp);
}

std::uint16_t CPU::pop16() {
    std::uint16_t l = pop();
    std::uint16_t h = pop();

    return (h << 8) | l;
}

void CPU::ADC(std::uint16_t address) {
    std::uint8_t m = read(address);
    std::uint16_t sum = std::uint16_t(a) + m + c;

    c = (sum > 0xFF ? 1 : 0);
    v = ((a ^ sum) & (m ^ sum) & 0x80 ? 1 : 0);

    a = std::uint8_t(sum);
    z = (a == 0 ? 1 : 0);
    n = (((a >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::AND(std::uint16_t address) {
    a = a & read(address);
    z = (a == 0 ? 1 : 0);
    n = (((a >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::ASL(std::uint16_t address) {
    if (addressingMode == Addressing::Acc) {
        c = (a >> 7) & 1;
        a <<= 1;
        z = (a == 0 ? 1 : 0);
        n = (((a >> 7) & 1) == 1 ? 1 : 0);
    } else {
        std::uint8_t m = read(address);
        c = (m >> 7) & 1;
        m <<= 1;
        write(address, m);
        z = (m == 0 ? 1 : 0);
        n = (((m >> 7) & 1) == 1 ? 1 : 0);
    }
}

void CPU::BCC(std::uint16_t address) {
    if (!c) {
        pc += address;
        if (isCrossed(pc, pc - address)) {
            cycles += 2;
        } else {
            cycles += 1;
        }
    }
}

void CPU::BCS(std::uint16_t address) {
    if (c) {
        pc += address;
        if (isCrossed(pc, pc - address)) {
            cycles += 2;
        } else {
            cycles += 1;
        }
    }
}

void CPU::BEQ(std::uint16_t address) {
    if (z) {
        pc += address;
        if (isCrossed(pc, pc - address)) {
            cycles += 2;
        } else {
            cycles += 1;
        }
    }
}

void CPU::BIT(std::uint16_t address) {
    std::uint8_t m = read(address);
    z = (m & a) == 0;
    v = (m >> 6) & 1;
    n = (m >> 7) & 1;
}

void CPU::BMI(std::uint16_t address) {
    if (n) {
        pc += address;
        cycles += (isCrossed(pc, pc - address) ? 2 : 1);
    }
}

void CPU::BNE(std::uint16_t address) {
    if (!z) {
        pc += address;
        cycles += (isCrossed(pc, pc - address) ? 2 : 1);
    }
}

void CPU::BPL(std::uint16_t address) {
    if (!n) {
        pc += address;
        cycles += (isCrossed(pc, pc - address) ? 2 : 1);
    }
}

void CPU::BRK(std::uint16_t address) {
    push16(pc);
    PHP(address);
    pc = read16(0xFFFE);
}

void CPU::BVC(std::uint16_t address) {
    if (!v) {
        pc += address;
        if (isCrossed(pc, pc - address)) {
            cycles += 2;
        } else {
            cycles += 1;
        }
    }
}

void CPU::BVS(std::uint16_t address) {
    if (v) {
        pc += address;
        if (isCrossed(pc, pc - address)) {
            cycles += 2;
        } else {
            cycles += 1;
        }
    }
}

void CPU::CLC(std::uint16_t) {
    c = 0;
}

void CPU::CLD(std::uint16_t) {
    d = 0;
}

void CPU::CLI(std::uint16_t) {
    i = 0;
}

void CPU::CLV(std::uint16_t) {
    v = 0;
}

void CPU::CMP(std::uint16_t address) {
    std::uint8_t m = read(address);
    c = (a >= m ? 1 : 0);
    z = (a == m ? 1 : 0);
    n = ((((a - m) >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::CPX(std::uint16_t address) {
    std::uint8_t m = read(address);
    c = (x >= m ? 1 : 0);
    z = (x == m ? 1 : 0);
    n = ((((x - m) >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::CPY(std::uint16_t address) {
    std::uint8_t m = read(address);
    c = (y >= m ? 1 : 0);
    z = (y == m ? 1 : 0);
    n = ((((y - m) >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::DEC(std::uint16_t address) {
    std::uint8_t m = read(address);
    m--;
    write(address, m);

    z = (m == 0 ? 1 : 0);
    n = (((m >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::DEX(std::uint16_t) {
    x--;
    z = (x == 0 ? 1 : 0);
    n = (((x >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::DEY(std::uint16_t) {
    y--;
    z = (y == 0 ? 1 : 0);
    n = (((y >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::EOR(std::uint16_t address) {
    a = a ^ read(address);
    z = (a == 0 ? 1 : 0);
    n = (((a >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::INC(std::uint16_t address) {
    std::uint8_t m = read(address);
    m++;
    write(address, m);

    z = (m == 0 ? 1 : 0);
    n = (((m >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::INX(std::uint16_t) {
    x++;
    z = (x == 0 ? 1 : 0);
    n = (((x >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::INY(std::uint16_t) {
    y++;
    z = (y == 0 ? 1 : 0);
    n = (((y >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::JMP(std::uint16_t address) {
    pc = address;
}

void CPU::JSR(std::uint16_t address) {
    push16(pc - 1);
    pc = address;
}

void CPU::LDA(std::uint16_t address) {
    a = read(address);
    z = (a == 0 ? 1 : 0);
    n = (((a >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::LDX(std::uint16_t address) {
    x = read(address);
    z = (x == 0 ? 1 : 0);
    n = (((x >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::LDY(std::uint16_t address) {
    y = read(address);
    z = (y == 0 ? 1 : 0);
    n = (((y >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::LSR(std::uint16_t address) {
    if (addressingMode == Addressing::Acc) {
        c = a & 1;
        a >>= 1;
        z = (a == 0 ? 1 : 0);
        n = (((a >> 7) & 1) == 1 ? 1 : 0);
    } else {
        std::uint8_t m = read(address);
        c = m & 1;
        m >>= 1;
        write(address, m);
        z = (m == 0 ? 1 : 0);
        n = (((m >> 7) & 1) == 1 ? 1 : 0);
    }
}

void CPU::NOP(std::uint16_t) {
    // do nothing
}

void CPU::ORA(std::uint16_t address) {
    a = a | read(address);
    z = (a == 0 ? 1 : 0);
    n = (((a >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::PHA(std::uint16_t) {
    push(a);
}

void CPU::PHP(std::uint16_t) {
    push(status | 0x10);
}

void CPU::PLA(std::uint16_t) {
    a = pop();
    z = (a == 0 ? 1 : 0);
    n = (((a >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::PLP(std::uint16_t) {
    status = pop() & 0xEF | 0x20;
}

void CPU::ROL(std::uint16_t address) {
    if (addressingMode == Addressing::Acc) {
        std::uint8_t oldC = c;
        c = (a >> 7) & 1;
        a = (a << 1) | oldC;
        z = (a == 0 ? 1 : 0);
        n = (((a >> 7) & 1) == 1 ? 1 : 0);
    } else {
        std::uint8_t m = read(address);
        std::uint8_t oldC = c;
        c = (m >> 7) & 1;
        m = (m << 1) | oldC;
        write(address, m);
        z = (m == 0 ? 1 : 0);
        n = (((m >> 7) & 1) == 1 ? 1 : 0);
    }
}

void CPU::ROR(std::uint16_t address) {
    if (addressingMode == Addressing::Acc) {
        std::uint8_t oldC = c;
        c = a & 1;
        a = (a >> 1) | (oldC << 7);
        z = (a == 0 ? 1 : 0);
        n = (((a >> 7) & 1) == 1 ? 1 : 0);
    } else {
        std::uint8_t m = read(address);
        std::uint8_t oldC = c;
        c = m & 1;
        m = (m >> 1) | (oldC << 7);
        write(address, m);
        z = (m == 0 ? 1 : 0);
        n = (((m >> 7) & 1) == 1 ? 1 : 0);
    }
}

void CPU::RTI(std::uint16_t) {
    status = pop() & 0xEF | 0x20;
    pc = pop16();
}

void CPU::RTS(std::uint16_t) {
    pc = pop16() + 1;
}

void CPU::SBC(std::uint16_t address) {
    std::uint8_t m = read(address);
    std::uint16_t diff = std::uint16_t(a) - m - (1 - c);

    c = (diff > 0xFF ? 0 : 1);
    v = ((a ^ diff) & (~m ^ diff) & 0x80 ? 1 : 0);

    a = std::uint8_t(diff);
    z = (a == 0 ? 1 : 0);
    n = (((a >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::SEC(std::uint16_t) {
    c = 1;
}

void CPU::SED(std::uint16_t) {
    d = 1;
}

void CPU::SEI(std::uint16_t) {
    i = 1;
}

void CPU::STA(std::uint16_t address) {
    write(address, a);
}

void CPU::STX(std::uint16_t address) {
    write(address, x);
}

void CPU::STY(std::uint16_t address) {
    write(address, y);
}

void CPU::TAX(std::uint16_t) {
    x = a;
    z = (x == 0 ? 1 : 0);
    n = (((x >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::TAY(std::uint16_t) {
    y = a;
    z = (y == 0 ? 1 : 0);
    n = (((y >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::TSX(std::uint16_t) {
    x = sp;
    z = (x == 0 ? 1 : 0);
    n = (((x >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::TXA(std::uint16_t) {
    a = x;
    z = (a == 0 ? 1 : 0);
    n = (((a >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::TXS(std::uint16_t) {
    sp = x;
}

void CPU::TYA(std::uint16_t) {
    a = y;
    z = (a == 0 ? 1 : 0);
    n = (((a >> 7) & 1) == 1 ? 1 : 0);
}

void CPU::NIL(std::uint16_t) {
    // unimplemented
}
