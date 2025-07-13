#pragma once
#include "../base/ASTNode.h"
#include "../visitor/ASTVisitor.h"

namespace DarkMatterVM {
namespace Translator {

class ContinueStatementNode : public ASTNode
{
public:
    ContinueStatementNode() : ASTNode(NodeType::ContinueStatement) {}

    std::string ToString() const override { return "continue"; }
    std::unique_ptr<ASTNode> Clone() const override
    {
        auto node = std::make_unique<ContinueStatementNode>();
        node->SetLocation(GetLine(), GetColumn());
        return node;
    }
    void Accept(ASTVisitor& visitor) const override { visitor.Visit(this); }
};

} // namespace Translator
} // namespace DarkMatterVM
