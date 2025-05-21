#pragma once

#include <cstddef>
#include <cstdint>
#include "../Memory/MemoryManager.h"
#include "FrameLayout.h"

namespace DarkMatterVM::ControlFlow {

/**
 * @brief 런타임 제어 흐름 관리
 *
 * CALL/RET, JMP, 조건 분기 등을 처리하고,
 * 스택 프레임을 MemoryManager와 FrameLayout에 따라 관리합니다.
 */
class ControlFlowManager 
{
public:
    /**
     * @param mem  스택·힙 메모리 관리 객체
     */
    explicit ControlFlowManager(Memory::MemoryManager& mem);

    /// 무조건 분기
    void Jump(size_t& ip, int16_t rel) const;

    /// 스택에서 값을 꺼내 0이면 분기
    void JZ(size_t& ip, int16_t rel, uint64_t value) const;
    /// 스택에서 값을 꺼내 0이 아니면 분기
    void JNZ(size_t& ip, int16_t rel, uint64_t value) const;

    /**
     * @brief 함수 호출 (CALL)
     * @param ip   다음 실행할 명령어의 주소 (CALL 다음 위치)
     * @param rel  상대 오프셋
     */
    void Call(size_t& ip, int16_t rel);

    /**
     * @brief 함수 반환 (RET)
     * @param ip  복원된 리턴 주소를 여기에 기록
     */
    void Ret(size_t& ip);

    /// 현재 프레임의 베이스 포인터 조회
    size_t GetBasePointer() const { return _currentBP; }

private:
    Memory::MemoryManager& _memory;
    size_t                 _currentBP = 0;
};

} // namespace DarkMatterVM::ControlFlow
