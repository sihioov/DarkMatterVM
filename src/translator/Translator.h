#pragma once

#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <common/Logger.h>
#include "ast/base/ASTNode.h"

namespace DarkMatterVM 
{
namespace Translator 
{

/**
 * @brief 코드 변환 옵션
 */
enum class TranslationOption
{
    None = 0,
    Optimize = 1 << 0,      ///< 코드 최적화 활성화
    Debug = 1 << 1,         ///< 디버그 정보 포함
    EmitComments = 1 << 2   ///< 바이트코드에 주석 포함
};

/**
 * @brief 변환 결과 코드
 */
enum class TranslationResult
{
    Success,                ///< 변환 성공
    ParseError,             ///< 구문 분석 오류
    SemanticError,          ///< 의미 분석 오류
    CodeGenError,           ///< 코드 생성 오류
    InternalError           ///< 내부 오류
};

/**
 * @brief 번역기 클래스
 * 
 * C++ 코드를 DarkMatterVM 바이트코드로 변환
 */
class Translator 
{
public:
    /**
     * @brief 생성자
     * 
     * @param options 변환 옵션
     */
    Translator(int options = 0);
    
    /**
     * @brief 소멸자
     */
    ~Translator() = default;
    
    /**
     * @brief C++ 소스 코드를 바이트코드로 변환
     * 
     * @param sourceCode C++ 소스 코드
     * @param moduleName 모듈 이름 (선택적)
     * @return TranslationResult 변환 결과
     */
    TranslationResult TranslateFromCpp(const std::string& sourceCode, const std::string& moduleName = "");
    
    /**
     * @brief 어셈블리 소스 코드를 바이트코드로 변환 (개발/테스트용)
     * 
     * @param sourceCode 어셈블리 소스 코드
     * @param moduleName 모듈 이름 (선택적)
     * @return TranslationResult 변환 결과
     */
    TranslationResult TranslateFromAssembly(const std::string& sourceCode, const std::string& moduleName = "");
    
    /**
     * @brief 생성된 바이트코드 가져오기
     * 
     * @return const std::vector<uint8_t>& 바이트코드
     */
    const std::vector<uint8_t>& GetBytecode() const { return _bytecode; }
    
    /**
     * @brief 마지막 오류 메시지 가져오기
     * 
     * @return const std::string& 오류 메시지
     */
    const std::string& GetLastError() const;
    
    /**
     * @brief 바이트코드 덤프 문자열 생성 (디버깅용)
     * 
     * @return std::string 바이트코드 덤프
     */
    std::string DumpBytecode() const;
    
private:
    int _options;                     ///< 변환 옵션
    std::vector<uint8_t> _bytecode;   ///< 생성된 바이트코드
    std::string _lastError;           ///< 마지막 오류 메시지
    std::string _moduleName;          ///< 현재 모듈 이름
    
    /**
     * @brief AST로부터 바이트코드 생성
     * 
     * @param rootNode AST 루트 노드
     * @return bool 성공 여부
     */
    bool _GenerateBytecode(const ASTNode* rootNode);
    
    /**
     * @brief 오류 메시지 설정
     * 
     * @param message 오류 메시지
     */
    void _SetError(const std::string& message);
};

} // namespace Translator
} // namespace DarkMatterVM 