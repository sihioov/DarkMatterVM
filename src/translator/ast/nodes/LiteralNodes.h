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
 * @brief 정수 리터럴 노드
 */
class IntegerLiteralNode : public ASTNode 
{
public:
	/**
	 * @brief 생성자
	 * @param value 정수 값
	 */
	explicit IntegerLiteralNode(int64_t value) 
		: ASTNode(NodeType::IntegerLiteral), _value(value) {}
	
	/**
	 * @brief 값 반환
	 * @return 정수 값
	 */
	int64_t GetValue() const { return _value; }
	
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
	int64_t _value;
};

/**
 * @brief 부동소수점 리터럴 노드
 */
class FloatLiteralNode : public ASTNode 
{
public:
	/**
	 * @brief 생성자
	 * @param value 부동소수점 값
	 */
	explicit FloatLiteralNode(double value) 
		: ASTNode(NodeType::FloatLiteral), _value(value) {}
	
	/**
	 * @brief 값 반환
	 * @return 부동소수점 값
	 */
	double GetValue() const { return _value; }
	
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
	double _value;
};

/**
 * @brief 문자열 리터럴 노드
 */
class StringLiteralNode : public ASTNode 
{
public:
	/**
	 * @brief 생성자
	 * @param value 문자열 값
	 * @throws std::invalid_argument 문자열이 비어있을 경우
	 */
	explicit StringLiteralNode(const std::string& value) 
		: ASTNode(NodeType::StringLiteral), _value(value) 
	{
		if (value.empty()) {
			throw std::invalid_argument("문자열 리터럴은 비어있을 수 없습니다");
		}
	}
	
	/**
	 * @brief 값 반환
	 * @return 문자열 값
	 */
	const std::string& GetValue() const { return _value; }
	
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
	std::string _value;
};

/**
 * @brief 불리언 리터럴 노드
 */
class BooleanLiteralNode : public ASTNode 
{
public:
	/**
	 * @brief 생성자
	 * @param value 불리언 값
	 */
	explicit BooleanLiteralNode(bool value) 
		: ASTNode(NodeType::BooleanLiteral), _value(value) {}
	
	/**
	 * @brief 값 반환
	 * @return 불리언 값
	 */
	bool GetValue() const { return _value; }
	
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
	bool _value;
};

} // namespace Translator
} // namespace DarkMatterVM 