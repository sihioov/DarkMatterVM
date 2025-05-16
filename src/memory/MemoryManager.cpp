#include "MemoryManager.h"
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
    : type(type), accessFlags(accessFlags) 
{
    memory.resize(size, 0);  // 모든 메모리를 0으로 초기화
}

void MemorySegment::_ValidateAccess(size_t offset, size_t size, MemoryAccessFlags flag) const 
{
    // 접근 권한 검사
    if (!HasAccess(flag)) 
    {
        throw MemoryAccessException("메모리 접근 권한이 없습니다.");
    }
    
    // 경계 검사
    if (offset + size > memory.size()) 
    {
        throw MemoryAccessException("메모리 경계를 벗어난 접근입니다.");
    }
}

void MemorySegment::Read(size_t offset, size_t size, void* buffer) const 
{
    _ValidateAccess(offset, size, MemoryAccessFlags::READ);

    std::memcpy(buffer, memory.data() + offset, size);
}

void MemorySegment::Write(size_t offset, size_t size, const void* data) 
{
    _ValidateAccess(offset, size, MemoryAccessFlags::WRITE);

    std::memcpy(memory.data() + offset, data, size);
}

uint8_t MemorySegment::ReadByte(size_t offset) const 
{
    uint8_t value;
    Read(offset, sizeof(value), &value);
    
    return value;
}

uint16_t MemorySegment::ReadUInt16(size_t offset) const 
{
    uint16_t value;
    Read(offset, sizeof(value), &value);
    
    return value;
}

uint32_t MemorySegment::ReadUInt32(size_t offset) const 
{
    uint32_t value;
    Read(offset, sizeof(value), &value);
    
    return value;
}

uint64_t MemorySegment::ReadUInt64(size_t offset) const 
{
    uint64_t value;
    Read(offset, sizeof(value), &value);
    
    return value;
}

void MemorySegment::WriteByte(size_t offset, uint8_t value) 
{
    Write(offset, sizeof(value), &value);
}

void MemorySegment::WriteUInt16(size_t offset, uint16_t value) 
{
    Write(offset, sizeof(value), &value);
}

void MemorySegment::WriteUInt32(size_t offset, uint32_t value) 
{
    Write(offset, sizeof(value), &value);
}

void MemorySegment::WriteUInt64(size_t offset, uint64_t value) 
{
    Write(offset, sizeof(value), &value);
}

/// MemoryManager 구현
MemoryManager::MemoryManager(size_t codeSize, size_t stackSize, size_t heapSize) 
{
    // 코드 세그먼트 생성 (읽기, 실행 권한)
    _segments.push_back(std::make_unique<MemorySegment>(
        MemorySegmentType::CODE,
        codeSize,
        static_cast<uint8_t>(MemoryAccessFlags::READ) | static_cast<uint8_t>(MemoryAccessFlags::EXECUTE)
    ));
    
    // 스택 세그먼트 생성 (읽기, 쓰기 권한)
    _segments.push_back(std::make_unique<MemorySegment>(
        MemorySegmentType::STACK,
        stackSize,
        static_cast<uint8_t>(MemoryAccessFlags::READ) | static_cast<uint8_t>(MemoryAccessFlags::WRITE)
    ));
    
    // 힙 세그먼트 생성 (읽기, 쓰기 권한)
    _segments.push_back(std::make_unique<MemorySegment>(
        MemorySegmentType::HEAP,
        heapSize,
        static_cast<uint8_t>(MemoryAccessFlags::READ) | static_cast<uint8_t>(MemoryAccessFlags::WRITE)
    ));
    
    // 상수 세그먼트 생성 (읽기 권한만)
    _segments.push_back(std::make_unique<MemorySegment>(
        MemorySegmentType::CONSTANT,
        4096,  // 4KB 크기의 상수 영역
        static_cast<uint8_t>(MemoryAccessFlags::READ)
    ));
    
    // 스택 포인터 초기화 (스택의 끝에서 시작해서 아래로 자람)
    _stackPointer = stackSize;
    // 힙 메모리 관리 변수 초기화
    _nextHeapAddress = 0;
}

const MemorySegment& MemoryManager::GetSegment(MemorySegmentType type) const 
{
    for (const auto& segment : _segments) 
    {
        if (segment->GetType() == type) 
        {
            return *segment;
        }
    }

    throw MemoryAccessException("존재하지 않는 메모리 세그먼트입니다.");
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

    throw MemoryAccessException("존재하지 않는 메모리 세그먼트입니다.");
}

