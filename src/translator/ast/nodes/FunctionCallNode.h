#pragma once
#include <string>
#include <vector>
#include <memory>
#include "../base/ASTNode.h"
#include "../visitor/ASTVisitor.h"

namespace DarkMatterVM {
namespace Translator {

class FunctionCallNode : public ASTNode
{
public:
    FunctionCallNode(std::string callee, std::vector<std::unique_ptr<ASTNode>> arguments);

    const std::string& GetCallee() const { return _callee; }
    const std::vector<std::unique_ptr<ASTNode>>& GetArguments() const { return _arguments; }

    // ASTNode overrides
    std::string ToString() const override;
    std::unique_ptr<ASTNode> Clone() const override;
    void Accept(ASTVisitor& visitor) const override;

private:
    std::string _callee;
    std::vector<std::unique_ptr<ASTNode>> _arguments;
};

} // namespace Translator
} // namespace DarkMatterVM
