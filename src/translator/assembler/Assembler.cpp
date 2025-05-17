#include "Assembler.h"

namespace DarkMatterVM 
{
namespace Translator 
{
namespace Assembler 
{

Assembler::Assembler()
    : _codeEmitter(_symbolTable) 
{
}

bool Assembler::Assemble(const std::string& sourceCode) 
{
    // 컴포넌트 초기화
    _symbolTable.Clear();
    _codeEmitter.Initialize();
    _errorMessage.clear();
    
    // 1단계: 소스 코드 파싱
    if (!_parser.Parse(sourceCode)) 
    {
        _errorMessage = "Parsing error: " + _parser.GetErrorMessage();

        return false;
    }
    
    // 2단계: 바이트코드 생성
    if (!_codeEmitter.EmitCode(_parser.GetTokens())) 
    {
        _errorMessage = "Code generation error: " + _codeEmitter.GetErrorMessage();

        return false;
    }
    
    // 미정의된 심볼 확인
    if (_symbolTable.GetUndefinedCount() > 0) 
    {
        _errorMessage = "Undefined symbols remain";
        
        return false;
    }
    
    return true;
}

const std::vector<uint8_t>& Assembler::GetBytecode() const 
{
    return _codeEmitter.GetBytecode();
}

} // namespace Assembler
} // namespace Translator
} // namespace DarkMatterVM 