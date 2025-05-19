#include "FlowControlExec.h"
#include <stdexcept>

namespace DarkMatterVM 
{
namespace Engine 
{

FlowControlExec::FlowControlExec(Memory::MemoryManager* memoryManager, size_t& ipRef)
    : _memoryManager(memoryManager), _ipRef(ipRef) 
{
    if (!memoryManager) 
    {
        throw std::invalid_argument("FlowControlExec: Memory manager is null");
    }
}

void FlowControlExec::ExecuteJmp(int16_t offset) 
{
    // 무조건 점프: IP를 offset만큼 조정
    _AdjustIP(offset);
}

void FlowControlExec::ExecuteJz(int16_t offset) 
{
    // 스택에서 값을 pop
    uint64_t value = _memoryManager->PopStack();
    
    // 값이 0이면 점프
    if (value == 0) 
    {
        _AdjustIP(offset);
    }
}

void FlowControlExec::ExecuteJnz(int16_t offset) 
{
    // 스택에서 값을 pop
    uint64_t value = _memoryManager->PopStack();
    
    // 값이 0이 아니면 점프
    if (value != 0) 
    {
        _AdjustIP(offset);
    }
}

void FlowControlExec::ExecuteJg(int16_t offset) 
{
    // 스택에서 두 값을 pop (op2, op1 순으로)
    uint64_t op2 = _memoryManager->PopStack();
    uint64_t op1 = _memoryManager->PopStack();
    
    // op1이 op2보다 크면 점프
    if (op1 > op2) 
    {
        _AdjustIP(offset);
    }
}

void FlowControlExec::ExecuteJl(int16_t offset) 
{
    // 스택에서 두 값을 pop (op2, op1 순으로)
    uint64_t op2 = _memoryManager->PopStack();
    uint64_t op1 = _memoryManager->PopStack();
    
    // op1이 op2보다 작으면 점프
    if (op1 < op2) 
    {
        _AdjustIP(offset);
    }
}

void FlowControlExec::ExecuteJge(int16_t offset) 
{
    // 스택에서 두 값을 pop (op2, op1 순으로)
    uint64_t op2 = _memoryManager->PopStack();
    uint64_t op1 = _memoryManager->PopStack();
    
    // op1이 op2보다 크거나 같으면 점프
    if (op1 >= op2) 
    {
        _AdjustIP(offset);
    }
}

void FlowControlExec::ExecuteJle(int16_t offset) 
{
    // 스택에서 두 값을 pop (op2, op1 순으로)
    uint64_t op2 = _memoryManager->PopStack();
    uint64_t op1 = _memoryManager->PopStack();
    
    // op1이 op2보다 작거나 같으면 점프
    if (op1 <= op2) 
    {
        _AdjustIP(offset);
    }
}

void FlowControlExec::ExecuteCall(int16_t offset) 
{
    // 현재 명령어 다음 주소를 스택에 푸시 (반환 주소)
    // 호출 명령어 크기(3바이트 = opcode 1 + offset 2)를 더한 주소가 반환 주소
    _memoryManager->PushStack(_ipRef + 3);
    
    // 목적지로 점프
    _AdjustIP(offset);
}

void FlowControlExec::ExecuteRet() 
{
    // 스택에서 반환 주소를 pop
    uint64_t returnAddr = _memoryManager->PopStack();
    
    // IP를 반환 주소로 설정 (상대 점프가 아니라 절대 주소 설정)
    _ipRef = returnAddr;
}

void FlowControlExec::_AdjustIP(int16_t offset) 
{
    // 명령어 포인터 조정 (이미 IP는 다음 명령어를 가리키고 있음)
    // 따라서 직접 offset만큼 조정
    if (offset < 0 && static_cast<size_t>(-offset) > _ipRef) 
    {
        throw std::runtime_error("FlowControlExec: Jump offset out of bounds (negative)");
    }
    
    _ipRef += offset;
}

} // namespace Engine
} // namespace DarkMatterVM
