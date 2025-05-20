#include "MemorySegment.h"
#include <cstring>

namespace DarkMatterVM::Memory
{

/// MemorySegment 구현
MemorySegment::MemorySegment(MemorySegmentType type, size_t size, uint8_t accessFlags)
    : _memory(std::make_unique<uint8_t[]>(size)), _type(type), _size(size), _accessFlags(accessFlags) 
{
}

MemorySegment::~MemorySegment()
{
}

void MemorySegment::Read(size_t offset, size_t size, void* buffer) const 
{
    _ValidateAccess(offset, size, MemoryAccessFlags::READ);
    
    std::memcpy(buffer, GetData() + offset, size);
}

void MemorySegment::Write(size_t offset, size_t size, const void* data) 
{
    _ValidateAccess(offset, size, MemoryAccessFlags::WRITE);

    std::memcpy(GetData() + offset, data, size);
}

uint8_t MemorySegment::ReadByte(size_t offset) const 
{
    _ValidateAccess(offset, sizeof(uint8_t), MemoryAccessFlags::READ);

    return GetData()[offset];
}

uint16_t MemorySegment::ReadUInt16(size_t offset) const 
{
     _ValidateAccess(offset, sizeof(uint16_t), MemoryAccessFlags::READ);

    uint16_t value;
    std::memcpy(&value, GetData() + offset, sizeof(value));

    return value;
}

uint32_t MemorySegment::ReadUInt32(size_t offset) const 
{
    // return static_cast<uint32_t>(GetData()[offset]) | 
    //        (static_cast<uint32_t>(GetData()[offset + 1]) << 8) |
    //        (static_cast<uint32_t>(GetData()[offset + 2]) << 16) |
    //        (static_cast<uint32_t>(GetData()[offset + 3]) << 24);
    _ValidateAccess(offset, sizeof(uint32_t), MemoryAccessFlags::READ);
    
    uint32_t value;
    std::memcpy(&value, GetData() + offset, sizeof(value));

    return value;
}

uint64_t MemorySegment::ReadUInt64(size_t offset) const 
{
    _ValidateAccess(offset, sizeof(uint64_t), MemoryAccessFlags::READ);

    uint64_t value;
    std::memcpy(&value, GetData() + offset, sizeof(value));

    return value;
}

void MemorySegment::WriteByte(size_t offset, uint8_t value) 
{
    _ValidateAccess(offset, sizeof(uint8_t), MemoryAccessFlags::WRITE);

    GetData()[offset] = value;
}

void MemorySegment::WriteUInt16(size_t offset, uint16_t value) 
{
    _ValidateAccess(offset, sizeof(uint16_t), MemoryAccessFlags::WRITE);

    GetData()[offset] = static_cast<uint8_t>(value);
    GetData()[offset + 1] = static_cast<uint8_t>(value >> 8);
}

void MemorySegment::WriteUInt32(size_t offset, uint32_t value) 
{
    _ValidateAccess(offset, sizeof(uint32_t), MemoryAccessFlags::WRITE);

    GetData()[offset] = static_cast<uint8_t>(value);
    GetData()[offset + 1] = static_cast<uint8_t>(value >> 8);
    GetData()[offset + 2] = static_cast<uint8_t>(value >> 16);
    GetData()[offset + 3] = static_cast<uint8_t>(value >> 24);
}

void MemorySegment::WriteUInt64(size_t offset, uint64_t value) 
{
    _ValidateAccess(offset, sizeof(uint64_t), MemoryAccessFlags::WRITE);

    GetData()[offset] = static_cast<uint8_t>(value);
    GetData()[offset + 1] = static_cast<uint8_t>(value >> 8);
    GetData()[offset + 2] = static_cast<uint8_t>(value >> 16);
    GetData()[offset + 3] = static_cast<uint8_t>(value >> 24);
    GetData()[offset + 4] = static_cast<uint8_t>(value >> 32);
    GetData()[offset + 5] = static_cast<uint8_t>(value >> 40);
    GetData()[offset + 6] = static_cast<uint8_t>(value >> 48);
    GetData()[offset + 7] = static_cast<uint8_t>(value >> 56);
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
