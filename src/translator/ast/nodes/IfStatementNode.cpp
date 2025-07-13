#include "IfStatementNode.h"

namespace DarkMatterVM {
namespace Translator {

IfStatementNode::IfStatementNode(std::unique_ptr<ASTNode> condition,
                                 std::unique_ptr<BlockNode> thenBlock,
                                 std::unique_ptr<BlockNode> elseBlock)
    : ASTNode(NodeType::IfStatement)
    , _condition(std::move(condition))
    , _thenBlock(std::move(thenBlock))
    , _elseBlock(std::move(elseBlock))
{}

std::string IfStatementNode::ToString() const
{
    return std::string("if …");
}

std::unique_ptr<ASTNode> IfStatementNode::Clone() const
{
    auto cond = _condition ? _condition->Clone() : nullptr;
    auto thenBlk = _thenBlock ? std::unique_ptr<BlockNode>(static_cast<BlockNode*>(_thenBlock->Clone().release())) : nullptr;
    auto elseBlk = _elseBlock ? std::unique_ptr<BlockNode>(static_cast<BlockNode*>(_elseBlock->Clone().release())) : nullptr;
    auto node = std::make_unique<IfStatementNode>(std::move(cond), std::move(thenBlk), std::move(elseBlk));
    node->SetLocation(GetLine(), GetColumn());
    return node;
}

void IfStatementNode::Accept(ASTVisitor& visitor) const
{
    visitor.Visit(this);
}

} // namespace Translator
} // namespace DarkMatterVM
