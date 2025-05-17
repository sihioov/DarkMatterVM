#include "ASTNode.h"

namespace DarkMatterVM 
{
namespace Translator 
{

void ASTNode::SetLocation(int line, int column) 
{
	_line = line;
	_column = column;
}

// IntegerLiteralNode 구현
std::string IntegerLiteralNode::ToString() const 
{
	return std::to_string(_value);
}

// VariableNode 구현
std::string VariableNode::ToString() const 
{
	return _name;
}

// BinaryOpNode 구현
std::string BinaryOpNode::ToString() const 
{
	return "(" + _left->ToString() + " " + _GetOpSymbol() + " " + _right->ToString() + ")";
}

std::string BinaryOpNode::_GetOpSymbol() const 
{
	switch (_opType) 
	{
		case BinaryOpType::Add: return "+";
		case BinaryOpType::Subtract: return "-";
		case BinaryOpType::Multiply: return "*";
		case BinaryOpType::Divide: return "/";
		case BinaryOpType::Modulo: return "%";
		case BinaryOpType::Equal: return "==";
		case BinaryOpType::NotEqual: return "!=";
		case BinaryOpType::Greater: return ">";
		case BinaryOpType::Less: return "<";
		case BinaryOpType::GreaterEq: return ">=";
		case BinaryOpType::LessEq: return "<=";
		case BinaryOpType::LogicalAnd: return "&&";
		case BinaryOpType::LogicalOr: return "||";
		case BinaryOpType::BitwiseAnd: return "&";
		case BinaryOpType::BitwiseOr: return "|";
		case BinaryOpType::BitwiseXor: return "^";
		case BinaryOpType::ShiftLeft: return "<<";
		case BinaryOpType::ShiftRight: return ">>";
		default: return "?";
	}
}

// VariableDeclNode 구현
std::string VariableDeclNode::ToString() const 
{
	std::string result = _type + " " + _name;
	if (_initializer) 
	{
		result += " = " + _initializer->ToString();
	}
	
	return result;
}

// BlockNode 구현
void BlockNode::AddStatement(std::unique_ptr<ASTNode> statement) 
{
	_statements.push_back(std::move(statement));
}

std::string BlockNode::ToString() const 
{
	std::string result = "{\n";
	for (const auto& stmt : _statements) 
	{
		result += "  " + stmt->ToString() + ";\n";
	}
	result += "}";

	return result;
}

// ASTNodeFactory 구현
std::unique_ptr<IntegerLiteralNode> ASTNodeFactory::CreateIntegerLiteral(int64_t value) 
{
	return std::make_unique<IntegerLiteralNode>(value);
}

std::unique_ptr<VariableNode> ASTNodeFactory::CreateVariable(const std::string& name) 
{
	return std::make_unique<VariableNode>(name);
}

std::unique_ptr<BinaryOpNode> ASTNodeFactory::CreateBinaryOp(
	BinaryOpType opType, 
	std::unique_ptr<ASTNode> left, 
	std::unique_ptr<ASTNode> right
) 
{
	return std::make_unique<BinaryOpNode>(opType, std::move(left), std::move(right));
}

std::unique_ptr<VariableDeclNode> ASTNodeFactory::CreateVariableDecl(
	const std::string& type, 
	const std::string& name, 
	std::unique_ptr<ASTNode> initializer
) 
{
	return std::make_unique<VariableDeclNode>(type, name, std::move(initializer));
}

std::unique_ptr<BlockNode> ASTNodeFactory::CreateBlock() 
{
	return std::make_unique<BlockNode>();
}

} // namespace Translator
} // namespace DarkMatterVM 