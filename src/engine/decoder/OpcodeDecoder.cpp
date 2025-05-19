#include "OpcodeDecoder.h"
#include <stdexcept>
#include <string>

namespace DarkMatterVM 
{
namespace Engine 
{

Opcode OpcodeDecoder::DecodeOpcode(const uint8_t* bytecode) const 
{
    if (!bytecode) 
    {
        throw std::invalid_argument("Bytecode pointer is null");
    }

    return static_cast<Opcode>(*bytecode);
}

size_t OpcodeDecoder::GetInstructionSize(Opcode opcode) const 
{
    // 기본 크기는 opcode 자체를 위한 1바이트
    size_t size = 1;
    
    // opcode에 따른 operand 크기 추가
    const OpcodeInfo& info = GetOpcodeInfo(opcode);
    size += info.operandSize;
    
    return size;
}

uint64_t OpcodeDecoder::DecodeOperand(const uint8_t* bytecode, size_t size) const 
{
    if (!bytecode) 
    {
        throw std::invalid_argument("Bytecode pointer is null");
    }
    
    // 리틀 엔디안으로 디코딩
    uint64_t value = 0;
    
    switch (size) 
    {
        case 1:
            value = static_cast<uint64_t>(*bytecode);
            break;
        case 2:
            value = static_cast<uint64_t>(bytecode[0]) | 
                   (static_cast<uint64_t>(bytecode[1]) << 8);
            break;
        case 4:
            value = static_cast<uint64_t>(bytecode[0]) | 
                   (static_cast<uint64_t>(bytecode[1]) << 8) |
                   (static_cast<uint64_t>(bytecode[2]) << 16) |
                   (static_cast<uint64_t>(bytecode[3]) << 24);
            break;
        case 8:
            value = static_cast<uint64_t>(bytecode[0]) | 
                   (static_cast<uint64_t>(bytecode[1]) << 8) |
                   (static_cast<uint64_t>(bytecode[2]) << 16) |
                   (static_cast<uint64_t>(bytecode[3]) << 24) |
                   (static_cast<uint64_t>(bytecode[4]) << 32) |
                   (static_cast<uint64_t>(bytecode[5]) << 40) |
                   (static_cast<uint64_t>(bytecode[6]) << 48) |
                   (static_cast<uint64_t>(bytecode[7]) << 56);
            break;
        default:
            throw std::invalid_argument("Invalid operand size: " + std::to_string(size));
    }
    
    return value;
}

} // namespace Engine
} // namespace DarkMatterVM
