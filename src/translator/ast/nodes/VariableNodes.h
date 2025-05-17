#pragma once

#include <string>
#include <stdexcept>
#include "../base/ASTNode.h"
#include "../visitor/ASTVisitor.h"

namespace DarkMatterVM 
{
namespace Translator 
{

/**
 * @brief 변수 참조 노드
 */
class VariableNode : public ASTNode 
{
public:
	/**
	 * @brief 생성자
	 * @param name 변수 이름
	 * @throws std::invalid_argument 변수 이름이 비어있을 경우
	 */
	explicit VariableNode(const std::string& name) 
		: ASTNode(NodeType::Variable), _name(name) 
	{
		if (name.empty()) {
			throw std::invalid_argument("변수 이름은 비어있을 수 없습니다");
		}
	}
	
	/**
	 * @brief 이름 반환
	 * @return 변수 이름
	 */
	const std::string& GetName() const { return _name; }
	
	/**
	 * @brief 문자열 표현 반환
	 * @return 문자열 표현
	 */
	std::string ToString() const override;
	
	/**
	 * @brief 노드 복제
	 * @return 복제된 노드
	 */
	std::unique_ptr<ASTNode> Clone() const override;
	
	/**
	 * @brief 방문자 수락
	 * @param visitor 방문자
	 */
	void Accept(ASTVisitor& visitor) const override;
	
private:
	std::string _name;
};

/**
 * @brief 변수 선언 노드
 */
class VariableDeclNode : public ASTNode 
{
public:
	/**
	 * @brief 생성자
	 * @param type 변수 타입
	 * @param name 변수 이름
	 * @param initializer 초기화 식 (선택적)
	 * @throws std::invalid_argument 타입이나 이름이 비어있을 경우
	 */
	VariableDeclNode(const std::string& type, const std::string& name, std::unique_ptr<ASTNode> initializer = nullptr) 
		: ASTNode(NodeType::VariableDecl), _type(type), _name(name), _initializer(std::move(initializer)) 
	{
		if (type.empty()) {
			throw std::invalid_argument("변수 타입은 비어있을 수 없습니다");
		}
		if (name.empty()) {
			throw std::invalid_argument("변수 이름은 비어있을 수 없습니다");
		}
	}
	
	/**
	 * @brief 타입 반환
	 * @return 변수 타입
	 */
	const std::string& GetType() const { return _type; }
	
	/**
	 * @brief 이름 반환
	 * @return 변수 이름
	 */
	const std::string& GetName() const { return _name; }
	
	/**
	 * @brief 초기화 식 반환
	 * @return 초기화 식
	 */
	const ASTNode* GetInitializer() const { return _initializer.get(); }
	
	/**
	 * @brief 초기화 식 존재 여부 확인
	 * @return 초기화 식 존재 여부
	 */
	bool HasInitializer() const { return _initializer != nullptr; }
	
	/**
	 * @brief 문자열 표현 반환
	 * @return 문자열 표현
	 */
	std::string ToString() const override;
	
	/**
	 * @brief 노드 복제
	 * @return 복제된 노드
	 */
	std::unique_ptr<ASTNode> Clone() const override;
	
	/**
	 * @brief 방문자 수락
	 * @param visitor 방문자
	 */
	void Accept(ASTVisitor& visitor) const override;
	
private:
	std::string _type;
	std::string _name;
	std::unique_ptr<ASTNode> _initializer;
};

} // namespace Translator
} // namespace DarkMatterVM 