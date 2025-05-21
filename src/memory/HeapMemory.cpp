#include "HeapMemory.h"
#include <stdexcept>

namespace DarkMatterVM::Memory
{

HeapMemory::HeapMemory(MemorySegment& segment)
    : _segment(segment), _nextHeapAddress(0)
{
}

size_t HeapMemory::Allocate(size_t size)
{
    if (size == 0)
    {
        throw std::runtime_error("HeapMemory: invalid allocation size");
    }

    auto alignedSize = _Aligned(size);

    std::lock_guard<std::mutex> lock(_heapMutex);
    
    // 간단한 선형 할당
    if (_nextHeapAddress + alignedSize > _segment.GetSize())
    {
        throw std::runtime_error("HeapMemory: heap allocation failed (out of memory)");
    }
    
    size_t address = _nextHeapAddress;
    _nextHeapAddress += alignedSize;
    
    // 할당 정보 저장
    _allocatedBlocks[address] = alignedSize;
    
    return address;
}

void HeapMemory::Free(size_t address)
{
    std::lock_guard<std::mutex> lock(_heapMutex);
    
    auto it = _allocatedBlocks.find(address);
    if (it == _allocatedBlocks.end())
    {
        throw std::runtime_error("HeapMemory: invalid heap address for free");
    }
    
    // 현재는 실제로 메모리를 해제하지 않고 블록 정보만 제거
    // 간단한 구현을 위해 재사용 없음
    _allocatedBlocks.erase(it);
}

void HeapMemory::ReadHeap(size_t address, void* buffer, size_t size)
{
    std::lock_guard<std::mutex> lock(_heapMutex);
    
    _ValidateAccess(address, size);
    _segment.Read(address, size, buffer);
}

void HeapMemory::WriteHeap(size_t address, const void* data, size_t size)
{
    std::lock_guard<std::mutex> lock(_heapMutex);
    
    _ValidateAccess(address, size);
    _segment.Write(address, size, data);
}

void HeapMemory::_ValidateAccess(size_t address, size_t size) const
{
    // 1) addr 보다 큰 첫 블록(upper_bound)을 찾고,
    //    그 앞 블록(이전 반복자)이 실제로 해당 addr을 포함하는지 검사
    auto it = _allocatedBlocks.upper_bound(address);
    if (it == _allocatedBlocks.begin()) 
    {
        throw std::out_of_range("HeapMemory: invalid heap address for access");
    }
    --it;

    size_t blockAddr = it->first;
    size_t blockSize = it->second;

    // 2) [addr, addr+size) 가 블록 범위 내에 완전히 들어오는지 확인
    if (address < blockAddr || address + size > blockAddr + blockSize) 
    {
        throw std::out_of_range("HeapMemory: access out of bounds");
    }
}

} // namespace DarkMatterVM::Memory 