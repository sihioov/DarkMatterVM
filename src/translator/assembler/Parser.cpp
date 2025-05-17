#include "Parser.h"
#include "../../common/Logger.h"
#include <cctype>
#include <algorithm>
#include <map>

namespace DarkMatterVM 
{
namespace Translator 
{
namespace Assembler 
{

Parser::Parser() 
    : _currentPos(0), _currentLine(1), _currentColumn(1) {}

bool Parser::Parse(const std::string& sourceCode) 
{
    // 초기화
    _sourceCode = sourceCode;
    _tokens.clear();
    _currentPos = 0;
    _currentLine = 1;
    _currentColumn = 1;
    
    // 소스 코드가 비어있는 경우
    if (_sourceCode.empty()) 
    {
        return true;
    }
    
    // 토큰 파싱
    while (_currentPos < _sourceCode.length()) 
    {
        if (!_ParseNextToken()) 
        {
            return false;
        }
    }
    
    // 파일 끝 토큰 추가
    Token endToken;
    endToken.type = TokenType::END;
    endToken.text = "END";
    endToken.line = _currentLine;
    endToken.column = _currentColumn;
    _tokens.push_back(endToken);
    
    return true;
}

bool Parser::_ParseNextToken() 
{
    char c = _CurrentChar();
    
    // 공백 건너뛰기
    if (std::isspace(c)) 
    {
        _SkipWhitespace();
        return true;
    }
    
    // 주석 건너뛰기
    if (c == ';') 
    {
        _SkipComment();
        return true;
    }
    
    // 숫자 처리
    if (std::isdigit(c) || (c == '-' && _currentPos + 1 < _sourceCode.length() && 
                           std::isdigit(_sourceCode[_currentPos + 1]))) 
    {
        Token token;
        if (!_ParseNumber(token)) 
        {
            return false;
        }
        _tokens.push_back(token);
        return true;
    }
    
    // 식별자 처리 (명령어, 레이블 등)
    if (std::isalpha(c) || c == '_' || c == '.') 
    {
        Token token;
        if (!_ParseIdentifier(token)) 
        {
            return false;
        }
        _tokens.push_back(token);
        return true;
    }
    
    // 문자열 처리
    if (c == '"') 
    {
        Token token;
        if (!_ParseString(token)) 
        {
            return false;
        }
        _tokens.push_back(token);
        return true;
    }
    
    // 새 줄 처리
    if (c == '\n') 
    {
        Token token;
        token.type = TokenType::EOL;
        token.text = "EOL";
        token.line = _currentLine;
        token.column = _currentColumn;
        _tokens.push_back(token);
        
        _NextChar();
        return true;
    }
    
    // 지원하지 않는 문자
    _LogError("Unexpected character: " + std::string(1, c));
    return false;
}

void Parser::_SkipWhitespace() 
{
    while (_currentPos < _sourceCode.length()) 
    {
        char c = _CurrentChar();
        
        if (!std::isspace(c) || c == '\n') 
        {
            break;
        }
        
        _NextChar();
    }
}

void Parser::_SkipComment() 
{
    // ';'로 시작하는 한 줄 주석
    while (_currentPos < _sourceCode.length() && _CurrentChar() != '\n') 
    {
        _NextChar();
    }
}

bool Parser::_ParseNumber(Token& token) 
{
    size_t startPos = _currentPos;
    size_t startLine = _currentLine;
    size_t startColumn = _currentColumn;
    
    bool isNegative = false;
    if (_CurrentChar() == '-') 
    {
        isNegative = true;
        _NextChar();
    }
    
    // 숫자 기본 파싱
    uint64_t value = 0;
    bool isHex = false;
    
    // 16진수 확인
    if (_CurrentChar() == '0' && _currentPos + 1 < _sourceCode.length() && 
        (_sourceCode[_currentPos + 1] == 'x' || _sourceCode[_currentPos + 1] == 'X')) 
    {
        _NextChar(); // '0' 건너뛰기
        _NextChar(); // 'x' 건너뛰기
        isHex = true;
        
        while (_currentPos < _sourceCode.length()) 
        {
            char c = _CurrentChar();
            
            if (std::isdigit(c)) 
            {
                value = value * 16 + (c - '0');
            } 
            else if (c >= 'a' && c <= 'f') 
            {
                value = value * 16 + (c - 'a' + 10);
            } 
            else if (c >= 'A' && c <= 'F') 
            {
                value = value * 16 + (c - 'A' + 10);
            } 
            else 
            {
                break;
            }
            
            _NextChar();
        }
    } 
    else 
    {
        // 10진수 파싱
        while (_currentPos < _sourceCode.length() && std::isdigit(_CurrentChar())) 
        {
            value = value * 10 + (_CurrentChar() - '0');
            _NextChar();
        }
    }
    
    // 음수인 경우 부호 적용
    if (isNegative) 
    {
        // 64비트 2의 보수로 변환
        value = static_cast<uint64_t>(-static_cast<int64_t>(value));
    }
    
    // 숫자 토큰 생성
    token.type = TokenType::NUMBER;
    token.text = _sourceCode.substr(startPos, _currentPos - startPos);
    token.value = value;
    token.line = startLine;
    token.column = startColumn;
    
    return true;
}

bool Parser::_ParseIdentifier(Token& token) 
{
    size_t startPos = _currentPos;
    size_t startLine = _currentLine;
    size_t startColumn = _currentColumn;
    
    // 첫 글자는 알파벳, 밑줄 또는 점
    if (!std::isalpha(_CurrentChar()) && _CurrentChar() != '_' && _CurrentChar() != '.') 
    {
        _LogError("Invalid identifier start character");
        return false;
    }
    
    // 식별자 문자 수집
    _NextChar();
    while (_currentPos < _sourceCode.length()) 
    {
        char c = _CurrentChar();
        
        if (!std::isalnum(c) && c != '_' && c != '.') 
        {
            break;
        }
        
        _NextChar();
    }
    
    // 토큰 텍스트 추출
    std::string text = _sourceCode.substr(startPos, _currentPos - startPos);
    
    // 토큰 타입 결정
    token.type = TokenType::MNEMONIC; // 기본 타입은 명령어
    token.text = text;
    token.line = startLine;
    token.column = startColumn;
    
    // 레이블 처리 (':' 문자로 끝나는 경우)
    if (_CurrentChar() == ':') 
    {
        token.type = TokenType::LABEL;
        _NextChar(); // ':' 건너뛰기
    }
    // 어셈블러 지시자 ('.db', '.dw' 등)
    else if (text[0] == '.') 
    {
        token.type = TokenType::DIRECTIVE;
    }
    
    return true;
}

bool Parser::_ParseString(Token& token) 
{
    size_t startPos = _currentPos;
    size_t startLine = _currentLine;
    size_t startColumn = _currentColumn;
    
    // 시작 따옴표 건너뛰기
    _NextChar();
    
    std::string value;
    bool escaped = false;
    
    while (_currentPos < _sourceCode.length()) 
    {
        char c = _CurrentChar();
        
        if (escaped) 
        {
            switch (c) 
            {
                case 'n': value += '\n'; break;
                case 'r': value += '\r'; break;
                case 't': value += '\t'; break;
                case '\\': value += '\\'; break;
                case '"': value += '"'; break;
                default: value += c; break;
            }
            escaped = false;
        } 
        else if (c == '\\') 
        {
            escaped = true;
        } 
        else if (c == '"') 
        {
            _NextChar(); // 닫는 따옴표 건너뛰기
            break;
        } 
        else if (c == '\n') 
        {
            _LogError("Unterminated string");
            return false;
        } 
        else 
        {
            value += c;
        }
        
        _NextChar();
    }
    
    // 문자열 끝에 도달했는데 닫는 따옴표가 없는 경우
    if (_currentPos >= _sourceCode.length() && _CurrentChar() != '"') 
    {
        _LogError("Unterminated string");
        return false;
    }
    
    // 문자열 토큰 생성
    token.type = TokenType::STRING;
    token.text = _sourceCode.substr(startPos, _currentPos - startPos);
    token.line = startLine;
    token.column = startColumn;
    
    return true;
}

char Parser::_CurrentChar() const 
{
    if (_currentPos < _sourceCode.length()) 
    {
        return _sourceCode[_currentPos];
    }
    
    return '\0';
}

void Parser::_NextChar() 
{
    if (_currentPos < _sourceCode.length()) 
    {
        if (_sourceCode[_currentPos] == '\n') 
        {
            _currentLine++;
            _currentColumn = 1;
        } 
        else 
        {
            _currentColumn++;
        }
        
        _currentPos++;
    }
}

void Parser::_LogError(const std::string& message) 
{
    std::string errorMsg = "Line " + std::to_string(_currentLine) + 
                         ", Column " + std::to_string(_currentColumn) + 
                         ": " + message;
    Logger::Error("Parser", errorMsg);
}

} // namespace Assembler
} // namespace Translator
} // namespace DarkMatterVM