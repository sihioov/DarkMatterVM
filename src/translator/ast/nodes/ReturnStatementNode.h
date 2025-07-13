#pragma once
#include <memory>
#include "../base/ASTNode.h"
#include "../visitor/ASTVisitor.h"

namespace DarkMatterVM {
namespace Translator {

class ReturnStatementNode : public ASTNode
{
public:
    explicit ReturnStatementNode(std::unique_ptr<ASTNode> expr);

    const ASTNode* GetExpr() const { return _expr.get(); }

    // overrides
    std::string ToString() const override;
    std::unique_ptr<ASTNode> Clone() const override;
    void Accept(ASTVisitor& visitor) const override;

private:
    std::unique_ptr<ASTNode> _expr;
};

} // namespace Translator
} // namespace DarkMatterVM
