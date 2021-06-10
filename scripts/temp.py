import re


def get_list(pattern, string):
    ret = re.findall(pattern, string)[0]
    ret = ret.replace('"', '')
    ret = ret.split(",")
    ret.pop()

    return ret


def map_addressing(addressing_modes):
    m = {
        "1": "Abs",
        "2": "Abx",
        "3": "Aby",
        "4": "Acc",
        "5": "Imm",
        "6": "Imp",
        "7": "Izx",
        "8": "Ind",
        "9": "Izy",
        "10": "Rel",
        "11": "Zp0",
        "12": "Zpx",
        "13": "Zpy",
    }
    ret = []
    for addressing_mode in addressing_modes:
        ret.append(m[addressing_mode.strip()])
    return ret


def get_result(names, addressing_modes, instructions, cycles, page_cycles):
    table = ["ADC", "AND", "ASL", "BCC", "BCS", "BEQ", "BIT", "BMI", "BNE", "BPL", "BRK", "BVC", "BVS", "CLC", "CLD", "CLI", "CLV", "CMP", "CPX", "CPY", "DEC", "DEX", "DEY", "EOR", "INC", "INX", "INY", "JMP",
             "JSR", "LDA", "LDX", "LDY", "LSR", "NOP", "ORA", "PHA", "PHP", "PLA", "PLP", "ROL", "ROR", "RTI", "RTS", "SBC", "SEC", "SED", "SEI", "STA", "STX", "STY", "TAX", "TAY", "TSX", "TXA", "TXS", "TYA"]

    res = ""
    for i in range(256):
        if names[i].strip() not in table:
            names[i] = "???"
            instructions[i] = "&Cpu::NIL"

        # {"BRK", Imp, &Cpu::BRK, 0, 0},
        res += f'Operate{{"{names[i].strip()}", {addressing_modes[i].strip()}, {instructions[i].strip()}, {cycles[i].strip()}, {page_cycles[i].strip()}}},\n'
    return res


# Generate the CPU operation table in C++.
# struct Operate {
#     std::string_view name;
#     Addressing addressing;
#     void (Cpu::*instruction)(uint16_t);
#     uint8_t cycle;
#     uint8_t pageCycle;
# };
if __name__ == "__main__":
    # this file is partly copied from https://github.com/fogleman/nes/blob/master/nes/cpu.go
    with open(r"./temp") as file:
        f = file.read()
        f = f.replace("\n", "")
        f = f.replace("\t", "")

        # names
        names = get_list(r"var instructionNames = \[256\]string{(.*?)}", f)
        print(names)

        # addressing modes
        addressing_modes = get_list(
            r"var instructionModes = \[256\]byte{(.*?)}", f)
        addressing_modes = map_addressing(addressing_modes)
        print(addressing_modes)

        # instructions
        instructions = []
        for name in names:
            instructions.append(f"&Cpu::{name.strip()}")
        print(instructions)

        # cycles
        cycles = get_list(r"var instructionCycles = \[256\]byte{(.*?)}", f)
        print(cycles)

        # page cycles
        page_cycles = get_list(
            r"var instructionPageCycles = \[256\]byte{(.*?)}", f)
        print(page_cycles)

        # result
        result = get_result(names, addressing_modes,
                            instructions, cycles, page_cycles)
        print(result)
