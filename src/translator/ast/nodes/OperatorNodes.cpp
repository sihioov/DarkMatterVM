#include "OperatorNodes.h"
#include "../visitor/ASTVisitor.h"

namespace DarkMatterVM 
{
namespace Translator 
{

// BinaryOpNode 구현
std::string BinaryOpNode::ToString() const 
{
	if (!_left || !_right) {
		return "BinaryOp(" + _GetOpSymbol() + ", <null>)";
	}
	return "(" + _left->ToString() + " " + _GetOpSymbol() + " " + _right->ToString() + ")";
}

std::unique_ptr<ASTNode> BinaryOpNode::Clone() const
{
	auto left = _left ? _left->Clone() : nullptr;
	auto right = _right ? _right->Clone() : nullptr;
	
	auto node = std::make_unique<BinaryOpNode>(_opType, std::move(left), std::move(right));
	node->SetLocation(GetLine(), GetColumn());
	return node;
}

void BinaryOpNode::Accept(ASTVisitor& visitor) const
{
	visitor.Visit(this);
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

// UnaryOpNode 구현
std::string UnaryOpNode::ToString() const
{
	if (!_operand) {
		return "UnaryOp(" + _GetOpSymbol() + ", <null>)";
	}
	
	if (_IsPostfix()) {
		return "(" + _operand->ToString() + _GetOpSymbol() + ")";
	} else {
		return "(" + _GetOpSymbol() + _operand->ToString() + ")";
	}
}

std::unique_ptr<ASTNode> UnaryOpNode::Clone() const
{
	auto operand = _operand ? _operand->Clone() : nullptr;
	
	auto node = std::make_unique<UnaryOpNode>(_opType, std::move(operand));
	node->SetLocation(GetLine(), GetColumn());
	return node;
}

void UnaryOpNode::Accept(ASTVisitor& visitor) const
{
	visitor.Visit(this);
}

std::string UnaryOpNode::_GetOpSymbol() const
{
	switch (_opType)
	{
		case UnaryOpType::Negate: return "-";
		case UnaryOpType::LogicalNot: return "!";
		case UnaryOpType::BitwiseNot: return "~";
		case UnaryOpType::PreIncrement: return "++";
		case UnaryOpType::PostIncrement: return "++";
		case UnaryOpType::PreDecrement: return "--";
		case UnaryOpType::PostDecrement: return "--";
		default: return "?";
	}
}

bool UnaryOpNode::_IsPostfix() const
{
	return _opType == UnaryOpType::PostIncrement || _opType == UnaryOpType::PostDecrement;
}

} // namespace Translator
} // namespace DarkMatterVM 