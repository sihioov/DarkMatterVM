#include "BytecodeReader.h"
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include "../../common/Logger.h"

namespace DarkMatterVM 
{
namespace Loader 
{

BytecodeReader::BytecodeReader()
    : _position(0)
{
}

bool BytecodeReader::LoadBytecode(const std::vector<uint8_t>& bytecode)
{
    try
    {
        _bytecode = bytecode;
        _position = 0;
        return true;
    }
    catch (const std::exception& e)
    {
        Logger::Error("BytecodeReader", "바이트코드 로드 실패: " + std::string(e.what()));
        return false;
    }
}

uint8_t BytecodeReader::ReadOpcode()
{
    _CheckRange(1);
    return _bytecode[_position++];
}

uint8_t BytecodeReader::ReadByte()
{
    _CheckRange(1);
    return _bytecode[_position++];
}

uint16_t BytecodeReader::ReadUInt16()
{
    _CheckRange(2);
    
    uint16_t value = static_cast<uint16_t>(_bytecode[_position]) |
                    (static_cast<uint16_t>(_bytecode[_position + 1]) << 8);
    
    _position += 2;
    return value;
}

uint32_t BytecodeReader::ReadUInt32()
{
    _CheckRange(4);
    
    uint32_t value = static_cast<uint32_t>(_bytecode[_position]) |
                    (static_cast<uint32_t>(_bytecode[_position + 1]) << 8) |
                    (static_cast<uint32_t>(_bytecode[_position + 2]) << 16) |
                    (static_cast<uint32_t>(_bytecode[_position + 3]) << 24);
    
    _position += 4;
    return value;
}

uint64_t BytecodeReader::ReadUInt64()
{
    _CheckRange(8);
    
    uint64_t value = static_cast<uint64_t>(_bytecode[_position]) |
                    (static_cast<uint64_t>(_bytecode[_position + 1]) << 8) |
                    (static_cast<uint64_t>(_bytecode[_position + 2]) << 16) |
                    (static_cast<uint64_t>(_bytecode[_position + 3]) << 24) |
                    (static_cast<uint64_t>(_bytecode[_position + 4]) << 32) |
                    (static_cast<uint64_t>(_bytecode[_position + 5]) << 40) |
                    (static_cast<uint64_t>(_bytecode[_position + 6]) << 48) |
                    (static_cast<uint64_t>(_bytecode[_position + 7]) << 56);
    
    _position += 8;
    return value;
}

std::string BytecodeReader::ReadString()
{
    // 문자열 길이 읽기 (2바이트)
    uint16_t length = ReadUInt16();
    
    // 문자열 데이터 읽기
    _CheckRange(length);
    std::string result(reinterpret_cast<const char*>(&_bytecode[_position]), length);
    _position += length;
    
    return result;
}

void BytecodeReader::SetPosition(size_t position)
{
    if (position > _bytecode.size())
    {
        throw std::out_of_range("유효하지 않은 읽기 위치");
    }
    _position = position;
}

size_t BytecodeReader::GetRemainingSize() const
{
    return _bytecode.size() - _position;
}

std::string BytecodeReader::DumpBytecode() const
{
    std::stringstream ss;
    
    ss << "BytecodeReader 덤프 - " << _bytecode.size() << " 바이트\n";
    ss << "=================================\n";
    
    for (size_t i = 0; i < _bytecode.size(); ++i)
    {
        ss << std::setw(4) << std::setfill('0') << std::hex << i << ": ";
        ss << std::setw(2) << std::setfill('0') << std::hex 
           << static_cast<int>(_bytecode[i]) << " ";
        
        // 명령어 인식 (첫 바이트가 명령어일 경우)
        if (i == 0 || _bytecode[i-1] == static_cast<uint8_t>(Engine::Opcode::HALT))
        {
            uint8_t opcodeByte = _bytecode[i];
            if (opcodeByte >= 0x01 && opcodeByte <= 0xFF)
            {
                Engine::Opcode opcode = static_cast<Engine::Opcode>(opcodeByte);
                ss << " ; " << Engine::GetOpcodeInfo(opcode).mnemonic;
            }
        }
        
        ss << "\n";
    }
    
    return ss.str();
}

void BytecodeReader::_CheckRange(size_t size) const
{
    if (_position + size > _bytecode.size())
    {
        throw std::runtime_error("바이트코드 읽기 범위 초과");
    }
}

} // namespace Loader
} // namespace DarkMatterVM
