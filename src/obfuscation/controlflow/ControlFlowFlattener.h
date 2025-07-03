#pragma once

#include <vector>
#include <cstdint>

namespace DarkMatterVM::Obfuscation::ControlFlow {

/**
 * @brief 제어 흐름 평탄화(Flattening) 기법 적용 클래스
 * 
 * 바이트코드 내의 분기 구조를 제거하고,
 * 단일 반복문 + 상태 전이 방식으로 대체하여
 * 역공학 난이도를 높이는 역할을 합니다.
 */
class ControlFlowFlattener {
public:
    ControlFlowFlattener() = delete;

    /**
     * @brief 바이트코드에 제어 흐름 평탄화 알고리즘 적용
     * 
     * @param bytecode 원본 바이트코드 시퀀스
     * @return 평탄화가 적용된 새로운 바이트코드 시퀀스
     */
    static std::vector<uint8_t> Flatten(const std::vector<uint8_t>& bytecode);

private:
    // (필요시) 내부 헬퍼 함수 선언
    // static void _encodeJumpTable(...);
};

} // namespace DarkMatterVM::Obfuscation::ControlFlow
