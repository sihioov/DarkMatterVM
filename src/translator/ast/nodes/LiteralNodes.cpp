#include "LiteralNodes.h"
#include "../visitor/ASTVisitor.h"

namespace DarkMatterVM 
{
namespace Translator 
{

// IntegerLiteralNode 구현
std::string IntegerLiteralNode::ToString() const 
{
	return std::to_string(_value);
}

std::unique_ptr<ASTNode> IntegerLiteralNode::Clone() const
{
	auto node = std::make_unique<IntegerLiteralNode>(_value);
	node->SetLocation(GetLine(), GetColumn());
	return node;
}

void IntegerLiteralNode::Accept(ASTVisitor& visitor) const
{
	visitor.Visit(this);
}

// FloatLiteralNode 구현
std::string FloatLiteralNode::ToString() const
{
	return std::to_string(_value);
}

std::unique_ptr<ASTNode> FloatLiteralNode::Clone() const
{
	auto node = std::make_unique<FloatLiteralNode>(_value);
	node->SetLocation(GetLine(), GetColumn());
	return node;
}

void FloatLiteralNode::Accept(ASTVisitor& visitor) const
{
	visitor.Visit(this);
}

// StringLiteralNode 구현
std::string StringLiteralNode::ToString() const
{
	return "\"" + _value + "\"";
}

std::unique_ptr<ASTNode> StringLiteralNode::Clone() const
{
	auto node = std::make_unique<StringLiteralNode>(_value);
	node->SetLocation(GetLine(), GetColumn());
	return node;
}

void StringLiteralNode::Accept(ASTVisitor& visitor) const
{
	visitor.Visit(this);
}

// BooleanLiteralNode 구현
std::string BooleanLiteralNode::ToString() const
{
	return _value ? "true" : "false";
}

std::unique_ptr<ASTNode> BooleanLiteralNode::Clone() const
{
	auto node = std::make_unique<BooleanLiteralNode>(_value);
	node->SetLocation(GetLine(), GetColumn());
	return node;
}

void BooleanLiteralNode::Accept(ASTVisitor& visitor) const
{
	visitor.Visit(this);
}

} // namespace Translator
} // namespace DarkMatterVM 