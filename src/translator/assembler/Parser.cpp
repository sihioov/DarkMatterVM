#include "Parser.h"
#include <cctype>
#include <algorithm>
#include <map>

namespace DarkMatterVM {
namespace Translator {
namespace Assembler {

Parser::Parser() 
    : _currentPos(0), _currentLine(1), _currentColumn(1), _hasError(false) {
}

bool Parser::Parse(const std::string& sourceCode) {
    // 초기화
    _sourceCode = sourceCode;
    _tokens.clear();
    _errorMessage.clear();
    _currentPos = 0;
    _currentLine = 1;
    _currentColumn = 1;
    _hasError = false;
    
    // 소스 코드가 비어있는 경우
    if (_sourceCode.empty()) {
        return true;
    }
    
    // 토큰 파싱
    while (_currentPos < _sourceCode.length()) {
        char c = _CurrentChar();
        
        // 공백 건너뛰기
        if (std::isspace(c)) {
            _SkipWhitespace();
            continue;
        }
        
        // 주석 건너뛰기
        if (c == ';') {
            _SkipComment();
            continue;
        }
        
        // 숫자 처리
        if (std::isdigit(c) || (c == '-' && _currentPos + 1 < _sourceCode.length() && 
                               std::isdigit(_sourceCode[_currentPos + 1]))) {
            if (!_ParseNumber()) {
                return false;
            }
            continue;
        }
        
        // 식별자 처리 (명령어, 레이블 등)
        if (std::isalpha(c) || c == '_' || c == '.') {
            if (!_ParseIdentifier()) {
                return false;
            }
            continue;
        }
        
        // 새 줄 처리
        if (c == '\n') {
            Token token;
            token.type = TokenType::EOL;
            token.text = "EOL";
            token.line = _currentLine;
            token.column = _currentColumn;
            _tokens.push_back(token);
            
            _NextChar();
            continue;
        }
        
        // 지원하지 않는 문자
        _SetError("Unexpected character: " + std::string(1, c));
        return false;
    }
    
    // 파일 끝 토큰 추가
    Token endToken;
    endToken.type = TokenType::END;
    endToken.text = "END";
    endToken.line = _currentLine;
    endToken.column = _currentColumn;
    _tokens.push_back(endToken);
    
    return !_hasError;
}

bool Parser::_ParseNextToken() {
    char c = _CurrentChar();
    
    // 공백 건너뛰기
    if (std::isspace(c)) {
        _SkipWhitespace();
        return true;
    }
    
    // 주석 건너뛰기
    if (c == ';') {
        _SkipComment();
        return true;
    }
    
    // 숫자 처리
    if (std::isdigit(c) || (c == '-' && _currentPos + 1 < _sourceCode.length() && 
                           std::isdigit(_sourceCode[_currentPos + 1]))) {
        return _ParseNumber();
    }
    
    // 식별자 처리 (명령어, 레이블 등)
    if (std::isalpha(c) || c == '_' || c == '.') {
        return _ParseIdentifier();
    }
    
    // 새 줄 처리
    if (c == '\n') {
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
    _SetError("Unexpected character: " + std::string(1, c));
    return false;
}

void Parser::_SkipWhitespace() {
    while (_currentPos < _sourceCode.length()) {
        char c = _CurrentChar();
        
        if (!std::isspace(c) || c == '\n') {
            break;
        }
        
        _NextChar();
    }
}

void Parser::_SkipComment() {
    // ';'로 시작하는 한 줄 주석
    while (_currentPos < _sourceCode.length() && _CurrentChar() != '\n') {
        _NextChar();
    }
}

bool Parser::_ParseNumber() {
    size_t startPos = _currentPos;
    size_t startLine = _currentLine;
    size_t startColumn = _currentColumn;
    
    bool isNegative = false;
    if (_CurrentChar() == '-') {
        isNegative = true;
        _NextChar();
    }
    
    // 숫자 기본 파싱
    uint64_t value = 0;
    bool isHex = false;
    
    // 16진수 확인
    if (_CurrentChar() == '0' && _currentPos + 1 < _sourceCode.length() && 
        (_sourceCode[_currentPos + 1] == 'x' || _sourceCode[_currentPos + 1] == 'X')) {
        _NextChar(); // '0' 건너뛰기
        _NextChar(); // 'x' 건너뛰기
        isHex = true;
        
        while (_currentPos < _sourceCode.length()) {
            char c = _CurrentChar();
            
            if (std::isdigit(c)) {
                value = value * 16 + (c - '0');
            } else if (c >= 'a' && c <= 'f') {
                value = value * 16 + (c - 'a' + 10);
            } else if (c >= 'A' && c <= 'F') {
                value = value * 16 + (c - 'A' + 10);
            } else {
                break;
            }
            
            _NextChar();
        }
    } else {
        // 10진수 파싱
        while (_currentPos < _sourceCode.length() && std::isdigit(_CurrentChar())) {
            value = value * 10 + (_CurrentChar() - '0');
            _NextChar();
        }
    }
    
    // 음수인 경우 부호 적용
    if (isNegative) {
        // 64비트 2의 보수로 변환
        value = static_cast<uint64_t>(-static_cast<int64_t>(value));
    }
    
    // 숫자 토큰 생성
    Token token;
    token.type = TokenType::NUMBER;
    token.text = _sourceCode.substr(startPos, _currentPos - startPos);
    token.value = value;
    token.line = startLine;
    token.column = startColumn;
    _tokens.push_back(token);
    
    return true;
}

bool Parser::_ParseIdentifier() {
    size_t startPos = _currentPos;
    size_t startLine = _currentLine;
    size_t startColumn = _currentColumn;
    
    // 식별자 문자열 추출
    while (_currentPos < _sourceCode.length()) {
        char c = _CurrentChar();
        
        if (!(std::isalnum(c) || c == '_' || c == '.')) {
            break;
        }
        
        _NextChar();
    }
    
    std::string identifier = _sourceCode.substr(startPos, _currentPos - startPos);
    
    // 레이블 확인 (식별자 뒤에 ':' 있는 경우)
    if (_currentPos < _sourceCode.length() && _CurrentChar() == ':') {
        // 레이블 토큰 생성
        Token token;
        token.type = TokenType::LABEL;
        token.text = identifier;
        token.line = startLine;
        token.column = startColumn;
        _tokens.push_back(token);
        
        _NextChar(); // ':' 건너뛰기
        return true;
    }
    
    // 지시자 확인 ('.' 으로 시작하는 경우)
    if (!identifier.empty() && identifier[0] == '.') {
        Token token;
        token.type = TokenType::DIRECTIVE;
        token.text = identifier;
        token.line = startLine;
        token.column = startColumn;
        _tokens.push_back(token);
        return true;
    }
    
    // 명령어 니모닉으로 처리
    Token token;
    token.type = TokenType::MNEMONIC;
    token.text = identifier;
    token.line = startLine;
    token.column = startColumn;
    _tokens.push_back(token);
    
    return true;
}

void Parser::_SetError(const std::string& message) {
    _errorMessage = "Line " + std::to_string(_currentLine) + ", Column " + 
                    std::to_string(_currentColumn) + ": " + message;
    _hasError = true;
}

char Parser::_CurrentChar() const {
    if (_currentPos < _sourceCode.length()) {
        return _sourceCode[_currentPos];
    }
    return '\0';
}

void Parser::_NextChar() {
    if (_currentPos < _sourceCode.length()) {
        if (_sourceCode[_currentPos] == '\n') {
            _currentLine++;
            _currentColumn = 1;
        } else {
            _currentColumn++;
        }
        _currentPos++;
    }
}

} // namespace Assembler
} // namespace Translator
} // namespace DarkMatterVM