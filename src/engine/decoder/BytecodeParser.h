#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include "../../../include/Opcodes.h"
#include "OpcodeDecoder.h"

namespace DarkMatterVM {
namespace Engine {

/**
 * @brief 바이트코드 파서 클래스
 * 
 * 원시 바이트코드 스트림을 파싱하고, 명령어와 피연산자를 추출하는 역할
 * 인터프리터의 명령어 실행 파이프라인에서 첫 단계(fetch)를 담당
 */
class BytecodeParser {
public:
    /**
     * @brief 생성자
     * 
     * @param bytecode 바이트코드 버퍼
     * @param size 바이트코드 크기
     */
    BytecodeParser(const uint8_t* bytecode, size_t size);
    
    /**
     * @brief 소멸자
     */
    ~BytecodeParser() = default;
    
    /**
     * @brief 현재 위치에서 opcode 파싱
     * 
     * @param offset 현재 명령어 포인터에서의 오프셋
     * @return Opcode 파싱된 opcode
     */
    Opcode ParseOpcode(size_t offset = 0) const;
    
    /**
     * @brief 현재 위치 다음에서 operand 파싱
     * 
     * @param offset 현재 명령어 포인터에서의 오프셋
     * @param size operand 크기 (바이트 단위)
     * @return uint64_t 파싱된 operand 값
     */
    uint64_t ParseOperand(size_t offset, size_t size) const;
    
    /**
     * @brief 명령어 전체 크기 반환
     * 
     * @param offset 현재 명령어 포인터에서의 오프셋
     * @return size_t 명령어 전체 크기 (opcode + operands)
     */
    size_t GetInstructionSize(size_t offset) const;
    
    /**
     * @brief 바이트코드 버퍼 전체 크기 반환
     * 
     * @return size_t 바이트코드 전체 크기
     */
    size_t Size() const { return _size; }
    
    /**
     * @brief 현재 위치의 바이트코드 포인터 반환
     * 
     * @param offset 현재 명령어 포인터에서의 오프셋
     * @return const uint8_t* 바이트코드 포인터
     */
    const uint8_t* GetBytecodePtr(size_t offset = 0) const;
    
private:
    // 바이트코드 버퍼
    const uint8_t* _bytecode;
    
    // 바이트코드 크기
    size_t _size;
    
    // Opcode 디코더
    OpcodeDecoder _decoder;
    
    /**
     * @brief 유효한 오프셋인지 검증
     * 
     * @param offset 검증할 오프셋
     * @param requiredSize 필요한 추가 크기
     */
    void _ValidateOffset(size_t offset, size_t requiredSize = 1) const;
};

} // namespace Engine
} // namespace DarkMatterVM 