#include "StackMemory.h"
#include <stdexcept>

namespace DarkMatterVM {
namespace Memory {

StackMemory::StackMemory(MemorySegment& segment)
    : _segment(segment), _stackPointer(segment.GetSize())
{
}

void StackMemory::SetStackPointer(size_t stackPointer)
{
    std::lock_guard<std::mutex> lock(_stackMutex);
    
    if (stackPointer > _segment.GetSize())
    {
        throw std::runtime_error("StackMemory: stack pointer out of bounds");
    }
    
    _stackPointer = stackPointer;
}

size_t StackMemory::GetStackPointer() const
{
    std::lock_guard<std::mutex> lock(_stackMutex);
    return _stackPointer;
}

void StackMemory::PushStack(uint64_t value)
{
    std::lock_guard<std::mutex> lock(_stackMutex);
    
    // 스택은 아래로 자라므로 포인터를 줄임
    _stackPointer -= sizeof(uint64_t);
    _ValidateStack(_stackPointer);
    
    // 값 쓰기
    _segment.WriteUInt64(_stackPointer, value);
}

uint64_t StackMemory::PopStack()
{
    std::lock_guard<std::mutex> lock(_stackMutex);
    
    _ValidateStack(_stackPointer);
    
    // 값 읽기
    uint64_t value = _segment.ReadUInt64(_stackPointer);
    
    // 스택 포인터 증가
    _stackPointer += sizeof(uint64_t);
    
    return value;
}

uint64_t StackMemory::PeekStack() const
{
    std::lock_guard<std::mutex> lock(_stackMutex);
    
    _ValidateStack(_stackPointer);
    
    // 값 읽기 (포인터 변경 없음)
    return _segment.ReadUInt64(_stackPointer);
}

uint64_t StackMemory::GetStackValue(size_t offset) const
{
    std::lock_guard<std::mutex> lock(_stackMutex);
    
    size_t address = _stackPointer + (offset * sizeof(uint64_t));
    _ValidateStack(address);
    
    return _segment.ReadUInt64(address);
}

void StackMemory::PushStackFrame(size_t basePointer, size_t returnAddress)
{
    std::lock_guard<std::mutex> lock(_stackMutex);
    
    // 반환 주소 푸시
    _stackPointer -= sizeof(uint64_t);
    _ValidateStack(_stackPointer);
    _segment.WriteUInt64(_stackPointer, returnAddress);
    
    // 이전 베이스 포인터 푸시
    _stackPointer -= sizeof(uint64_t);
    _ValidateStack(_stackPointer);
    _segment.WriteUInt64(_stackPointer, basePointer);
    
    // 새 베이스 포인터 설정
    // 실제로는 Interpreter에서 이 값을 저장하고 관리해야 함
}

void StackMemory::PopStackFrame(size_t& basePointer, size_t& returnAddress)
{
    std::lock_guard<std::mutex> lock(_stackMutex);
    
    // 베이스 포인터 복원
    _ValidateStack(_stackPointer);
    basePointer = _segment.ReadUInt64(_stackPointer);
    _stackPointer += sizeof(uint64_t);
    
    // 반환 주소 복원
    _ValidateStack(_stackPointer);
    returnAddress = _segment.ReadUInt64(_stackPointer);
    _stackPointer += sizeof(uint64_t);
}

void StackMemory::_ValidateStack(size_t offset) const
{
    // 이미 락이 걸린 상태에서 호출되므로 추가 락은 필요 없음
    if (offset < 0 || offset >= _segment.GetSize())
    {
        throw MemoryAccessException("Stack access violation: out of bounds");
    }
}

} // namespace Memory
} // namespace DarkMatterVM 