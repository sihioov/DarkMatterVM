#include "HeapMemory.h"
#include <stdexcept>

namespace DarkMatterVM 
{
namespace Memory 
{

HeapMemory::HeapMemory(MemorySegment& segment)
    : _segment(segment), _nextHeapAddress(0)
{
}

size_t HeapMemory::AllocateHeap(size_t size)
{
    std::lock_guard<std::mutex> lock(_heapMutex);
    
    // 8바이트 정렬
    size = (size + (sizeof(uint64_t) - 1)) & ~(sizeof(uint64_t) - 1);
    
    // 간단한 선형 할당
    if (_nextHeapAddress + size > _segment.GetSize())
    {
        throw std::runtime_error("HeapMemory: heap allocation failed (out of memory)");
    }
    
    size_t address = _nextHeapAddress;
    _nextHeapAddress += size;
    
    // 할당 정보 저장
    _allocatedBlocks[address] = size;
    
    return address;
}

void HeapMemory::FreeHeap(size_t address)
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

void HeapMemory::_ValidateAccess(size_t address, size_t size)
{
    auto it = _allocatedBlocks.find(address);
    if (it == _allocatedBlocks.end())
    {
        throw std::runtime_error("HeapMemory: invalid heap address for access");
    }
    
    if (size > it->second)
    {
        throw std::runtime_error("HeapMemory: access out of bounds");
    }
}

} // namespace Memory
} // namespace DarkMatterVM 