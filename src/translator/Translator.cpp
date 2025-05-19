#include "Translator.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include "ast/ASTNode.h"
#include "codegen/BytecodeBuilder.h"
#include "assembler/Assembler.h"
#include "optimizer/Optimizer.h"

namespace DarkMatterVM 
{
namespace Translator 
{

Translator::Translator(int options)
    : _options(options)
{
}

TranslationResult Translator::TranslateFromCpp(const std::string& sourceCode, const std::string& moduleName)
{
    _bytecode.clear();
    _lastError.clear();
    _moduleName = !moduleName.empty() ? moduleName : "unnamed_module";
    
    try
    {
        // TODO: C++ 파서 구현 후 활성화
        Logger::Info("Translator", "C++ 코드 파싱 시작: " + _moduleName);
        
        // 현재는 임시 구현: 파싱 실패로 처리
        _SetError("C++ 파서가 아직 구현되지 않았습니다");
        return TranslationResult::ParseError;
        
        /*
        // C++ 코드 파싱 및 AST 생성 (향후 구현)
        AST ast;
        if (!ast.ParseCpp(sourceCode))
        {
            _SetError("C++ 코드 파싱 실패: " + ast.GetLastError());
            return TranslationResult::ParseError;
        }
        
        // AST 최적화 적용
        if (_options & static_cast<int>(TranslationOption::Optimize))
        {
            Optimizer optimizer;
            optimizer.OptimizeAST(ast.GetRootNode());
        }
        
        // 바이트코드 생성
        if (!_GenerateBytecode(ast.GetRootNode()))
        {
            return TranslationResult::CodeGenError;
        }
        */
    }
    catch (const std::exception& e)
    {
        _SetError("C++ 변환 중 예외 발생: " + std::string(e.what()));
        return TranslationResult::InternalError;
    }
    
    return TranslationResult::Success;
}

TranslationResult Translator::TranslateFromAssembly(const std::string& sourceCode, const std::string& moduleName)
{
    _bytecode.clear();
    _lastError.clear();
    _moduleName = !moduleName.empty() ? moduleName : "unnamed_module";
    
    try
    {
        Logger::Info("Translator", "어셈블리 코드 파싱 시작: " + _moduleName);
        
        // 어셈블리 코드 파싱 및 바이트코드 생성
        DarkMatterVM::Translator::Assembler::Assembler assembler;
        
        if (!assembler.Assemble(sourceCode))
        {
            _SetError("어셈블리 코드 변환 실패");
            return TranslationResult::ParseError;
        }
        
        // 생성된 바이트코드 저장
        _bytecode = assembler.GetBytecode();
        
        Logger::Info("Translator", "어셈블리 코드 변환 완료: " + std::to_string(_bytecode.size()) + " 바이트");
    }
    catch (const std::exception& e)
    {
        _SetError("어셈블리 변환 중 예외 발생: " + std::string(e.what()));
        return TranslationResult::InternalError;
    }
    
    return TranslationResult::Success;
}

const std::string& Translator::GetLastError() const
{
    return _lastError;
}

std::string Translator::DumpBytecode() const
{
    std::stringstream ss;
    
    ss << "모듈: " << _moduleName << "\n";
    ss << "바이트코드 크기: " << _bytecode.size() << " 바이트\n";
    ss << "====================================\n";
    
    // 바이트코드 16진수 덤프
    for (size_t i = 0; i < _bytecode.size(); ++i)
    {
        // 주소 표시
        if (i % 16 == 0)
        {
            ss << std::setw(4) << std::setfill('0') << std::hex << i << ": ";
        }
        
        // 바이트 값 표시
        ss << std::setw(2) << std::setfill('0') << std::hex 
           << static_cast<int>(_bytecode[i]) << " ";
        
        // 16바이트마다 줄바꿈
        if ((i + 1) % 16 == 0 || i == _bytecode.size() - 1)
        {
            ss << "\n";
        }
    }
    
    return ss.str();
}

bool Translator::_GenerateBytecode(const ASTNode* rootNode)
{
    if (!rootNode)
    {
        _SetError("AST 루트 노드가 null입니다");
        return false;
    }
    
    // BytecodeBuilder를 사용하여 바이트코드 생성
    BytecodeBuilder builder;
    
    if (!builder.GenerateFromAST(rootNode))
    {
        _SetError("바이트코드 생성 실패");
        return false;
    }
    
    // 생성된 바이트코드 저장
    _bytecode = builder.GetBytecode();
    
    Logger::Info("Translator", "바이트코드 생성 완료: " + std::to_string(_bytecode.size()) + " 바이트");
    
    return true;
}

void Translator::_SetError(const std::string& message)
{
    _lastError = message;
    Logger::Error("Translator", message);
}

} // namespace Translator
} // namespace DarkMatterVM
