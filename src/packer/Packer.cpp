#include "Packer.h"
#include <fstream>
#include <iostream>
#include <chrono>
#include <filesystem>
#include <algorithm>
#include <zlib.h>

namespace DarkMatterVM 
{

// CRC32 테이블 - zlib 없이 직접 계산하는 경우를 위한 테이블
static uint32_t crc32_table[256];

// CRC32 테이블 초기화
static void InitCRC32Table() 
{
	const uint32_t polynomial = 0xEDB88320;
	for (uint32_t i = 0; i < 256; i++) 
	{
		uint32_t c = i;
		for (size_t j = 0; j < 8; j++) 
		{
			if (c & 1) 
			{
				c = polynomial ^ (c >> 1);
			} 
			else 
			{
				c >>= 1;
			}
		}
		crc32_table[i] = c;
	}
}

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

Packer::Packer(PackingOption option) 
	: _packingOption(option) 
{
	// CRC32 테이블 초기화
	InitCRC32Table();
	
	// 기본 메타데이터 설정
	_metadata.creationTimestamp = static_cast<uint32_t>(
		std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())
	);
	_metadata.name = "DarkMatterVM Package";
	_metadata.version = "1.0";
	_metadata.author = "DarkMatterVM User";
	_metadata.crc32Checksum = 0;
}

bool Packer::AddBytecode(const std::vector<uint8_t>& bytecode, const std::string& name) 
{
	if (bytecode.empty()) 
	{
		std::cerr << "오류: 빈 바이트코드를 추가할 수 없습니다." << std::endl;
		return false;
	}
	
	// 이미 같은 이름의 모듈이 있는지 확인
	auto it = std::find_if(_bytecodeModules.begin(), _bytecodeModules.end(),
		[&name](const auto& module) { return module.first == name; });
	
	if (it != _bytecodeModules.end()) 
	{
		std::cerr << "오류: 이미 '" << name << "' 모듈이 존재합니다." << std::endl;
		return false;
	}
	
	_bytecodeModules.emplace_back(name, bytecode);
	
	return true;
}

bool Packer::AddResource(const std::string& filePath, const std::string& resourceName) 
{
	// 파일 존재 여부 확인
	if (!std::filesystem::exists(filePath)) 
	{
		std::cerr << "오류: 리소스 파일 '" << filePath << "'을(를) 찾을 수 없습니다." << std::endl;
		return false;
	}
	
	// 이미 같은 이름의 리소스가 있는지 확인
	auto it = std::find_if(_resources.begin(), _resources.end(),
		[&resourceName](const auto& resource) { return resource.first == resourceName; });
	
	if (it != _resources.end()) 
	{
		std::cerr << "오류: 이미 '" << resourceName << "' 리소스가 존재합니다." << std::endl;
		return false;
	}
	
	// 파일 읽기
	std::ifstream file(filePath, std::ios::binary);
	if (!file) 
	{
		std::cerr << "오류: 리소스 파일 '" << filePath << "'을(를) 열 수 없습니다." << std::endl;
		return false;
	}
	
	// 파일 크기 확인
	file.seekg(0, std::ios::end);
	size_t fileSize = static_cast<size_t>(file.tellg());
	file.seekg(0, std::ios::beg);
	
	// 파일 내용 읽기
	std::vector<uint8_t> fileContent(fileSize);
	file.read(reinterpret_cast<char*>(fileContent.data()), fileSize);
	
	if (!file) 
	{
		std::cerr << "오류: 리소스 파일 '" << filePath << "' 읽기 실패." << std::endl;
		return false;
	}
	
	_resources.emplace_back(resourceName, std::move(fileContent));
	
	return true;
}

void Packer::SetMetadata(const PackageMetadata& metadata) 
{
	_metadata = metadata;
	
	// 타임스탬프가 설정되지 않은 경우 현재 시간으로 설정
	if (_metadata.creationTimestamp == 0) 
	{
		_metadata.creationTimestamp = static_cast<uint32_t>(
			std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())
		);
	}
}

