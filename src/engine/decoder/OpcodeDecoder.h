#pragma once

#include <cstdint>
#include "../../../include/Opcodes.h"

namespace DarkMatterVM {
namespace Engine {

/**
 * @brief 바이트코드 명령어 디코더
 * 
 * 바이트코드 스트림을 opcode와 operand로 디코딩하는 역할
 */
class OpcodeDecoder {
public:
    OpcodeDecoder() = default;
    ~OpcodeDecoder() = default;

    /**
     * @brief 바이트코드에서 opcode 읽기
     * 
     * @param bytecode 바이트코드 포인터
     * @return Opcode 디코딩된 opcode
     */
    Opcode DecodeOpcode(const uint8_t* bytecode) const;
    
    /**
     * @brief 명령어의 총 크기 계산 (opcode + operands)
     * 
     * @param opcode 명령어 opcode
     * @return size_t 총 바이트 크기
     */
    size_t GetInstructionSize(Opcode opcode) const;
    
    /**
     * @brief 바이트코드에서 operand 읽기
     * 
     * @param bytecode 바이트코드 포인터 (opcode 이후 위치)
     * @param size 읽을 operand 크기 (1, 2, 4, 8 바이트)
     * @return uint64_t 디코딩된 operand 값
     */
    uint64_t DecodeOperand(const uint8_t* bytecode, size_t size) const;
};

} // namespace Engine
} // namespace DarkMatterVM
