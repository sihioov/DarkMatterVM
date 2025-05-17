#include "Loader.h"
#include <fstream>
#include <cstring>
#include <filesystem>
#include <zlib.h>
#include "../common/Logger.h"

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

Loader::Loader()
    : _packingOption(PackingOption::None)
{
}

LoaderStatus Loader::LoadPackage(const std::string& packagePath)
{
    // 파일 존재 여부 확인
    if (!std::filesystem::exists(packagePath))
    {
        _lastError = "패키지 파일을 찾을 수 없습니다: " + packagePath;
        Logger::Error("Loader", _lastError);
        return LoaderStatus::FILE_NOT_FOUND;
    }
    
    // 파일 열기 및 데이터 읽기
    std::ifstream file(packagePath, std::ios::binary);
    if (!file)
    {
        _lastError = "패키지 파일을 열 수 없습니다: " + packagePath;
        Logger::Error("Loader", _lastError);
        return LoaderStatus::FILE_NOT_FOUND;
    }
    
    // 파일 크기 확인
    file.seekg(0, std::ios::end);
    size_t fileSize = static_cast<size_t>(file.tellg());
    file.seekg(0, std::ios::beg);
    
    // 최소 헤더 크기 확인
    if (fileSize < sizeof(PackageHeader))
    {
        _lastError = "유효하지 않은 패키지 파일 형식: 파일이 너무 작습니다";
        Logger::Error("Loader", _lastError);
        return LoaderStatus::INVALID_FORMAT;
    }
    
    // 파일 데이터 읽기
    std::vector<uint8_t> fileData(fileSize);
    file.read(reinterpret_cast<char*>(fileData.data()), fileSize);
    
    if (!file)
    {
        _lastError = "패키지 파일 읽기 실패";
        Logger::Error("Loader", _lastError);
        return LoaderStatus::UNKNOWN_ERROR;
    }
    
    // 체크섬 확인 준비
    uint32_t storedChecksum = 0;
    std::memcpy(&storedChecksum, fileData.data() + offsetof(PackageHeader, crc32Checksum), sizeof(uint32_t));
    
    // 체크섬 계산을 위해 체크섬 필드를 0으로 설정
    uint32_t zeroChecksum = 0;
    std::memcpy(fileData.data() + offsetof(PackageHeader, crc32Checksum), &zeroChecksum, sizeof(uint32_t));
    
    // 체크섬 계산
    uint32_t calculatedChecksum = crc32(0L, Z_NULL, 0);
    calculatedChecksum = crc32(calculatedChecksum, fileData.data(), static_cast<uInt>(fileData.size()));
    
    // 체크섬 복원
    std::memcpy(fileData.data() + offsetof(PackageHeader, crc32Checksum), &storedChecksum, sizeof(uint32_t));
    
    // 체크섬 비교
    if (calculatedChecksum != storedChecksum)
    {
        _lastError = "체크섬 불일치: 패키지가 손상되었을 수 있습니다";
        Logger::Error("Loader", _lastError);
        return LoaderStatus::CHECKSUM_MISMATCH;
    }
    
    // 헤더 읽기
    size_t offset = 0;
    if (!_ReadPackageHeader(fileData, offset))
    {
        return LoaderStatus::INVALID_FORMAT;
    }
    
    // 메타데이터 읽기
    size_t metadataOffset = offset;
    if (!_ReadMetadata(fileData, metadataOffset))
    {
        return LoaderStatus::INVALID_FORMAT;
    }
    
    // 패키지 헤더 다시 읽기 (다른 정보 가져오기)
    PackageHeader header;
    std::memcpy(&header, fileData.data(), sizeof(PackageHeader));
    
    // 바이트코드 모듈 읽기
    if (!_ReadBytecodeModules(fileData, header.bytecodeOffset, header.bytecodeModuleCount))
    {
        return LoaderStatus::UNKNOWN_ERROR;
    }
    
    // 리소스 읽기
    if (!_ReadResources(fileData, header.resourceOffset, header.resourceCount))
    {
        return LoaderStatus::UNKNOWN_ERROR;
    }
    
    Logger::Info("Loader", "패키지 로드 성공: " + packagePath);
    Logger::Info("Loader", "패키지 이름: " + _metadata.name);
    Logger::Info("Loader", "바이트코드 모듈: " + std::to_string(_bytecodeModules.size()) + "개");
    Logger::Info("Loader", "리소스: " + std::to_string(_resources.size()) + "개");
    
    return LoaderStatus::SUCCESS;
}

