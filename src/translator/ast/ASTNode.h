#pragma once

#include <string>
#include <vector>
#include <memory>
#include <cstdint>

namespace DarkMatterVM 
{
namespace Translator 
{

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
 * @brief 이항 연산자 타입
 */
enum class BinaryOpType 
{
	Add,        // +
	Subtract,   // -
	Multiply,   // *
	Divide,     // /
	Modulo,     // %
	
	Equal,      // ==
	NotEqual,   // !=
	Greater,    // >
	Less,       // <
	GreaterEq,  // >=
	LessEq,     // <=
	
	LogicalAnd, // &&
	LogicalOr,  // ||
	
	BitwiseAnd, // &
	BitwiseOr,  // |
	BitwiseXor, // ^
	ShiftLeft,  // <<
	ShiftRight  // >>
};

/**
 * @brief 단항 연산자 타입
 */
enum class UnaryOpType 
{
	Negate,     // -
	LogicalNot, // !
	BitwiseNot, // ~
	PreIncrement,  // ++x
	PostIncrement, // x++
	PreDecrement,  // --x
	PostDecrement  // x--
};

/**
 * @brief 기본 AST 노드 클래스 (추상 클래스)
 */
class ASTNode 
{
public:
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
	
protected:
	ASTNode(NodeType type) : _type(type), _line(0), _column(0) {}
	
private:
	NodeType _type;
	int _line;
	int _column;
};

/**
 * @brief 정수 리터럴 노드
 */
class IntegerLiteralNode : public ASTNode 
{
public:
	IntegerLiteralNode(int64_t value) 
		: ASTNode(NodeType::IntegerLiteral), _value(value) {}
	
	int64_t GetValue() const { return _value; }
	
	std::string ToString() const override;
	
private:
	int64_t _value;
};

/**
 * @brief 변수 참조 노드
 */
class VariableNode : public ASTNode 
{
public:
	VariableNode(const std::string& name) 
		: ASTNode(NodeType::Variable), _name(name) {}
	
	const std::string& GetName() const { return _name; }
	
	std::string ToString() const override;
	
private:
	std::string _name;
};

/**
 * @brief 이항 연산자 노드
 */
class BinaryOpNode : public ASTNode 
{
public:
	BinaryOpNode(BinaryOpType opType, std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right)
		: ASTNode(NodeType::BinaryOp), _opType(opType), _left(std::move(left)), _right(std::move(right)) {}
	
	BinaryOpType GetOpType() const { return _opType; }
	const ASTNode* GetLeft() const { return _left.get(); }
	const ASTNode* GetRight() const { return _right.get(); }
	
	std::string ToString() const override;
	
private:
	std::string _GetOpSymbol() const;
	
	BinaryOpType _opType;
	std::unique_ptr<ASTNode> _left;
	std::unique_ptr<ASTNode> _right;
};

/**
 * @brief 변수 선언 노드
 */
class VariableDeclNode : public ASTNode 
{
public:
	VariableDeclNode(const std::string& type, const std::string& name, std::unique_ptr<ASTNode> initializer = nullptr) 
		: ASTNode(NodeType::VariableDecl), _type(type), _name(name), _initializer(std::move(initializer)) {}
	
	const std::string& GetType() const { return _type; }
	const std::string& GetName() const { return _name; }
	const ASTNode* GetInitializer() const { return _initializer.get(); }
	
	std::string ToString() const override;
	
private:
	std::string _type;
	std::string _name;
	std::unique_ptr<ASTNode> _initializer;
};

/**
 * @brief 블록 노드 (여러 문장을 포함)
 */
class BlockNode : public ASTNode 
{
public:
	BlockNode() : ASTNode(NodeType::Block) {}
	
	void AddStatement(std::unique_ptr<ASTNode> statement);
	
	const std::vector<std::unique_ptr<ASTNode>>& GetStatements() const { return _statements; }
	
	std::string ToString() const override;
	
private:
	std::vector<std::unique_ptr<ASTNode>> _statements;
};

// AST 노드 팩토리 - 새 노드 생성 헬퍼 함수들
class ASTNodeFactory 
{
public:
	static std::unique_ptr<IntegerLiteralNode> CreateIntegerLiteral(int64_t value);
	static std::unique_ptr<VariableNode> CreateVariable(const std::string& name);
	static std::unique_ptr<BinaryOpNode> CreateBinaryOp(
		BinaryOpType opType, 
		std::unique_ptr<ASTNode> left, 
		std::unique_ptr<ASTNode> right
	);
	static std::unique_ptr<VariableDeclNode> CreateVariableDecl(
		const std::string& type, 
		const std::string& name, 
		std::unique_ptr<ASTNode> initializer = nullptr
	);
	static std::unique_ptr<BlockNode> CreateBlock();
};

} // namespace Translator
} // namespace DarkMatterVM