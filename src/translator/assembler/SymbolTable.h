#pragma once

#include <string>
#include <unordered_map>
#include <cstdint>

namespace DarkMatterVM 
{
namespace Translator 
{
namespace Assembler 
{

/**
 * @brief 심볼 타입
 */
enum class SymbolType 
{
    LABEL,      ///< 코드 레이블
    CONSTANT,   ///< 상수 값
    VARIABLE    ///< 변수
};

/**
 * @brief 심볼 정보
 */
struct SymbolInfo 
{
    SymbolType type;     ///< 심볼 타입
    size_t offset;       ///< 코드 내 오프셋
    uint64_t value;      ///< 값 (상수/변수인 경우)
    bool isDefined;      ///< 정의 여부
};

/**
 * @brief 심볼 테이블 클래스
 * 
 * 어셈블리 과정에서 레이블과 심볼 관리
 */
class SymbolTable 
{
public:
    /**
     * @brief 생성자
     */
    SymbolTable();
    
    /**
     * @brief 소멸자
     */
    ~SymbolTable() = default;
    
    /**
     * @brief 테이블 초기화
     */
    void Clear();
    
    /**
     * @brief 레이블 추가
     * 
     * @param name 레이블 이름
     * @param offset 코드 내 오프셋
     * @return bool 성공 여부
     */
    bool AddLabel(const std::string& name, size_t offset);
    
    /**
     * @brief 상수 추가
     * 
     * @param name 상수 이름
     * @param value 상수 값
     * @return bool 성공 여부
     */
    bool AddConstant(const std::string& name, uint64_t value);
    
    /**
     * @brief 심볼 조회
     * 
     * @param name 심볼 이름
     * @return const SymbolInfo* 심볼 정보 (없으면 nullptr)
     */
    const SymbolInfo* GetSymbol(const std::string& name) const;
    
    /**
     * @brief 심볼 존재 여부 확인
     * 
     * @param name 심볼 이름
     * @return bool 존재 여부
     */
    bool HasSymbol(const std::string& name) const;
    
    /**
     * @brief 정의되지 않은 심볼 수 조회
     * 
     * @return size_t 미정의 심볼 수
     */
    size_t GetUndefinedCount() const;
    
private:
    // 심볼 테이블
    std::unordered_map<std::string, SymbolInfo> _symbols;
    
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