bool Packer::CreatePackage(const std::string& outputPath) 
{
	// 바이트코드가 비어있는지 확인
	if (_bytecodeModules.empty()) 
	{
		std::cerr << "오류: 패키지에 바이트코드가 없습니다." << std::endl;
		return false;
	}
	
	// 출력 파일 생성
	std::ofstream outFile(outputPath, std::ios::binary);
	if (!outFile) 
	{
		std::cerr << "오류: 패키지 파일 '" << outputPath << "'을(를) 생성할 수 없습니다." << std::endl;
		return false;
	}
	
	// 패키지 헤더 준비
	PackageHeader header;
	header.magic = PACKAGE_MAGIC;
	header.version = 1;
	header.packingFlags = static_cast<uint8_t>(_packingOption);
	header.bytecodeModuleCount = static_cast<uint16_t>(_bytecodeModules.size());
	header.resourceCount = static_cast<uint16_t>(_resources.size());
	
	// 헤더 크기로 초기 오프셋 설정
	size_t currentOffset = sizeof(PackageHeader);
	
	// 메타데이터 오프셋 설정
	header.metadataOffset = static_cast<uint32_t>(currentOffset);
	
	// 메타데이터 크기 계산 (이름, 버전, 작성자 문자열 길이 + 나머지 필드)
	currentOffset += sizeof(uint32_t) * 3; // 문자열 길이를 저장할 공간
	currentOffset += _metadata.name.size() + _metadata.version.size() + _metadata.author.size();
	currentOffset += sizeof(uint32_t) * 2; // 타임스탬프와 체크섬
	
	// 바이트코드 섹션 오프셋 설정
	header.bytecodeOffset = static_cast<uint32_t>(currentOffset);
	
	// 바이트코드 모듈 섹션 크기 계산
	for (const auto& module : _bytecodeModules) 
	{
		currentOffset += sizeof(uint32_t) + module.first.size(); // 모듈 이름 길이 + 이름
		currentOffset += sizeof(uint32_t) + module.second.size(); // 바이트코드 크기 + 바이트코드
	}
	
	// 리소스 섹션 오프셋 설정
	header.resourceOffset = static_cast<uint32_t>(currentOffset);
	
	// 리소스 섹션 크기 계산
	for (const auto& resource : _resources) 
	{
		currentOffset += sizeof(uint32_t) + resource.first.size(); // 리소스 이름 길이 + 이름
		currentOffset += sizeof(uint32_t) + resource.second.size(); // 리소스 크기 + 데이터
	}
	
	// 전체 패키지 크기 설정
	header.totalSize = static_cast<uint32_t>(currentOffset);
	
	// 패키지 데이터 버퍼 생성
	std::vector<uint8_t> packageData(currentOffset);
	size_t pos = 0;
	
	// 헤더 복사
	std::memcpy(packageData.data() + pos, &header, sizeof(PackageHeader));
	pos += sizeof(PackageHeader);
	
	// 메타데이터 복사
	uint32_t nameLength = static_cast<uint32_t>(_metadata.name.size());
	uint32_t versionLength = static_cast<uint32_t>(_metadata.version.size());
	uint32_t authorLength = static_cast<uint32_t>(_metadata.author.size());
	
	std::memcpy(packageData.data() + pos, &nameLength, sizeof(uint32_t));
	pos += sizeof(uint32_t);
	std::memcpy(packageData.data() + pos, _metadata.name.c_str(), nameLength);
	pos += nameLength;
	
	std::memcpy(packageData.data() + pos, &versionLength, sizeof(uint32_t));
	pos += sizeof(uint32_t);
	std::memcpy(packageData.data() + pos, _metadata.version.c_str(), versionLength);
	pos += versionLength;
	
	std::memcpy(packageData.data() + pos, &authorLength, sizeof(uint32_t));
	pos += sizeof(uint32_t);
	std::memcpy(packageData.data() + pos, _metadata.author.c_str(), authorLength);
	pos += authorLength;
	
	std::memcpy(packageData.data() + pos, &_metadata.creationTimestamp, sizeof(uint32_t));
	pos += sizeof(uint32_t);
	
	// 체크섬은 나중에 계산하므로 임시로 0 설정
	uint32_t tempChecksum = 0;
	std::memcpy(packageData.data() + pos, &tempChecksum, sizeof(uint32_t));
	pos += sizeof(uint32_t);
	
	// 바이트코드 모듈 복사
	for (const auto& module : _bytecodeModules) 
	{
		// 모듈 이름 복사
		uint32_t moduleNameLength = static_cast<uint32_t>(module.first.size());
		std::memcpy(packageData.data() + pos, &moduleNameLength, sizeof(uint32_t));
		pos += sizeof(uint32_t);
		std::memcpy(packageData.data() + pos, module.first.c_str(), moduleNameLength);
		pos += moduleNameLength;
		
		// 바이트코드 데이터 복사
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
		
		uint32_t bytecodeSize = static_cast<uint32_t>(processedData.size());
		std::memcpy(packageData.data() + pos, &bytecodeSize, sizeof(uint32_t));
		pos += sizeof(uint32_t);
		std::memcpy(packageData.data() + pos, processedData.data(), bytecodeSize);
		pos += bytecodeSize;
	}
	
	// 리소스 복사
	for (const auto& resource : _resources) 
	{
		// 리소스 이름 복사
		uint32_t resourceNameLength = static_cast<uint32_t>(resource.first.size());
		std::memcpy(packageData.data() + pos, &resourceNameLength, sizeof(uint32_t));
		pos += sizeof(uint32_t);
		std::memcpy(packageData.data() + pos, resource.first.c_str(), resourceNameLength);
		pos += resourceNameLength;
		
		// 리소스 데이터 복사
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
		
		uint32_t resourceSize = static_cast<uint32_t>(processedData.size());
		std::memcpy(packageData.data() + pos, &resourceSize, sizeof(uint32_t));
		pos += sizeof(uint32_t);
		std::memcpy(packageData.data() + pos, processedData.data(), resourceSize);
		pos += resourceSize;
	}
	
	// 체크섬 계산 및 업데이트
	uint32_t checksum = CalculateCRC32(packageData);
	
	// 헤더에 체크섬 설정
	std::memcpy(packageData.data() + sizeof(PackageHeader) - sizeof(uint32_t), 
				&checksum, sizeof(uint32_t));
	
	// 메타데이터 체크섬 필드 업데이트
	size_t checksumPos = header.metadataOffset + 
						 sizeof(uint32_t) + _metadata.name.size() +
						 sizeof(uint32_t) + _metadata.version.size() +
						 sizeof(uint32_t) + _metadata.author.size() +
						 sizeof(uint32_t); // 타임스탬프 이후
						 
	std::memcpy(packageData.data() + checksumPos, &checksum, sizeof(uint32_t));
	
	// 패키지 파일에 데이터 쓰기
	outFile.write(reinterpret_cast<const char*>(packageData.data()), packageData.size());
	
	if (!outFile) 
	{
		std::cerr << "오류: 패키지 파일 쓰기 실패." << std::endl;
		return false;
	}
	
	std::cout << "패키지가 성공적으로 생성되었습니다: " << outputPath << std::endl;
	std::cout << "총 크기: " << packageData.size() << " 바이트" << std::endl;
	std::cout << "바이트코드 모듈: " << _bytecodeModules.size() << "개" << std::endl;
	std::cout << "리소스: " << _resources.size() << "개" << std::endl;
	
	return true;
}

