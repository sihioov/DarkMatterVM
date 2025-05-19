#pragma once
#include <cstdint>
#include <mutex>
#include "MemoryManager.h"

namespace DarkMatterVM {
namespace Memory {

/**
 * @brief 스택 메모리
 * 
 * VM의 스택 메모리를 관리하는 클래스
 */
class StackMemory {
public:
    /**
     * @brief 스택 메모리 생성
     * 
     * @param segment 스택 세그먼트 참조
     */
    StackMemory(MemorySegment& segment);
    
    /**
     * @brief 스택 메모리 소멸자
     */
    ~StackMemory() = default;
    
    /**
     * @brief 스택 포인터 설정
     * 
     * @param stackPointer 스택 포인터 위치
     */
    void SetStackPointer(size_t stackPointer);
    
    /**
     * @brief 현재 스택 포인터 위치 조회
     * 
     * @return size_t 스택 포인터 위치
     */
    size_t GetStackPointer() const;
    
    /**
     * @brief 스택에 값 푸시
     * 
     * @param value 푸시할 값
     */
    void PushStack(uint64_t value);
    
    /**
     * @brief 스택에서 값 팝
     * 
     * @return uint64_t 팝한 값
     */
    uint64_t PopStack();
    
    /**
     * @brief 스택 최상위 값 조회 (팝하지 않음)
     * 
     * @return uint64_t 스택 최상위 값
     */
    uint64_t PeekStack() const;
    
    /**
     * @brief 스택 특정 위치 값 조회
     * 
     * @param offset 스택 상단에서의 오프셋 (0 = 최상위)
     * @return uint64_t 해당 위치의 값
     */
    uint64_t GetStackValue(size_t offset) const;
    
    /**
     * @brief 스택 프레임 정보 저장
     * 
     * @param basePointer 이전 베이스 포인터
     * @param returnAddress 반환 주소
     */
    void PushStackFrame(size_t basePointer, size_t returnAddress);
    
    /**
     * @brief 스택 프레임 정보 복원
     * 
     * @param basePointer 복원할 베이스 포인터 주소
     * @param returnAddress 복원할 반환 주소
     */
    void PopStackFrame(size_t& basePointer, size_t& returnAddress);
    
private:
    MemorySegment& _segment;          ///< 스택 세그먼트 참조
    size_t _stackPointer;             ///< 현재 스택 포인터 위치
    mutable std::mutex _stackMutex;   ///< 스택 접근 동기화를 위한 뮤텍스
    
    /**
     * @brief 스택 경계 확인
     * 
     * @param offset 확인할 스택 오프셋
     * @throw MemoryAccessException 스택 범위 초과 시
     */
    void _ValidateStack(size_t offset) const;
};

} // namespace Memory
} // namespace DarkMatterVM 