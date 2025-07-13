#include "ReturnStatementNode.h"

namespace DarkMatterVM {
namespace Translator {

ReturnStatementNode::ReturnStatementNode(std::unique_ptr<ASTNode> expr)
    : ASTNode(NodeType::ReturnStatement)
    , _expr(std::move(expr))
{}

std::string ReturnStatementNode::ToString() const
{
    return std::string("return") + (_expr ? " " + _expr->ToString() : "");
}

std::unique_ptr<ASTNode> ReturnStatementNode::Clone() const
{
    std::unique_ptr<ASTNode> clonedExpr = _expr ? _expr->Clone() : nullptr;
    auto node = std::make_unique<ReturnStatementNode>(std::move(clonedExpr));
    node->SetLocation(GetLine(), GetColumn());
    return node;
}

void ReturnStatementNode::Accept(ASTVisitor& visitor) const
{
    visitor.Visit(this);
}

} // namespace Translator
} // namespace DarkMatterVM
