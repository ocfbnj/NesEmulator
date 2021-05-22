#ifndef CPU_H
#define CPU_H

#include <cstdint>

class Bus;

class Cpu {
public:
    Cpu(Bus& bus);

    void clock();
    void reset();
    void irq();
    void nmi();

private:
    void step();
    void push(uint8_t data);
    void push16(uint16_t data);
    uint8_t pop();
    uint16_t pop16();
    uint8_t fetch();

    // TODO
    // See http://obelisk.me.uk/6502/addressing.html
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

    // TODO
    // See http://obelisk.me.uk/6502/reference.html
    enum Opcodes {
        ADC,
        AND,
        ASL,
        BCC,
        BCS,
        BEQ,
        BIT,
        BMI,
        BNE,
        BPL,
        BRK,
        BVC,
        BVS,
        CLC,
        CLD,
        CLI,
        CLV,
        CMP,
        CPX,
        CPY,
        DEC,
        DEX,
        DEY,
        EOR,
        INC,
        INX,
        INY,
        JMP,
        JSR,
        LDA,
        LDX,
        LDY,
        LSR,
        NOP,
        ORA,
        PHA,
        PHP,
        PLA,
        PLP,
        ROL,
        ROR,
        RTI,
        RTS,
        SBC,
        SEC,
        SED,
        SEI,
        STA,
        STX,
        STY,
        TAX,
        TAY,
        TSX,
        TXA,
        TXS,
        TYA,
    };

    Bus& bus;
    uint8_t cycles;

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
};

#endif
