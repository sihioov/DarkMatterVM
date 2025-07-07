#include "ControlFlowManager.h"
#include <stdexcept>

namespace DarkMatterVM::ControlFlow {

ControlFlowManager::ControlFlowManager(Memory::MemoryManager& mem)
    : _memoryManager(mem)
    , _currentBP(0)
{}

// 무조건 분기 처리
void ControlFlowManager::Jump(size_t& ip, int16_t rel) const 
{
    ip = static_cast<size_t>(ip + rel);
}

// 값 == 0일 때 분기
void ControlFlowManager::JZ(size_t& ip, int16_t rel, uint64_t value) const 
{
    if (value == 0) 
    {
        Jump(ip, rel);
    }
}

// 값 != 0일 때 분기
void ControlFlowManager::JNZ(size_t& ip, int16_t rel, uint64_t value) const 
{
    if (value != 0) 
    {
        Jump(ip, rel);
    }
}

// 함수 호출
void ControlFlowManager::Call(size_t& ip, int16_t rel) 
{
    // CALL 다음 명령 위치를 리턴 주소로 저장
    size_t returnIP = ip;
    // 스택 프레임: [returnIP] → [oldBP]
    _memoryManager.EnterStackFrame(_currentBP, returnIP);
    // 새 BP는 스택 포인터 위치
    _currentBP = _memoryManager.GetStackPointer();
    // IP를 호출 대상 블록으로 이동
    ip = static_cast<size_t>(returnIP + rel);
}

// 함수 반환
void ControlFlowManager::Ret(size_t& ip) 
{
    size_t oldBP, returnIP;
    _memoryManager.LeaveStackFrame(oldBP, returnIP);
    _currentBP = oldBP;
    ip = returnIP;
}


} // namespace DarkMatterVM::ControlFlow
