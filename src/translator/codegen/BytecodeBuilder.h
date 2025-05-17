#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <memory>
#include "../ast/ASTNode.h"
#include "../../../include/Opcodes.h"

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
};

/**
 * @brief 바이트코드 생성 클래스
 * 
 * AST를 순회하면서 VM 바이트코드를 생성하는 클래스
 */
class BytecodeBuilder 
{
public:
	/**
	 * @brief 생성자
	 */
	BytecodeBuilder();
	
	/**
	 * @brief 소멸자
	 */
	~BytecodeBuilder() = default;
	
	/**
	 * @brief AST에서 바이트코드 생성
	 * 
	 * @param rootNode AST 루트 노드
	 * @return 생성 성공 여부
	 */
	bool GenerateFromAST(const ASTNode* rootNode);
	
	/**
	 * @brief 생성된 바이트코드 반환
	 * 
	 * @return 바이트코드 바이트 배열
	 */
	const std::vector<uint8_t>& GetBytecode() const { return _bytecode; }
	
	/**
	 * @brief 바이트코드 초기화
	 */
	void Reset();
	
	/**
	 * @brief 심볼 테이블 현재 크기 반환 (다음 변수 주소)
	 * @return 심볼 테이블 크기
	 */
	size_t GetCurrentAddress() const { return _currentAddress; }
	
	/**
	 * @brief 바이트코드 덤프 (디버깅용)
	 * @return 바이트코드 문자열 표현
	 */
	std::string DumpBytecode() const;
	
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
	
	// 노드 타입별 처리 함수들
	void ProcessBlock(const BlockNode* node);
	void ProcessIntegerLiteral(const IntegerLiteralNode* node);
	void ProcessVariable(const VariableNode* node);
	void ProcessVariableDecl(const VariableDeclNode* node);
	void ProcessBinaryOp(const BinaryOpNode* node);
	
	// 일반 노드 처리 함수
	void ProcessNode(const ASTNode* node);
	
	// 새 변수 등록
	void RegisterVariable(const std::string& name, const std::string& type);
	
	// 변수 주소 찾기
	size_t GetVariableAddress(const std::string& name);
};

} // namespace Translator
} // namespace DarkMatterVM
