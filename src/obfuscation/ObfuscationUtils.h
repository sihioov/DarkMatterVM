#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>
#include <random>

namespace DarkMatterVM::Obfuscation 
{

/**
 * @brief 난독화 공통 유틸리티
 */
class ObfuscationUtils 
{
public:
    ObfuscationUtils() = delete;

    /**
     * @brief 무의미한 바이트코드(오퍼코드) 시퀀스 생성
     * @param length 생성할 바이트 수
     * @param rng 난수 생성기
     * @return 부적절한 연산이 섞인 바이트 시퀀스
     */
    static std::vector<uint8_t> GenerateJunkCode(size_t length, std::mt19937& rng);

    /**
     * @brief 바이트블록을 랜덤하게 섞음
     * @param data 블록 시작 포인터
     * @param size 블록 크기(바이트)
     * @param rng 난수 생성기
     */
    static void ShuffleBlock(uint8_t* data, size_t size, std::mt19937& rng);

    /**
     * @brief 블록을 XOR 암호화/복호화
     * @param data 블록 시작 포인터
     * @param size 블록 크기(바이트)
     * @param key  XOR키
     */
    static void XOREncodeBlock(uint8_t* data, size_t size, uint8_t key);

    /**
     * @brief 난독화용 랜덤 시드 생성
     * @return 32비트 난수 시드
     */
    static uint32_t GenerateSeed();

    /**
     * @brief 제어 흐름 평탄화(Flattening) 적용
     * @param bytecode 원본 바이트코드
     * @return 평탄화된 바이트코드 시퀀스
     */
    static std::vector<uint8_t> FlattenControlFlow(const std::vector<uint8_t>& bytecode);

    /**
     * @brief 메타데이터 태그 삽입
     * @param bytecode 바이트코드 시퀀스 (참조)
     * @param offset 삽입할 위치 오프셋
     * @param tag    삽입할 문자열 태그
     */
    static void InsertMetadataTag(std::vector<uint8_t>& bytecode, size_t offset, const std::string& tag);

    /**
     * @brief 정렬된 크기 계산 (alignment 단위로 올림)
     * @param value     원본 크기
     * @param alignment 정렬 단위 (예: 4, 8)
     * @return alignment의 배수로 올림된 값
     */
    static size_t Align(size_t value, size_t alignment);
    
    static std::vector<uint8_t> FlattenControlFlow(const std::vector<uint8_t>& bytecode);
};

} // namespace DarkMatterVM::Obfuscation
