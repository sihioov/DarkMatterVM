#include "MemorySegment.h"
#include <cstring>

namespace DarkMatterVM::Memory
{

/// MemorySegment 구현
MemorySegment::MemorySegment(MemorySegmentType type, size_t size, uint8_t accessFlags)
    : _memory(std::make_unique<uint8_t[]>(size)), _type(type), _size(size), _accessFlags(accessFlags) 
{
}

uint8_t MemorySegment::ReadByte(size_t offset) const 
{
    uint8_t value; 
    Read(offset, sizeof(uint8_t), &value);
    
    return value;
}

uint16_t MemorySegment::ReadUInt16(size_t offset) const 
{
    uint16_t value;
    Read(offset, sizeof(uint16_t), &value);

    return value;
}

uint32_t MemorySegment::ReadUInt32(size_t offset) const 
{
    // return static_cast<uint32_t>(GetData()[offset]) | 
    //        (static_cast<uint32_t>(GetData()[offset + 1]) << 8) |
    //        (static_cast<uint32_t>(GetData()[offset + 2]) << 16) |
    //        (static_cast<uint32_t>(GetData()[offset + 3]) << 24);
    uint32_t value;
    Read(offset, sizeof(uint32_t), &value);

    return value;
}

uint64_t MemorySegment::ReadUInt64(size_t offset) const 
{
    uint64_t value;
    Read(offset, sizeof(uint64_t), &value);

    return value;
}

void MemorySegment::WriteByte(size_t offset, uint8_t value) 
{
    Write(offset, sizeof(uint8_t), &value);
}

void MemorySegment::WriteUInt16(size_t offset, uint16_t value) 
{
    Write(offset, sizeof(uint16_t), &value);
}

void MemorySegment::WriteUInt32(size_t offset, uint32_t value) 
{
    Write(offset, sizeof(uint32_t), &value);
}

void MemorySegment::WriteUInt64(size_t offset, uint64_t value) 
{
    Write(offset, sizeof(uint64_t), &value);
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

} // namespace DarkMatterVM::Memory
