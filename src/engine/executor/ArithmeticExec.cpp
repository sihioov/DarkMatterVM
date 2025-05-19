#include "ArithmeticExec.h"
#include <stdexcept>

namespace DarkMatterVM 
{
namespace Engine 
{

ArithmeticExec::ArithmeticExec(Memory::MemoryManager* memoryManager)
    : _memoryManager(memoryManager) 
{
    if (!memoryManager) 
    {
        throw std::invalid_argument("ArithmeticExec: Memory manager is null");
    }
}

void ArithmeticExec::ExecuteAdd() 
{
    // 스택에서 두 값을 pop
    uint64_t op2 = _memoryManager->PopStack();
    uint64_t op1 = _memoryManager->PopStack();
    
    // 두 값을 더해서 다시 스택에 push
    uint64_t result = op1 + op2;
    _memoryManager->PushStack(result);
}

void ArithmeticExec::ExecuteSub() 
{
    // 스택에서 두 값을 pop (순서 중요: op2가 오른쪽에 있는 피연산자)
    uint64_t op2 = _memoryManager->PopStack();
    uint64_t op1 = _memoryManager->PopStack();
    
    // op1에서 op2를 빼서 결과를 스택에 push
    uint64_t result = op1 - op2;
    _memoryManager->PushStack(result);
}

void ArithmeticExec::ExecuteMul() 
{
    // 스택에서 두 값을 pop
    uint64_t op2 = _memoryManager->PopStack();
    uint64_t op1 = _memoryManager->PopStack();
    
    // 두 값을 곱해서 결과를 스택에 push
    uint64_t result = op1 * op2;
    _memoryManager->PushStack(result);
}

void ArithmeticExec::ExecuteDiv() 
{
    // 스택에서 두 값을 pop
    uint64_t op2 = _memoryManager->PopStack();
    uint64_t op1 = _memoryManager->PopStack();
    
    // 0으로 나누기 검사
    if (op2 == 0) 
    {
        throw std::runtime_error("ArithmeticExec: Division by zero");
    }
    
    // op1을 op2로 나눈 결과를 스택에 push
    uint64_t result = op1 / op2;
    _memoryManager->PushStack(result);
}

void ArithmeticExec::ExecuteMod() 
{
    // 스택에서 두 값을 pop
    uint64_t op2 = _memoryManager->PopStack();
    uint64_t op1 = _memoryManager->PopStack();
    
    // 0으로 나누기 검사
    if (op2 == 0) 
    {
        throw std::runtime_error("ArithmeticExec: Modulo by zero");
    }
    
    // op1을 op2로 나눈 나머지를 스택에 push
    uint64_t result = op1 % op2;
    _memoryManager->PushStack(result);
}

void ArithmeticExec::ExecuteAnd() 
{
    // 스택에서 두 값을 pop
    uint64_t op2 = _memoryManager->PopStack();
    uint64_t op1 = _memoryManager->PopStack();
    
    // 비트 AND 연산 결과를 스택에 push
    uint64_t result = op1 & op2;
    _memoryManager->PushStack(result);
}

void ArithmeticExec::ExecuteOr() 
{
    // 스택에서 두 값을 pop
    uint64_t op2 = _memoryManager->PopStack();
    uint64_t op1 = _memoryManager->PopStack();
    
    // 비트 OR 연산 결과를 스택에 push
    uint64_t result = op1 | op2;
    _memoryManager->PushStack(result);
}

void ArithmeticExec::ExecuteXor() 
{
    // 스택에서 두 값을 pop
    uint64_t op2 = _memoryManager->PopStack();
    uint64_t op1 = _memoryManager->PopStack();
    
    // 비트 XOR 연산 결과를 스택에 push
    uint64_t result = op1 ^ op2;
    _memoryManager->PushStack(result);
}

void ArithmeticExec::ExecuteNot() 
{
    // 스택에서 값을 하나 pop
    uint64_t operand = _memoryManager->PopStack();
    
    // 비트 NOT 연산 결과를 스택에 push
    uint64_t result = ~operand;
    _memoryManager->PushStack(result);
}

void ArithmeticExec::ExecuteShl() 
{
    // 스택에서 두 값을 pop (op2는 시프트할 비트 수)
    uint64_t op2 = _memoryManager->PopStack();
    uint64_t op1 = _memoryManager->PopStack();
    
    // 왼쪽 시프트 연산 결과를 스택에 push
    uint64_t result = op1 << op2;
    _memoryManager->PushStack(result);
}

void ArithmeticExec::ExecuteShr() 
{
    // 스택에서 두 값을 pop (op2는 시프트할 비트 수)
    uint64_t op2 = _memoryManager->PopStack();
    uint64_t op1 = _memoryManager->PopStack();
    
    // 오른쪽 시프트 연산 결과를 스택에 push
    uint64_t result = op1 >> op2;
    _memoryManager->PushStack(result);
}

} // namespace Engine
} // namespace DarkMatterVM
