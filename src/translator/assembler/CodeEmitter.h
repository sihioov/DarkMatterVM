#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>
#include "../../../include/Opcodes.h"
#include "Parser.h"
#include "SymbolTable.h"

namespace DarkMatterVM {
namespace Translator {
namespace Assembler {

/**
 * @brief 코드 내 수정(fix-up) 항목
 */
struct Fixup {
    size_t offset;           ///< 수정해야 할 위치
    std::string targetLabel; ///< 대상 레이블 이름
    uint8_t size;            ///< 수정 크기 (바이트)
    bool isRelative;         ///< 상대 주소 여부
};

/**
 * @brief 코드 생성기 클래스
 * 
 * 파싱된 어셈블리 코드를 바이트코드로 변환
 */
class CodeEmitter {
public:
    /**
     * @brief 생성자
     * 
     * @param symbolTable 심볼 테이블 참조
     */
    CodeEmitter(SymbolTable& symbolTable);
    
    /**
     * @brief 소멸자
     */
    ~CodeEmitter() = default;
    
    /**
     * @brief 초기화
     */
    void Initialize();
    
    /**
     * @brief 바이트코드 생성
     * 
     * @param tokens 파싱된 토큰 목록
     * @return bool 성공 여부
     */
    bool EmitCode(const std::vector<Token>& tokens);
    
    /**
     * @brief 생성된 바이트코드 조회
     * 
     * @return const std::vector<uint8_t>& 바이트코드
     */
    const std::vector<uint8_t>& GetBytecode() const { return _bytecode; }
    
    /**
     * @brief 현재 코드 오프셋 조회
     * 
     * @return size_t 현재 오프셋
     */
    size_t GetCurrentOffset() const { return _bytecode.size(); }
    
    /**
     * @brief 오류 메시지 조회
     * 
     * @return const std::string& 마지막 오류 메시지
     */
    const std::string& GetErrorMessage() const { return _errorMessage; }
    
private:
    // 코드 생성 상태
    std::vector<uint8_t> _bytecode;
    std::vector<Fixup> _fixups;
    SymbolTable& _symbolTable;
    
    // 토큰 처리
    size_t _currentTokenIndex;
    const std::vector<Token>* _tokens;
    
    // 오류 처리
    std::string _errorMessage;
    bool _hasError;
    
    /**
     * @brief 바이트 추가
     * 
     * @param value 바이트 값
     */
    void _EmitByte(uint8_t value);
    
    /**
     * @brief 2바이트 추가
     * 
     * @param value 2바이트 값
     */
    void _EmitUInt16(uint16_t value);
    
    /**
     * @brief 4바이트 추가
     * 
     * @param value 4바이트 값
     */
    void _EmitUInt32(uint32_t value);
    
    /**
     * @brief 8바이트 추가
     * 
     * @param value 8바이트 값
     */
    void _EmitUInt64(uint64_t value);
    
    /**
     * @brief 명령어 처리
     * 
     * @param opcode 명령어 코드
     * @return bool 성공 여부
     */
    bool _ProcessInstruction(Engine::Opcode opcode);
    
    /**
     * @brief 레이블 수정(fix-up) 적용
     * 
     * @return bool 성공 여부
     */
    bool _ApplyFixups();
    
    /**
     * @brief 오류 설정
     * 
     * @param message 오류 메시지
     * @param token 오류 발생 토큰 (선택적)
     */
    void _SetError(const std::string& message, const Token* token = nullptr);
    
    /**
     * @brief 현재 토큰 가져오기
     * 
     * @return const Token& 현재 토큰
     */
    const Token& _CurrentToken() const;
    
    /**
     * @brief 다음 토큰으로 이동
     */
    void _NextToken();
};

} // namespace Assembler
} // namespace Translator
} // namespace DarkMatterVM 