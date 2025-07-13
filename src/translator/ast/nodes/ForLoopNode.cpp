#include "ForLoopNode.h"

namespace DarkMatterVM {
namespace Translator {

ForLoopNode::ForLoopNode(std::unique_ptr<ASTNode> initializer,
                         std::unique_ptr<ASTNode> condition,
                         std::unique_ptr<ASTNode> increment,
                         std::unique_ptr<BlockNode> body)
    : ASTNode(NodeType::ForLoop)
    , _initializer(std::move(initializer))
    , _condition(std::move(condition))
    , _increment(std::move(increment))
    , _body(std::move(body))
{
}

std::string ForLoopNode::ToString() const
{
    return std::string("for (…) …");
}

std::unique_ptr<ASTNode> ForLoopNode::Clone() const
{
    auto init = _initializer ? _initializer->Clone() : nullptr;
    auto cond = _condition ? _condition->Clone() : nullptr;
    auto inc  = _increment ? _increment->Clone() : nullptr;
    auto bodyClone = _body ? std::unique_ptr<BlockNode>(static_cast<BlockNode*>(_body->Clone().release())) : nullptr;
    auto node = std::make_unique<ForLoopNode>(std::move(init), std::move(cond), std::move(inc), std::move(bodyClone));
    node->SetLocation(GetLine(), GetColumn());
    return node;
}

void ForLoopNode::Accept(ASTVisitor& visitor) const
{
    visitor.Visit(this);
}

} // namespace Translator
} // namespace DarkMatterVM
