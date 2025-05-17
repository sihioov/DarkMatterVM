#include "BytecodeGeneratorVisitor.h"
#include "../nodes/LiteralNodes.h"
#include "../nodes/VariableNodes.h"
#include "../nodes/OperatorNodes.h"
#include "../nodes/ContainerNodes.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstring>

namespace DarkMatterVM 
{
namespace Translator 
{

BytecodeGeneratorVisitor::BytecodeGeneratorVisitor() 
	: _currentAddress(0) 
{
	Reset();
}

void BytecodeGeneratorVisitor::Reset() 
{
	_bytecode.clear();
	_symbolTable.clear();
	_currentAddress = 0;
}

std::string BytecodeGeneratorVisitor::DumpBytecode() const 
{
	std::stringstream ss;
	
	ss << "BytecodeGeneratorVisitor 덤프 - " << _bytecode.size() << " 바이트\n";
	ss << "=================================\n";
	
	for (size_t i = 0; i < _bytecode.size(); ++i) 
	{
		ss << std::setw(4) << std::setfill('0') << std::hex << i << ": ";
		ss << std::setw(2) << std::setfill('0') << std::hex 
		   << static_cast<int>(_bytecode[i]) << " ";
		
		// 명령어 인식 (첫 바이트가 명령어일 경우)
		if (i == 0 || _bytecode[i-1] == static_cast<uint8_t>(Engine::Opcode::HALT)) 
		{
			uint8_t opcodeByte = _bytecode[i];
			if (opcodeByte >= 0x01 && opcodeByte <= 0xFF) 
			{
				Engine::Opcode opcode = static_cast<Engine::Opcode>(opcodeByte);
				ss << " ; " << Engine::GetOpcodeInfo(opcode).mnemonic;
			}
		}
		
		ss << "\n";
	}
	
	ss << "\n심볼 테이블:\n";
	ss << "=================================\n";
	for (const auto& symbol : _symbolTable) 
	{
		ss << symbol.first << " (" << symbol.second.type << "): "
		   << "주소 0x" << std::hex << symbol.second.address
		   << (symbol.second.isGlobal ? " (전역)" : " (지역)") << "\n";
	}
	
	return ss.str();
}

// 내부 헬퍼 메서드들
void BytecodeGeneratorVisitor::EmitByte(uint8_t byte) 
{
	_bytecode.push_back(byte);
}

void BytecodeGeneratorVisitor::EmitInt16(int16_t value) 
{
	EmitByte(static_cast<uint8_t>(value & 0xFF));
	EmitByte(static_cast<uint8_t>((value >> 8) & 0xFF));
}

void BytecodeGeneratorVisitor::EmitInt32(int32_t value) 
{
	EmitByte(static_cast<uint8_t>(value & 0xFF));
	EmitByte(static_cast<uint8_t>((value >> 8) & 0xFF));
	EmitByte(static_cast<uint8_t>((value >> 16) & 0xFF));
	EmitByte(static_cast<uint8_t>((value >> 24) & 0xFF));
}

void BytecodeGeneratorVisitor::EmitInt64(int64_t value) 
{
	for (int i = 0; i < 8; ++i) 
	{
		EmitByte(static_cast<uint8_t>((value >> (i * 8)) & 0xFF));
	}
}

void BytecodeGeneratorVisitor::EmitOpcode(DarkMatterVM::Engine::Opcode opcode) 
{
	EmitByte(static_cast<uint8_t>(opcode));
}

void BytecodeGeneratorVisitor::RegisterVariable(const std::string& name, const std::string& type) 
{
	// 이미 존재하는 변수인지 확인
	if (_symbolTable.find(name) != _symbolTable.end()) 
	{
		throw std::runtime_error("변수 '" + name + "'가 이미 정의되어 있습니다.");
	}
	
	// 새 변수 등록
	_symbolTable.emplace(name, SymbolInfo(name, type, _currentAddress));
	
	// 다음 변수를 위한 주소 업데이트 (8바이트 정렬)
	_currentAddress += 8;
}

size_t BytecodeGeneratorVisitor::GetVariableAddress(const std::string& name) 
{
	auto it = _symbolTable.find(name);
	if (it == _symbolTable.end()) 
	{
		throw std::runtime_error("정의되지 않은 변수: " + name);
	}
	
	return it->second.address;
}

// 방문자 메서드 구현
void BytecodeGeneratorVisitor::Visit(const BlockNode* node) 
{
	// 블록 내의 모든 문장을 순차적으로 처리
	for (const auto& stmt : node->GetStatements()) 
	{
		stmt->Accept(*this);
	}
}

void BytecodeGeneratorVisitor::Visit(const ProgramNode* node) 
{
	// 프로그램의 모든 선언 및 문장을 처리
	for (const auto& decl : node->GetDeclarations()) 
	{
		decl->Accept(*this);
	}
	
	// 프로그램 종료 명령어 추가
	EmitOpcode(Engine::Opcode::HALT);
}

void BytecodeGeneratorVisitor::Visit(const IntegerLiteralNode* node) 
{
	int64_t value = node->GetValue();
	
	// 값의 크기에 따라 적절한 PUSH 명령어 선택
	if (value >= 0 && value <= 255) 
	{
		EmitOpcode(Engine::Opcode::PUSH8);
		EmitByte(static_cast<uint8_t>(value));
	} 
	else if (value >= -32768 && value <= 32767) 
	{
		EmitOpcode(Engine::Opcode::PUSH16);
		EmitInt16(static_cast<int16_t>(value));
	} 
	else if (value >= -2147483648LL && value <= 2147483647LL) 
	{
		EmitOpcode(Engine::Opcode::PUSH32);
		EmitInt32(static_cast<int32_t>(value));
	} 
	else 
	{
		EmitOpcode(Engine::Opcode::PUSH64);
		EmitInt64(value);
	}
}

void BytecodeGeneratorVisitor::Visit(const FloatLiteralNode* node) 
{
	// 부동소수점 리터럴은 64비트로 처리
	double value = node->GetValue();
	
	// double을 int64_t로 비트 변환하여 메모리에 저장
	int64_t bits;
	std::memcpy(&bits, &value, sizeof(double));
	
	// 현재 VM에는 FLOAT 타입이 없으므로 정수로 처리
	EmitOpcode(Engine::Opcode::PUSH64);
	EmitInt64(bits);
}

void BytecodeGeneratorVisitor::Visit(const StringLiteralNode* node) 
{
	// 현재 VM에는 문자열 처리 기능이 없으므로 주소로 처리
	// 실제 구현에서는 문자열 상수 풀에 저장하고 참조해야 함
	// 이 부분은 VM이 문자열 처리 기능을 추가하면 업데이트 필요
	
	// 임시로 간단히 구현
	// ALLOC 명령어를 사용해 힙에 메모리 할당
	const std::string& str = node->GetValue();
	size_t len = str.length() + 1; // null 종료 문자 포함
	
	// 메모리 할당 크기 푸시
	EmitOpcode(Engine::Opcode::PUSH32);
	EmitInt32(static_cast<int32_t>(len));
	
	// 메모리 할당
	EmitOpcode(Engine::Opcode::ALLOC);
	
	// 각 문자를 메모리에 저장 (문자열의 각 바이트를 저장)
	// 이 부분은 실제로는 더 효율적으로 구현해야 함
}

void BytecodeGeneratorVisitor::Visit(const BooleanLiteralNode* node) 
{
	bool value = node->GetValue();
	
	EmitOpcode(Engine::Opcode::PUSH8);
	EmitByte(value ? 1 : 0);
}

void BytecodeGeneratorVisitor::Visit(const VariableNode* node) 
{
	// 변수 접근 - 주소를 찾아서 LOAD 명령어 생성
	size_t address = GetVariableAddress(node->GetName());
	
	// 주소 푸시
	EmitOpcode(Engine::Opcode::PUSH32);
	EmitInt32(static_cast<int32_t>(address));
	
	// 값 로드 (64비트 변수 가정)
	EmitOpcode(Engine::Opcode::LOAD64);
}

void BytecodeGeneratorVisitor::Visit(const VariableDeclNode* node) 
{
	// 변수 선언 - 심볼 테이블에 등록
	RegisterVariable(node->GetName(), node->GetType());
	
	// 초기값이 있는 경우 처리
	if (node->GetInitializer()) 
	{
		// 초기값 계산
		node->GetInitializer()->Accept(*this);
		
		// 계산된 값을 변수에 저장
		size_t address = GetVariableAddress(node->GetName());
		EmitOpcode(Engine::Opcode::PUSH32);
		EmitInt32(static_cast<int32_t>(address));
		
		// 값 저장 (64비트 변수 가정)
		EmitOpcode(Engine::Opcode::STORE64);
	}
}

void BytecodeGeneratorVisitor::Visit(const BinaryOpNode* node) 
{
	// 피연산자 평가 (후위 표기법으로 변환)
	node->GetLeft()->Accept(*this);
	node->GetRight()->Accept(*this);
	
	// 연산자에 따른 명령어 선택
	switch (node->GetOpType()) 
	{
		case BinaryOpType::Add:
			EmitOpcode(Engine::Opcode::ADD);
			break;
		case BinaryOpType::Subtract:
			EmitOpcode(Engine::Opcode::SUB);
			break;
		case BinaryOpType::Multiply:
			EmitOpcode(Engine::Opcode::MUL);
			break;
		case BinaryOpType::Divide:
			EmitOpcode(Engine::Opcode::DIV);
			break;
		case BinaryOpType::Modulo:
			EmitOpcode(Engine::Opcode::MOD);
			break;
		case BinaryOpType::BitwiseAnd:
			EmitOpcode(Engine::Opcode::AND);
			break;
		case BinaryOpType::BitwiseOr:
			EmitOpcode(Engine::Opcode::OR);
			break;
		case BinaryOpType::BitwiseXor:
			EmitOpcode(Engine::Opcode::XOR);
			break;
		case BinaryOpType::ShiftLeft:
			EmitOpcode(Engine::Opcode::SHL);
			break;
		case BinaryOpType::ShiftRight:
			EmitOpcode(Engine::Opcode::SHR);
			break;
		case BinaryOpType::Equal:
			// 현재 VM에는 CMP 계열 명령어가 없어 구현 방식 변경
			// 두 값을 XOR한 후 결과가 0인지 검사
			EmitOpcode(Engine::Opcode::XOR);
			EmitOpcode(Engine::Opcode::PUSH8);
			EmitByte(0);
			// JZ 사용을 위해서는 점프 위치 계산 필요
			// 현재는 단순 비교 구현
			break;
		case BinaryOpType::NotEqual:
			// 두 값을 XOR한 후 결과가 0이 아닌지 검사
			EmitOpcode(Engine::Opcode::XOR);
			EmitOpcode(Engine::Opcode::PUSH8);
			EmitByte(0);
			// JNZ 사용을 위해서는 점프 위치 계산 필요
			break;
		case BinaryOpType::Greater:
			// 비교 로직 구현 (stack[sp-2] > stack[sp-1])
			// 현재 VM에는 직접적인 비교 명령어가 없어 JG 등 사용 필요
			break;
		case BinaryOpType::Less:
			// 비교 로직 구현 (stack[sp-2] < stack[sp-1])
			break;
		case BinaryOpType::GreaterEq:
			// 비교 로직 구현 (stack[sp-2] >= stack[sp-1])
			break;
		case BinaryOpType::LessEq:
			// 비교 로직 구현 (stack[sp-2] <= stack[sp-1])
			break;
		case BinaryOpType::LogicalAnd:
			// 논리 AND 구현 - 두 값이 모두 0이 아니면 1, 아니면 0
			// JZ 명령어를 이용한 단락 평가 구현 필요
			break;
		case BinaryOpType::LogicalOr:
			// 논리 OR 구현 - 두 값 중 하나라도 0이 아니면 1, 아니면 0
			// JNZ 명령어를 이용한 단락 평가 구현 필요
			break;
		default:
			throw std::runtime_error("지원하지 않는 이항 연산자 타입: " + 
									std::to_string(static_cast<int>(node->GetOpType())));
	}
}

void BytecodeGeneratorVisitor::Visit(const UnaryOpNode* node) 
{
	// 피연산자 평가
	node->GetOperand()->Accept(*this);
	
	// 연산자에 따른 명령어 선택
	switch (node->GetOpType()) 
	{
		case UnaryOpType::Negate:
			// 부호 반전 (-x)
			// 0에서 값을 빼서 구현
			EmitOpcode(Engine::Opcode::PUSH8);
			EmitByte(0);
			EmitOpcode(Engine::Opcode::SWAP);
			EmitOpcode(Engine::Opcode::SUB);
			break;
		case UnaryOpType::LogicalNot:
			// 논리 부정 (!x)
			// 값이 0인지 검사하여 0이면 1, 아니면 0
			EmitOpcode(Engine::Opcode::PUSH8);
			EmitByte(0);
			EmitOpcode(Engine::Opcode::XOR); // 현재 값을 0과 XOR (값을 그대로 유지)
			// JZ를 이용해 값이 0이면 1 푸시, 아니면 0 푸시하는 로직 필요
			break;
		case UnaryOpType::BitwiseNot:
			// 비트 반전 (~x)
			EmitOpcode(Engine::Opcode::NOT);
			break;
		case UnaryOpType::PreIncrement:
		case UnaryOpType::PostIncrement:
			// 증가 연산 (++x, x++)
			if (node->GetOpType() == UnaryOpType::PostIncrement) 
			{
				EmitOpcode(Engine::Opcode::DUP); // 후위 증가는 원래 값 복제
			}
			
			// 1 더하기
			EmitOpcode(Engine::Opcode::PUSH8);
			EmitByte(1);
			EmitOpcode(Engine::Opcode::ADD);
			
			// 변수에 저장하는 코드는 상위 수준에서 처리해야 함
			break;
		case UnaryOpType::PreDecrement:
		case UnaryOpType::PostDecrement:
			// 감소 연산 (--x, x--)
			if (node->GetOpType() == UnaryOpType::PostDecrement) 
			{
				EmitOpcode(Engine::Opcode::DUP); // 후위 감소는 원래 값 복제
			}
			
			// 1 빼기
			EmitOpcode(Engine::Opcode::PUSH8);
			EmitByte(1);
			EmitOpcode(Engine::Opcode::SUB);
			
			// 변수에 저장하는 코드는 상위 수준에서 처리해야 함
			break;
		default:
			throw std::runtime_error("지원하지 않는 단항 연산자 타입: " + 
									std::to_string(static_cast<int>(node->GetOpType())));
	}
}

// 아직 구현되지 않은 노드들에 대한 Visit 메서드는 비워둠
void BytecodeGeneratorVisitor::Visit(const FunctionDeclNode* node) {}
void BytecodeGeneratorVisitor::Visit(const FunctionCallNode* node) {}
void BytecodeGeneratorVisitor::Visit(const IfStatementNode* node) {}
void BytecodeGeneratorVisitor::Visit(const WhileLoopNode* node) {}
void BytecodeGeneratorVisitor::Visit(const ForLoopNode* node) {}
void BytecodeGeneratorVisitor::Visit(const ReturnStatementNode* node) {}
void BytecodeGeneratorVisitor::Visit(const BreakStatementNode* node) {}
void BytecodeGeneratorVisitor::Visit(const ContinueStatementNode* node) {}
void BytecodeGeneratorVisitor::Visit(const AssignmentOpNode* node) {}

} // namespace Translator
} // namespace DarkMatterVM 