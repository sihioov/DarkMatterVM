#include "TestTranslator.h"
#include <iostream>
#include <sstream>

namespace DarkMatterVM 
{
namespace Tests 
{

TestTranslator::TestTranslator() 
    : _totalTests(0), _passedTests(0), _failedTests(0)
{
    _translator = std::make_unique<Translator::Translator>(Translator::TranslationOption::Obfuscate);
    _interpreter = std::make_unique<Engine::Interpreter>();
}

bool TestTranslator::RunAllTests() 
{
    std::cout << "\n=== Translator 테스트 스위트 시작 ===" << std::endl;
    
    // 모든 테스트 실행
    std::vector<std::pair<std::string, std::function<bool()>>> tests = {
        {"빈 코드 테스트", [this]() { return TestEmptyCode(); }},
        {"단순 변수 선언", [this]() { return TestSimpleVariableDeclaration(); }},
        {"다중 변수 선언", [this]() { return TestMultipleVariableDeclarations(); }},
        {"단순 산술 연산", [this]() { return TestSimpleArithmetic(); }},
        {"복합 산술 연산", [this]() { return TestComplexArithmetic(); }},
        {"바이트코드 생성", [this]() { return TestBytecodeGeneration(); }},
        {"바이트코드 실행", [this]() { return TestBytecodeExecution(); }},
        {"오류 처리", [this]() { return TestErrorHandling(); }}
        ,{"난독화 무결성", [this]() { return TestObfuscationIntegrity(); }}
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

bool TestTranslator::RunTest(const std::string& testName) 
{
    if (testName == "빈 코드 테스트") return TestEmptyCode();
    if (testName == "단순 변수 선언") return TestSimpleVariableDeclaration();
    if (testName == "다중 변수 선언") return TestMultipleVariableDeclarations();
    if (testName == "단순 산술 연산") return TestSimpleArithmetic();
    if (testName == "복합 산술 연산") return TestComplexArithmetic();
    if (testName == "바이트코드 생성") return TestBytecodeGeneration();
    if (testName == "바이트코드 실행") return TestBytecodeExecution();
    if (testName == "오류 처리") return TestErrorHandling();
    
    std::cout << "알 수 없는 테스트: " << testName << std::endl;
    return false;
}

void TestTranslator::PrintResults() const 
{
    std::cout << "\n=== Translator 테스트 결과 ===" << std::endl;
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
bool TestTranslator::TestEmptyCode() 
{
    return AssertTranslationSuccess("", "빈 코드 테스트");
}

bool TestTranslator::TestSimpleVariableDeclaration() 
{
    std::string cppCode = R"(
        int x = 42;
    )";
    
    return AssertTranslationSuccess(cppCode, "단순 변수 선언");
}

bool TestTranslator::TestMultipleVariableDeclarations() 
{
    std::string cppCode = R"(
        int a = 10;
        int b = 20;
        int c = 30;
    )";
    
    return AssertTranslationSuccess(cppCode, "다중 변수 선언");
}

bool TestTranslator::TestSimpleArithmetic() 
{
    std::string cppCode = R"(
        int x = 10;
        int y = 5;
        int sum = x + y;
    )";
    
    return AssertTranslationSuccess(cppCode, "단순 산술 연산");
}

bool TestTranslator::TestComplexArithmetic() 
{
    std::string cppCode = R"(
        int a = 10;
        int b = 3;
        int c = 2;
        int result = a * b + c;
    )";
    
    return AssertTranslationSuccess(cppCode, "복합 산술 연산");
}

bool TestTranslator::TestBytecodeGeneration() 
{
    std::string cppCode = R"(
        int x = 42;
    )";
    
    auto result = _translator->TranslateFromCpp(cppCode, "test_module");
    if (result != Translator::TranslationResult::Success) 
    {
        LogTestResult("바이트코드 생성", false, "번역 실패");
        return false;
    }
    
    const auto& bytecode = _translator->GetBytecode();
    if (bytecode.empty()) 
    {
        LogTestResult("바이트코드 생성", false, "바이트코드가 비어있음");
        return false;
    }
    
    std::string dump = _translator->DumpBytecode();
    if (dump.empty()) 
    {
        LogTestResult("바이트코드 생성", false, "바이트코드 덤프 실패");
        return false;
    }
    
    LogTestResult("바이트코드 생성", true, "바이트코드 크기: " + std::to_string(bytecode.size()) + " 바이트");
    return true;
}

bool TestTranslator::TestBytecodeExecution() 
{
    std::string cppCode = R"(
        int x = 42;
    )";
    
    auto result = _translator->TranslateFromCpp(cppCode, "test_module");
    if (result != Translator::TranslationResult::Success) 
    {
        LogTestResult("바이트코드 실행", false, "번역 실패");
        return false;
    }
    
    const auto& bytecode = _translator->GetBytecode();
    if (!ExecuteBytecode(bytecode)) 
    {
        LogTestResult("바이트코드 실행", false, "VM 실행 실패");
        return false;
    }
    
    LogTestResult("바이트코드 실행", true, "VM에서 성공적으로 실행됨");
    return true;
}

