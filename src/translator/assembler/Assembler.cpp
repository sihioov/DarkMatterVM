#include "Assembler.h"
#include "../../common/Logger.h"

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
    
    // 1단계: 소스 코드 파싱
    if (!_parser.Parse(sourceCode)) 
    {
        Logger::Error("Assembler", "Parsing failed");
        return false;
    }
    
    // 2단계: 바이트코드 생성
    if (!_codeEmitter.EmitCode(_parser.GetTokens())) 
    {
        Logger::Error("Assembler", "Code generation failed");
        return false;
    }
    
    // 미정의된 심볼 확인
    if (_symbolTable.GetUndefinedCount() > 0) 
    {
        Logger::Error("Assembler", "Undefined symbols remain");
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