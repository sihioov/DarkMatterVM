#pragma once
#include <string>
#include "../base/ASTNode.h"
#include "../visitor/ASTVisitor.h"

namespace DarkMatterVM {
namespace Translator {

class ParameterNode : public ASTNode
{
public:
    ParameterNode(const std::string& type, const std::string& name)
        : ASTNode(NodeType::Variable) // 재사용
        , _type(type)
        , _name(name)
    {}

    const std::string& GetType() const { return _type; }
    const std::string& GetName() const { return _name; }

    std::string ToString() const override { return _type + " " + _name; }
    std::unique_ptr<ASTNode> Clone() const override
    {
        auto node = std::make_unique<ParameterNode>(_type, _name);
        node->SetLocation(GetLine(), GetColumn());
        return node;
    }
    void Accept(ASTVisitor& visitor) const override { visitor.Visit(this); }

private:
    std::string _type;
    std::string _name;
};

} // namespace Translator
} // namespace DarkMatterVM
