#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <cstdint>
#include "ASTVisitor.h"
#include <Opcodes.h>

namespace DarkMatterVM 
{
namespace Translator 
{

/**
 * @brief 심볼 정보 (변수, 함수 등)
 */
struct SymbolInfo 
{
	std::string name;
	std::string type;
	size_t address;
	bool isGlobal;
	
	SymbolInfo(const std::string& name, const std::string& type, size_t address, bool isGlobal = false)
		: name(name), type(type), address(address), isGlobal(isGlobal) {}

	SymbolInfo()
		: name(), type(), address(0), isGlobal(false)
	{}
};

/**
 * @brief 바이트코드 생성 방문자 클래스
 * 
 * AST를 방문하면서 VM 바이트코드를 생성하는 클래스
 */
class BytecodeGeneratorVisitor : public ASTVisitor 
{
public:
	/**
	 * @brief 생성자
	 */
	BytecodeGeneratorVisitor();
	
	/**
	 * @brief 소멸자
	 */
	~BytecodeGeneratorVisitor() = default;
	
	/**
	 * @brief 바이트코드 생성 초기화
	 */
	void Reset();
	
	/**
	 * @brief 생성된 바이트코드 반환
	 * 
	 * @return 바이트코드 바이트 배열
	 */
	const std::vector<uint8_t>& GetBytecode() const { return _bytecode; }
	
	/**
	 * @brief 바이트코드 덤프 (디버깅용)
	 * @return 바이트코드 문자열 표현
	 */
	std::string DumpBytecode() const;
	
	// 리터럴 노드 방문자 메서드
	void Visit(const IntegerLiteralNode* node) override;
	void Visit(const FloatLiteralNode* node) override;
	void Visit(const StringLiteralNode* node) override;
	void Visit(const BooleanLiteralNode* node) override;
	
	// 변수 노드 방문자 메서드
	void Visit(const VariableNode* node) override;
	void Visit(const VariableDeclNode* node) override;
	
	// 연산자 노드 방문자 메서드
	void Visit(const BinaryOpNode* node) override;
	void Visit(const UnaryOpNode* node) override;
	void Visit(const AssignmentOpNode* node) override;
	
	// 제어 구조 노드 방문자 메서드
	void Visit(const IfStatementNode* node) override;
	void Visit(const WhileLoopNode* node) override;
	void Visit(const ForLoopNode* node) override;
	void Visit(const ReturnStatementNode* node) override;
	void Visit(const BreakStatementNode* node) override;
	void Visit(const ContinueStatementNode* node) override;
	
	// 함수 노드 방문자 메서드
	void Visit(const FunctionDeclNode* node) override;
	void Visit(const FunctionCallNode* node) override;
	
	// 컨테이너 노드 방문자 메서드
	void Visit(const BlockNode* node) override;
	void Visit(const ProgramNode* node) override;

private:
	// 바이트코드 저장
	std::vector<uint8_t> _bytecode;
	
	// 심볼 테이블 (변수/함수 이름 -> 심볼 정보)
	std::unordered_map<std::string, SymbolInfo> _symbolTable;
	
	// 현재 데이터 주소 (메모리 할당용)
	size_t _currentAddress;
	
	// 1바이트를 바이트코드에 추가
	void EmitByte(uint8_t byte);
	
	// 2바이트(int16_t)를 바이트코드에 추가
	void EmitInt16(int16_t value);
	
	// 4바이트(int32_t)를 바이트코드에 추가
	void EmitInt32(int32_t value);
	
	// 8바이트(int64_t)를 바이트코드에 추가
	void EmitInt64(int64_t value);
	
	// VM 명령어(Opcode)를 바이트코드에 추가
	void EmitOpcode(DarkMatterVM::Engine::Opcode opcode);
	
	// 새 변수 등록
	void RegisterVariable(const std::string& name, const std::string& type);
	
	// 변수 주소 찾기
	size_t GetVariableAddress(const std::string& name);
};

} // namespace Translator
} // namespace DarkMatterVM 