#include "WhileLoopNode.h"

namespace DarkMatterVM {
namespace Translator {

WhileLoopNode::WhileLoopNode(std::unique_ptr<ASTNode> condition,
                             std::unique_ptr<BlockNode> body)
    : ASTNode(NodeType::WhileLoop)
    , _condition(std::move(condition))
    , _body(std::move(body))
{
}

std::string WhileLoopNode::ToString() const
{
    return std::string("while …");
}

std::unique_ptr<ASTNode> WhileLoopNode::Clone() const
{
    auto cond = _condition ? _condition->Clone() : nullptr;
    auto bodyClone = _body ? std::unique_ptr<BlockNode>(static_cast<BlockNode*>(_body->Clone().release())) : nullptr;
    auto node = std::make_unique<WhileLoopNode>(std::move(cond), std::move(bodyClone));
    node->SetLocation(GetLine(), GetColumn());
    return node;
}

void WhileLoopNode::Accept(ASTVisitor& visitor) const
{
    visitor.Visit(this);
}

} // namespace Translator
} // namespace DarkMatterVM
