#pragma once
#include <cstddef>
#include <map>
#include <mutex>
#include "MemorySegment.h"

namespace DarkMatterVM::Memory
{

class HeapMemory
{
public:
    /**
     * @brief 힙 메모리 생성
     * 
     * @param segment 힙 세그먼트 참조
     */
    explicit HeapMemory(MemorySegment& segment);

    HeapMemory(const HeapMemory&)            = delete;
    HeapMemory& operator=(const HeapMemory&) = delete;

    /**
     * @brief 힙 메모리 할당
     * 
     * @param size 할당할 크기
     * @return size_t 할당된 메모리 주소
     */
    size_t Allocate(size_t size);

    /**
     * @brief 힙 메모리 해제
     * 
     * @param address 해제할 메모리 주소
     */ 
    void Free(size_t address);

    /**
     * @brief 힙 메모리 읽기
     * 
     * @param address 읽을 메모리 주소
     * @param buffer 읽은 데이터를 저장할 버퍼
     * @param size 읽을 크기
     */
    void ReadHeap(size_t address, void* buffer, size_t size);
    
    /**
     * @brief 힙 메모리 쓰기
     * 
     * @param address 쓸 메모리 주소
     * @param data 쓸 데이터
     * @param size 쓸 크기
     */
    void WriteHeap(size_t address, const void* data, size_t size);

private:
    /**
     * @brief 힙 메모리 검증
     */
    void _ValidateHeap(size_t address, size_t size) const;

    /**
     * @brief 힙 메모리 접근 검증
     */
    void _ValidateAccess(size_t address, size_t size) const;

    /**
     * @brief 힙 메모리 정렬
     * 
     * @param n 정렬할 크기
     * @param alignment 정렬 단위
     * @return size_t 정렬된 크기
     */
    inline size_t _Aligned(size_t n, size_t alignment = sizeof(uint64_t)) 
    {
        return (n + alignment - 1) & ~(alignment - 1);
    }

    std::map<size_t, size_t> _allocatedBlocks;
    MemorySegment& _segment;    //<<< 힙 세그먼트 참조
    size_t _nextHeapAddress;    //<<< 다음 힙 주소
    mutable std::mutex _heapMutex; //<<< 힙 접근 동기화를 위한 뮤텍스
};
} // namespace DarkMatterVM::Memory