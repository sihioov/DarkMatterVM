#pragma once

#include <cstdint>
#include <Opcodes.h>
#include <memory/MemoryManager.h>

namespace DarkMatterVM 
{
namespace Engine 
{

/**
 * @brief 제어 흐름 실행기
 * 
 * VM의 제어 흐름(JMP, CALL, RET 등) 관련 명령어를 실행하는 클래스
 */
class FlowControlExec 
{
public:
    /**
     * @brief 생성자
     * 
     * @param memoryManager 메모리 관리자 인스턴스
     * @param ipRef 인터프리터의 명령어 포인터 참조
     */
    FlowControlExec(Memory::MemoryManager* memoryManager, size_t& ipRef);
    
    /**
     * @brief 소멸자
     */
    ~FlowControlExec() = default;
    
    /**
     * @brief 무조건 점프 실행
     * 
     * @param offset 점프할 상대 오프셋
     */
    void ExecuteJmp(int16_t offset);
    
    /**
     * @brief 0이면 점프 실행
     * 
     * @param offset 점프할 상대 오프셋
     */
    void ExecuteJz(int16_t offset);
    
    /**
     * @brief 0이 아니면 점프 실행
     * 
     * @param offset 점프할 상대 오프셋
     */
    void ExecuteJnz(int16_t offset);
    
    /**
     * @brief 값1이 값2보다 크면 점프 실행
     * 
     * @param offset 점프할 상대 오프셋
     */
    void ExecuteJg(int16_t offset);
    
    /**
     * @brief 값1이 값2보다 작으면 점프 실행
     * 
     * @param offset 점프할 상대 오프셋
     */
    void ExecuteJl(int16_t offset);
    
    /**
     * @brief 값1이 값2보다 크거나 같으면 점프 실행
     * 
     * @param offset 점프할 상대 오프셋
     */
    void ExecuteJge(int16_t offset);
    
    /**
     * @brief 값1이 값2보다 작거나 같으면 점프 실행
     * 
     * @param offset 점프할 상대 오프셋
     */
    void ExecuteJle(int16_t offset);
    
    /**
     * @brief 함수 호출 실행
     * 
     * @param offset 호출할 함수의 상대 오프셋
     */
    void ExecuteCall(int16_t offset);
    
    /**
     * @brief 함수 반환 실행
     */
    void ExecuteRet();
    
private:
    // 메모리 관리자 참조
    Memory::MemoryManager* _memoryManager;
    
    // 인터프리터의 명령어 포인터 참조
    size_t& _ipRef;
    
    /**
     * @brief 명령어 포인터 조정
     * 
     * @param offset 조정할 상대 오프셋
     */
    void _AdjustIP(int16_t offset);
};

} // namespace Engine
} // namespace DarkMatterVM
