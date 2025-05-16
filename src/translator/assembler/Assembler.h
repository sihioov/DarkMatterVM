#pragma once

#include <string>
#include <vector>
#include <memory>
#include "Parser.h"
#include "SymbolTable.h"
#include "CodeEmitter.h"

namespace DarkMatterVM 
{
namespace Translator 
{
namespace Assembler 
{

/**
 * @brief 어셈블러 클래스
 * 
 * 어셈블리 코드를 받아 바이트코드로 변환하는 클래스
 * Parser, SymbolTable, CodeEmitter 컴포넌트를 조정
 */
class Assembler 
{
public:
    /**
     * @brief 생성자
     */
    Assembler();
    
    /**
     * @brief 소멸자
     */
    ~Assembler() = default;
    
    /**
     * @brief 어셈블 수행
     * 
     * @param sourceCode 어셈블리 소스 코드
     * @return bool 성공 여부
     */
    bool Assemble(const std::string& sourceCode);
    
    /**
     * @brief 생성된 바이트코드 조회
     * 
     * @return const std::vector<uint8_t>& 바이트코드
     */
    const std::vector<uint8_t>& GetBytecode() const;
    
    /**
     * @brief 오류 메시지 조회
     * 
     * @return const std::string& 오류 메시지
     */
    const std::string& GetErrorMessage() const { return _errorMessage; }
    
private:
    // 컴포넌트
    Parser _parser;
    SymbolTable _symbolTable;
    CodeEmitter _codeEmitter;
    
    // 결과 및 상태
    std::string _errorMessage;
};

} // namespace Assembler
} // namespace Translator
} // namespace DarkMatterVM