bool Packer::ValidatePackage(const std::string& packagePath) 
{
	// 파일 존재 여부 확인
	if (!std::filesystem::exists(packagePath)) 
	{
		std::cerr << "오류: 패키지 파일 '" << packagePath << "'을(를) 찾을 수 없습니다." << std::endl;
		return false;
	}
	
	// 파일 열기
	std::ifstream file(packagePath, std::ios::binary);
	if (!file) 
	{
		std::cerr << "오류: 패키지 파일 '" << packagePath << "'을(를) 열 수 없습니다." << std::endl;
		return false;
	}
	
	// 파일 크기 확인
	file.seekg(0, std::ios::end);
	size_t fileSize = static_cast<size_t>(file.tellg());
	file.seekg(0, std::ios::beg);
	
	// 최소 헤더 크기 확인
	if (fileSize < sizeof(PackageHeader)) 
	{
		std::cerr << "오류: 유효하지 않은 패키지 파일 형식." << std::endl;
		return false;
	}
	
	// 헤더 읽기
	PackageHeader header;
	file.read(reinterpret_cast<char*>(&header), sizeof(PackageHeader));
	
	// 매직 넘버 확인
	if (header.magic != PACKAGE_MAGIC) 
	{
		std::cerr << "오류: 유효하지 않은 패키지 파일 (매직 넘버 불일치)." << std::endl;
		return false;
	}
	
	// 버전 확인
	if (header.version != 1) 
	{
		std::cerr << "오류: 지원되지 않는 패키지 버전: " << static_cast<int>(header.version) << std::endl;
		return false;
	}
	
	// 전체 크기 확인
	if (header.totalSize != fileSize) 
	{
		std::cerr << "경고: 패키지 크기 불일치. 예상: " << header.totalSize 
				  << ", 실제: " << fileSize << std::endl;
	}
	
	// 체크섬 확인
	uint32_t storedChecksum = header.crc32Checksum;
	
	// 체크섬 필드를 0으로 설정하고 체크섬 계산
	header.crc32Checksum = 0;
	
	// 파일 전체 내용 읽기
	std::vector<uint8_t> fileContent(fileSize);
	file.seekg(0, std::ios::beg);
	file.read(reinterpret_cast<char*>(fileContent.data()), fileSize);
	
	// 체크섬 필드를 0으로 설정
	std::memcpy(fileContent.data() + offsetof(PackageHeader, crc32Checksum), 
				&header.crc32Checksum, sizeof(uint32_t));
	
	// 체크섬 계산
	uint32_t calculatedChecksum = CalculateCRC32(fileContent);
	
	// 체크섬 비교
	if (calculatedChecksum != storedChecksum) 
	{
		std::cerr << "오류: 체크섬 불일치. 패키지가 손상되었을 수 있습니다." << std::endl;
		return false;
	}
	
	std::cout << "패키지 유효성 검사 성공: " << packagePath << std::endl;
	std::cout << "바이트코드 모듈: " << header.bytecodeModuleCount << "개" << std::endl;
	std::cout << "리소스: " << header.resourceCount << "개" << std::endl;
	
	return true;
}

std::vector<uint8_t> Packer::CompressData(const std::vector<uint8_t>& input) 
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
		std::cerr << "압축 오류: " << result << std::endl;
		return input; // 압축 실패 시 원본 반환
	}
	
	// 실제 압축 크기로 버퍼 조정
	compressed.resize(compressedSize);
	
	return compressed;
}

std::vector<uint8_t> Packer::EncryptData(const std::vector<uint8_t>& input) 
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

uint32_t Packer::CalculateCRC32(const std::vector<uint8_t>& data) 
{
	// zlib의 CRC32 함수 사용
	uint32_t crc = crc32(0L, Z_NULL, 0);
	
	return crc32(crc, data.data(), static_cast<uInt>(data.size()));
}

} // namespace DarkMatterVM