bool Loader::HasBytecodeModule(const std::string& moduleName) const
{
    return _bytecodeModules.find(moduleName) != _bytecodeModules.end();
}

const std::vector<uint8_t>& Loader::GetBytecodeModule(const std::string& moduleName) const
{
    return _bytecodeModules.at(moduleName);
}

std::vector<std::string> Loader::GetBytecodeModuleNames() const
{
    std::vector<std::string> names;
    names.reserve(_bytecodeModules.size());
    
    for (const auto& module : _bytecodeModules)
    {
        names.push_back(module.first);
    }
    
    return names;
}

bool Loader::HasResource(const std::string& resourceName) const
{
    return _resources.find(resourceName) != _resources.end();
}

const std::vector<uint8_t>& Loader::GetResource(const std::string& resourceName) const
{
    return _resources.at(resourceName);
}

std::vector<std::string> Loader::GetResourceNames() const
{
    std::vector<std::string> names;
    names.reserve(_resources.size());
    
    for (const auto& resource : _resources)
    {
        names.push_back(resource.first);
    }
    
    return names;
}

bool Loader::_ReadPackageHeader(const std::vector<uint8_t>& fileData, size_t& offset)
{
    // 헤더 읽기
    PackageHeader header;
    std::memcpy(&header, fileData.data() + offset, sizeof(PackageHeader));
    offset += sizeof(PackageHeader);
    
    // 매직 넘버 확인
    if (header.magic != PACKAGE_MAGIC)
    {
        _lastError = "유효하지 않은 패키지 파일 형식: 매직 넘버 불일치";
        Logger::Error("Loader", _lastError);
        return false;
    }
    
    // 버전 확인
    if (header.version != 1)
    {
        _lastError = "지원되지 않는 패키지 버전: " + std::to_string(header.version);
        Logger::Error("Loader", _lastError);
        return false;
    }
    
    // 패킹 옵션 설정
    _packingOption = static_cast<PackingOption>(header.packingFlags);
    
    return true;
}

