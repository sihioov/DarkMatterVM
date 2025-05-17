#pragma once

namespace DarkMatterVM 
{
namespace Translator 
{

// 전방 선언
class ASTNode;
class IntegerLiteralNode;
class FloatLiteralNode;
class StringLiteralNode;
class BooleanLiteralNode;
class VariableNode;
class VariableDeclNode;
class BinaryOpNode;
class UnaryOpNode;
class AssignmentOpNode;
class IfStatementNode;
class WhileLoopNode;
class ForLoopNode;
class ReturnStatementNode;
class BreakStatementNode;
class ContinueStatementNode;
class FunctionDeclNode;
class FunctionCallNode;
class BlockNode;
class ProgramNode;

/**
 * @brief 방문자 인터페이스
 * 
 * AST 방문자 패턴 구현을 위한 인터페이스
 */
class IASTVisitor
{
public:
	virtual ~IASTVisitor() = default;
	
	// 리터럴 노드
	virtual void Visit(const IntegerLiteralNode* node) = 0;
	virtual void Visit(const FloatLiteralNode* node) = 0;
	virtual void Visit(const StringLiteralNode* node) = 0;
	virtual void Visit(const BooleanLiteralNode* node) = 0;
	
	// 변수 노드
	virtual void Visit(const VariableNode* node) = 0;
	virtual void Visit(const VariableDeclNode* node) = 0;
	
	// 연산자 노드
	virtual void Visit(const BinaryOpNode* node) = 0;
	virtual void Visit(const UnaryOpNode* node) = 0;
	virtual void Visit(const AssignmentOpNode* node) = 0;
	
	// 제어 구조 노드
	virtual void Visit(const IfStatementNode* node) = 0;
	virtual void Visit(const WhileLoopNode* node) = 0;
	virtual void Visit(const ForLoopNode* node) = 0;
	virtual void Visit(const ReturnStatementNode* node) = 0;
	virtual void Visit(const BreakStatementNode* node) = 0;
	virtual void Visit(const ContinueStatementNode* node) = 0;
	
	// 함수 노드
	virtual void Visit(const FunctionDeclNode* node) = 0;
	virtual void Visit(const FunctionCallNode* node) = 0;
	
	// 컨테이너 노드
	virtual void Visit(const BlockNode* node) = 0;
	virtual void Visit(const ProgramNode* node) = 0;
};

/**
 * @brief 기본 방문자 구현
 * 
 * 기본 동작으로 모든 메서드를 구현한 방문자 클래스
 */
class ASTVisitor : public IASTVisitor
{
public:
	// 리터럴 노드
	void Visit(const IntegerLiteralNode* node) override {}
	void Visit(const FloatLiteralNode* node) override {}
	void Visit(const StringLiteralNode* node) override {}
	void Visit(const BooleanLiteralNode* node) override {}
	
	// 변수 노드
	void Visit(const VariableNode* node) override {}
	void Visit(const VariableDeclNode* node) override {}
	
	// 연산자 노드
	void Visit(const BinaryOpNode* node) override {}
	void Visit(const UnaryOpNode* node) override {}
	void Visit(const AssignmentOpNode* node) override {}
	
	// 제어 구조 노드
	void Visit(const IfStatementNode* node) override {}
	void Visit(const WhileLoopNode* node) override {}
	void Visit(const ForLoopNode* node) override {}
	void Visit(const ReturnStatementNode* node) override {}
	void Visit(const BreakStatementNode* node) override {}
	void Visit(const ContinueStatementNode* node) override {}
	
	// 함수 노드
	void Visit(const FunctionDeclNode* node) override {}
	void Visit(const FunctionCallNode* node) override {}
	
	// 컨테이너 노드
	void Visit(const BlockNode* node) override {}
	void Visit(const ProgramNode* node) override {}
};

} // namespace Translator
} // namespace DarkMatterVM 