#include "BytecodeParser.h"
#include <stdexcept>
#include <string>

namespace DarkMatterVM 
{
namespace Engine 
{

BytecodeParser::BytecodeParser(const uint8_t* bytecode, size_t size)
    : _bytecode(bytecode), _size(size) 
{
    if (!bytecode && size > 0) 
    {
        throw std::invalid_argument("Bytecode pointer is null but size is non-zero");
    }
}

Opcode BytecodeParser::ParseOpcode(size_t offset) const 
{
    _ValidateOffset(offset);

    return _decoder.DecodeOpcode(_bytecode + offset);
}

uint64_t BytecodeParser::ParseOperand(size_t offset, size_t size) const 
{
    _ValidateOffset(offset, size);

    return _decoder.DecodeOperand(_bytecode + offset, size);
}

size_t BytecodeParser::GetInstructionSize(size_t offset) const 
{
    _ValidateOffset(offset);
    Opcode opcode = ParseOpcode(offset);

    return _decoder.GetInstructionSize(opcode);
}

const uint8_t* BytecodeParser::GetBytecodePtr(size_t offset) const 
{
    _ValidateOffset(offset);

    return _bytecode + offset;
}

void BytecodeParser::_ValidateOffset(size_t offset, size_t requiredSize) const 
{
    if (!_bytecode) 
    {
        throw std::runtime_error("BytecodeParser: Bytecode is null");
    }
    
    if (offset + requiredSize > _size) 
    {
        throw std::out_of_range(
            "BytecodeParser: Offset out of range: " + std::to_string(offset) + 
            " + " + std::to_string(requiredSize) + 
            " > " + std::to_string(_size)
        );
    }
}

} // namespace Engine
} // namespace DarkMatterVM 