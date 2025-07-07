#include "MemoryManager.h"
#include "StackMemory.h"
#include "HeapMemory.h"
#include <common/Logger.h>
#include <cstring>
#include <algorithm>
#include <stdexcept>
#include <unordered_map>

namespace DarkMatterVM::Memory 
{

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
    
    // 코드 구역은 실행 중 WRITE 권한이 없도록 설계되어 있으므로
    // 초기 로딩 단계에서는 권한 체크를 우회해 직접 메모리에 복사한다.
    // codeSegment.Write(0, size, code)
    std::memcpy(codeSegment.GetData(), code, size);
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

void MemoryManager::EnterStackFrame(size_t basePointer, size_t returnAddress)
{
    _stackMemory->EnterStackFrame(basePointer, returnAddress);
}

void MemoryManager::LeaveStackFrame(size_t& basePointer, size_t& returnAddress)
{
    _stackMemory->LeaveStackFrame(basePointer, returnAddress);
}

// 힙 관련 메서드 구현

size_t MemoryManager::Allocate(size_t size)
{
    return _heapMemory->Allocate(size);
}

void MemoryManager::Free(size_t address)
{
    _heapMemory->Free(address);
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
MemorySegment& MemoryManager::GetSegmentByAddress(size_t address)
{
    auto [segmentType, offset] = _ResolveAddress(address);
    return GetSegment(segmentType);
}

const MemorySegment& MemoryManager::GetSegmentByAddress(size_t address) const
{
    auto [segmentType, offset] = _ResolveAddress(address);
    return GetSegment(segmentType);
}

uint64_t MemoryManager::ReadUInt64(size_t address) const
{
    Logger::Debug("MemoryManager", "ReadUInt64 호출 - 주소=0x" + std::to_string(address));
    auto [segmentType, offset] = _ResolveAddress(address);
    
    std::string segTypeName;
    switch(segmentType) {
        case MemorySegmentType::CODE: segTypeName = "CODE"; break;
        case MemorySegmentType::STACK: segTypeName = "STACK"; break;
        case MemorySegmentType::HEAP: segTypeName = "HEAP"; break;
        case MemorySegmentType::CONSTANT: segTypeName = "CONSTANT"; break;
        default: segTypeName = "UNKNOWN"; break;
    }
    
    Logger::Debug("MemoryManager", "주소 변환 결과 - 세그먼트=" + segTypeName + ", 오프셋=0x" + std::to_string(offset));
    
    return GetSegment(segmentType).ReadUInt64(offset);
}

void MemoryManager::WriteUInt64(size_t address, uint64_t value)
{
    Logger::Debug("MemoryManager", "WriteUInt64 호출 - 주소=0x" + std::to_string(address) + ", 값=" + std::to_string(value));
    auto [segmentType, offset] = _ResolveAddress(address);
    
    std::string segTypeName;
    switch(segmentType) {
        case MemorySegmentType::CODE: segTypeName = "CODE"; break;
        case MemorySegmentType::STACK: segTypeName = "STACK"; break;
        case MemorySegmentType::HEAP: segTypeName = "HEAP"; break;
        case MemorySegmentType::CONSTANT: segTypeName = "CONSTANT"; break;
        default: segTypeName = "UNKNOWN"; break;
    }
    
    Logger::Debug("MemoryManager", "주소 변환 결과 - 세그먼트=" + segTypeName + ", 오프셋=0x" + std::to_string(offset));
    
    GetSegment(segmentType).WriteUInt64(offset, value);
}

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

} // namespace DarkMatterVM::Memory 