bool TestTranslator::TestErrorHandling() 
{
    // 잘못된 C++ 코드로 테스트
    std::string invalidCode = R"(
        int x = ;  // 세미콜론 누락
    )";
    
    auto result = _translator->TranslateFromCpp(invalidCode, "test_module");
    if (result == Translator::TranslationResult::Success) 
    {
        LogTestResult("오류 처리", false, "잘못된 코드가 성공으로 처리됨");
        return false;
    }
    
    LogTestResult("오류 처리", true, "잘못된 코드가 적절히 거부됨");
    return true;
}

bool TestTranslator::TestObfuscationIntegrity()
{
    std::string cppCode = R"(
        int a = 10;
        int b = 20;
        int c = a + b;
    )";

    // 1) 난독화 Translator (멤버 _translator는 이미 Obfuscate 옵션 사용)
    if (_translator->TranslateFromCpp(cppCode, "obf_module") != Translator::TranslationResult::Success)
    {
        LogTestResult("난독화 무결성", false, "Obfuscate 번역 실패");
        return false;
    }
    auto obfCode = _translator->GetBytecode();

    // 2) 클린 Translator (난독화 옵션 없음)
    Translator::Translator cleanTr; // 옵션 없음
    if (cleanTr.TranslateFromCpp(cppCode, "clean_module") != Translator::TranslationResult::Success)
    {
        LogTestResult("난독화 무결성", false, "Clean 번역 실패");
        return false;
    }
    auto cleanCode = cleanTr.GetBytecode();

    if (obfCode == cleanCode)
    {
        LogTestResult("난독화 무결성", false, "Obfuscation 결과가 원본과 동일");
        return false;
    }

    // Execute both and compare result
    try
    {
        // 난독화 바이트코드 실행
        _interpreter->Reset();
        _interpreter->LoadBytecode(obfCode.data(), obfCode.size());
        int execStatus = _interpreter->Execute();
        if (execStatus != 0)
        {
            Logger::Error("TestTranslator", "Interpreter 실행 반환값 !=0 (" + std::to_string(execStatus) + ")");
            return false;
        }
        uint64_t resObf = _interpreter->GetReturnValue();

        // 원본 바이트코드 실행
        _interpreter->Reset();
        _interpreter->LoadBytecode(cleanCode.data(), cleanCode.size());
        execStatus = _interpreter->Execute();
        if (execStatus != 0)
        {
            Logger::Error("TestTranslator", "Interpreter 실행 반환값 !=0 (" + std::to_string(execStatus) + ")");
            return false;
        }
        uint64_t resClean = _interpreter->GetReturnValue();

        if (resObf == resClean)
        {
            LogTestResult("난독화 무결성", true, "결과값=" + std::to_string(resClean));
            return true;
        }
        else
        {
            LogTestResult("난독화 무결성", false, "Obf 결과=" + std::to_string(resObf) + ", Clean 결과=" + std::to_string(resClean));
            return false;
        }
    }
    catch(const std::exception& e)
    {
        LogTestResult("난독화 무결성", false, e.what());
        return false;
    }
}

// 헬퍼 메서드 구현들
bool TestTranslator::ExecuteBytecode(const std::vector<uint8_t>& bytecode) 
{
    try 
    {
        _interpreter->LoadBytecode(bytecode.data(), bytecode.size());
        int execStatus = _interpreter->Execute();
        if (execStatus != 0)
        {
            Logger::Error("TestTranslator", "Interpreter 실행 반환값 !=0 (" + std::to_string(execStatus) + ")");
            return false;
        }
        return true;
    }
    catch (const std::exception& e) 
    {
        Logger::Error("TestTranslator", "바이트코드 실행 실패: " + std::string(e.what()));
        return false;
    }
}

bool TestTranslator::AssertTranslationSuccess(const std::string& cppCode, const std::string& testName) 
{
    auto result = _translator->TranslateFromCpp(cppCode, "test_module");
    if (result == Translator::TranslationResult::Success) 
    {
        LogTestResult(testName, true);
        return true;
    } 
    else 
    {
        std::string errorMsg = "번역 실패: " + _translator->GetLastError();
        LogTestResult(testName, false, errorMsg);
        return false;
    }
}

bool TestTranslator::AssertTranslationFailure(const std::string& cppCode, const std::string& testName) 
{
    auto result = _translator->TranslateFromCpp(cppCode, "test_module");
    if (result != Translator::TranslationResult::Success) 
    {
        LogTestResult(testName, true, "예상대로 번역 실패");
        return true;
    } 
    else 
    {
        LogTestResult(testName, false, "잘못된 코드가 성공으로 처리됨");
        return false;
    }
}

void TestTranslator::LogTestResult(const std::string& testName, bool passed, const std::string& message) 
{
    TestResult result;
    result.name = testName;
    result.passed = passed;
    result.message = message;
    _testResults.push_back(result);
    
    if (passed) 
    {
        Logger::Info("TestTranslator", "✅ " + testName + " 통과" + (message.empty() ? "" : ": " + message));
    } 
    else 
    {
        Logger::Error("TestTranslator", "❌ " + testName + " 실패" + (message.empty() ? "" : ": " + message));
    }
}

} // namespace Tests
} // namespace DarkMatterVM
