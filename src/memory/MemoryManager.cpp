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
    _ValidateStack(_stackPointer);
    
    auto& stackSegment = GetSegment(MemorySegmentType::STACK);
    uint64_t value = stackSegment.ReadUInt64(_stackPointer);
    
    // 포인터를 증가시켜 스택에서 제거
    _stackPointer += sizeof(uint64_t);
    
    return value;
}

size_t MemoryManager::AllocateHeap(size_t size) 
{
    // 힙 세그먼트 가져오기
    auto& heapSegment = GetSegment(MemorySegmentType::HEAP);
    
    // 메모리 정렬 (8바이트 경계)
    size = (size + 7) & ~7;
    
    // 메모리 부족 체크
    if (_nextHeapAddress + size > heapSegment.GetSize()) 
    {
        throw MemoryAccessException("힙 메모리가 부족합니다.");
    }
    
    // 할당할 주소
    size_t address = _nextHeapAddress;
    _nextHeapAddress += size;
    
    // 할당 정보 저장
    _allocatedBlocks[address] = size;
    
    // 할당된 메모리 초기화 (0으로)
    for (size_t i = 0; i < size; ++i) 
    {
        heapSegment.WriteByte(address + i, 0);
    }
    
    return address;
}

void MemoryManager::FreeHeap(size_t address) 
{
    // 힙 세그먼트 가져오기
    auto& heapSegment = GetSegment(MemorySegmentType::HEAP);
    
    // 주소가 유효한지 확인
    auto it = _allocatedBlocks.find(address);
    if (it == _allocatedBlocks.end()) 
    {
        throw MemoryAccessException("유효하지 않은 힙 주소를 해제하려고 시도했습니다.");
    }
    
    // 블록 크기 확인
    size_t size = it->second;
    
    // 메모리 영역 초기화 (보안상 이유로)
    for (size_t i = 0; i < size; ++i) 
    {
        heapSegment.WriteByte(address + i, 0);
    }
    
    // 할당 목록에서 제거
    _allocatedBlocks.erase(it);
    
    // 참고: 이 구현은 메모리 공간을 재사용하지 않습니다 (단편화 처리 없음).
    // 실제 프로덕션 환경에서는 더 복잡한 메모리 관리가 필요합니다.
}

} // namespace Memory
} // namespace DarkMatterVM
