#include "HostCallExec.h"
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <stdexcept>

namespace DarkMatterVM 
{
namespace Engine 
{

HostCallExec::HostCallExec(Memory::MemoryManager* memoryManager)
    : _memoryManager(memoryManager) 
{
    if (!memoryManager) 
    {
        throw std::invalid_argument("HostCallExec: Memory manager is null");
    }
    
    // 기본 호스트 함수 초기화
    _InitializeDefaultFunctions();
}

void HostCallExec::ExecuteHostCall(uint32_t functionId) 
{
    // 함수 ID로 호스트 함수 검색
    auto it = _hostFunctions.find(functionId);
    if (it == _hostFunctions.end()) 
    {
        throw std::runtime_error("HostCallExec: Unknown host function ID: " + std::to_string(functionId));
    }
    
    // 호스트 함수 호출
    it->second(_memoryManager);
}

void HostCallExec::ExecuteThread() 
{
    // 스택에서 스레드 함수 주소와 인자를 pop
    uint64_t threadFunctionAddr = _memoryManager->PopStack();
    uint64_t threadArg = _memoryManager->PopStack();
    
    // VM 메모리 클론을 위한 준비 (새 인터프리터 생성은 여기서 구현하지 않음)
    // 실제 구현에서는 메모리 복제 및 새 인터프리터로 스레드 생성해야 함
    
    // 현재는 미구현 상태를 알림
    std::cout << "Thread creation requested for function at " << threadFunctionAddr 
              << " with argument " << threadArg << " (not implemented yet)" << std::endl;
    
    // 스레드 ID를 스택에 push (여기서는 더미 값)
    _memoryManager->PushStack(1);
}

void HostCallExec::RegisterHostFunction(uint32_t functionId, HostFunction function) 
{
    if (!function) 
    {
        throw std::invalid_argument("HostCallExec: Cannot register null function");
    }
    
    _hostFunctions[functionId] = function;
}

void HostCallExec::_InitializeDefaultFunctions() 
{
    // 기본 호스트 함수 등록
    RegisterHostFunction(1, [this](Memory::MemoryManager* memory) { _HostPrintInt(memory); });
    RegisterHostFunction(2, [this](Memory::MemoryManager* memory) { _HostPrintString(memory); });
    RegisterHostFunction(3, [this](Memory::MemoryManager* memory) { _HostReadInt(memory); });
    RegisterHostFunction(4, [this](Memory::MemoryManager* memory) { _HostGetTimeMs(memory); });
}

void HostCallExec::_HostPrintInt(Memory::MemoryManager* memory) 
{
    // 스택에서 출력할 정수 값을 pop
    uint64_t value = memory->PopStack();
    
    // 콘솔에 출력
    std::cout << value << std::endl;
}

void HostCallExec::_HostPrintString(Memory::MemoryManager* memory) 
{
    // 스택에서 문자열 주소와 길이를 pop
    uint64_t strAddr = memory->PopStack();
    uint64_t strLen = memory->PopStack();
    
    // 메모리에서 문자열 데이터 읽기
    std::string result;
    result.reserve(strLen);
    
    for (uint64_t i = 0; i < strLen; ++i) 
    {
        result.push_back(static_cast<char>(memory->ReadByte(strAddr + i)));
    }
    
    // 콘솔에 출력
    std::cout << result << std::endl;
}

void HostCallExec::_HostReadInt(Memory::MemoryManager* memory) 
{
    // 콘솔에서 정수 입력 받기
    int64_t value;
    std::cin >> value;
    
    // 스택에 push
    memory->PushStack(static_cast<uint64_t>(value));
}

void HostCallExec::_HostGetTimeMs(Memory::MemoryManager* memory) 
{
    // 현재 시간을 밀리초로 구하기
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    
    // 스택에 push
    memory->PushStack(static_cast<uint64_t>(millis));
}

} // namespace Engine
} // namespace DarkMatterVM
