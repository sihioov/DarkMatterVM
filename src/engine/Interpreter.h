#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include "../../include/Opcodes.h"
#include "../../include/MemoryManager.h"

namespace DarkMatterVM {
namespace Engine {

/**
 * @brief VM Interpreter 클래스
 * 
 * 바이트코드를 실행하는 메인 인터프리터 엔진
 * 바이트코드를 fetch → decode → execute 하는 사이클을 관리
 */
class Interpreter 
{
public:
    /**
     * @brief Interpreter 생성자
     * 
     * @param codeSize 코드 세그먼트 크기 (기본 64KB)
     * @param stackSize 스택 세그먼트 크기 (기본 1MB)
     * @param heapSize 힙 세그먼트 크기 (기본 1MB)
     */
    Interpreter(size_t codeSize = 64 * 1024,
                size_t stackSize = 1024 * 1024,
                size_t heapSize = 1024 * 1024);
    
    /**
     * @brief 소멸자
     */
    ~Interpreter() = default;
    
    /**
     * @brief 바이트코드 로드
     * 
     * @param bytecode 바이트코드 버퍼
     * @param size 바이트코드 크기
     */
    void LoadBytecode(const uint8_t* bytecode, size_t size);
    
    /**
     * @brief VM 리셋
     * 모든 레지스터와 스택을 초기 상태로 리셋
     */
    void Reset();
    
    /**
     * @brief 바이트코드 실행
     * 
     * @param startAddress 시작 주소 (기본 0)
     * @return int 실행 결과 코드
     */
    int Execute(size_t startAddress = 0);
    
    /**
     * @brief 단일 명령어 실행 (디버깅 용)
     * 
     * @return bool 계속 실행 가능한지 여부
     */
    bool Step();
    
    /**
     * @brief VM 상태 덤프 (디버깅 용)
     */
    void DumpState() const;
    
    /**
     * @brief 파라미터 푸시
     * 
     * @param value 푸시할 64비트 값
     */
    void PushParameter(uint64_t value);
    
    /**
     * @brief 실행 결과 반환 값 조회
     * 
     * @return uint64_t 반환 값
     */
    uint64_t GetReturnValue() const;
    
    /**
     * @brief 실행 결과 값 템플릿 버전
     * 
     * @tparam T 반환 타입
     * @return T 형변환된 반환 값
     */
    template<typename T>
    T GetReturnValue() const {
        static_assert(sizeof(T) <= sizeof(uint64_t), "반환값이 uint64_t 이하여야 합니다.");
        
        return static_cast<T>(GetReturnValue());
    }
    
private:
    // 명령어 포인터
    size_t _ip = 0;
    
    // 메모리 관리자
    std::unique_ptr<Memory::MemoryManager> _memory;
    
    // 실행 플래그
    bool _running = false;
    
    // 반환 값
    uint64_t _returnValue = 0;
    
    /**
     * @brief 명령어 가져오기 (fetch)
     * 
     * @return uint8_t 명령어 코드
     */
    uint8_t _FetchByte();
    
    /**
     * @brief 16비트 오퍼랜드 가져오기
     * 
     * @return int16_t 오퍼랜드 값 (부호 있음)
     */
    int16_t _FetchInt16();
    
    /**
     * @brief 32비트 오퍼랜드 가져오기
     * 
     * @return int32_t 오퍼랜드 값 (부호 있음)
     */
    int32_t _FetchInt32();
    
    /**
     * @brief 64비트 오퍼랜드 가져오기
     * 
     * @return int64_t 오퍼랜드 값 (부호 있음)
     */
    int64_t _FetchInt64();
    
    /**
     * @brief 명령어 실행 함수 타입 정의
     */
    using OpcodeHandler = std::function<void(Interpreter*)>;
    
    /**
     * @brief 명령어 핸들러 맵 초기화
     * 
     * @return std::unordered_map<uint8_t, OpcodeHandler> 핸들러 맵
     */
    static std::unordered_map<uint8_t, OpcodeHandler> _InitializeOpcodeHandlers();
    
    /**
     * @brief 명령어 핸들러 맵
     */
    static const std::unordered_map<uint8_t, OpcodeHandler> _opcodeHandlers;
    
    // 명령어 실행 핸들러 함수들
    void _Handle_PUSH8();
    void _Handle_PUSH16();
    void _Handle_PUSH32();
    void _Handle_PUSH64();
    void _Handle_POP();
    void _Handle_DUP();
    void _Handle_SWAP();
    
    void _Handle_ADD();
    void _Handle_SUB();
    void _Handle_MUL();
    void _Handle_DIV();
    void _Handle_MOD();
    void _Handle_AND();
    void _Handle_OR();
    void _Handle_XOR();
    void _Handle_NOT();
    void _Handle_SHL();
    void _Handle_SHR();
    
    void _Handle_LOAD8();
    void _Handle_LOAD16();
    void _Handle_LOAD32();
    void _Handle_LOAD64();
    void _Handle_STORE8();
    void _Handle_STORE16();
    void _Handle_STORE32();
    void _Handle_STORE64();
    
    void _Handle_JMP();
    void _Handle_JZ();
    void _Handle_JNZ();
    void _Handle_JG();
    void _Handle_JL();
    void _Handle_JGE();
    void _Handle_JLE();
    
    void _Handle_CALL();
    void _Handle_RET();
    
    void _Handle_ALLOC();
    void _Handle_FREE();
    
    void _Handle_HOSTCALL();
    void _Handle_THREAD();
    
    void _Handle_HALT();
};

} // namespace Engine
} // namespace DarkMatterVM

