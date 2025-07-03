#pragma once

#include <cstdint>
#include <unordered_map>
#include <functional>
#include <Opcodes.h>
#include <memory/MemoryManager.h>

namespace DarkMatterVM 
{
namespace Engine 
{

/**
 * @brief 호스트 함수 호출 실행기
 * 
 * VM에서 호스트 시스템의 API를 호출하는 명령어를 실행하는 클래스
 */
class HostCallExec 
{
public:
    /**
     * @brief 호스트 함수 타입 정의
     * 
     * 매개변수와 반환값은 스택을 통해 전달
     */
    using HostFunction = std::function<void(Memory::MemoryManager*)>;
    
    /**
     * @brief 생성자
     * 
     * @param memoryManager 메모리 관리자 인스턴스
     */
    HostCallExec(Memory::MemoryManager* memoryManager);
    
    /**
     * @brief 소멸자
     */
    ~HostCallExec() = default;
    
    /**
     * @brief 호스트 함수 호출 실행
     * 
     * @param functionId 호출할 함수 ID
     */
    void ExecuteHostCall(uint32_t functionId);
    
    /**
     * @brief 스레드 생성 실행
     */
    void ExecuteThread();
    
    /**
     * @brief 호스트 함수 등록
     * 
     * @param functionId 함수 ID
     * @param function 함수 객체
     */
    void RegisterHostFunction(uint32_t functionId, HostFunction function);
    
private:
    // 메모리 관리자 참조
    Memory::MemoryManager* _memoryManager;
    
    // 호스트 함수 맵 (ID -> 함수)
    std::unordered_map<uint32_t, HostFunction> _hostFunctions;
    
    /**
     * @brief 기본 호스트 함수 초기화
     */
    void _InitializeDefaultFunctions();
    
    /**
     * @brief 콘솔 출력 함수 (정수)
     */
    void _HostPrintInt(Memory::MemoryManager* memory);
    
    /**
     * @brief 콘솔 출력 함수 (문자열)
     */
    void _HostPrintString(Memory::MemoryManager* memory);
    
    /**
     * @brief 콘솔 입력 함수 (정수)
     */
    void _HostReadInt(Memory::MemoryManager* memory);
    
    /**
     * @brief 시간 함수 (밀리초 타임스탬프)
     */
    void _HostGetTimeMs(Memory::MemoryManager* memory);
};

} // namespace Engine
} // namespace DarkMatterVM
