#include "SectionBuilder.h"
#include <cstring>
#include <zlib.h>
#include "../../common/Logger.h"

namespace DarkMatterVM 
{

// 패키지 파일 매직 넘버
static const uint32_t PACKAGE_MAGIC = 0x4D564D44; // "DMVM" in ASCII

// 패키지 헤더 구조체 (파일 형식)
struct PackageHeader 
{
    uint32_t magic;              // 매직 넘버 (DMVM)
    uint8_t version;             // 패키지 형식 버전
    uint8_t packingFlags;        // 압축/암호화 플래그
    uint16_t bytecodeModuleCount; // 바이트코드 모듈 수
    uint16_t resourceCount;      // 리소스 수
    uint32_t metadataOffset;     // 메타데이터 오프셋
    uint32_t bytecodeOffset;     // 바이트코드 섹션 오프셋
    uint32_t resourceOffset;     // 리소스 섹션 오프셋
    uint32_t totalSize;          // 전체 패키지 크기
    uint32_t crc32Checksum;      // 패키지 체크섬
};

SectionBuilder::SectionBuilder(PackingOption packingOption)
    : _packingOption(packingOption)
{
}

std::vector<uint8_t> SectionBuilder::BuildHeaderSection(
    uint16_t bytecodeCount,
    uint16_t resourceCount,
    uint32_t metadataOffset,
    uint32_t bytecodeOffset,
    uint32_t resourceOffset,
    uint32_t totalSize,
    uint32_t checksum)
{
    // 헤더 구조체 초기화
    PackageHeader header;
    header.magic = PACKAGE_MAGIC;
    header.version = 1;
    header.packingFlags = static_cast<uint8_t>(_packingOption);
    header.bytecodeModuleCount = bytecodeCount;
    header.resourceCount = resourceCount;
    header.metadataOffset = metadataOffset;
    header.bytecodeOffset = bytecodeOffset;
    header.resourceOffset = resourceOffset;
    header.totalSize = totalSize;
    header.crc32Checksum = checksum;
    
    // 헤더 데이터를 벡터로 변환
    std::vector<uint8_t> headerData(sizeof(PackageHeader));
    std::memcpy(headerData.data(), &header, sizeof(PackageHeader));
    
    return headerData;
}

std::vector<uint8_t> SectionBuilder::BuildMetadataSection(const PackageMetadata& metadata)
{
    std::vector<uint8_t> metadataData;
    
    // 이름, 버전, 작성자 문자열 추가
    _AppendString(metadataData, metadata.name);
    _AppendString(metadataData, metadata.version);
    _AppendString(metadataData, metadata.author);
    
    // 타임스탬프 추가
    _AppendToVector(metadataData, &metadata.creationTimestamp, sizeof(uint32_t));
    
    // 체크섬 (임시로 0 설정, 나중에 업데이트됨)
    uint32_t tempChecksum = 0;
    _AppendToVector(metadataData, &tempChecksum, sizeof(uint32_t));
    
    return metadataData;
}

std::vector<uint8_t> SectionBuilder::BuildBytecodeSection(
    const std::vector<std::pair<std::string, std::vector<uint8_t>>>& bytecodeModules)
{
    std::vector<uint8_t> bytecodeData;
    
    // 각 바이트코드 모듈 처리
    for (const auto& module : bytecodeModules)
    {
        // 모듈 이름 추가
        _AppendString(bytecodeData, module.first);
        
        // 바이트코드 데이터 처리
        std::vector<uint8_t> processedData = module.second;
        
        // 옵션에 따라 압축/암호화 적용
        if (_packingOption == PackingOption::Compress || _packingOption == PackingOption::CompressEncrypt)
        {
            processedData = CompressData(processedData);
        }
        
        if (_packingOption == PackingOption::Encrypt || _packingOption == PackingOption::CompressEncrypt)
        {
            processedData = EncryptData(processedData);
        }
        
        // 바이트코드 크기와 데이터 추가
        uint32_t bytecodeSize = static_cast<uint32_t>(processedData.size());
        _AppendToVector(bytecodeData, &bytecodeSize, sizeof(uint32_t));
        _AppendToVector(bytecodeData, processedData.data(), processedData.size());
    }
    
    return bytecodeData;
}

std::vector<uint8_t> SectionBuilder::BuildResourceSection(
    const std::vector<std::pair<std::string, std::vector<uint8_t>>>& resources)
{
    std::vector<uint8_t> resourceData;
    
    // 각 리소스 처리
    for (const auto& resource : resources)
    {
        // 리소스 이름 추가
        _AppendString(resourceData, resource.first);
        
        // 리소스 데이터 처리
        std::vector<uint8_t> processedData = resource.second;
        
        // 옵션에 따라 압축/암호화 적용
        if (_packingOption == PackingOption::Compress || _packingOption == PackingOption::CompressEncrypt)
        {
            processedData = CompressData(processedData);
        }
        
        if (_packingOption == PackingOption::Encrypt || _packingOption == PackingOption::CompressEncrypt)
        {
            processedData = EncryptData(processedData);
        }
        
        // 리소스 크기와 데이터 추가
        uint32_t resourceSize = static_cast<uint32_t>(processedData.size());
        _AppendToVector(resourceData, &resourceSize, sizeof(uint32_t));
        _AppendToVector(resourceData, processedData.data(), processedData.size());
    }
    
    return resourceData;
}

std::vector<uint8_t> SectionBuilder::CompressData(const std::vector<uint8_t>& input)
{
    if (input.empty())
    {
        return {};
    }
    
    // zlib 압축 버퍼 크기 계산 (최악의 경우 + 헤더)
    uLong compressBound = ::compressBound(static_cast<uLong>(input.size()));
    std::vector<uint8_t> compressed(compressBound);
    
    // 압축
    uLongf compressedSize = static_cast<uLongf>(compressed.size());
    int result = ::compress(compressed.data(), &compressedSize,
                          input.data(), static_cast<uLong>(input.size()));
    
    if (result != Z_OK)
    {
        Logger::Error("SectionBuilder", "압축 오류: " + std::to_string(result));
        return input; // 압축 실패 시 원본 반환
    }
    
    // 실제 압축 크기로 버퍼 조정
    compressed.resize(compressedSize);
    
    return compressed;
}

std::vector<uint8_t> SectionBuilder::EncryptData(const std::vector<uint8_t>& input)
{
    if (input.empty())
    {
        return {};
    }
    
    // 간단한 XOR 암호화 (실제 구현에서는 더 강력한 암호화 알고리즘 사용 권장)
    const uint8_t key[] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0};
    const size_t keySize = sizeof(key);
    
    std::vector<uint8_t> encrypted(input.size());
    
    for (size_t i = 0; i < input.size(); i++)
    {
        encrypted[i] = input[i] ^ key[i % keySize];
    }
    
    return encrypted;
}

void SectionBuilder::_AppendToVector(std::vector<uint8_t>& target, const void* data, size_t size)
{
    size_t currentSize = target.size();
    target.resize(currentSize + size);
    std::memcpy(target.data() + currentSize, data, size);
}

void SectionBuilder::_AppendString(std::vector<uint8_t>& target, const std::string& str)
{
    // 문자열 길이 추가
    uint32_t length = static_cast<uint32_t>(str.size());
    _AppendToVector(target, &length, sizeof(uint32_t));
    
    // 문자열 내용 추가
    if (length > 0)
    {
        _AppendToVector(target, str.c_str(), length);
    }
}

} // namespace DarkMatterVM
