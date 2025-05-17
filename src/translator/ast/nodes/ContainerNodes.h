#pragma once

#include <vector>
#include <stdexcept>
#include "../base/ASTNode.h"
#include "../visitor/ASTVisitor.h"

namespace DarkMatterVM 
{
namespace Translator 
{

/**
 * @brief 블록 노드 (여러 문장을 포함)
 */
class BlockNode : public ASTNode 
{
public:
	/**
	 * @brief 생성자
	 */
	BlockNode() : ASTNode(NodeType::Block) {}
	
	/**
	 * @brief 문장 추가
	 * @param statement 추가할 문장
	 * @throws std::invalid_argument 문장이 null인 경우
	 */
	void AddStatement(std::unique_ptr<ASTNode> statement);
	
	/**
	 * @brief 문장 목록 반환
	 * @return 문장 목록
	 */
	const std::vector<std::unique_ptr<ASTNode>>& GetStatements() const { return _statements; }
	
	/**
	 * @brief 문장 개수 반환
	 * @return 문장 개수
	 */
	size_t GetStatementCount() const { return _statements.size(); }
	
	/**
	 * @brief 문자열 표현 반환
	 * @return 문자열 표현
	 */
	std::string ToString() const override;
	
	/**
	 * @brief 노드 복제
	 * @return 복제된 노드
	 */
	std::unique_ptr<ASTNode> Clone() const override;
	
	/**
	 * @brief 방문자 수락
	 * @param visitor 방문자
	 */
	void Accept(ASTVisitor& visitor) const override;
	
private:
	std::vector<std::unique_ptr<ASTNode>> _statements;
};

/**
 * @brief 프로그램 노드 (AST 루트)
 */
class ProgramNode : public ASTNode
{
public:
	/**
	 * @brief 생성자
	 */
	ProgramNode() : ASTNode(NodeType::Program) {}
	
	/**
	 * @brief 선언 추가
	 * @param declaration 추가할 선언
	 * @throws std::invalid_argument 선언이 null인 경우
	 */
	void AddDeclaration(std::unique_ptr<ASTNode> declaration);
	
	/**
	 * @brief 선언 목록 반환
	 * @return 선언 목록
	 */
	const std::vector<std::unique_ptr<ASTNode>>& GetDeclarations() const { return _declarations; }
	
	/**
	 * @brief 선언 개수 반환
	 * @return 선언 개수
	 */
	size_t GetDeclarationCount() const { return _declarations.size(); }
	
	/**
	 * @brief 문자열 표현 반환
	 * @return 문자열 표현
	 */
	std::string ToString() const override;
	
	/**
	 * @brief 노드 복제
	 * @return 복제된 노드
	 */
	std::unique_ptr<ASTNode> Clone() const override;
	
	/**
	 * @brief 방문자 수락
	 * @param visitor 방문자
	 */
	void Accept(ASTVisitor& visitor) const override;
	
private:
	std::vector<std::unique_ptr<ASTNode>> _declarations;
};

} // namespace Translator
} // namespace DarkMatterVM 