#include "FunctionDeclNode.h"

namespace DarkMatterVM {
namespace Translator {

FunctionDeclNode::FunctionDeclNode(const std::string& returnType,
                                   const std::string& name,
                                   std::vector<std::pair<std::string,std::string>> parameters,
                                   std::unique_ptr<BlockNode> body)
    : ASTNode(NodeType::FunctionDecl)
    , _returnType(returnType)
    , _name(name)
    , _parameters(std::move(parameters))
    , _body(std::move(body))
{
}

std::string FunctionDeclNode::ToString() const
{
    return "func " + _returnType + " " + _name + "()";
}

std::unique_ptr<ASTNode> FunctionDeclNode::Clone() const
{
    // 깊은 복사 구현 (파라미터 벡터는 값 복사, body는 Clone())
    std::vector<std::pair<std::string,std::string>> params = _parameters;
    std::unique_ptr<BlockNode> newBody;
    if (_body)
        newBody = std::unique_ptr<BlockNode>(static_cast<BlockNode*>(_body->Clone().release()));
    auto node = std::make_unique<FunctionDeclNode>(_returnType, _name, std::move(params), std::move(newBody));
    node->SetLocation(GetLine(), GetColumn());
    return node;
}

void FunctionDeclNode::Accept(ASTVisitor& visitor) const
{
    visitor.Visit(this);
}

} // namespace Translator
} // namespace DarkMatterVM
