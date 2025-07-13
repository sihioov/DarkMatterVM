#include "FunctionCallNode.h"

namespace DarkMatterVM {
namespace Translator {

FunctionCallNode::FunctionCallNode(std::string callee, std::vector<std::unique_ptr<ASTNode>> arguments)
    : ASTNode(NodeType::FunctionCall)
    , _callee(std::move(callee))
    , _arguments(std::move(arguments))
{
}

std::string FunctionCallNode::ToString() const
{
    return _callee + "(…)";
}

std::unique_ptr<ASTNode> FunctionCallNode::Clone() const
{
    std::vector<std::unique_ptr<ASTNode>> clonedArgs;
    for (const auto& arg : _arguments) {
        clonedArgs.push_back(arg ? arg->Clone() : nullptr);
    }
    auto node = std::make_unique<FunctionCallNode>(_callee, std::move(clonedArgs));
    node->SetLocation(GetLine(), GetColumn());
    return node;
}

void FunctionCallNode::Accept(ASTVisitor& visitor) const
{
    visitor.Visit(this);
}

} // namespace Translator
} // namespace DarkMatterVM
