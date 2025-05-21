#pragma once
#include <cstdint>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <memory>

namespace DarkMatterVM::Memory 
{

/**
 * @brief 메모리 접근 권한
 */
enum class MemoryAccessFlags : uint8_t 
{
    READ    = 0x01,  ///< 읽기 권한
    WRITE   = 0x02,  ///< 쓰기 권한
    EXECUTE = 0x04   ///< 실행 권한
};

/**
 * @brief 메모리 구역 유형
 */
enum class MemorySegmentType : uint8_t 
{
    CODE,       ///< 코드 영역 (바이트코드 저장)
    STACK,      ///< 스택 영역 (스택 및 호출 스택)
    HEAP,       ///< 힙 영역 (동적 할당)
    CONSTANT    ///< 상수 영역 (읽기 전용 데이터)
};

/**
 * @brief 메모리 접근 예외
 */
class MemoryAccessException : public std::runtime_error 
{
public:
    explicit MemoryAccessException(const std::string& msg) : std::runtime_error(msg) {}
};

/**
 * @brief 메모리 세그먼트
 * 
 * VM 메모리의 특정 영역을 나타내며 접근 권한 관리
 */
class MemorySegment 
{
public:
    /**
     * @brief 메모리 세그먼트 생성
     * 
     * @param type 세그먼트 유형
     * @param size 세그먼트 크기 (바이트)
     * @param accessFlags 접근 권한 플래그
     */
    MemorySegment(MemorySegmentType type, size_t size, uint8_t accessFlags);
    
    /**
     * @brief 메모리 세그먼트 소멸자
     */
    virtual ~MemorySegment() = default;
    
     /**
     * @brief 메모리 읽기
     * 
     * @param offset 세그먼트 내 오프셋
     * @param size 읽을 바이트 수
     * @param buffer 데이터를 저장할 버퍼
     * @throw MemoryAccessException 읽기 권한 없거나 범위 초과 시
     */
    inline void Read(size_t offset, size_t size, void* buffer) const
    {
        _ValidateAccess(offset, size, MemoryAccessFlags::READ);
        std::memcpy(buffer, GetData() + offset, size);
    }
    
    /**
     * @brief 메모리 쓰기
     * 
     * @param offset 세그먼트 내 오프셋
     * @param size 쓸 바이트 수
     * @param data 쓸 데이터
     * @throw MemoryAccessException 쓰기 권한 없거나 범위 초과 시
     */
    inline void Write(size_t offset, size_t size, const void* data)
    {
        _ValidateAccess(offset, size, MemoryAccessFlags::WRITE);
        std::memcpy(GetData() + offset, data, size);
    }

    /**
     * @brief 바이트 한 개 읽기
     * 
     * @param offset 오프셋
     * @return uint8_t 읽은 바이트
     */
    uint8_t ReadByte(size_t offset) const;
    
    /**
     * @brief 16비트 정수 읽기
     * 
     * @param offset 오프셋
     * @return uint16_t 읽은 16비트 값
     */
    uint16_t ReadUInt16(size_t offset) const;
    
    /**
     * @brief 32비트 정수 읽기
     * 
     * @param offset 오프셋
     * @return uint32_t 읽은 32비트 값
     */
    uint32_t ReadUInt32(size_t offset) const;
    
    /**
     * @brief 64비트 정수 읽기
     * 
     * @param offset 오프셋
     * @return uint64_t 읽은 64비트 값
     */
    uint64_t ReadUInt64(size_t offset) const;
    
    /**
     * @brief 바이트 한 개 쓰기
     * 
     * @param offset 오프셋
     * @param value 쓸 바이트 값
     */
    void WriteByte(size_t offset, uint8_t value);
    
    /**
     * @brief 16비트 정수 쓰기
     * 
     * @param offset 오프셋
     * @param value 쓸 16비트 값
     */
    void WriteUInt16(size_t offset, uint16_t value);
    
    /**
     * @brief 32비트 정수 쓰기
     * 
     * @param offset 오프셋
     * @param value 쓸 32비트 값
     */
    void WriteUInt32(size_t offset, uint32_t value);
    
    /**
     * @brief 64비트 정수 쓰기
     * 
     * @param offset 오프셋
     * @param value 쓸 64비트 값
     */
    void WriteUInt64(size_t offset, uint64_t value);
    
    /**
     * @brief 메모리 세그먼트 크기 조회
     * 
     * @return size_t 세그먼트 크기 (바이트)
     */
    size_t GetSize() const { return _size; }
    
    /**
     * @brief 세그먼트 유형 조회
     * 
     * @return MemorySegmentType 세그먼트 유형
     */
    MemorySegmentType GetType() const { return _type; }
    
    /**
     * @brief 접근 권한 확인
     * 
     * @param flag 확인할 접근 권한
     * @return true 권한 있음
     * @return false 권한 없음
     */
    bool HasAccess(MemoryAccessFlags flag) const 
    {
        return (_accessFlags & static_cast<uint8_t>(flag)) != 0;
    }
    
    /**
     * @brief 메모리 데이터 포인터 획득
     * 
     * @return uint8_t* 메모리 데이터 포인터
     */
    uint8_t* GetData() { return _memory.get(); }
    
    /**
     * @brief 메모리 데이터 포인터 획득 (읽기 전용)
     * 
     * @return const uint8_t* 메모리 데이터 포인터
     */
    const uint8_t* GetData() const { return _memory.get(); }
    
private:
    /**
     * @brief 메모리 접근 유효성 검사
     * 
     * @param offset 오프셋
     * @param size 접근 크기
     * @param flag 접근 유형 플래그
     * @throw MemoryAccessException 접근 권한 없거나 범위 초과 시
     */
    void _ValidateAccess(size_t offset, size_t size, MemoryAccessFlags flag) const;

    std::unique_ptr<uint8_t[]> _memory; ///< 실제 메모리 저장 공간 (OS 힙에 할당)
    size_t _size;                   ///< 메모리 크기
    MemorySegmentType _type;        ///< 세그먼트 유형
    uint8_t _accessFlags;           ///< 접근 권한 플래그
};

} // namespace DarkMatterVM::Memory
