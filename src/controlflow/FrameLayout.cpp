#include "FrameLayout.h"
#include <limits>
#include <stdexcept>

namespace DarkMatterVM::ControlFlow 
{

size_t FrameLayout::GetLocalVarAddress(size_t frameBase, size_t varIndex) {
    // 8바이트 단위
    constexpr size_t WordSize = sizeof(uint64_t);

    // overflow 방지
    if (varIndex > (std::numeric_limits<size_t>::max() - LocalAreaOffset - frameBase) / WordSize) 
    {
        throw std::overflow_error("FrameLayout: local variable index overflow");
    }

    return frameBase + LocalAreaOffset + varIndex * WordSize;
    // address = frameBase
    //     + LocalAreaOffset        // 헤더 크기
    //     + varIndex * 8           // varIndex번째 로컬 변수

}

} // namespace DarkMatterVM::ControlFlow
