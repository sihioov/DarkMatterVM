#pragma once
#include <memory>
#include "../base/ASTNode.h"
#include "../visitor/ASTVisitor.h"
#include "ContainerNodes.h"

namespace DarkMatterVM {
namespace Translator {

class ForLoopNode : public ASTNode
{
public:
    ForLoopNode(std::unique_ptr<ASTNode> initializer,
                std::unique_ptr<ASTNode> condition,
                std::unique_ptr<ASTNode> increment,
                std::unique_ptr<BlockNode> body);

    const ASTNode* GetInitializer() const { return _initializer.get(); }
    const ASTNode* GetCondition() const { return _condition.get(); }
    const ASTNode* GetIncrement() const { return _increment.get(); }
    const BlockNode* GetBody() const { return _body.get(); }

    std::string ToString() const override;
    std::unique_ptr<ASTNode> Clone() const override;
    void Accept(ASTVisitor& visitor) const override;

private:
    std::unique_ptr<ASTNode> _initializer;
    std::unique_ptr<ASTNode> _condition;
    std::unique_ptr<ASTNode> _increment;
    std::unique_ptr<BlockNode> _body;
};

} // namespace Translator
} // namespace DarkMatterVM
