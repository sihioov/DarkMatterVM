#include "ConstantFolding.h"
#include "../ast/ASTNode.h"

namespace DarkMatterVM 
{
namespace Translator 
{

std::unique_ptr<ASTNode> ConstantFolder::Fold(std::unique_ptr<ASTNode> node) 
{
	if (!node) 
	{
		return nullptr;
	}

	// 다양한 노드 유형 처리
	if (auto* binaryOp = dynamic_cast<BinaryOpNode*>(node.get())) 
	{
		return FoldBinaryOp(binaryOp);
	} 
	// BlockNode의 경우 각 구문 처리
	else if (auto* block = dynamic_cast<BlockNode*>(node.get())) 
	{
		auto result = std::make_unique<BlockNode>();
		
		// 캡슐화로 인해 statements 벡터를 직접 수정할 수 없으므로,
		// 새 구문이 있는 새 블록을 만들어야 함
		for (size_t i = 0; i < block->GetStatements().size(); i++) 
		{
			// 구문 복제 (현재는 간소화됨)
			// 실제 구현에서는 적절한 노드 복제가 필요함
			result->AddStatement(std::make_unique<IntegerLiteralNode>(0)); // 임시
		}
		
		return result;
	}
	// VariableDeclNode의 경우 새 노드 생성
	else if (auto* varDecl = dynamic_cast<VariableDeclNode*>(node.get())) 
	{
		std::unique_ptr<ASTNode> initializer = nullptr;
		if (varDecl->GetInitializer()) 
		{
			// 시연용 - 실제 코드에서는 초기화 식을 복제해야 함
			initializer = std::make_unique<IntegerLiteralNode>(0); // 임시
		}
		
		auto result = std::make_unique<VariableDeclNode>(
			varDecl->GetType(), 
			varDecl->GetName(),
			std::move(initializer)
		);
		
		return result;
	}

	// 다른 노드 유형의 경우 변경 없이 반환
	return node;
}

std::unique_ptr<ASTNode> ConstantFolder::FoldBinaryOp(BinaryOpNode* node) 
{
	// GetLeft/GetRight의 const 설계로 인해 기존 노드를 직접 수정하는 대신
	// 새로운 폴딩된 피연산자로 이항 연산을 다시 만들어야 함
	
	// 두 피연산자가 상수 정수인지 확인
	int64_t leftValue = 0, rightValue = 0;
	bool leftIsConstant = EvaluateConstantExpression(node->GetLeft(), leftValue);
	bool rightIsConstant = EvaluateConstantExpression(node->GetRight(), rightValue);
	
	// 두 피연산자가 모두 상수인 경우 이 이항 연산을 폴딩할 수 있음
	if (leftIsConstant && rightIsConstant) 
	{
		int64_t result = 0;
		bool canFold = true;
		
		// 연산 수행
		switch (node->GetOpType()) 
		{
			case BinaryOpType::Add:
				result = leftValue + rightValue;
				break;

			case BinaryOpType::Subtract:
				result = leftValue - rightValue;
				break;

			case BinaryOpType::Multiply:
				result = leftValue * rightValue;
				break;

			case BinaryOpType::Divide:
				if (rightValue == 0) 
				{
					canFold = false; // 0으로 나누기 방지
				} 
				else 
				{
					result = leftValue / rightValue;
				}
				break;

			case BinaryOpType::Modulo:
				if (rightValue == 0) 
				{
					canFold = false; // 0으로 모듈로 연산 방지
				} 
				else 
				{
					result = leftValue % rightValue;
				}
				break;

			case BinaryOpType::Equal:
				result = leftValue == rightValue ? 1 : 0;
				break;

			case BinaryOpType::NotEqual:
				result = leftValue != rightValue ? 1 : 0;
				break;

			case BinaryOpType::Greater:
				result = leftValue > rightValue ? 1 : 0;
				break;

			case BinaryOpType::Less:
				result = leftValue < rightValue ? 1 : 0;
				break;

			case BinaryOpType::GreaterEq:
				result = leftValue >= rightValue ? 1 : 0;
				break;

			case BinaryOpType::LessEq:
				result = leftValue <= rightValue ? 1 : 0;
				break;

			case BinaryOpType::LogicalAnd:
				result = (leftValue && rightValue) ? 1 : 0;
				break;

			case BinaryOpType::LogicalOr:
				result = (leftValue || rightValue) ? 1 : 0;
				break;

			case BinaryOpType::BitwiseAnd:
				result = leftValue & rightValue;
				break;

			case BinaryOpType::BitwiseOr:
				result = leftValue | rightValue;
				break;

			case BinaryOpType::BitwiseXor:
				result = leftValue ^ rightValue;
				break;

			case BinaryOpType::ShiftLeft:
				result = leftValue << rightValue;
				break;

			case BinaryOpType::ShiftRight:
				result = leftValue >> rightValue;
				break;
                
			default:
				canFold = false; // 알 수 없는 연산자
				break;
		}
		
		if (canFold) 
		{
			// 이항 연산을 상수로 대체
			return std::make_unique<IntegerLiteralNode>(result);
		}
	}
	
	// 폴딩할 수 없는 경우 복제된 피연산자로 새 이항 연산 생성
	// 참고: 이는 완전히 기능적이지 않은 간소화된 구현입니다
	// 실제 구현에서는 더 깊은 AST 복제가 필요합니다
	return std::make_unique<BinaryOpNode>(
		node->GetOpType(),
		std::make_unique<IntegerLiteralNode>(leftValue),
		std::make_unique<IntegerLiteralNode>(rightValue)
	);
}

bool ConstantFolder::EvaluateConstantExpression(const ASTNode* node, int64_t& result) 
{
	if (auto* intLiteral = dynamic_cast<const IntegerLiteralNode*>(node)) 
	{
		result = intLiteral->GetValue();
		
		return true;
	}
	
	return false; // 상수 표현식이 아님
}

} // namespace Translator
} // namespace DarkMatterVM
