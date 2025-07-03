#include "ControlFlowFlattener.h"
#include <stdexcept>

namespace DarkMatterVM::Obfuscation::ControlFlow 
{

// helper: little-endian 16bit 읽기
static uint16_t read_u16(const std::vector<uint8_t>& code, size_t pos) 
{
    return uint16_t(code[pos]) |
           (uint16_t(code[pos+1]) << 8);
}

std::vector<uint8_t> ControlFlowFlattener::Flatten(const std::vector<uint8_t>& bytecode)
{
    std::vector<uint8_t> out;
    out.reserve(bytecode.size());

    size_t ip = 0, n = bytecode.size();
    while (ip < n) 
    {
        uint8_t op = bytecode[ip];

        if (op == 0x30) // JMP rel16 
        { 
            if (ip + 3 > n) 
            {
                throw std::runtime_error("Flatten: truncated JMP operand");
            }
            // 상대점프 오프셋 읽고
            uint16_t rel = read_u16(bytecode, ip + 1);
            // 다음 위치 = ip + 3 + rel
            size_t target = ip + 3 + rel;
            if (target >= n) 
            {
                throw std::runtime_error("Flatten: JMP target out of range");
            }
            // ip를 곧장 대상 블록으로 옮김 (JMP 제거 → 인라이닝)
            ip = target;
        }
        else 
        {
            // operand 길이 결정 (간단한 예)
            size_t operand = 0;
            switch (op) 
            {
                case 0x01: operand = 1; break; // PUSH8
                case 0x02: operand = 2; break; // PUSH16
                case 0x03: operand = 4; break; // PUSH32
                case 0x04: operand = 8; break; // PUSH64
                case 0x31: // JZ rel16
                case 0x32: // JNZ rel16
                case 0x33: // JG rel16
                case 0x34: // JL rel16
                case 0x35: // JGE rel16
                case 0x36: // JLE rel16
                case 0x40: // CALL rel16
                    operand = 2;
                    break;
                case 0x50: // ALLOC imm8
                case 0x60: // HOSTCALL imm8
                    operand = 1;
                    break;
                default:
                    operand = 0; break;
            }
            // opcode + operand 복사
            out.push_back(op);
            for (size_t i = 1; i <= operand; ++i) 
            {
                if (ip + i >= n) 
                    throw std::runtime_error("Flatten: truncated operand");
                    
                out.push_back(bytecode[ip + i]);
            }
            ip += 1 + operand;
        }
    }

    return out;
}

} // namespace DarkMatterVM::Obfuscation::ControlFlow
