#pragma once

#include "../ast/ASTNode.h"
#include <memory>

namespace DarkMatterVM 
{
namespace Translator 
{

/**
 * @brief 상수 폴딩 최적화
 * 
 * 이 클래스는 AST에 대한 상수 폴딩 최적화를 구현합니다.
 * 상수 폴딩은 컴파일 타임에 평가할 수 있는 표현식을 결과값으로 
 * 대체하여 런타임 계산을 줄입니다.
 * 
 * 참고: 이것은 개념을 보여주기 위한 간소화된 구현이며
 * 현재 ASTNode 클래스 설계로 인한 제한사항이 있습니다:
 * - AST 노드의 적절한 깊은 복제 능력 제한
 * - 캡슐화로 인해 노드의 private 멤버를 수정할 수 없음
 * - IntegerLiteralNode만 상수로 처리함
 */
class ConstantFolder 
{
public:
	/**
	 * @brief AST 노드에 상수 폴딩 최적화 적용
	 * 
	 * @param node 최적화할 AST 노드
	 * @return 새로운 최적화된 AST 노드
	 */
	std::unique_ptr<ASTNode> Fold(std::unique_ptr<ASTNode> node);

private:
	/**
	 * @brief 두 피연산자가 상수인 경우 이항 연산을 폴딩
	 * 
	 * @param node 이항 연산 노드
	 * @return 새 노드 (상수 또는 복제된 피연산자가 있는 이항 연산)
	 */
	std::unique_ptr<ASTNode> FoldBinaryOp(BinaryOpNode* node);
	
	/**
	 * @brief 노드가 상수인지 확인하고 평가
	 * 
	 * @param node 평가할 노드
	 * @param result 상수 값을 저장할 출력 매개변수
	 * @return 노드가 상수이면 true, 그렇지 않으면 false
	 */
	bool EvaluateConstantExpression(const ASTNode* node, int64_t& result);
};

} // namespace Translator
} // namespace DarkMatterVM
