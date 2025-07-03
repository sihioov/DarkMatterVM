#pragma once

#include <cstddef>
#include <cstdint>

namespace DarkMatterVM::ControlFlow 
{

/**
 * @brief 스택 프레임 레이아웃 정의
 *
 * ┌────────────────────────────┐
 * │ ... 이전 프레임 데이터 ... │
 * ├────────────────────────────┤
 * │ return address  (8 bytes)  │  ← frameBase + ReturnAddressOffset
 * ├────────────────────────────┤
 * │ old base pointer (8 bytes) │  ← frameBase + BasePointerOffset
 * ├────────────────────────────┤
 * │  로컬 변수 영역 시작      │  ← frameBase + LocalAreaOffset
 * ├────────────────────────────┤
 * │   로컬[0] (8 bytes)        │
 * │   로컬[1] (8 bytes)        │
 * │   …                        │
 * └────────────────────────────┘
 */
class FrameLayout 
{
public:
    /// 리턴 주소 오프셋 (프레임 베이스에서)
    static constexpr size_t ReturnAddressOffset = 0;

    /// 이전 베이스 포인터 오프셋
    static constexpr size_t BasePointerOffset = ReturnAddressOffset + sizeof(uint64_t);

    /// 로컬 변수 영역 시작 오프셋
    static constexpr size_t LocalAreaOffset = BasePointerOffset + sizeof(uint64_t);

    /// 프레임 헤더 전체 크기 (로컬 변수 시작까지)
    static constexpr size_t HeaderSize = LocalAreaOffset;

    /**
     * @brief 이 프레임에서 varIndex번째 로컬 변수의 절대 주소 계산
     * @param frameBase  스택 프레임 베이스(새 BP)
     * @param varIndex   로컬 변수 인덱스 (0부터)
     * @return 메모리 상의 주소 (frameBase + LocalAreaOffset + varIndex*8)
     */
    static size_t GetLocalVarAddress(size_t frameBase, size_t varIndex);
};

} // namespace DarkMatterVM::ControlFlow
