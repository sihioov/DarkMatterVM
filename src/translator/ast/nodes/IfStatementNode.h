#pragma once
#include <memory>
#include "../base/ASTNode.h"
#include "../visitor/ASTVisitor.h"
#include "ContainerNodes.h" // BlockNode

namespace DarkMatterVM {
namespace Translator {

class IfStatementNode : public ASTNode
{
public:
    IfStatementNode(std::unique_ptr<ASTNode> condition,
                    std::unique_ptr<BlockNode> thenBlock,
                    std::unique_ptr<BlockNode> elseBlock);

    const ASTNode* GetCondition() const { return _condition.get(); }
    const BlockNode* GetThenBlock() const { return _thenBlock.get(); }
    const BlockNode* GetElseBlock() const { return _elseBlock.get(); }

    std::string ToString() const override;
    std::unique_ptr<ASTNode> Clone() const override;
    void Accept(ASTVisitor& visitor) const override;

private:
    std::unique_ptr<ASTNode> _condition;
    std::unique_ptr<BlockNode> _thenBlock;
    std::unique_ptr<BlockNode> _elseBlock;
};

} // namespace Translator
} // namespace DarkMatterVM
