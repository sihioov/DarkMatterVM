#include "Translator.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <regex>
#include <vector>
#include "ast/base/ASTNode.h"
#include "ast/nodes/LiteralNodes.h"
#include "ast/nodes/VariableNodes.h"
#include "ast/nodes/OperatorNodes.h"
#include "ast/nodes/ContainerNodes.h"
#include "codegen/BytecodeBuilder.h"
#include "assembler/Assembler.h"

namespace DarkMatterVM 
{
namespace Translator 
{

// 간단한 토큰 구조체
struct Token 
{
    enum Type 
    {
        INT_KEYWORD,    // int
        IDENTIFIER,     // 변수명
        NUMBER,         // 숫자
        ASSIGN,         // =
        PLUS,           // +
        MINUS,          // -
        MULTIPLY,       // *
        DIVIDE,         // /
        SEMICOLON,      // ;
        EOF_TOKEN,      // 파일 끝
        UNKNOWN         // 알 수 없는 토큰
    };
    
    Type type;
    std::string value;
    
    Token(Type t, const std::string& v) : type(t), value(v) {}
};

// 간단한 토큰화 함수
std::vector<Token> TokenizeSimpleCpp(const std::string& code) 
{
    std::vector<Token> tokens;
    std::istringstream stream(code);
    std::string word;
    
    // 공백으로 분리된 토큰들 처리
    while (stream >> word) 
    {
        // 세미콜론 분리 처리
        if (word.back() == ';') 
        {
            if (word.length() > 1) 
            {
                std::string mainPart = word.substr(0, word.length() - 1);
                
                // 숫자인지 확인
                if (std::regex_match(mainPart, std::regex("\\d+"))) 
                {
                    tokens.emplace_back(Token::NUMBER, mainPart);
                } 
                else 
                {
                    tokens.emplace_back(Token::IDENTIFIER, mainPart);
                }
            }
            tokens.emplace_back(Token::SEMICOLON, ";");
            continue;
        }
        
        // 키워드 및 토큰 타입 결정
        if (word == "int") 
        {
            tokens.emplace_back(Token::INT_KEYWORD, word);
        } 
        else if (word == "=") 
        {
            tokens.emplace_back(Token::ASSIGN, word);
        } 
        else if (word == "+") 
        {
            tokens.emplace_back(Token::PLUS, word);
        } 
        else if (word == "-") 
        {
            tokens.emplace_back(Token::MINUS, word);
        } 
        else if (word == "*") 
        {
            tokens.emplace_back(Token::MULTIPLY, word);
        } 
        else if (word == "/") 
        {
            tokens.emplace_back(Token::DIVIDE, word);
        } 
        else if (std::regex_match(word, std::regex("\\d+"))) 
        {
            tokens.emplace_back(Token::NUMBER, word);
        } 
        else if (std::regex_match(word, std::regex("[a-zA-Z_][a-zA-Z0-9_]*"))) 
        {
            tokens.emplace_back(Token::IDENTIFIER, word);
        } 
        else 
        {
            tokens.emplace_back(Token::UNKNOWN, word);
        }
    }
    
    tokens.emplace_back(Token::EOF_TOKEN, "");
    return tokens;
}

// 간단한 파서 클래스
class SimpleCppParser 
{
private:
    std::vector<Token> _tokens;
    size_t _currentIndex;
    
    Token& CurrentToken() 
    {
        if (_currentIndex < _tokens.size()) 
        {
            return _tokens[_currentIndex];
        }
        static Token eofToken(Token::EOF_TOKEN, "");
        return eofToken;
    }
    
    void Advance() 
    {
        if (_currentIndex < _tokens.size()) 
        {
            _currentIndex++;
        }
    }
    
    bool Match(Token::Type expectedType) 
    {
        if (CurrentToken().type == expectedType) 
        {
            Advance();
            return true;
        }
        return false;
    }
    
public:
    SimpleCppParser(const std::vector<Token>& tokens) 
        : _tokens(tokens), _currentIndex(0) {}
    
    // 표현식 파싱 (간단한 산술식)
    std::unique_ptr<ASTNode> ParseExpression() 
    {
        auto left = ParsePrimary();
        
        while (CurrentToken().type == Token::PLUS || 
               CurrentToken().type == Token::MINUS ||
               CurrentToken().type == Token::MULTIPLY ||
               CurrentToken().type == Token::DIVIDE) 
        {
            BinaryOpType opType;
            if (CurrentToken().type == Token::PLUS) 
            {
                opType = BinaryOpType::Add;
            } 
            else if (CurrentToken().type == Token::MINUS) 
            {
                opType = BinaryOpType::Subtract;
            } 
            else if (CurrentToken().type == Token::MULTIPLY) 
            {
                opType = BinaryOpType::Multiply;
            } 
            else 
            {
                opType = BinaryOpType::Divide;
            }
            
            Advance(); // 연산자 소비
            auto right = ParsePrimary();
            left = std::make_unique<BinaryOpNode>(opType, std::move(left), std::move(right));
        }
        
        return left;
    }
    
