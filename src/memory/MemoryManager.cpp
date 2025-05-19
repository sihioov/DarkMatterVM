#include "MemoryManager.h"
#include "StackMemory.h"
#include "HeapMemory.h"
#include <cstring>
#include <algorithm>
#include <stdexcept>
#include <unordered_map>

namespace DarkMatterVM 
{
namespace Memory 
{

/// MemorySegment 구현

MemorySegment::MemorySegment(MemorySegmentType type, size_t size, uint8_t accessFlags)
    : _type(type), _size(size), _accessFlags(accessFlags) 
{
    // OS 힙에 메모리 할당
    _memory = new uint8_t[size]();  // 0으로 초기화
}

MemorySegment::~MemorySegment()
{
    // OS 힙에 할당된 메모리 해제
    delete[] _memory;
}

void MemorySegment::Read(size_t offset, size_t size, void* buffer) const 
{
    _ValidateAccess(offset, size, MemoryAccessFlags::READ);
    std::memcpy(buffer, _memory + offset, size);
}

void MemorySegment::Write(size_t offset, size_t size, const void* data) 
{
    _ValidateAccess(offset, size, MemoryAccessFlags::WRITE);
    std::memcpy(_memory + offset, data, size);
}

uint8_t MemorySegment::ReadByte(size_t offset) const 
{
    _ValidateAccess(offset, 1, MemoryAccessFlags::READ);
    return _memory[offset];
}

uint16_t MemorySegment::ReadUInt16(size_t offset) const 
{
    _ValidateAccess(offset, 2, MemoryAccessFlags::READ);
    return static_cast<uint16_t>(_memory[offset]) | 
           (static_cast<uint16_t>(_memory[offset + 1]) << 8);
}

uint32_t MemorySegment::ReadUInt32(size_t offset) const 
{
    _ValidateAccess(offset, 4, MemoryAccessFlags::READ);
    return static_cast<uint32_t>(_memory[offset]) | 
           (static_cast<uint32_t>(_memory[offset + 1]) << 8) |
           (static_cast<uint32_t>(_memory[offset + 2]) << 16) |
           (static_cast<uint32_t>(_memory[offset + 3]) << 24);
}

uint64_t MemorySegment::ReadUInt64(size_t offset) const 
{
    _ValidateAccess(offset, 8, MemoryAccessFlags::READ);
    return static_cast<uint64_t>(_memory[offset]) | 
           (static_cast<uint64_t>(_memory[offset + 1]) << 8) |
           (static_cast<uint64_t>(_memory[offset + 2]) << 16) |
           (static_cast<uint64_t>(_memory[offset + 3]) << 24) |
           (static_cast<uint64_t>(_memory[offset + 4]) << 32) |
           (static_cast<uint64_t>(_memory[offset + 5]) << 40) |
           (static_cast<uint64_t>(_memory[offset + 6]) << 48) |
           (static_cast<uint64_t>(_memory[offset + 7]) << 56);
}

void MemorySegment::WriteByte(size_t offset, uint8_t value) 
{
    _ValidateAccess(offset, 1, MemoryAccessFlags::WRITE);
    _memory[offset] = value;
}

void MemorySegment::WriteUInt16(size_t offset, uint16_t value) 
{
    _ValidateAccess(offset, 2, MemoryAccessFlags::WRITE);
    _memory[offset] = static_cast<uint8_t>(value);
    _memory[offset + 1] = static_cast<uint8_t>(value >> 8);
}

void MemorySegment::WriteUInt32(size_t offset, uint32_t value) 
{
    _ValidateAccess(offset, 4, MemoryAccessFlags::WRITE);
    _memory[offset] = static_cast<uint8_t>(value);
    _memory[offset + 1] = static_cast<uint8_t>(value >> 8);
    _memory[offset + 2] = static_cast<uint8_t>(value >> 16);
    _memory[offset + 3] = static_cast<uint8_t>(value >> 24);
}

void MemorySegment::WriteUInt64(size_t offset, uint64_t value) 
{
    _ValidateAccess(offset, 8, MemoryAccessFlags::WRITE);
    _memory[offset] = static_cast<uint8_t>(value);
    _memory[offset + 1] = static_cast<uint8_t>(value >> 8);
    _memory[offset + 2] = static_cast<uint8_t>(value >> 16);
    _memory[offset + 3] = static_cast<uint8_t>(value >> 24);
    _memory[offset + 4] = static_cast<uint8_t>(value >> 32);
    _memory[offset + 5] = static_cast<uint8_t>(value >> 40);
    _memory[offset + 6] = static_cast<uint8_t>(value >> 48);
    _memory[offset + 7] = static_cast<uint8_t>(value >> 56);
}

void MemorySegment::_ValidateAccess(size_t offset, size_t size, MemoryAccessFlags flag) const 
{
    if (!HasAccess(flag)) 
    {
        throw MemoryAccessException("Memory access violation: no permission");
    }
    
    if (offset + size > _size) 
    {
        throw MemoryAccessException("Memory access violation: out of bounds");
    }
}

/// MemoryManager 구현
MemoryManager::MemoryManager(size_t codeSize, size_t stackSize, size_t heapSize)
{
    // 코드 세그먼트 생성 (읽기+실행)
    _segments.push_back(std::make_unique<MemorySegment>(
        MemorySegmentType::CODE, 
        codeSize, 
        static_cast<uint8_t>(MemoryAccessFlags::READ) | 
        static_cast<uint8_t>(MemoryAccessFlags::EXECUTE)
    ));
    
    // 스택 세그먼트 생성 (읽기+쓰기)
    _segments.push_back(std::make_unique<MemorySegment>(
        MemorySegmentType::STACK, 
        stackSize, 
        static_cast<uint8_t>(MemoryAccessFlags::READ) | 
        static_cast<uint8_t>(MemoryAccessFlags::WRITE)
    ));
    
    // 힙 세그먼트 생성 (읽기+쓰기)
    _segments.push_back(std::make_unique<MemorySegment>(
        MemorySegmentType::HEAP, 
        heapSize, 
        static_cast<uint8_t>(MemoryAccessFlags::READ) | 
        static_cast<uint8_t>(MemoryAccessFlags::WRITE)
    ));
    
    // 상수 세그먼트 생성 (읽기 전용)
    _segments.push_back(std::make_unique<MemorySegment>(
        MemorySegmentType::CONSTANT, 
        1024,  // 1KB
        static_cast<uint8_t>(MemoryAccessFlags::READ)
    ));
    
    // 스택 메모리 생성
    _stackMemory = std::make_unique<StackMemory>(GetSegment(MemorySegmentType::STACK));
    
    // 힙 메모리 생성
    _heapMemory = std::make_unique<HeapMemory>(GetSegment(MemorySegmentType::HEAP));
}

MemoryManager::~MemoryManager() = default;

const MemorySegment& MemoryManager::GetSegment(MemorySegmentType type) const 
{
    for (const auto& segment : _segments) 
    {
        if (segment->GetType() == type) 
        {
            return *segment;
        }
    }
    
    throw std::runtime_error("MemoryManager: segment not found");
}

MemorySegment& MemoryManager::GetSegment(MemorySegmentType type) 
{
    for (auto& segment : _segments) 
    {
        if (segment->GetType() == type) 
        {
            return *segment;
        }
    }
    
    throw std::runtime_error("MemoryManager: segment not found");
}

void MemoryManager::InitializeCode(const uint8_t* code, size_t size) 
{
    auto& codeSegment = GetSegment(MemorySegmentType::CODE);
    if (size > codeSegment.GetSize()) 
    {
        throw std::runtime_error("MemoryManager: code size exceeds code segment");
    }
    
    codeSegment.Write(0, size, code);
}

// 스택 관련 메서드 구현

void MemoryManager::SetStackPointer(size_t stackPointer)
{
    _stackMemory->SetStackPointer(stackPointer);
}

size_t MemoryManager::GetStackPointer() const
{
    return _stackMemory->GetStackPointer();
}

void MemoryManager::PushStack(uint64_t value)
{
    _stackMemory->PushStack(value);
}

uint64_t MemoryManager::PopStack()
{
    return _stackMemory->PopStack();
}

uint64_t MemoryManager::PeekStack() const
{
    return _stackMemory->PeekStack();
}

uint64_t MemoryManager::GetStackValue(size_t offset) const
{
    return _stackMemory->GetStackValue(offset);
}

void MemoryManager::PushStackFrame(size_t basePointer, size_t returnAddress)
{
    _stackMemory->PushStackFrame(basePointer, returnAddress);
}

void MemoryManager::PopStackFrame(size_t& basePointer, size_t& returnAddress)
{
    _stackMemory->PopStackFrame(basePointer, returnAddress);
}

// 힙 관련 메서드 구현

size_t MemoryManager::AllocateHeap(size_t size)
{
    return _heapMemory->AllocateHeap(size);
}

void MemoryManager::FreeHeap(size_t address)
{
    _heapMemory->FreeHeap(address);
}

void MemoryManager::ReadHeap(size_t address, void* buffer, size_t size)
{
    _heapMemory->ReadHeap(address, buffer, size);
}

void MemoryManager::WriteHeap(size_t address, const void* data, size_t size)
{
    _heapMemory->WriteHeap(address, data, size);
}

uint8_t MemoryManager::ReadByte(size_t address) const 
{
    auto [segmentType, offset] = _ResolveAddress(address);
    return GetSegment(segmentType).ReadByte(offset);
}

// 주소 해석
std::pair<MemorySegmentType, size_t> MemoryManager::_ResolveAddress(size_t address) const 
{
    // 주소 범위에 따라 세그먼트 유형과 오프셋 결정
    // 이 예제에서는 단순 구현 (실제 구현에서는 더 복잡할 수 있음)
    
    // 코드 영역: 0x00000000 ~ 0x0000FFFF
    if (address < 0x10000) 
    {
        return {MemorySegmentType::CODE, address};
    }
    // 상수 영역: 0x00010000 ~ 0x000103FF
    else if (address < 0x10400) 
    {
        return {MemorySegmentType::CONSTANT, address - 0x10000};
    }
    // 스택 영역: 0x00020000 ~ 0x0011FFFF
    else if (address < 0x120000) 
    {
        return {MemorySegmentType::STACK, address - 0x20000};
    }
    // 힙 영역: 0x00200000 ~ 0x002FFFFF
    else if (address < 0x300000) 
    {
        return {MemorySegmentType::HEAP, address - 0x200000};
    }
    
    throw MemoryAccessException("유효하지 않은 메모리 주소 접근");
}

} // namespace Memory
} // namespace DarkMatterVM
