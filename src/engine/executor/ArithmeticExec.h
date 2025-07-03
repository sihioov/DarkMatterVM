#pragma once

#include <cstdint>
#include <Opcodes.h>
#include <memory/MemoryManager.h>


namespace DarkMatterVM 
{
namespace Engine 
{

/**
 * @brief 산술 연산 실행기
 * 
 * VM의 산술 연산(ADD, SUB, MUL 등) 관련 명령어를 실행하는 클래스
 */
class ArithmeticExec 
{
public:
    /**
     * @brief 생성자
     * 
     * @param memoryManager 메모리 관리자 인스턴스
     */
    ArithmeticExec(Memory::MemoryManager* memoryManager);
    
    /**
     * @brief 소멸자
     */
    ~ArithmeticExec() = default;
    
    /**
     * @brief 덧셈 연산 실행
     * 
     * 스택에서 두 값을 꺼내 더한 후 결과를 다시 스택에 푸시
     */
    void ExecuteAdd();
    
    /**
     * @brief 뺄셈 연산 실행
     * 
     * 스택에서 두 값을 꺼내 뺀 후 결과를 다시 스택에 푸시
     */
    void ExecuteSub();
    
    /**
     * @brief 곱셈 연산 실행
     * 
     * 스택에서 두 값을 꺼내 곱한 후 결과를 다시 스택에 푸시
     */
    void ExecuteMul();
    
    /**
     * @brief 나눗셈 연산 실행
     * 
     * 스택에서 두 값을 꺼내 나눈 후 결과를 다시 스택에 푸시
     */
    void ExecuteDiv();
    
    /**
     * @brief 나머지 연산 실행
     * 
     * 스택에서 두 값을 꺼내 나눈 나머지를 다시 스택에 푸시
     */
    void ExecuteMod();
    
    /**
     * @brief 비트 AND 연산 실행
     */
    void ExecuteAnd();
    
    /**
     * @brief 비트 OR 연산 실행
     */
    void ExecuteOr();
    
    /**
     * @brief 비트 XOR 연산 실행
     */
    void ExecuteXor();
    
    /**
     * @brief 비트 NOT 연산 실행
     */
    void ExecuteNot();
    
    /**
     * @brief 왼쪽 시프트 연산 실행
     */
    void ExecuteShl();
    
    /**
     * @brief 오른쪽 시프트 연산 실행
     */
    void ExecuteShr();
    
private:
    // 메모리 관리자 참조
    Memory::MemoryManager* _memoryManager;
};

} // namespace Engine
} // namespace DarkMatterVM
