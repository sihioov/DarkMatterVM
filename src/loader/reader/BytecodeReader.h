#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <memory>
#include "../../../include/Opcodes.h"

namespace DarkMatterVM 
{
namespace Loader 
{

/**
 * @brief 바이트코드 리더 클래스
 * 
 * 패키지 파일에서 바이트코드를 읽고 파싱하는 기능 제공
 */
class BytecodeReader 
{
public:
    /**
     * @brief 생성자
     */
    BytecodeReader();
    
    /**
     * @brief 소멸자
     */
    ~BytecodeReader() = default;
    
    /**
     * @brief 바이트코드 로드
     * 
     * @param bytecode 바이트코드 데이터
     * @return bool 성공 여부
     */
    bool LoadBytecode(const std::vector<uint8_t>& bytecode);
    
    /**
     * @brief 다음 명령어 읽기
     * 
     * @return uint8_t 명령어 코드
     * @throws std::runtime_error 더 이상 읽을 명령어가 없는 경우
     */
    uint8_t ReadOpcode();
    
    /**
     * @brief 1바이트 읽기
     * 
     * @return uint8_t 읽은 값
     * @throws std::runtime_error 더 이상 읽을 데이터가 없는 경우
     */
    uint8_t ReadByte();
    
    /**
     * @brief 2바이트 읽기 (little-endian)
     * 
     * @return uint16_t 읽은 값
     * @throws std::runtime_error 더 이상 읽을 데이터가 없는 경우
     */
    uint16_t ReadUInt16();
    
    /**
     * @brief 4바이트 읽기 (little-endian)
     * 
     * @return uint32_t 읽은 값
     * @throws std::runtime_error 더 이상 읽을 데이터가 없는 경우
     */
    uint32_t ReadUInt32();
    
    /**
     * @brief 8바이트 읽기 (little-endian)
     * 
     * @return uint64_t 읽은 값
     * @throws std::runtime_error 더 이상 읽을 데이터가 없는 경우
     */
    uint64_t ReadUInt64();
    
    /**
     * @brief 문자열 읽기
     * 
     * @return std::string 읽은 문자열
     * @throws std::runtime_error 더 이상 읽을 데이터가 없는 경우
     */
    std::string ReadString();
    
    /**
     * @brief 현재 읽기 위치 가져오기
     * 
     * @return size_t 현재 위치
     */
    size_t GetCurrentPosition() const { return _position; }
    
    /**
     * @brief 읽기 위치 설정
     * 
     * @param position 새로운 위치
     * @throws std::out_of_range 위치가 유효하지 않은 경우
     */
    void SetPosition(size_t position);
    
    /**
     * @brief 남은 데이터 크기 가져오기
     * 
     * @return size_t 남은 데이터 크기
     */
    size_t GetRemainingSize() const;
    
    /**
     * @brief 바이트코드 덤프 (디버깅용)
     * 
     * @return std::string 바이트코드 문자열 표현
     */
    std::string DumpBytecode() const;
    
private:
    std::vector<uint8_t> _bytecode;  ///< 바이트코드 데이터
    size_t _position;                ///< 현재 읽기 위치
    
    /**
     * @brief 데이터 범위 확인
     * 
     * @param size 읽을 데이터 크기
     * @throws std::runtime_error 데이터가 부족한 경우
     */
    void _CheckRange(size_t size) const;
};

} // namespace Loader
} // namespace DarkMatterVM
