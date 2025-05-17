#include "VariableNodes.h"
#include "../visitor/ASTVisitor.h"

namespace DarkMatterVM 
{
namespace Translator 
{

// VariableNode 구현
std::string VariableNode::ToString() const 
{
	return _name;
}

std::unique_ptr<ASTNode> VariableNode::Clone() const
{
	auto node = std::make_unique<VariableNode>(_name);
	node->SetLocation(GetLine(), GetColumn());
	return node;
}

void VariableNode::Accept(ASTVisitor& visitor) const
{
	visitor.Visit(this);
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

std::unique_ptr<ASTNode> VariableDeclNode::Clone() const
{
	auto initializer = _initializer ? _initializer->Clone() : nullptr;
	
	auto node = std::make_unique<VariableDeclNode>(_type, _name, std::move(initializer));
	node->SetLocation(GetLine(), GetColumn());
	return node;
}

void VariableDeclNode::Accept(ASTVisitor& visitor) const
{
	visitor.Visit(this);
}

} // namespace Translator
} // namespace DarkMatterVM 