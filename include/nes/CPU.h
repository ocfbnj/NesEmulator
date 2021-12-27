#ifndef OCFBNJ_NES_CPU_H
#define OCFBNJ_NES_CPU_H

#include <array>
#include <cstdint>
#include <istream>
#include <ostream>

class Bus;

class CPU {
public:
    CPU() = default;
    CPU(const CPU&) = delete;
    CPU& operator=(const CPU&) = delete;
    CPU(CPU&&) = default;
    CPU& operator=(CPU&&) = default;

    void connect(Bus* bus);

    void clock();
    void reset();
    void nmi();
    void irq();

    void serialize(std::ostream& os) const;
    void deserialize(std::istream& is);

    // for test
    void setPc(std::uint16_t newPc);
    std::string debugStr();

private:
    // Addressing Mode
    // See http://obelisk.me.uk/6502/addressing.html
    enum class Addressing {
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

    struct Operation {
        const char* name;
        Addressing addressing;
        void (CPU::*instruction)(std::uint16_t);
        std::uint8_t cycle;
        std::uint8_t pageCycle;
    };

    struct StatusRegister {
        union {
            struct {
                std::uint8_t c : 1; // carry flag
                std::uint8_t z : 1; // zero flag
                std::uint8_t i : 1; // interrupt disable flag
                std::uint8_t d : 1; // decimal mode flag
                std::uint8_t B : 2; // break command flag
                std::uint8_t v : 1; // overflow flag
                std::uint8_t n : 1; // negative flag
            };
            std::uint8_t reg;
        };
    };

    void step();

    std::uint8_t read(std::uint16_t addr);
    std::uint16_t read16(std::uint16_t addr);
    void write(std::uint16_t addr, std::uint8_t data);

    void push(std::uint8_t data);
    void push16(std::uint16_t data);
    std::uint8_t pop();
    std::uint16_t pop16();

    // Assembly Instructions
    // See http://obelisk.me.uk/6502/reference.html
    // The function name is capitalized because we cannot use `and` as the function name.
    void ADC(std::uint16_t address);
    void AND(std::uint16_t address);
    void ASL(std::uint16_t address);
    void BCC(std::uint16_t address);
    void BCS(std::uint16_t address);
    void BEQ(std::uint16_t address);
    void BIT(std::uint16_t address);
    void BMI(std::uint16_t address);
    void BNE(std::uint16_t address);
    void BPL(std::uint16_t address);
    void BRK(std::uint16_t address);
    void BVC(std::uint16_t address);
    void BVS(std::uint16_t address);
    void CLC(std::uint16_t address);
    void CLD(std::uint16_t address);
    void CLI(std::uint16_t address);
    void CLV(std::uint16_t address);
    void CMP(std::uint16_t address);
    void CPX(std::uint16_t address);
    void CPY(std::uint16_t address);
    void DEC(std::uint16_t address);
    void DEX(std::uint16_t address);
    void DEY(std::uint16_t address);
    void EOR(std::uint16_t address);
    void INC(std::uint16_t address);
    void INX(std::uint16_t address);
    void INY(std::uint16_t address);
    void JMP(std::uint16_t address);
    void JSR(std::uint16_t address);
    void LDA(std::uint16_t address);
    void LDX(std::uint16_t address);
    void LDY(std::uint16_t address);
    void LSR(std::uint16_t address);
    void NOP(std::uint16_t address);
    void ORA(std::uint16_t address);
    void PHA(std::uint16_t address);
    void PHP(std::uint16_t address);
    void PLA(std::uint16_t address);
    void PLP(std::uint16_t address);
    void ROL(std::uint16_t address);
    void ROR(std::uint16_t address);
    void RTI(std::uint16_t address);
    void RTS(std::uint16_t address);
    void SBC(std::uint16_t address);
    void SEC(std::uint16_t address);
    void SED(std::uint16_t address);
    void SEI(std::uint16_t address);
    void STA(std::uint16_t address);
    void STX(std::uint16_t address);
    void STY(std::uint16_t address);
    void TAX(std::uint16_t address);
    void TAY(std::uint16_t address);
    void TSX(std::uint16_t address);
    void TXA(std::uint16_t address);
    void TXS(std::uint16_t address);
    void TYA(std::uint16_t address);
    void NIL(std::uint16_t address);

    // Instruction Set
    // See https://www.masswerk.at/6502/6502_instruction_set.html
    static std::array<Operation, 256> opTable;

    // The following member variables are CPU components, they can be used to serialize and deserialize.

    // CPU Components Begin
    // CPU Registers
    // See http://wiki.nesdev.com/w/index.php/CPU_registers
    std::uint16_t pc = 0;   // program counter
    std::uint8_t a = 0;     // accumulator
    std::uint8_t x = 0;     // x register
    std::uint8_t y = 0;     // y register
    std::uint8_t sp = 0xFD; // stack pointer

    StatusRegister status{.reg = 0x24};

    std::uint8_t cycles = 8;
    // CPU Components End

    Bus* bus = nullptr;

    // for convenience
    std::uint8_t opcode = 0;

    // for debug
    std::uint32_t totalCycles = 7;
};

#endif // OCFBNJ_NES_CPU_H
