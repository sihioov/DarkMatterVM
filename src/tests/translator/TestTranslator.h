#pragma once

#include "../../translator/Translator.h"
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
 * @brief Translator 컴포넌트 테스트 클래스
 * 
 * C++ 코드 파싱, 바이트코드 생성, VM 실행 등을 테스트
 */
class TestTranslator 
{
public:
    TestTranslator();
    ~TestTranslator() = default;
    
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
    bool TestEmptyCode();
    bool TestSimpleVariableDeclaration();
    bool TestMultipleVariableDeclarations();
    bool TestSimpleArithmetic();
    bool TestComplexArithmetic();
    bool TestBytecodeGeneration();
    bool TestBytecodeExecution();
    bool TestErrorHandling();
    bool TestObfuscationIntegrity();
    bool TestVisitorPipeline();
    
    // 헬퍼 메서드들
    bool ExecuteBytecode(const std::vector<uint8_t>& bytecode);
    bool AssertTranslationSuccess(const std::string& cppCode, const std::string& testName);
    bool AssertTranslationFailure(const std::string& cppCode, const std::string& testName);
    void LogTestResult(const std::string& testName, bool passed, const std::string& message = "");
    
    // 테스트 결과 저장
    struct TestResult 
    {
        std::string name;
        bool passed;
        std::string message;
        std::string expectedOutput;
        std::string actualOutput;
    };
    
    std::vector<TestResult> _testResults;
    std::unique_ptr<Translator::Translator> _translator;
    std::unique_ptr<Engine::Interpreter> _interpreter;
    
    // 테스트 통계
    int _totalTests;
    int _passedTests;
    int _failedTests;
};

} // namespace Tests
} // namespace DarkMatterVM