void MemoryManager::InitializeCode(const uint8_t* code, size_t size) 
{
    auto& codeSegment = GetSegment(MemorySegmentType::CODE);
    if (size > codeSegment.GetSize()) 
    {
        throw MemoryAccessException("코드 세그먼트 크기가 부족합니다.");
    }
    
    for (size_t i = 0; i < size; ++i) 
    {
        codeSegment.WriteByte(i, code[i]);
    }
}

void MemoryManager::SetStackPointer(size_t stackPointer) 
{
    auto& stackSegment = GetSegment(MemorySegmentType::STACK);
    if (stackPointer > stackSegment.GetSize()) 
    {
        throw MemoryAccessException("유효하지 않은 스택 포인터 값입니다.");
    }

    _stackPointer = stackPointer;
}

void MemoryManager::_ValidateStack(size_t offset) const 
{
    auto& stackSegment = GetSegment(MemorySegmentType::STACK);
    if (offset >= stackSegment.GetSize()) 
    {
        throw MemoryAccessException("스택 오버플로우: 스택 포인터가 경계를 벗어났습니다.");
    }
}

void MemoryManager::PushUInt64(uint64_t value) 
{
    // 스택은 위에서 아래로 자라므로, 포인터를 감소시킴
    _stackPointer -= sizeof(uint64_t);
    _ValidateStack(_stackPointer);
    
    auto& stackSegment = GetSegment(MemorySegmentType::STACK);
    stackSegment.WriteUInt64(_stackPointer, value);
}

uint64_t MemoryManager::PopUInt64() 
{
    if (_stackPointer >= GetSegment(MemorySegmentType::STACK).GetSize()) 
    {
        throw MemoryAccessException("스택 언더플로우: 스택에서 더 이상 값을 꺼낼 수 없습니다.");
    }
    
    auto& stackSegment = GetSegment(MemorySegmentType::STACK);
    uint64_t value = stackSegment.ReadUInt64(_stackPointer);
    
    // 스택 포인터 이동 (8바이트)
    _stackPointer += sizeof(uint64_t);
    
    return value;
}

size_t MemoryManager::AllocateHeap(size_t size) 
{
    // 메모리 정렬을 위해 8바이트(64비트) 단위로 올림
    size = (size + 7) & ~7;
    
    if (size == 0) 
    {
        throw MemoryAccessException("할당 크기는 0보다 커야 합니다.");
    }
    
    auto& heapSegment = GetSegment(MemorySegmentType::HEAP);
    
    // 간단한 구현: 힙의 현재 위치에서 메모리 할당
    // 프리리스트나 더 복잡한 메모리 관리는 구현하지 않음
    size_t address = _nextHeapAddress;
    
    // 할당 가능한지 확인
    if (address + size > heapSegment.GetSize()) 
    {
        throw MemoryAccessException("힙 메모리가 부족합니다.");
    }
    
    // 할당 정보 저장 (현재 위치의 8바이트에 크기 저장)
    heapSegment.WriteUInt64(address, size);
    
    // 다음 할당 위치 업데이트 (헤더(8바이트) + 실제 크기)
    _nextHeapAddress = address + sizeof(uint64_t) + size;
    
    // 실제 메모리 시작 주소 반환 (헤더 다음부터)
    return address + sizeof(uint64_t);
}

void MemoryManager::FreeHeap(size_t address) 
{
    auto& heapSegment = GetSegment(MemorySegmentType::HEAP);
    
    // 주소가 올바른지 확인
    if (address < sizeof(uint64_t) || address >= heapSegment.GetSize()) 
    {
        throw MemoryAccessException("잘못된 힙 주소입니다.");
    }
    
    // 실제 블록 시작 주소 (헤더 포함)
    size_t blockStart = address - sizeof(uint64_t);
    
    // 할당된 사이즈 읽기
    uint64_t blockSize = heapSegment.ReadUInt64(blockStart);
    
    // 간단한 구현: 실제로 메모리를 해제하진 않고, 할당 정보만 지움
    // 현재 구현에서는 메모리 재사용이나 최적화가 없음
    // 보다 복잡한 메모리 관리 시스템이 필요할 수 있음
    heapSegment.WriteUInt64(blockStart, 0);
}

} // namespace Memory
} // namespace DarkMatterVM
