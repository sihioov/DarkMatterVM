#include "ContainerNodes.h"
#include <sstream>
#include "../visitor/ASTVisitor.h"

namespace DarkMatterVM 
{
namespace Translator 
{

// BlockNode 구현
void BlockNode::AddStatement(std::unique_ptr<ASTNode> statement) 
{
	if (!statement) {
		throw std::invalid_argument("블록에 null 문장을 추가할 수 없습니다");
	}
	_statements.push_back(std::move(statement));
}

std::string BlockNode::ToString() const 
{
	std::string result = "{\n";
	for (const auto& stmt : _statements) 
	{
		if (stmt) {
			result += "  " + stmt->ToString() + ";\n";
		} else {
			result += "  <null>;\n";
		}
	}
	result += "}";

	return result;
}

std::unique_ptr<ASTNode> BlockNode::Clone() const
{
	auto node = std::make_unique<BlockNode>();
	node->SetLocation(GetLine(), GetColumn());
	
	for (const auto& stmt : _statements)
	{
		if (stmt) {
			node->AddStatement(stmt->Clone());
		}
	}
	
	return node;
}

void BlockNode::Accept(ASTVisitor& visitor) const
{
	visitor.Visit(this);
}

// ProgramNode 구현
void ProgramNode::AddDeclaration(std::unique_ptr<ASTNode> declaration)
{
	if (!declaration) {
		throw std::invalid_argument("프로그램에 null 선언을 추가할 수 없습니다");
	}
	_declarations.push_back(std::move(declaration));
}

std::string ProgramNode::ToString() const
{
	std::stringstream ss;
	ss << "Program[\n";
	
	for (const auto& decl : _declarations)
	{
		if (decl) {
			ss << "  " << decl->ToString() << ";\n";
		} else {
			ss << "  <null>;\n";
		}
	}
	
	ss << "]";
	return ss.str();
}

std::unique_ptr<ASTNode> ProgramNode::Clone() const
{
	auto node = std::make_unique<ProgramNode>();
	node->SetLocation(GetLine(), GetColumn());
	
	for (const auto& decl : _declarations)
	{
		if (decl) {
			node->AddDeclaration(decl->Clone());
		}
	}
	
	return node;
}

void ProgramNode::Accept(ASTVisitor& visitor) const
{
	visitor.Visit(this);
}

} // namespace Translator
} // namespace DarkMatterVM 