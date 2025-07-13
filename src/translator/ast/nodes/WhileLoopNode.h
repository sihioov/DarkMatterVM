#pragma once
#include <memory>
#include "../base/ASTNode.h"
#include "../visitor/ASTVisitor.h"
#include "ContainerNodes.h"

namespace DarkMatterVM {
namespace Translator {

class WhileLoopNode : public ASTNode
{
public:
    WhileLoopNode(std::unique_ptr<ASTNode> condition,
                  std::unique_ptr<BlockNode> body);

    const ASTNode* GetCondition() const { return _condition.get(); }
    const BlockNode* GetBody() const { return _body.get(); }

    std::string ToString() const override;
    std::unique_ptr<ASTNode> Clone() const override;
    void Accept(ASTVisitor& visitor) const override;

private:
    std::unique_ptr<ASTNode> _condition;
    std::unique_ptr<BlockNode> _body;
};

} // namespace Translator
} // namespace DarkMatterVM
