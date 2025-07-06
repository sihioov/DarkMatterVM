#pragma once
#include <cstdint>
#include <vector>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include "MemorySegment.h"

namespace DarkMatterVM::Memory 
{

class StackMemory;
class HeapMemory;

/**
 * @brief 메모리 관리자
 * 
 * VM의 전체 메모리를 관리하는 클래스
 */
class MemoryManager 
{
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
    ~MemoryManager();
    
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
     * @brief 스택 메모리 조회
     * 
     * @return StackMemory& 스택 메모리 참조
     */
    StackMemory& GetStackMemory() { return *_stackMemory; }
    
    /**
     * @brief 스택 메모리 조회 (읽기 전용)
     * 
     * @return const StackMemory& 스택 메모리 참조
     */
    const StackMemory& GetStackMemory() const { return *_stackMemory; }
    
    /**
     * @brief 힙 메모리 조회
     * 
     * @return HeapMemory& 힙 메모리 참조
     */
    HeapMemory& GetHeapMemory() { return *_heapMemory; }
    
    /**
     * @brief 힙 메모리 조회 (읽기 전용)
     * 
     * @return const HeapMemory& 힙 메모리 참조
     */
    const HeapMemory& GetHeapMemory() const { return *_heapMemory; }
    
    /**
     * @brief 지정된 주소에서 바이트 읽기
     * 
     * @param address 읽을 메모리 주소
     * @return uint8_t 읽은 바이트
     */
    uint8_t ReadByte(size_t address) const;
    
    // 스택 편의 메서드
    
    /**
     * @brief 스택 포인터 설정
     * 
     * @param stackPointer 스택 포인터 값
     */
    void SetStackPointer(size_t stackPointer);
    
    /**
     * @brief 현재 스택 포인터 조회
     * 
     * @return size_t 현재 스택 포인터
     */
    size_t GetStackPointer() const;
    
    /**
     * @brief 스택에 값 푸시
     * 
     * @param value 푸시할 값
     */
    void PushStack(uint64_t value);
    
    /**
     * @brief 스택에서 값 팝
     * 
     * @return uint64_t 팝한 값
     */
    uint64_t PopStack();
    
    /**
     * @brief 스택 최상위 값 조회 (팝하지 않음)
     * 
     * @return uint64_t 스택 최상위 값
     */
    uint64_t PeekStack() const;
    
    /**
     * @brief 스택 특정 위치 값 조회
     * 
     * @param offset 스택 상단에서의 오프셋 (0 = 최상위)
     * @return uint64_t 해당 위치의 값
     */
    uint64_t GetStackValue(size_t offset) const;
    
    /**
     * @brief 스택 프레임 정보 저장
     * 
     * @param basePointer 이전 베이스 포인터
     * @param returnAddress 반환 주소
     */
    void EnterStackFrame(size_t basePointer, size_t returnAddress);
    
    /**
     * @brief 스택 프레임 정보 복원
     * 
     * @param basePointer 복원할 베이스 포인터 주소
     * @param returnAddress 복원할 반환 주소
     */
    void LeaveStackFrame(size_t& basePointer, size_t& returnAddress);
    
    // 힙 편의 메서드
    
    /**
     * @brief 힙에 메모리 할당
     * 
     * @param size 할당할 크기
     * @return size_t 할당된 메모리 시작 주소
     */
    size_t Allocate(size_t size);
    
    /**
     * @brief 힙 메모리 해제
     * 
     * @param address 해제할 메모리 주소
     */
    void Free(size_t address);
    
    /**
     * @brief 힙 메모리에서 데이터 읽기
     * 
     * @param address 읽을 메모리 주소
     * @param buffer 데이터를 저장할 버퍼
     * @param size 읽을 크기
     */
    void ReadHeap(size_t address, void* buffer, size_t size);
    
    /**
     * @brief 힙 메모리에 데이터 쓰기
     * 
     * @param address 쓸 메모리 주소
     * @param data 쓸 데이터
     * @param size 쓸 크기
     */
    void WriteHeap(size_t address, const void* data, size_t size);
    
    /**
     * @brief 주소를 기반으로 적절한 세그먼트 찾기
     * 
     * @param address 메모리 주소
     * @return MemorySegment& 해당 세그먼트 참조
     */
    MemorySegment& GetSegmentByAddress(size_t address);
    
    /**
     * @brief 주소를 기반으로 적절한 세그먼트 찾기 (읽기 전용)
     * 
     * @param address 메모리 주소
     * @return const MemorySegment& 해당 세그먼트 참조
     */
    const MemorySegment& GetSegmentByAddress(size_t address) const;
    
    /**
     * @brief 주소에서 64비트 값 읽기
     * 
     * @param address 메모리 주소
     * @return uint64_t 읽은 값
     */
    uint64_t ReadUInt64(size_t address) const;
    
    /**
     * @brief 주소에 64비트 값 쓰기
     * 
     * @param address 메모리 주소
     * @param value 쓸 값
     */
    void WriteUInt64(size_t address, uint64_t value);
    
    /**
     * @brief 특정 세그먼트 조회
     * 
     * @param t 세그먼트 유형
     * @return MemorySegment& 세그먼트 참조
     */
    MemorySegment& operator[](MemorySegmentType t) 
    {
        return *(_segments[static_cast<size_t>(t)]);
    }

    /**
     * @brief 상수 세그먼트 조회
     * 
     * @param t 세그먼트 유형
     * @return const MemorySegment& 상수 세그먼트 참조
     */
    const MemorySegment& operator[](MemorySegmentType t) const 
    {
        return const_cast<MemoryManager*>(this)->operator[](t);
    }

private:
    std::vector<std::unique_ptr<MemorySegment>> _segments;  ///< 메모리 세그먼트 목록
    std::unique_ptr<StackMemory> _stackMemory;                ///< 스택 메모리
    std::unique_ptr<HeapMemory> _heapMemory;                  ///< 힙 메모리
    
    /**
     * @brief 가상 주소 해결 (세그먼트 + 오프셋)
     * 
     * @param address 가상 메모리 주소
     * @return std::pair<MemorySegmentType, size_t> <세그먼트 타입, 세그먼트 내 오프셋>
     */
    std::pair<MemorySegmentType, size_t> _ResolveAddress(size_t address) const;
};

} // namespace DarkMatterVM::Memory 