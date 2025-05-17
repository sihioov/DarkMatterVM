#pragma once

#include <memory>
#include <string>
#include "base/OperatorTypes.h"
#include "nodes/LiteralNodes.h"
#include "nodes/VariableNodes.h"
#include "nodes/OperatorNodes.h"
#include "nodes/ContainerNodes.h"

namespace DarkMatterVM 
{
namespace Translator 
{

/**
 * @brief AST 노드 팩토리
 * 
 * 새 노드 생성 헬퍼 함수들
 */
class ASTNodeFactory 
{
public:
	// 리터럴 노드 생성
	static std::unique_ptr<IntegerLiteralNode> CreateIntegerLiteral(int64_t value);
	static std::unique_ptr<FloatLiteralNode> CreateFloatLiteral(double value);
	static std::unique_ptr<StringLiteralNode> CreateStringLiteral(const std::string& value);
	static std::unique_ptr<BooleanLiteralNode> CreateBooleanLiteral(bool value);
	
	// 변수 노드 생성
	static std::unique_ptr<VariableNode> CreateVariable(const std::string& name);
	static std::unique_ptr<VariableDeclNode> CreateVariableDecl(
		const std::string& type, 
		const std::string& name, 
		std::unique_ptr<ASTNode> initializer = nullptr
	);
	
	// 연산자 노드 생성
	static std::unique_ptr<BinaryOpNode> CreateBinaryOp(
		BinaryOpType opType, 
		std::unique_ptr<ASTNode> left, 
		std::unique_ptr<ASTNode> right
	);
	
	static std::unique_ptr<UnaryOpNode> CreateUnaryOp(
		UnaryOpType opType,
		std::unique_ptr<ASTNode> operand
	);
	
	// 컨테이너 노드 생성
	static std::unique_ptr<BlockNode> CreateBlock();
	static std::unique_ptr<ProgramNode> CreateProgram();
};

} // namespace Translator
} // namespace DarkMatterVM 