    // 기본 요소 파싱 (숫자, 변수)
    std::unique_ptr<ASTNode> ParsePrimary() 
    {
        if (CurrentToken().type == Token::NUMBER) 
        {
            int64_t value = std::stoll(CurrentToken().value);
            Advance();
            return std::make_unique<IntegerLiteralNode>(value);
        } 
        else if (CurrentToken().type == Token::IDENTIFIER) 
        {
            std::string name = CurrentToken().value;
            Advance();
            return std::make_unique<VariableNode>(name);
        }
        
        throw std::runtime_error("예상하지 못한 토큰: " + CurrentToken().value);
    }
    
    // 변수 선언 파싱 (int x = expression;)
    std::unique_ptr<ASTNode> ParseVariableDeclaration() 
    {
        if (!Match(Token::INT_KEYWORD)) 
        {
            throw std::runtime_error("'int' 키워드를 예상했습니다");
        }
        
        if (CurrentToken().type != Token::IDENTIFIER) 
        {
            throw std::runtime_error("변수 이름을 예상했습니다");
        }
        
        std::string varName = CurrentToken().value;
        Advance();
        
        if (!Match(Token::ASSIGN)) 
        {
            throw std::runtime_error("'=' 를 예상했습니다");
        }
        
        auto initializer = ParseExpression();
        
        if (!Match(Token::SEMICOLON)) 
        {
            throw std::runtime_error("';' 를 예상했습니다");
        }
        
        return std::make_unique<VariableDeclNode>("int", varName, std::move(initializer));
    }
    
    // 프로그램 파싱 (여러 문장)
    std::unique_ptr<ASTNode> ParseProgram() 
    {
        auto block = std::make_unique<BlockNode>();
        
        while (CurrentToken().type != Token::EOF_TOKEN) 
        {
            if (CurrentToken().type == Token::INT_KEYWORD) 
            {
                auto varDecl = ParseVariableDeclaration();
                block->AddStatement(std::move(varDecl));
            } 
            else 
            {
                throw std::runtime_error("지원하지 않는 문장 타입");
            }
        }
        
        return std::move(block);
    }
};

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
        Logger::Info("Translator", "==== C++ 코드 파싱 시작: " + _moduleName + " ====");
        
        // 1단계: 토큰화
        Logger::Info("Translator", "1단계: 소스 코드 토큰화 시작");
        auto tokens = TokenizeSimpleCpp(sourceCode);
        
        // 토큰 정보 출력
        Logger::Info("Translator", "생성된 토큰 개수: " + std::to_string(tokens.size()));
        for (size_t i = 0; i < tokens.size(); ++i) 
        {
            std::string tokenTypeStr;
            switch (tokens[i].type) 
            {
                case Token::INT_KEYWORD:  tokenTypeStr = "INT_KEYWORD"; break;
                case Token::IDENTIFIER:   tokenTypeStr = "IDENTIFIER"; break;
                case Token::NUMBER:       tokenTypeStr = "NUMBER"; break;
                case Token::ASSIGN:       tokenTypeStr = "ASSIGN"; break;
                case Token::PLUS:         tokenTypeStr = "PLUS"; break;
                case Token::MINUS:        tokenTypeStr = "MINUS"; break;
                case Token::MULTIPLY:     tokenTypeStr = "MULTIPLY"; break;
                case Token::DIVIDE:       tokenTypeStr = "DIVIDE"; break;
                case Token::SEMICOLON:    tokenTypeStr = "SEMICOLON"; break;
                case Token::EOF_TOKEN:    tokenTypeStr = "EOF_TOKEN"; break;
                default:                  tokenTypeStr = "UNKNOWN"; break;
            }
            Logger::Info("Translator", "  토큰[" + std::to_string(i) + "]: " + tokenTypeStr + " = '" + tokens[i].value + "'");
        }
        
        // 2단계: 파싱 및 AST 생성
        Logger::Info("Translator", "2단계: AST 파싱 시작");
        SimpleCppParser parser(tokens);
        auto rootNode = parser.ParseProgram();
        Logger::Info("Translator", "AST 생성 완료");
        
        // 3단계: 바이트코드 생성
        Logger::Info("Translator", "3단계: 바이트코드 생성 시작");
        if (!_GenerateBytecode(rootNode.get()))
        {
            return TranslationResult::CodeGenError;
        }
        
        Logger::Info("Translator", "==== C++ 코드 변환 완료: " + std::to_string(_bytecode.size()) + " 바이트 ====");
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

const std::unordered_map<std::string, SymbolInfo>& Translator::GetSymbolTable() const
{
    return _symbolTable;
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
    
    // 심볼 테이블 저장
    _symbolTable = builder.GetSymbolTable();
    
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
