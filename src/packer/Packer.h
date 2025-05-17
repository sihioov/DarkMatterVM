#pragma once

#include <string>
#include <vector>
#include <memory>
#include <cstdint>

namespace DarkMatterVM 
{

/**
 * @brief 바이트코드 패킹 옵션
 */
enum class PackingOption 
{
	None,           // 기본 패킹 (압축/암호화 없음)
	Compress,       // 데이터 압축 적용
	Encrypt,        // 데이터 암호화 적용
	CompressEncrypt // 압축 후 암호화 적용
};

/**
 * @brief 패키지 메타데이터 구조체
 */
struct PackageMetadata 
{
	std::string name;           // 패키지 이름
	std::string version;        // 패키지 버전
	std::string author;         // 패키지 작성자
	uint32_t creationTimestamp; // 패키지 생성 시간 (유닉스 타임스탬프)
	uint32_t crc32Checksum;     // 체크섬
};

/**
 * @brief 바이트코드 패커 클래스
 * 
 * 이 클래스는 컴파일된 바이트코드를 실행 가능한 패키지로 묶는 기능을 제공합니다.
 * 패키지는 바이트코드, 리소스, 메타데이터를 포함할 수 있으며
 * 선택적으로 압축 및 암호화를 적용할 수 있습니다.
 */
class Packer 
{
public:
	/**
	 * @brief 패커 생성자
	 * 
	 * @param option 패킹 옵션 (압축, 암호화 등)
	 */
	Packer(PackingOption option = PackingOption::None);
	
	/**
	 * @brief 바이트코드 추가
	 * 
	 * @param bytecode 바이트코드 바이너리 데이터
	 * @param name 바이트코드 모듈 이름 (선택적)
	 * @return 성공 여부
	 */
	bool AddBytecode(const std::vector<uint8_t>& bytecode, const std::string& name = "main");
	
	/**
	 * @brief 리소스 파일 추가
	 * 
	 * @param filePath 리소스 파일 경로
	 * @param resourceName 패키지 내에서 사용할 리소스 이름
	 * @return 성공 여부
	 */
	bool AddResource(const std::string& filePath, const std::string& resourceName);
	
	/**
	 * @brief 메타데이터 설정
	 * 
	 * @param metadata 패키지 메타데이터
	 */
	void SetMetadata(const PackageMetadata& metadata);
	
	/**
	 * @brief 패키지 생성
	 * 
	 * @param outputPath 출력 패키지 파일 경로
	 * @return 성공 여부
	 */
	bool CreatePackage(const std::string& outputPath);
	
	/**
	 * @brief 패키지 유효성 검사
	 * 
	 * @param packagePath 검사할 패키지 파일 경로
	 * @return 유효성 여부
	 */
	static bool ValidatePackage(const std::string& packagePath);
	
private:
	PackingOption _packingOption;
	PackageMetadata _metadata;
	std::vector<std::pair<std::string, std::vector<uint8_t>>> _bytecodeModules;
	std::vector<std::pair<std::string, std::vector<uint8_t>>> _resources;
	
	/**
	 * @brief 데이터 압축
	 * 
	 * @param input 원본 데이터
	 * @return 압축된 데이터
	 */
	std::vector<uint8_t> CompressData(const std::vector<uint8_t>& input);
	
	/**
	 * @brief 데이터 암호화
	 * 
	 * @param input 원본/압축된 데이터
	 * @return 암호화된 데이터
	 */
	std::vector<uint8_t> EncryptData(const std::vector<uint8_t>& input);
	
	/**
	 * @brief CRC32 체크섬 계산
	 * 
	 * @param data 체크섬을 계산할 데이터
	 * @return 계산된 체크섬
	 */
	uint32_t CalculateCRC32(const std::vector<uint8_t>& data);
};

} // namespace DarkMatterVM