bool Loader::_ReadMetadata(const std::vector<uint8_t>& fileData, size_t& offset)
{
    try
    {
        // 이름, 버전, 작성자 문자열 읽기
        _metadata.name = _ReadString(fileData, offset);
        _metadata.version = _ReadString(fileData, offset);
        _metadata.author = _ReadString(fileData, offset);
        
        // 타임스탬프 읽기
        std::memcpy(&_metadata.creationTimestamp, fileData.data() + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        
        // 체크섬 읽기
        std::memcpy(&_metadata.crc32Checksum, fileData.data() + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        
        return true;
    }
    catch (const std::exception& e)
    {
        _lastError = "메타데이터 읽기 오류: " + std::string(e.what());
        Logger::Error("Loader", _lastError);
        return false;
    }
}

bool Loader::_ReadBytecodeModules(const std::vector<uint8_t>& fileData, size_t offset, uint16_t moduleCount)
{
    try
    {
        // 바이트코드 모듈 맵 초기화
        _bytecodeModules.clear();
        
        // 각 모듈 읽기
        for (uint16_t i = 0; i < moduleCount; i++)
        {
            // 모듈 이름 읽기
            std::string moduleName = _ReadString(fileData, offset);
            
            // 바이트코드 데이터 읽기
            std::vector<uint8_t> bytecodeData = _ReadDataBlock(fileData, offset);
            
            // 압축/암호화 해제
            if (_packingOption == PackingOption::Encrypt || _packingOption == PackingOption::CompressEncrypt)
            {
                bytecodeData = _DecryptData(bytecodeData);
            }
            
            if (_packingOption == PackingOption::Compress || _packingOption == PackingOption::CompressEncrypt)
            {
                bytecodeData = _DecompressData(bytecodeData);
            }
            
            // 모듈 저장
            _bytecodeModules[moduleName] = std::move(bytecodeData);
        }
        
        return true;
    }
    catch (const std::exception& e)
    {
        _lastError = "바이트코드 모듈 읽기 오류: " + std::string(e.what());
        Logger::Error("Loader", _lastError);
        
        return false;
    }
}

bool Loader::_ReadResources(const std::vector<uint8_t>& fileData, size_t offset, uint16_t resourceCount)
{
    try
    {
        // 리소스 맵 초기화
        _resources.clear();
        
        // 각 리소스 읽기
        for (uint16_t i = 0; i < resourceCount; i++)
        {
            // 리소스 이름 읽기
            std::string resourceName = _ReadString(fileData, offset);
            
            // 리소스 데이터 읽기
            std::vector<uint8_t> resourceData = _ReadDataBlock(fileData, offset);
            
            // 압축/암호화 해제
            if (_packingOption == PackingOption::Encrypt || _packingOption == PackingOption::CompressEncrypt)
            {
                resourceData = _DecryptData(resourceData);
            }
            
            if (_packingOption == PackingOption::Compress || _packingOption == PackingOption::CompressEncrypt)
            {
                resourceData = _DecompressData(resourceData);
            }
            
            // 리소스 저장
            _resources[resourceName] = std::move(resourceData);
        }
        
        return true;
    }
    catch (const std::exception& e)
    {
        _lastError = "리소스 읽기 오류: " + std::string(e.what());
        Logger::Error("Loader", _lastError);
        return false;
    }
}

std::string Loader::_ReadString(const std::vector<uint8_t>& fileData, size_t& offset)
{
    // 문자열 길이 읽기
    uint32_t length = 0;
    if (offset + sizeof(uint32_t) > fileData.size())
    {
        throw std::out_of_range("파일 끝을 넘어 읽으려고 시도했습니다 (문자열 길이)");
    }
    
    std::memcpy(&length, fileData.data() + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    
    // 문자열 내용 읽기
    if (length > 0)
    {
        if (offset + length > fileData.size())
        {
            throw std::out_of_range("파일 끝을 넘어 읽으려고 시도했습니다 (문자열 내용)");
        }
        
        std::string str(reinterpret_cast<const char*>(fileData.data() + offset), length);
        offset += length;
        
        return str;
    }
    
    return "";
}

std::vector<uint8_t> Loader::_ReadDataBlock(const std::vector<uint8_t>& fileData, size_t& offset)
{
    // 데이터 길이 읽기
    uint32_t length = 0;
    if (offset + sizeof(uint32_t) > fileData.size())
    {
        throw std::out_of_range("파일 끝을 넘어 읽으려고 시도했습니다 (데이터 길이)");
    }
    
    std::memcpy(&length, fileData.data() + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    
    // 데이터 내용 읽기
    if (length > 0)
    {
        if (offset + length > fileData.size())
        {
            throw std::out_of_range("파일 끝을 넘어 읽으려고 시도했습니다 (데이터 내용)");
        }
        
        std::vector<uint8_t> data(fileData.data() + offset, fileData.data() + offset + length);
        offset += length;
        
        return data;
    }
    
    return {};
}

std::vector<uint8_t> Loader::_DecompressData(const std::vector<uint8_t>& input)
{
    if (input.empty())
    {
        return {};
    }
    
    // 압축해제 전 원본 데이터 크기 추정 (최대 10배로 가정)
    // 실제로는 compressed data에서 원본 크기 정보를 추출해야 함
    uLong decompressedSize = static_cast<uLong>(input.size() * 10);
    std::vector<uint8_t> decompressed(decompressedSize);
    
    // 압축 해제 시도
    int result = Z_DATA_ERROR;
    while (result == Z_DATA_ERROR || result == Z_BUF_ERROR)
    {
        // 버퍼가 충분하지 않으면 크기 증가
        if (result == Z_BUF_ERROR)
        {
            decompressedSize *= 2;
            decompressed.resize(decompressedSize);
        }
        
        uLongf actualSize = static_cast<uLongf>(decompressed.size());
        result = uncompress(decompressed.data(), &actualSize,
                          input.data(), static_cast<uLong>(input.size()));
        
        if (result == Z_OK)
        {
            // 실제 압축 해제 크기로 조정
            decompressed.resize(actualSize);
            return decompressed;
        }
    }
    
    if (result != Z_OK)
    {
        _lastError = "압축 해제 오류: " + std::to_string(result);
        Logger::Error("Loader", _lastError);
        throw std::runtime_error(_lastError);
    }
    
    return decompressed;
}

std::vector<uint8_t> Loader::_DecryptData(const std::vector<uint8_t>& input)
{
    if (input.empty())
    {
        return {};
    }
    
    // XOR 암호화는 복호화도 동일한 동작을 수행함
    // 암호화와 동일한 코드 사용
    const uint8_t key[] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0};
    const size_t keySize = sizeof(key);
    
    std::vector<uint8_t> decrypted(input.size());
    
    for (size_t i = 0; i < input.size(); i++)
    {
        decrypted[i] = input[i] ^ key[i % keySize];
    }
    
    return decrypted;
}

} // namespace DarkMatterVM
