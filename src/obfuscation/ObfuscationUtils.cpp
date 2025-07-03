// ObfuscationUtils.cpp

#include "ObfuscationUtils.h"
#include "controlflow/ControlFlowFlattener.h"
#include <algorithm>
#include <random>
#include <stdexcept>

namespace DarkMatterVM::Obfuscation 
{

std::vector<uint8_t> ObfuscationUtils::GenerateJunkCode(size_t length, std::mt19937& rng)
{
    // 예시로 사용할 무의미한 opcode 집합
    static const uint8_t junkOpcodes[] = 
    {
        0x00, // NOP
        0x05, // POP
        0x06, // DUP
        0x07, // SWAP
        0xFF  // HALT
    };
    std::uniform_int_distribution<size_t> dist(0, std::size(junkOpcodes) - 1);

    std::vector<uint8_t> junk;
    junk.reserve(length);
    for (size_t i = 0; i < length; ++i) 
    {
        junk.push_back(junkOpcodes[dist(rng)]);
    }

    return junk;
}

void ObfuscationUtils::ShuffleBlock(uint8_t* data, size_t size, std::mt19937& rng)
{
    std::shuffle(data, data + size, rng);
}

void ObfuscationUtils::XOREncodeBlock(uint8_t* data, size_t size, uint8_t key)
{
    for (size_t i = 0; i < size; ++i) 
    {
        data[i] ^= key;
    }
}

uint32_t ObfuscationUtils::GenerateSeed()
{
    std::random_device rd;

    return static_cast<uint32_t>(rd());
}

void ObfuscationUtils::InsertMetadataTag(
    std::vector<uint8_t>& bytecode,
    size_t offset,
    const std::string& tag)
{
    if (offset > bytecode.size()) 
    {
        throw std::out_of_range("InsertMetadataTag: offset out of range");
    }
    bytecode.insert(bytecode.begin() + offset,
                    tag.begin(), tag.end());
}

size_t ObfuscationUtils::Align(size_t value, size_t alignment)
{
    if (alignment == 0) 
    {
        throw std::invalid_argument("Align: alignment must be non-zero");
    }

    size_t rem = value % alignment;

    return rem == 0 ? value : value + (alignment - rem);
}

std::vector<uint8_t> ObfuscationUtils::FlattenControlFlow(const std::vector<uint8_t>& bytecode)
{
    return ControlFlow::ControlFlowFlattener::Flatten(bytecode);
}

} // namespace DarkMatterVM::Obfuscation
