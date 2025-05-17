#include "ASTNodeFactory.h"

namespace DarkMatterVM 
{
namespace Translator 
{

// 리터럴 노드 생성
std::unique_ptr<IntegerLiteralNode> ASTNodeFactory::CreateIntegerLiteral(int64_t value) 
{
	return std::make_unique<IntegerLiteralNode>(value);
}

std::unique_ptr<FloatLiteralNode> ASTNodeFactory::CreateFloatLiteral(double value)
{
	return std::make_unique<FloatLiteralNode>(value);
}

std::unique_ptr<StringLiteralNode> ASTNodeFactory::CreateStringLiteral(const std::string& value)
{
	return std::make_unique<StringLiteralNode>(value);
}

std::unique_ptr<BooleanLiteralNode> ASTNodeFactory::CreateBooleanLiteral(bool value)
{
	return std::make_unique<BooleanLiteralNode>(value);
}

// 변수 노드 생성
std::unique_ptr<VariableNode> ASTNodeFactory::CreateVariable(const std::string& name) 
{
	return std::make_unique<VariableNode>(name);
}

std::unique_ptr<VariableDeclNode> ASTNodeFactory::CreateVariableDecl(
	const std::string& type, 
	const std::string& name, 
	std::unique_ptr<ASTNode> initializer
) 
{
	return std::make_unique<VariableDeclNode>(type, name, std::move(initializer));
}

// 연산자 노드 생성
std::unique_ptr<BinaryOpNode> ASTNodeFactory::CreateBinaryOp(
	BinaryOpType opType, 
	std::unique_ptr<ASTNode> left, 
	std::unique_ptr<ASTNode> right
) 
{
	return std::make_unique<BinaryOpNode>(opType, std::move(left), std::move(right));
}

std::unique_ptr<UnaryOpNode> ASTNodeFactory::CreateUnaryOp(
	UnaryOpType opType,
	std::unique_ptr<ASTNode> operand
)
{
	return std::make_unique<UnaryOpNode>(opType, std::move(operand));
}

// 컨테이너 노드 생성
std::unique_ptr<BlockNode> ASTNodeFactory::CreateBlock() 
{
	return std::make_unique<BlockNode>();
}

std::unique_ptr<ProgramNode> ASTNodeFactory::CreateProgram()
{
	return std::make_unique<ProgramNode>();
}

} // namespace Translator
} // namespace DarkMatterVM 