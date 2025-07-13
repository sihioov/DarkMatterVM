#pragma once
#include "../base/ASTNode.h"
#include "../visitor/ASTVisitor.h"

namespace DarkMatterVM {
namespace Translator {

class BreakStatementNode : public ASTNode
{
public:
    BreakStatementNode() : ASTNode(NodeType::BreakStatement) {}

    std::string ToString() const override { return "break"; }
    std::unique_ptr<ASTNode> Clone() const override
    {
        auto node = std::make_unique<BreakStatementNode>();
        node->SetLocation(GetLine(), GetColumn());
        return node;
    }
    void Accept(ASTVisitor& visitor) const override { visitor.Visit(this); }
};

} // namespace Translator
} // namespace DarkMatterVM
