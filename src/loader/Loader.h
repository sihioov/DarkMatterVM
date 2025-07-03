#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <packer/Packer.h>

namespace DarkMatterVM
{

/**
 * @brief 패키지 로드 결과 상태
 */
enum class LoaderStatus
{
    SUCCESS,              ///< 성공
    FILE_NOT_FOUND,       ///< 파일 찾기 실패
    INVALID_FORMAT,       ///< 유효하지 않은 파일 형식
    CHECKSUM_MISMATCH,    ///< 체크섬 불일치
    DECOMPRESSION_ERROR,  ///< 압축 해제 오류
    DECRYPTION_ERROR,     ///< 복호화 오류
    UNKNOWN_ERROR         ///< 알 수 없는 오류
};

/**
 * @brief 로더 클래스
 * 
 * DarkMatterVM 패키지 파일을 로드하고 처리하는 기능 제공
 */
class Loader
{
public:
    /**
     * @brief 생성자
     */
    Loader();
    
    /**
     * @brief 소멸자
     */
    ~Loader() = default;
    
    /**
     * @brief 패키지 파일 로드
     * 
     * @param packagePath 패키지 파일 경로
     * @return LoaderStatus 로드 결과 상태
     */
    LoaderStatus LoadPackage(const std::string& packagePath);
    
    /**
     * @brief 패키지 메타데이터 가져오기
     * 
     * @return const PackageMetadata& 패키지 메타데이터
     */
    const PackageMetadata& GetMetadata() const { return _metadata; }
    
    /**
     * @brief 바이트코드 모듈 존재 여부 확인
     * 
     * @param moduleName 모듈 이름
     * @return bool 존재 여부
     */
    bool HasBytecodeModule(const std::string& moduleName) const;
    
    /**
     * @brief 바이트코드 모듈 가져오기
     * 
     * @param moduleName 모듈 이름
     * @return const std::vector<uint8_t>& 바이트코드 데이터
     * @throws std::out_of_range 모듈이 존재하지 않는 경우
     */
    const std::vector<uint8_t>& GetBytecodeModule(const std::string& moduleName) const;
    
    /**
     * @brief 모든 바이트코드 모듈 이름 가져오기
     * 
     * @return std::vector<std::string> 모듈 이름 목록
     */
    std::vector<std::string> GetBytecodeModuleNames() const;
    
    /**
     * @brief 리소스 존재 여부 확인
     * 
     * @param resourceName 리소스 이름
     * @return bool 존재 여부
     */
    bool HasResource(const std::string& resourceName) const;
    
    /**
     * @brief 리소스 가져오기
     * 
     * @param resourceName 리소스 이름
     * @return const std::vector<uint8_t>& 리소스 데이터
     * @throws std::out_of_range 리소스가 존재하지 않는 경우
     */
    const std::vector<uint8_t>& GetResource(const std::string& resourceName) const;
    
    /**
     * @brief 모든 리소스 이름 가져오기
     * 
     * @return std::vector<std::string> 리소스 이름 목록
     */
    std::vector<std::string> GetResourceNames() const;
    
    /**
     * @brief 마지막 오류 메시지 가져오기
     * 
     * @return const std::string& 오류 메시지
     */
    const std::string& GetLastError() const { return _lastError; }
    
private:
    PackageMetadata _metadata;  ///< 패키지 메타데이터
    std::unordered_map<std::string, std::vector<uint8_t>> _bytecodeModules;  ///< 바이트코드 모듈
    std::unordered_map<std::string, std::vector<uint8_t>> _resources;  ///< 리소스
    PackingOption _packingOption;  ///< 패킹 옵션
    std::string _lastError;  ///< 마지막 오류 메시지
    
    /**
     * @brief 패키지 헤더 읽기
     * 
     * @param fileData 파일 데이터
     * @param offset 오프셋 (출력 매개변수)
     * @return bool 성공 여부
     */
    bool _ReadPackageHeader(const std::vector<uint8_t>& fileData, size_t& offset);
    
    /**
     * @brief 메타데이터 읽기
     * 
     * @param fileData 파일 데이터
     * @param offset 오프셋 (입출력 매개변수)
     * @return bool 성공 여부
     */
    bool _ReadMetadata(const std::vector<uint8_t>& fileData, size_t& offset);
    
    /**
     * @brief 바이트코드 모듈 읽기
     * 
     * @param fileData 파일 데이터
     * @param offset 오프셋 (입력 매개변수)
     * @param moduleCount 모듈 수
     * @return bool 성공 여부
     */
    bool _ReadBytecodeModules(const std::vector<uint8_t>& fileData, size_t offset, uint16_t moduleCount);
    
    /**
     * @brief 리소스 읽기
     * 
     * @param fileData 파일 데이터
     * @param offset 오프셋 (입력 매개변수)
     * @param resourceCount 리소스 수
     * @return bool 성공 여부
     */
    bool _ReadResources(const std::vector<uint8_t>& fileData, size_t offset, uint16_t resourceCount);
    
    /**
     * @brief 문자열 읽기
     * 
     * @param fileData 파일 데이터
     * @param offset 오프셋 (입출력 매개변수)
     * @return std::string 읽은 문자열
     */
    std::string _ReadString(const std::vector<uint8_t>& fileData, size_t& offset);
    
    /**
     * @brief 데이터 블록 읽기
     * 
     * @param fileData 파일 데이터
     * @param offset 오프셋 (입출력 매개변수)
     * @return std::vector<uint8_t> 읽은 데이터
     */
    std::vector<uint8_t> _ReadDataBlock(const std::vector<uint8_t>& fileData, size_t& offset);
    
    /**
     * @brief 데이터 압축 해제
     * 
     * @param input 압축된 데이터
     * @return std::vector<uint8_t> 압축 해제된 데이터
     */
    std::vector<uint8_t> _DecompressData(const std::vector<uint8_t>& input);
    
    /**
     * @brief 데이터 복호화
     * 
     * @param input 암호화된 데이터
     * @return std::vector<uint8_t> 복호화된 데이터
     */
    std::vector<uint8_t> _DecryptData(const std::vector<uint8_t>& input);
};

} // namespace DarkMatterVM
