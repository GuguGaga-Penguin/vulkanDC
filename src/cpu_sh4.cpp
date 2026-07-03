#include <emscripten/emscripten.h>
#include <cstdint>

struct SH4_Registers {
    uint32_t R[16];    // 16 General Purpose 32-bit Registers
    uint32_t PC;       // Program Counter
    uint32_t PR;       // Procedure Register (Return Address)
    uint32_t SR;       // Status Register
    uint32_t MACL;     // Multiply-Accumulate Low Register
    uint32_t MACH;     // Multiply-Accumulate High Register
};

SH4_Registers cpu_regs;
extern uint8_t system_ram[16 * 1024 * 1024];

// Fast linear safe memory reader wrapping inside system ram allocations
inline uint16_t fetch_instruction(uint32_t address) {
    // Mask addresses to map inside our standard 16MB Main RAM execution bounds
    uint32_t masked_addr = address & 0x00FFFFFF; 
    return *reinterpret_cast<uint16_t*>(&system_ram[masked_addr]);
}

void execute_opcode(uint16_t opcode) {
    // Extract common bitfields used by SH-4 instructions
    uint32_t n = (opcode >> 8) & 0x0F; // Destination register identifier
    uint32_t m = (opcode >> 4) & 0x0F; // Source register identifier
    uint32_t imm = opcode & 0xFF;      // 8-bit Immediate Value

    // High 4-bit nibble defines the instruction group category
    switch ((opcode >> 12) & 0x0F) {
        case 0x6: // Logical / Data Move structural instruction sets
            switch (opcode & 0x0F) {
                case 0x03: // MOV.L @Rm, Rn (Load 32-bit value from memory pointer)
                    cpu_regs.R[n] = *reinterpret_cast<uint32_t*>(&system_ram[cpu_regs.R[m] & 0x00FFFFFF]);
                    cpu_regs.PC += 2;
                    break;
                default:
                    cpu_regs.PC += 2; // Advanced fallback skip
                    break;
            }
            break;

        case 0x7: // ADD #imm, Rn
            cpu_regs.R[n] += static_cast<int32_t>(static_cast<int8_t>(imm));
            cpu_regs.PC += 2;
            break;

        case 0xE: // MOV #imm, Rn (Load signed immediate values straight to register)
            cpu_regs.R[n] = static_cast<int32_t>(static_cast<int8_t>(imm));
            cpu_regs.PC += 2;
            break;

        case 0xA: // BRA label (Branch to target address with 12-bit relative spacing)
            {
                int32_t disp = opcode & 0x0FFF;
                if (disp & 0x0800) disp |= 0xFFFFF000; // Sign-extend 12-bit displacement
                
                // SH-4 features a branch delay slot: it executes the *next* instruction 
                // before the branch officially takes place.
                uint16_t delay_slot_opcode = fetch_instruction(cpu_regs.PC + 2);
                execute_opcode(delay_slot_opcode);
                
                cpu_regs.PC = cpu_regs.PC + 4 + (disp << 1);
            }
            break;

        default:
            // Unimplemented opcode fallback placeholder to prevent full core crash loops
            cpu_regs.PC += 2;
            break;
    }
}
