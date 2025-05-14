#pragma once
#include <cstdint>
#include <vector>
#include <memory>
#include <stdexcept>
#include <unordered_map>

namespace DarkMatterVM {
namespace Memory {

/**
 * @brief 메모리 접근 권한
 */
enum class MemoryAccessFlags : uint8_t {
    READ    = 0x01,  ///< 읽기 권한
    WRITE   = 0x02,  ///< 쓰기 권한
    EXECUTE = 0x04   ///< 실행 권한
};

/**
 * @brief 메모리 구역 유형
 */
enum class MemorySegmentType : uint8_t {
    CODE,       ///< 코드 영역 (바이트코드 저장)
    STACK,      ///< 스택 영역 (스택 및 호출 스택)
    HEAP,       ///< 힙 영역 (동적 할당)
    CONSTANT    ///< 상수 영역 (읽기 전용 데이터)
};

/**
 * @brief 메모리 접근 예외
 */
class MemoryAccessException : public std::runtime_error {
public:
    explicit MemoryAccessException(const std::string& msg) : std::runtime_error(msg) {}
};

/**
 * @brief 메모리 세그먼트
 * 
 * VM 메모리의 특정 영역을 나타내며 접근 권한 관리
 */
class MemorySegment {
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
    ~MemorySegment() = default;
    
    /**
     * @brief 메모리 읽기
     * 
     * @param offset 세그먼트 내 오프셋
     * @param size 읽을 바이트 수
     * @param buffer 데이터를 저장할 버퍼
     * @throw MemoryAccessException 읽기 권한 없거나 범위 초과 시
     */
    void Read(size_t offset, size_t size, void* buffer) const;
    
    /**
     * @brief 메모리 쓰기
     * 
     * @param offset 세그먼트 내 오프셋
     * @param size 쓸 바이트 수
     * @param data 쓸 데이터
     * @throw MemoryAccessException 쓰기 권한 없거나 범위 초과 시
     */
    void Write(size_t offset, size_t size, const void* data);
    
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
    size_t GetSize() const { return memory.size(); }
    
    /**
     * @brief 세그먼트 유형 조회
     * 
     * @return MemorySegmentType 세그먼트 유형
     */
    MemorySegmentType GetType() const { return type; }
    
    /**
     * @brief 접근 권한 확인
     * 
     * @param flag 확인할 접근 권한
     * @return true 권한 있음
     * @return false 권한 없음
     */
    bool HasAccess(MemoryAccessFlags flag) const {
        return (accessFlags & static_cast<uint8_t>(flag)) != 0;
    }
    
private:
    std::vector<uint8_t> memory;     ///< 실제 메모리 저장 공간
    MemorySegmentType type;          ///< 세그먼트 유형
    uint8_t accessFlags;             ///< 접근 권한 플래그
    
    /**
     * @brief 메모리 접근 유효성 검사
     * 
     * @param offset 오프셋
     * @param size 접근 크기
     * @param flag 접근 유형 플래그
     * @throw MemoryAccessException 접근 권한 없거나 범위 초과 시
     */
    void _ValidateAccess(size_t offset, size_t size, MemoryAccessFlags flag) const;
};

/**
 * @brief 메모리 관리자
 * 
 * VM의 전체 메모리를 관리하는 클래스
 */
class MemoryManager {
public:
    /**
     * @brief 메모리 관리자 생성
     * 
     * @param codeSize 코드 세그먼트 크기
     * @param stackSize 스택 세그먼트 크기
     * @param heapSize 힙 세그먼트 초기 크기
     */
    MemoryManager(size_t codeSize = 64 * 1024,    // 64KB
                  size_t stackSize = 1024 * 1024,  // 1MB
                  size_t heapSize = 1024 * 1024);  // 1MB
    
    /**
     * @brief 메모리 관리자 소멸자
     */
    ~MemoryManager() = default;
    
    /**
     * @brief 특정 세그먼트 조회
     * 
     * @param type 세그먼트 유형
     * @return const MemorySegment& 세그먼트 참조
     */
    const MemorySegment& GetSegment(MemorySegmentType type) const;
    
    /**
     * @brief 특정 세그먼트 조회 (쓰기 가능)
     * 
     * @param type 세그먼트 유형
     * @return MemorySegment& 세그먼트 참조
     */
    MemorySegment& GetSegment(MemorySegmentType type);
    
    /**
     * @brief 코드 세그먼트 초기화
     * 
     * @param code 바이트코드
     * @param size 바이트코드 크기
     */
    void InitializeCode(const uint8_t* code, size_t size);
    
    /**
     * @brief 스택 위치 설정
     * 
     * @param stackPointer 스택 포인터 위치
     */
    void SetStackPointer(size_t stackPointer);
    
    /**
     * @brief 현재 스택 포인터 위치 조회
     * 
     * @return size_t 스택 포인터 위치
     */
    size_t GetStackPointer() const { return _stackPointer; }
    
    /**
     * @brief 스택에 값 푸시
     * 
     * @param value 푸시할 값
     */
    void PushUInt64(uint64_t value);
    
    /**
     * @brief 스택에서 값 팝
     * 
     * @return uint64_t 팝한 값
     */
    uint64_t PopUInt64();
    
    /**
     * @brief 힙에 메모리 할당
     * 
     * @param size 할당할 크기
     * @return size_t 할당된 메모리 시작 주소
     */
    size_t AllocateHeap(size_t size);
    
    /**
     * @brief 힙 메모리 해제
     * 
     * @param address 해제할 메모리 주소
     */
    void FreeHeap(size_t address);
    
private:
    std::vector<std::unique_ptr<MemorySegment>> _segments;  ///< 메모리 세그먼트 목록
    size_t _stackPointer;                                   ///< 현재 스택 포인터 위치
    
    // 힙 메모리 관리를 위한 변수들
    size_t _nextHeapAddress = 0;                            ///< 다음 할당할 힙 주소
    std::unordered_map<size_t, size_t> _allocatedBlocks;    ///< 할당된 힙 블록 정보 <주소, 크기>
    
    /**
     * @brief 스택 경계 확인
     * 
     * @param offset 확인할 스택 오프셋
     * @throw MemoryAccessException 스택 범위 초과 시
     */
    void _ValidateStack(size_t offset) const;
};

} // namespace Memory
} // namespace DarkMatterVM 