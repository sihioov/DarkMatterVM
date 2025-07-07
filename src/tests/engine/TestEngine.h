#pragma once

#include "../../engine/Interpreter.h"
#include "../../common/Logger.h"
#include <string>
#include <vector>
#include <memory>

namespace DarkMatterVM 
{
namespace Tests 
{

/**
 * @brief Engine 컴포넌트 테스트 클래스
 * 
 * VM 엔진, 바이트코드 실행, 메모리 관리 등을 테스트
 */
class TestEngine 
{
public:
    TestEngine();
    ~TestEngine() = default;
    
    /**
     * @brief 모든 테스트 실행
     * 
     * @return true 모든 테스트 통과
     * @return false 하나라도 실패
     */
    bool RunAllTests();
    
    /**
     * @brief 개별 테스트 실행
     * 
     * @param testName 테스트 이름
     * @return true 테스트 통과
     * @return false 테스트 실패
     */
    bool RunTest(const std::string& testName);
    
    /**
     * @brief 테스트 결과 출력
     */
    void PrintResults() const;

private:
    // 테스트 케이스들
    bool TestBasicArithmetic();
    bool TestStackOperations();
    bool TestMemoryOperations();
    bool TestControlFlow();
    bool TestLargeNumbers();
    bool TestErrorHandling();
    bool TestMemorySegments();
    bool TestInterpreterState();
    
    // 헬퍼 메서드들
    bool ExecuteBytecode(const std::vector<uint8_t>& bytecode, uint64_t expectedResult = 0);
    bool AssertResult(uint64_t expected, uint64_t actual, const std::string& testName);
    void LogTestResult(const std::string& testName, bool passed, const std::string& message = "");
    
    // 테스트 결과 저장
    struct TestResult 
    {
        std::string name;
        bool passed;
        std::string message;
        uint64_t expectedValue;
        uint64_t actualValue;
    };
    
    std::vector<TestResult> _testResults;
    std::unique_ptr<Engine::Interpreter> _interpreter;
    
    // 테스트 통계
    int _totalTests;
    int _passedTests;
    int _failedTests;
};

} // namespace Tests
} // namespace DarkMatterVM
