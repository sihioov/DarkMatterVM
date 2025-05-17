#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../../../Opcodes.h"

namespace DarkMatterVM {
namespace Translator {
namespace Assembler {

/**
 * @brief 어셈블리 토큰 타입
 */
enum class TokenType {
    MNEMONIC,   ///< 명령어 니모닉
    NUMBER,     ///< 숫자 리터럴
    LABEL,      ///< 레이블 심볼
    DIRECTIVE,  ///< 어셈블러 지시자
    STRING,     ///< 문자열
    EOL,        ///< 줄 끝
    END,        ///< 파일 끝
    UNKNOWN     ///< 알 수 없는 토큰
};

/**
 * @brief 어셈블리 토큰
 */
struct Token {
    TokenType type;     ///< 토큰 타입
    std::string text;   ///< 토큰 텍스트
    uint64_t value;     ///< 숫자 값 (타입이 NUMBER인 경우)
    size_t line;        ///< 소스 코드 라인 번호
    size_t column;      ///< 소스 코드 컬럼 번호
};

/**
 * @brief 어셈블리 파서 클래스
 * 
 * 텍스트 기반 어셈블리 코드를 토큰으로 분석
 */
class Parser {
public:
    /**
     * @brief 생성자
     */
    Parser();
    
    /**
     * @brief 소멸자
     */
    ~Parser() = default;
    
    /**
     * @brief 소스 코드 파싱
     * 
     * @param sourceCode 어셈블리 소스 코드
     * @return bool 성공 여부
     */
    bool Parse(const std::string& sourceCode);
    
    /**
     * @brief 토큰 목록 조회
     * 
     * @return const std::vector<Token>& 토큰 목록
     */
    const std::vector<Token>& GetTokens() const { return _tokens; }
    
    /**
     * @brief 오류 메시지 조회
     * 
     * @return const std::string& 마지막 오류 메시지
     */
    const std::string& GetErrorMessage() const { return _errorMessage; }
    
private:
    // 파서 상태
    std::string _sourceCode;
    std::vector<Token> _tokens;
    size_t _currentPos;
    size_t _currentLine;
    size_t _currentColumn;
    
    // 오류 처리
    std::string _errorMessage;
    bool _hasError;
    
    /**
     * @brief 다음 토큰 처리
     * 
     * @return bool 성공 여부
     */
    bool _ParseNextToken();
    
    /**
     * @brief 개행 문자 처리
     */
    void _SkipWhitespace();
    
    /**
     * @brief 주석 처리
     */
    void _SkipComment();
    
    /**
     * @brief 숫자 리터럴 파싱
     * 
     * @return bool 성공 여부
     */
    bool _ParseNumber();
    
    /**
     * @brief 식별자 파싱 (명령어/레이블)
     * 
     * @return bool 성공 여부
     */
    bool _ParseIdentifier();
    
    /**
     * @brief 오류 설정
     * 
     * @param message 오류 메시지
     */
    void _SetError(const std::string& message);
    
    /**
     * @brief 현재 문자 가져오기
     * 
     * @return char 현재 문자
     */
    char _CurrentChar() const;
    
    /**
     * @brief 다음 문자로 이동
     */
    void _NextChar();
};

} // namespace Assembler
} // namespace Translator
} // namespace DarkMatterVM 