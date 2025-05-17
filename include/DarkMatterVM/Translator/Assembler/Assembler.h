#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include "Parser.h"
#include "SymbolTable.h"
#include "CodeEmitter.h"

namespace DarkMatterVM {
namespace Translator {
namespace Assembler {

/**
 * @brief 어셈블러 클래스
 * 
 * 어셈블리 소스 코드를 바이트코드로 변환
 */
class Assembler {
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
    
private:
    // 컴포넌트
    Parser _parser;
    SymbolTable _symbolTable;
    CodeEmitter _codeEmitter;
};

} // namespace Assembler
} // namespace Translator
} // namespace DarkMatterVM 