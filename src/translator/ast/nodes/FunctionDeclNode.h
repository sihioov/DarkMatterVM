#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../base/ASTNode.h"
#include "../visitor/ASTVisitor.h"
#include "ContainerNodes.h" // BlockNode

namespace DarkMatterVM {
namespace Translator {

/**
 * @brief 함수 선언/정의 노드
 */
class FunctionDeclNode : public ASTNode
{
public:
    FunctionDeclNode(const std::string& returnType,
                     const std::string& name,
                     std::vector<std::pair<std::string, std::string>> parameters, // {type,name}
                     std::unique_ptr<BlockNode> body);

    // 접근자
    const std::string& GetName() const { return _name; }
    const std::string& GetReturnType() const { return _returnType; }
    const std::vector<std::pair<std::string,std::string>>& GetParameters() const { return _parameters; }
    const BlockNode* GetBody() const { return _body.get(); }

    // ASTNode 인터페이스 구현
    std::string ToString() const override;
    std::unique_ptr<ASTNode> Clone() const override;
    void Accept(ASTVisitor& visitor) const override;

private:
    std::string _returnType;
    std::string _name;
    std::vector<std::pair<std::string,std::string>> _parameters;
    std::unique_ptr<BlockNode> _body;
};

} // namespace Translator
} // namespace DarkMatterVM
