#pragma once
#include <cstdint>

namespace DarkMatterVM {
namespace Engine {

/**
 * @brief DarkMatterVM 명령어 집합
 * 
 * 이 enum은 DarkMatterVM에서 지원하는 모든 명령어를 정의.
 * 각 명령어는 1바이트 값으로, 이후에 오퍼랜드(있는 경우)가 따라옴
 * Stack 기반 아키텍처 사용
 */
enum class Opcode : uint8_t {
    // Stack Operations
    PUSH8       = 0x01, ///< 1바이트 즉시값을 스택에 푸시
    PUSH16      = 0x02, ///< 2바이트 즉시값을 스택에 푸시
    PUSH32      = 0x03, ///< 4바이트 즉시값을 스택에 푸시
    PUSH64      = 0x04, ///< 8바이트 즉시값을 스택에 푸시
    POP         = 0x05, ///< 스택에서 최상위 값 제거
    DUP         = 0x06, ///< 스택 최상위 값 복제
    SWAP        = 0x07, ///< 스택 최상위 두 값 교환

    // Arithmetic Operations
    ADD         = 0x10, ///< 두 값 더하기: stack[sp-1] + stack[sp-2]
    SUB         = 0x11, ///< 빼기: stack[sp-2] - stack[sp-1]
    MUL         = 0x12, ///< 곱하기: stack[sp-2] * stack[sp-1]
    DIV         = 0x13, ///< 나누기: stack[sp-2] / stack[sp-1]
    MOD         = 0x14, ///< 나머지: stack[sp-2] % stack[sp-1]
    
    // Bitwise Operations
    AND         = 0x15, ///< 비트 AND 연산
    OR          = 0x16, ///< 비트 OR 연산
    XOR         = 0x17, ///< 비트 XOR 연산
    NOT         = 0x18, ///< 비트 NOT 연산 (단항)
    SHL         = 0x19, ///< 왼쪽 시프트
    SHR         = 0x1A, ///< 오른쪽 시프트
    
    // Memory Operations
    LOAD8       = 0x20, ///< 스택의 주소에서 1바이트 로드
    LOAD16      = 0x21, ///< 스택의 주소에서 2바이트 로드
    LOAD32      = 0x22, ///< 스택의 주소에서 4바이트 로드
    LOAD64      = 0x23, ///< 스택의 주소에서 8바이트 로드
    STORE8      = 0x24, ///< 스택의 주소에 1바이트 저장
    STORE16     = 0x25, ///< 스택의 주소에 2바이트 저장
    STORE32     = 0x26, ///< 스택의 주소에 4바이트 저장
    STORE64     = 0x27, ///< 스택의 주소에 8바이트 저장
    
    // Control Flow Operations
    JMP         = 0x30, ///< 무조건 점프 (상대적, ±2바이트 오프셋)
    JZ          = 0x31, ///< 0이면 점프
    JNZ         = 0x32, ///< 0이 아니면 점프
    JG          = 0x33, ///< 크면 점프
    JL          = 0x34, ///< 작으면 점프
    JGE         = 0x35, ///< 크거나 같으면 점프
    JLE         = 0x36, ///< 작거나 같으면 점프
    
    // Function Operations
    CALL        = 0x40, ///< 함수 호출 (반환 주소 푸시)
    RET         = 0x41, ///< 함수에서 반환
    
    // Memory Allocation
    ALLOC       = 0x50, ///< 힙에 메모리 할당, 주소를 스택에 푸시
    FREE        = 0x51, ///< 이전에 할당된 메모리 해제
    
    // Host Interface
    HOSTCALL    = 0x60, ///< 호스트 함수 호출
    THREAD      = 0x61, ///< 새 스레드 생성
    
    // System
    HALT        = 0xFF, ///< VM 실행 중지
};

/**
 * @brief 명령어별 오퍼랜드 크기 정보
 * 
 * 이 구조체는 각 명령어에 대한 오퍼랜드의 바이트 크기제공
 * 디코더가 명령어를 올바르게 파싱하는 데 사용
 */
struct OpcodeInfo {
    uint8_t operandSize;  ///< 오퍼랜드의 바이트 크기
    bool modifiesIP;      ///< 이 명령어가 명령어 포인터를 수정하는지 여부
    const char* mnemonic; ///< 디버깅/디스어셈블리를 위한 문자열 표현
};

/**
 * @brief 명령어 정보 조회 함수
 * 
 * @param op 정보를 조회할 명령어
 * @return 해당 명령어에 대한 OpcodeInfo 구조체
 */
inline const OpcodeInfo& GetOpcodeInfo(Opcode op) {
    static const OpcodeInfo opcodeInfoTable[] = {
        // Stack Operations
        {1, false, "PUSH8"},    // PUSH8
        {2, false, "PUSH16"},   // PUSH16
        {4, false, "PUSH32"},   // PUSH32
        {8, false, "PUSH64"},   // PUSH64
        {0, false, "POP"},      // POP
        {0, false, "DUP"},      // DUP
        {0, false, "SWAP"},     // SWAP
        
        // Arithmetic Operations
        {0, false, "ADD"},      // ADD
        {0, false, "SUB"},      // SUB
        {0, false, "MUL"},      // MUL
        {0, false, "DIV"},      // DIV
        {0, false, "MOD"},      // MOD
        
        // Bitwise Operations
        {0, false, "AND"},      // AND
        {0, false, "OR"},       // OR
        {0, false, "XOR"},      // XOR
        {0, false, "NOT"},      // NOT
        {0, false, "SHL"},      // SHL
        {0, false, "SHR"},      // SHR
        
        // Memory Operations
        {0, false, "LOAD8"},    // LOAD8
        {0, false, "LOAD16"},   // LOAD16
        {0, false, "LOAD32"},   // LOAD32
        {0, false, "LOAD64"},   // LOAD64
        {0, false, "STORE8"},   // STORE8
        {0, false, "STORE16"},  // STORE16
        {0, false, "STORE32"},  // STORE32
        {0, false, "STORE64"},  // STORE64
        
        // Control Flow Operations
        {2, true, "JMP"},       // JMP
        {2, true, "JZ"},        // JZ
        {2, true, "JNZ"},       // JNZ
        {2, true, "JG"},        // JG
        {2, true, "JL"},        // JL
        {2, true, "JGE"},       // JGE
        {2, true, "JLE"},       // JLE
        
        // Function Operations
        {2, true, "CALL"},      // CALL
        {0, true, "RET"},       // RET
        
        // Memory Allocation
        {4, false, "ALLOC"},    // ALLOC
        {0, false, "FREE"},     // FREE
        
        // Host Interface
        {4, false, "HOSTCALL"}, // HOSTCALL
        {0, false, "THREAD"},   // THREAD
        
        // System
        {0, true, "HALT"},      // HALT
    };
    
    return opcodeInfoTable[static_cast<uint8_t>(op) - 1];
}

} // namespace Engine
} // namespace DarkMatterVM
