#pragma once

#include <string>
#include <vector>

namespace DarkMatterVM 
{
namespace Translator 
{
namespace Assembler 
{

/**
 * @brief 토큰 타입
 */
enum class TokenType 
{
    MNEMONIC,   ///< 명령어 (ADD, SUB 등)
    LABEL,      ///< 레이블 (main:, loop: 등)
    DIRECTIVE,  ///< 지시자 (.db, .dw 등)
    NUMBER,     ///< 숫자 (10, 0x20 등)
    STRING,     ///< 문자열 ("hello" 등)
    EOL,        ///< 줄 끝
    END,        ///< 파일 끝
    UNKNOWN     ///< 알 수 없는 토큰
};

/**
 * @brief 토큰 정보
 */
struct Token 
{
    TokenType type;       ///< 토큰 타입
    std::string text;     ///< 토큰 텍스트
    uint64_t value;       ///< 숫자 토큰의 값
    size_t line;          ///< 토큰의 라인 번호
    size_t column;        ///< 토큰의 열 번호
};

/**
 * @brief 어셈블리 코드 파서 클래스
 * 
 * 어셈블리 소스 코드를 토큰으로 파싱
 */
class Parser 
{
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
     * @brief 파싱된 토큰 목록 조회
     * 
     * @return const std::vector<Token>& 토큰 목록
     */
    const std::vector<Token>& GetTokens() const { return _tokens; }
    
private:
    // 입력 처리
    std::string _sourceCode;
    size_t _currentPos;
    size_t _currentLine;
    size_t _currentColumn;
    
    // 토큰 목록
    std::vector<Token> _tokens;
    
    /**
     * @brief 다음 토큰 파싱
     * 
     * @return bool 성공 여부 (더 이상 토큰이 없으면 false)
     */
    bool _ParseNextToken();
    
    /**
     * @brief 숫자 파싱
     * 
     * @param token 결과 토큰
     * @return bool 성공 여부
     */
    bool _ParseNumber(Token& token);
    
    /**
     * @brief 식별자 파싱 (레이블, 명령어 등)
     * 
     * @param token 결과 토큰
     * @return bool 성공 여부
     */
    bool _ParseIdentifier(Token& token);
    
    /**
     * @brief 문자열 파싱
     * 
     * @param token 결과 토큰
     * @return bool 성공 여부
     */
    bool _ParseString(Token& token);
    
    /**
     * @brief 현재 문자 확인
     * 
     * @return char 현재 문자 (끝이면 '\0')
     */
    char _CurrentChar() const;
    
    /**
     * @brief 다음 문자로 이동
     */
    void _NextChar();
    
    /**
     * @brief 공백 건너뛰기
     */
    void _SkipWhitespace();
    
    /**
     * @brief 주석 건너뛰기
     */
    void _SkipComment();
    
    /**
     * @brief 오류 로깅
     * 
     * @param message 오류 메시지
     */
    void _LogError(const std::string& message);
};

} // namespace Assembler
} // namespace Translator
} // namespace DarkMatterVM 