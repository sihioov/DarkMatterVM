#pragma once

#include <string>
#include <vector>
#include <utility>
#include "../Packer.h"

namespace DarkMatterVM 
{

/**
 * @brief 패키지 섹션 빌더 클래스
 * 
 * 패키지 파일의 섹션(헤더, 메타데이터, 바이트코드, 리소스)을 구성하는 기능 제공
 */
class SectionBuilder 
{
public:
    /**
     * @brief 생성자
     * 
     * @param packingOption 패킹 옵션 (압축/암호화)
     */
    SectionBuilder(PackingOption packingOption);
    
    /**
     * @brief 소멸자
     */
    ~SectionBuilder() = default;
    
    /**
     * @brief 헤더 섹션 생성
     * 
     * @param bytecodeCount 바이트코드 모듈 수
     * @param resourceCount 리소스 수
     * @param metadataOffset 메타데이터 오프셋
     * @param bytecodeOffset 바이트코드 오프셋
     * @param resourceOffset 리소스 오프셋
     * @param totalSize 전체 패키지 크기
     * @param checksum 체크섬
     * @return std::vector<uint8_t> 헤더 데이터
     */
    std::vector<uint8_t> BuildHeaderSection(
        uint16_t bytecodeCount,
        uint16_t resourceCount,
        uint32_t metadataOffset,
        uint32_t bytecodeOffset,
        uint32_t resourceOffset,
        uint32_t totalSize,
        uint32_t checksum
    );
    
    /**
     * @brief 메타데이터 섹션 생성
     * 
     * @param metadata 패키지 메타데이터
     * @return std::vector<uint8_t> 메타데이터 데이터
     */
    std::vector<uint8_t> BuildMetadataSection(const PackageMetadata& metadata);
    
    /**
     * @brief 바이트코드 섹션 생성
     * 
     * @param bytecodeModules 바이트코드 모듈 목록
     * @return std::vector<uint8_t> 바이트코드 섹션 데이터
     */
    std::vector<uint8_t> BuildBytecodeSection(
        const std::vector<std::pair<std::string, std::vector<uint8_t>>>& bytecodeModules
    );
    
    /**
     * @brief 리소스 섹션 생성
     * 
     * @param resources 리소스 목록
     * @return std::vector<uint8_t> 리소스 섹션 데이터
     */
    std::vector<uint8_t> BuildResourceSection(
        const std::vector<std::pair<std::string, std::vector<uint8_t>>>& resources
    );
    
    /**
     * @brief 데이터 압축
     * 
     * @param input 입력 데이터
     * @return std::vector<uint8_t> 압축된 데이터
     */
    std::vector<uint8_t> CompressData(const std::vector<uint8_t>& input);
    
    /**
     * @brief 데이터 암호화
     * 
     * @param input 입력 데이터
     * @return std::vector<uint8_t> 암호화된 데이터
     */
    std::vector<uint8_t> EncryptData(const std::vector<uint8_t>& input);
    
private:
    PackingOption _packingOption;  ///< 패킹 옵션
    
    /**
     * @brief 벡터에 데이터 추가
     * 
     * @param target 대상 벡터
     * @param data 추가할 데이터
     * @param size 데이터 크기
     */
    void _AppendToVector(std::vector<uint8_t>& target, const void* data, size_t size);
    
    /**
     * @brief 문자열을 벡터에 추가 (길이 + 문자열)
     * 
     * @param target 대상 벡터
     * @param str 추가할 문자열
     */
    void _AppendString(std::vector<uint8_t>& target, const std::string& str);
};

} // namespace DarkMatterVM
