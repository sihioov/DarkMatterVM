#include "TestEngine.h"
#include <iostream>
#include <sstream>

namespace DarkMatterVM 
{
namespace Tests 
{

TestEngine::TestEngine() 
    : _totalTests(0), _passedTests(0), _failedTests(0)
{
    _interpreter = std::make_unique<Engine::Interpreter>();
}

bool TestEngine::RunAllTests() 
{
    std::cout << "\n=== Engine 테스트 스위트 시작 ===" << std::endl;
    
    // 모든 테스트 실행
    std::vector<std::pair<std::string, std::function<bool()>>> tests = {
        {"기본 산술 연산", [this]() { return TestBasicArithmetic(); }},
        {"스택 연산", [this]() { return TestStackOperations(); }},
        {"메모리 연산", [this]() { return TestMemoryOperations(); }},
        {"제어 흐름", [this]() { return TestControlFlow(); }},
        {"큰 수 연산", [this]() { return TestLargeNumbers(); }},
        {"오류 처리", [this]() { return TestErrorHandling(); }},
        {"메모리 세그먼트", [this]() { return TestMemorySegments(); }},
        {"인터프리터 상태", [this]() { return TestInterpreterState(); }}
    };
    
    for (const auto& test : tests) 
    {
        _totalTests++;
        std::cout << "테스트 실행 중: " << test.first << " ... ";
        
        bool result = test.second();
        if (result) 
        {
            _passedTests++;
            std::cout << "✅ PASS" << std::endl;
        } 
        else 
        {
            _failedTests++;
            std::cout << "❌ FAIL" << std::endl;
        }
    }
    
    PrintResults();
    return _failedTests == 0;
}

bool TestEngine::RunTest(const std::string& testName) 
{
    if (testName == "기본 산술 연산") return TestBasicArithmetic();
    if (testName == "스택 연산") return TestStackOperations();
    if (testName == "메모리 연산") return TestMemoryOperations();
    if (testName == "제어 흐름") return TestControlFlow();
    if (testName == "큰 수 연산") return TestLargeNumbers();
    if (testName == "오류 처리") return TestErrorHandling();
    if (testName == "메모리 세그먼트") return TestMemorySegments();
    if (testName == "인터프리터 상태") return TestInterpreterState();
    
    std::cout << "알 수 없는 테스트: " << testName << std::endl;
    return false;
}

void TestEngine::PrintResults() const 
{
    std::cout << "\n=== Engine 테스트 결과 ===" << std::endl;
    std::cout << "총 테스트: " << _totalTests << "개" << std::endl;
    std::cout << "통과: " << _passedTests << "개" << std::endl;
    std::cout << "실패: " << _failedTests << "개" << std::endl;
    std::cout << "성공률: " << (_totalTests > 0 ? (_passedTests * 100 / _totalTests) : 0) << "%" << std::endl;
    
    if (_failedTests > 0) 
    {
        std::cout << "\n=== 실패한 테스트 상세 ===" << std::endl;
        for (const auto& result : _testResults) 
        {
            if (!result.passed) 
            {
                std::cout << "❌ " << result.name << ": " << result.message << std::endl;
            }
        }
    }
}

// 테스트 케이스 구현들
bool TestEngine::TestBasicArithmetic() 
{
    // PUSH8 42, PUSH8 13, ADD, HALT
    std::vector<uint8_t> bytecode = {
        static_cast<uint8_t>(Engine::Opcode::PUSH8), 42,
        static_cast<uint8_t>(Engine::Opcode::PUSH8), 13,
        static_cast<uint8_t>(Engine::Opcode::ADD),
        static_cast<uint8_t>(Engine::Opcode::HALT)
    };
    
    return ExecuteBytecode(bytecode, 55);
}

bool TestEngine::TestStackOperations() 
{
    // PUSH8 1, PUSH8 2, DUP, ADD, HALT (결과: 1 + 2 + 2 = 5)
    std::vector<uint8_t> bytecode = {
        static_cast<uint8_t>(Engine::Opcode::PUSH8), 1,
        static_cast<uint8_t>(Engine::Opcode::PUSH8), 2,
        static_cast<uint8_t>(Engine::Opcode::DUP),
        static_cast<uint8_t>(Engine::Opcode::ADD),
        static_cast<uint8_t>(Engine::Opcode::HALT)
    };
    
    return ExecuteBytecode(bytecode, 5);
}

bool TestEngine::TestMemoryOperations() 
{
    // PUSH64 8, ALLOC, DUP, PUSH64 123, STORE64, LOAD64, HALT
    std::vector<uint8_t> bytecode = {
        static_cast<uint8_t>(Engine::Opcode::PUSH64), 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        static_cast<uint8_t>(Engine::Opcode::ALLOC),
        static_cast<uint8_t>(Engine::Opcode::DUP),
        static_cast<uint8_t>(Engine::Opcode::PUSH64), 0x7B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        static_cast<uint8_t>(Engine::Opcode::STORE64),
        static_cast<uint8_t>(Engine::Opcode::LOAD64),
        static_cast<uint8_t>(Engine::Opcode::HALT)
    };
    
    return ExecuteBytecode(bytecode, 123);
}

bool TestEngine::TestControlFlow() 
{
    // PUSH8 10, PUSH8 5, JG, PUSH8 100, HALT
    // 10 > 5 이므로 점프하여 100을 푸시
    std::vector<uint8_t> bytecode = {
        static_cast<uint8_t>(Engine::Opcode::PUSH8), 10,
        static_cast<uint8_t>(Engine::Opcode::PUSH8), 5,
        static_cast<uint8_t>(Engine::Opcode::JG), 0x02, 0x00, // 2바이트 점프
        static_cast<uint8_t>(Engine::Opcode::PUSH8), 50,      // 건너뛸 코드
        static_cast<uint8_t>(Engine::Opcode::PUSH8), 100,     // 점프 목적지
        static_cast<uint8_t>(Engine::Opcode::HALT)
    };
    
    return ExecuteBytecode(bytecode, 100);
}

bool TestEngine::TestLargeNumbers() 
{
    // PUSH64 1000000, PUSH64 2000000, ADD, HALT
    std::vector<uint8_t> bytecode = {
        static_cast<uint8_t>(Engine::Opcode::PUSH64), 0x40, 0x42, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, // 1000000
        static_cast<uint8_t>(Engine::Opcode::PUSH64), 0x80, 0x84, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, // 2000000
        static_cast<uint8_t>(Engine::Opcode::ADD),
        static_cast<uint8_t>(Engine::Opcode::HALT)
    };
    
    return ExecuteBytecode(bytecode, 3000000);
}

bool TestEngine::TestErrorHandling() 
{
    // 잘못된 바이트코드로 테스트
    std::vector<uint8_t> invalidBytecode = {
        0xFF, 0xFF, 0xFF // 알 수 없는 명령어
    };
    
    try 
    {
        _interpreter->LoadBytecode(invalidBytecode.data(), invalidBytecode.size());
        _interpreter->Execute();
        
        LogTestResult("오류 처리", false, "잘못된 바이트코드가 실행됨");
        return false;
    }
    catch (const std::exception& e) 
    {
        LogTestResult("오류 처리", true, "예상대로 예외 발생: " + std::string(e.what()));
        return true;
    }
}

bool TestEngine::TestMemorySegments() 
{
    // 메모리 세그먼트 접근 테스트
    try 
    {
        // 간단한 바이트코드를 로드하여 메모리 세그먼트가 정상 작동하는지 확인
        std::vector<uint8_t> bytecode = {
            static_cast<uint8_t>(Engine::Opcode::PUSH8), 42,
            static_cast<uint8_t>(Engine::Opcode::HALT)
        };
        
        _interpreter->LoadBytecode(bytecode.data(), bytecode.size());
        _interpreter->Execute();
        
        uint64_t result = _interpreter->GetReturnValue();
        if (result == 42) 
        {
            LogTestResult("메모리 세그먼트", true, "메모리 세그먼트 정상 작동");
            return true;
        } 
        else 
        {
            LogTestResult("메모리 세그먼트", false, "메모리 세그먼트 오류: 예상값=42, 실제값=" + std::to_string(result));
            return false;
        }
    }
    catch (const std::exception& e) 
    {
        LogTestResult("메모리 세그먼트", false, "메모리 세그먼트 접근 실패: " + std::string(e.what()));
        return false;
    }
}

bool TestEngine::TestInterpreterState() 
{
    // 인터프리터 상태 테스트
    _interpreter->Reset();
    
    // 초기 상태 확인
    uint64_t returnValue = _interpreter->GetReturnValue();
    if (returnValue != 0) 
    {
        LogTestResult("인터프리터 상태", false, "초기 반환값이 0이 아님: " + std::to_string(returnValue));
        return false;
    }
    
    // 간단한 바이트코드 실행 후 상태 확인
    std::vector<uint8_t> bytecode = {
        static_cast<uint8_t>(Engine::Opcode::PUSH8), 42,
        static_cast<uint8_t>(Engine::Opcode::HALT)
    };
    
    _interpreter->LoadBytecode(bytecode.data(), bytecode.size());
    _interpreter->Execute();
    
    returnValue = _interpreter->GetReturnValue();
    if (returnValue != 42) 
    {
        LogTestResult("인터프리터 상태", false, "실행 후 반환값 오류: " + std::to_string(returnValue));
        return false;
    }
    
    LogTestResult("인터프리터 상태", true, "상태 관리 정상");
    return true;
}

// 헬퍼 메서드 구현들
bool TestEngine::ExecuteBytecode(const std::vector<uint8_t>& bytecode, uint64_t expectedResult) 
{
    try 
    {
        _interpreter->LoadBytecode(bytecode.data(), bytecode.size());
        _interpreter->Execute();
        
        uint64_t result = _interpreter->GetReturnValue();
        return AssertResult(expectedResult, result, "바이트코드 실행");
    }
    catch (const std::exception& e) 
    {
        Logger::Error("TestEngine", "바이트코드 실행 실패: " + std::string(e.what()));
        return false;
    }
}

bool TestEngine::AssertResult(uint64_t expected, uint64_t actual, const std::string& testName) 
{
    if (expected == actual) 
    {
        LogTestResult(testName, true, "예상값과 일치: " + std::to_string(expected));
        return true;
    } 
    else 
    {
        std::string message = "예상값=" + std::to_string(expected) + ", 실제값=" + std::to_string(actual);
        LogTestResult(testName, false, message);
        return false;
    }
}

void TestEngine::LogTestResult(const std::string& testName, bool passed, const std::string& message) 
{
    TestResult result;
    result.name = testName;
    result.passed = passed;
    result.message = message;
    _testResults.push_back(result);
    
    if (passed) 
    {
        Logger::Info("TestEngine", "✅ " + testName + " 통과" + (message.empty() ? "" : ": " + message));
    } 
    else 
    {
        Logger::Error("TestEngine", "❌ " + testName + " 실패" + (message.empty() ? "" : ": " + message));
    }
}

} // namespace Tests
} // namespace DarkMatterVM
