#pragma once

#include <string>
#include <memory>
#include <cstdint>

namespace DarkMatterVM 
{
namespace Translator 
{

// 전방 선언
class ASTVisitor;

/**
 * @brief AST 노드 타입 열거형
 */
enum class NodeType 
{
	// 리터럴
	IntegerLiteral,
	FloatLiteral,
	StringLiteral,
	BooleanLiteral,
	
	// 변수 & 심볼
	Variable,
	VariableDecl,
	
	// 연산자
	BinaryOp,
	UnaryOp,
	AssignmentOp,
	
	// 제어 구조
	IfStatement,
	WhileLoop,
	ForLoop,
	SwitchStatement,
	CaseStatement,
	ReturnStatement,
	BreakStatement,
	ContinueStatement,
	
	// 함수
	FunctionDecl,
	FunctionCall,
	
	// 컨테이너
	Block,
	Program,
	
	// 클래스/구조체
	ClassDecl,
	StructDecl,
	MemberAccess
};

/**
 * @brief 기본 AST 노드 클래스 (추상 클래스)
 */
class ASTNode 
{
public:
	/**
	 * @brief 가상 소멸자
	 */
	virtual ~ASTNode() = default;
	
	/**
	 * @brief 노드 타입 반환
	 * @return 노드 타입
	 */
	NodeType GetType() const { return _type; }
	
	/**
	 * @brief 노드 위치 정보 설정
	 * @param line 소스 코드 라인 번호
	 * @param column 소스 코드 컬럼 번호
	 */
	void SetLocation(int line, int column);
	
	/**
	 * @brief 라인 번호 반환
	 * @return 라인 번호
	 */
	int GetLine() const { return _line; }
	
	/**
	 * @brief 컬럼 번호 반환
	 * @return 컬럼 번호
	 */
	int GetColumn() const { return _column; }
	
	/**
	 * @brief 노드 문자열 표현 반환 (디버깅용)
	 * @return 노드 문자열 표현
	 */
	virtual std::string ToString() const = 0;
	
	/**
	 * @brief 노드 복제
	 * @return 복제된 노드
	 */
	virtual std::unique_ptr<ASTNode> Clone() const = 0;
	
	/**
	 * @brief 방문자 수락
	 * @param visitor 방문자
	 */
	virtual void Accept(ASTVisitor& visitor) const = 0;
	
protected:
	/**
	 * @brief 생성자
	 * @param type 노드 타입
	 */
	ASTNode(NodeType type) : _type(type), _line(0), _column(0) {}
	
private:
	NodeType _type;
	int _line;
	int _column;
};

} // namespace Translator
} // namespace DarkMatterVM 