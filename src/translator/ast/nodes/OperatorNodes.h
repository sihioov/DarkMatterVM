#pragma once

#include <stdexcept>
#include "../base/ASTNode.h"
#include "../base/OperatorTypes.h"
#include "../visitor/ASTVisitor.h"

namespace DarkMatterVM 
{
namespace Translator 
{

/**
 * @brief 이항 연산자 노드
 */
class BinaryOpNode : public ASTNode 
{
public:
	/**
	 * @brief 생성자
	 * @param opType 연산자 타입
	 * @param left 좌측 피연산자
	 * @param right 우측 피연산자
	 * @throws std::invalid_argument 피연산자가 null인 경우
	 */
	BinaryOpNode(BinaryOpType opType, std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right)
		: ASTNode(NodeType::BinaryOp), _opType(opType)
	{
		if (!left || !right) {
			throw std::invalid_argument("이항 연산자의 피연산자는 null일 수 없습니다");
		}
		_left = std::move(left);
		_right = std::move(right);
	}
	
	/**
	 * @brief 연산자 타입 반환
	 * @return 연산자 타입
	 */
	BinaryOpType GetOpType() const { return _opType; }
	
	/**
	 * @brief 좌측 피연산자 반환
	 * @return 좌측 피연산자
	 */
	const ASTNode* GetLeft() const { return _left.get(); }
	
	/**
	 * @brief 우측 피연산자 반환
	 * @return 우측 피연산자
	 */
	const ASTNode* GetRight() const { return _right.get(); }
	
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
	/**
	 * @brief 연산자 심볼 반환
	 * @return 연산자 심볼
	 */
	std::string _GetOpSymbol() const;
	
	BinaryOpType _opType;
	std::unique_ptr<ASTNode> _left;
	std::unique_ptr<ASTNode> _right;
};

/**
 * @brief 단항 연산자 노드
 */
class UnaryOpNode : public ASTNode 
{
public:
	/**
	 * @brief 생성자
	 * @param opType 연산자 타입
	 * @param operand 피연산자
	 * @throws std::invalid_argument 피연산자가 null인 경우
	 */
	UnaryOpNode(UnaryOpType opType, std::unique_ptr<ASTNode> operand)
		: ASTNode(NodeType::UnaryOp), _opType(opType)
	{
		if (!operand) {
			throw std::invalid_argument("단항 연산자의 피연산자는 null일 수 없습니다");
		}
		_operand = std::move(operand);
	}
	
	/**
	 * @brief 연산자 타입 반환
	 * @return 연산자 타입
	 */
	UnaryOpType GetOpType() const { return _opType; }
	
	/**
	 * @brief 피연산자 반환
	 * @return 피연산자
	 */
	const ASTNode* GetOperand() const { return _operand.get(); }
	
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
	/**
	 * @brief 연산자 심볼 반환
	 * @return 연산자 심볼
	 */
	std::string _GetOpSymbol() const;
	
	/**
	 * @brief 후위 연산자 여부 확인
	 * @return 후위 연산자 여부
	 */
	bool _IsPostfix() const;
	
	UnaryOpType _opType;
	std::unique_ptr<ASTNode> _operand;
};

} // namespace Translator
} // namespace DarkMatterVM 