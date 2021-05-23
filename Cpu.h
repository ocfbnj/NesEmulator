#ifndef CPU_H
#define CPU_H

#include <array>
#include <cstdint>
#include <string_view>

class Bus;

class Cpu {
public:
    explicit Cpu(Bus& bus);

    void clock();
    void reset();
    void irq();
    void nmi();

private:
    // Addressing Mode
    // See http://obelisk.me.uk/6502/addressing.html
    // TODO We should use `enum class` instead of `enum`.
    // In C++20, we can use `using enum` to reduce repetitive code.
    // See https://en.cppreference.com/w/cpp/language/enum#Using-enum-declaration
    enum Addressing {
        Imp,
        Acc,
        Imm,
        Zp0,
        Zpx,
        Zpy,
        Rel,
        Abs,
        Abx,
        Aby,
        Ind,
        Izx,
        Izy,
    };

    struct Operate {
        std::string_view name;
        Addressing addressing;
        void (Cpu::*instruction)(uint16_t);
        uint8_t cycle;
        uint8_t pageCycle;
    };

    void step();
    void push(uint8_t data);
    void push16(uint16_t data);
    uint8_t pop();
    uint16_t pop16();
    uint8_t getStatus();
    void setStatus(uint8_t status);

    // Assembly Instructions
    // See http://obelisk.me.uk/6502/reference.html
    // The function name is capitalized because we cannot use `and` as the function name.
    void ADC(uint16_t address);
    void AND(uint16_t address);
    void ASL(uint16_t address);
    void BCC(uint16_t address);
    void BCS(uint16_t address);
    void BEQ(uint16_t address);
    void BIT(uint16_t address);
    void BMI(uint16_t address);
    void BNE(uint16_t address);
    void BPL(uint16_t address);
    void BRK(uint16_t address);
    void BVC(uint16_t address);
    void BVS(uint16_t address);
    void CLC(uint16_t address);
    void CLD(uint16_t address);
    void CLI(uint16_t address);
    void CLV(uint16_t address);
    void CMP(uint16_t address);
    void CPX(uint16_t address);
    void CPY(uint16_t address);
    void DEC(uint16_t address);
    void DEX(uint16_t address);
    void DEY(uint16_t address);
    void EOR(uint16_t address);
    void INC(uint16_t address);
    void INX(uint16_t address);
    void INY(uint16_t address);
    void JMP(uint16_t address);
    void JSR(uint16_t address);
    void LDA(uint16_t address);
    void LDX(uint16_t address);
    void LDY(uint16_t address);
    void LSR(uint16_t address);
    void NOP(uint16_t address);
    void ORA(uint16_t address);
    void PHA(uint16_t address);
    void PHP(uint16_t address);
    void PLA(uint16_t address);
    void PLP(uint16_t address);
    void ROL(uint16_t address);
    void ROR(uint16_t address);
    void RTI(uint16_t address);
    void RTS(uint16_t address);
    void SBC(uint16_t address);
    void SEC(uint16_t address);
    void SED(uint16_t address);
    void SEI(uint16_t address);
    void STA(uint16_t address);
    void STX(uint16_t address);
    void STY(uint16_t address);
    void TAX(uint16_t address);
    void TAY(uint16_t address);
    void TSX(uint16_t address);
    void TXA(uint16_t address);
    void TXS(uint16_t address);
    void TYA(uint16_t address);

    // unimplemented
    void NIL(uint16_t address);

    // Instruction Set
    // See https://www.masswerk.at/6502/6502_instruction_set.html
    static std::array<Operate, 256> opTable;

    // CPU Registers
    // See http://wiki.nesdev.com/w/index.php/CPU_registers
    uint16_t pc;   // program counter
    uint8_t a;     // accumulator
    uint8_t x;     // x register
    uint8_t y;     // y register
    uint8_t sp;    // stack pointer
    uint8_t c : 1; // carry flag
    uint8_t z : 1; // zero flag
    uint8_t i : 1; // interrupt disable flag
    uint8_t d : 1; // decimal mode flag
    uint8_t b : 1; // break command flag
    uint8_t u : 1; // unused flag
    uint8_t v : 1; // overflow flag
    uint8_t n : 1; // negative flag

    Bus& bus;
    uint8_t cycles;
    uint32_t totalCycles{7};
    Addressing addressingMode;
};

#endif
