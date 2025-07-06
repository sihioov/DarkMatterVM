#include "BytecodeBuilder.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include "../ast/nodes/ContainerNodes.h"
#include "../ast/nodes/LiteralNodes.h"
#include "../ast/nodes/OperatorNodes.h"
#include "../ast/nodes/VariableNodes.h"
#include "../../common/Logger.h"

namespace DarkMatterVM 
{
namespace Translator 
{

BytecodeBuilder::BytecodeBuilder() 
	: _currentAddress(0) 
{
	Reset();
}

bool BytecodeBuilder::GenerateFromAST(const ASTNode* rootNode) 
{
	if (!rootNode) 
	{
		Logger::Error("BytecodeBuilder", "AST 루트 노드가 null입니다");
		return false;
	}
	
	// AST 처리 시작
	try 
	{
		Logger::Info("BytecodeBuilder", "AST 노드 처리 시작");
		ProcessNode(rootNode);
		
		Logger::Info("BytecodeBuilder", "프로그램 종료 명령어 추가");
		// 프로그램 종료 명령어 추가
		EmitOpcode(Engine::Opcode::HALT);
		
		Logger::Info("BytecodeBuilder", "바이트코드 생성 완료");
		return true;
	} 
	catch (const std::exception& e) 
	{
		Logger::Error("BytecodeBuilder", "바이트코드 생성 중 오류 발생: " + std::string(e.what()));
		
		return false;
	}
}

void BytecodeBuilder::Reset() 
{
	_bytecode.clear();
	_symbolTable.clear();
	_currentAddress = 0;
}

std::string BytecodeBuilder::DumpBytecode() const 
{
	std::stringstream ss;
	
	ss << "BytecodeBuilder 덤프 - " << _bytecode.size() << " 바이트\n";
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
void BytecodeBuilder::EmitByte(uint8_t byte) 
{
	_bytecode.push_back(byte);
}

void BytecodeBuilder::EmitInt16(int16_t value) 
{
	// Little-endian 방식으로 저장
	EmitByte(static_cast<uint8_t>(value & 0xFF));
	EmitByte(static_cast<uint8_t>((value >> 8) & 0xFF));
}

void BytecodeBuilder::EmitInt32(int32_t value) 
{
	// Little-endian 방식으로 저장
	EmitByte(static_cast<uint8_t>(value & 0xFF));
	EmitByte(static_cast<uint8_t>((value >> 8) & 0xFF));
	EmitByte(static_cast<uint8_t>((value >> 16) & 0xFF));
	EmitByte(static_cast<uint8_t>((value >> 24) & 0xFF));
}

void BytecodeBuilder::EmitInt64(int64_t value) 
{
	// Little-endian 방식으로 저장
	EmitByte(static_cast<uint8_t>(value & 0xFF));
	EmitByte(static_cast<uint8_t>((value >> 8) & 0xFF));
	EmitByte(static_cast<uint8_t>((value >> 16) & 0xFF));
	EmitByte(static_cast<uint8_t>((value >> 24) & 0xFF));
	EmitByte(static_cast<uint8_t>((value >> 32) & 0xFF));
	EmitByte(static_cast<uint8_t>((value >> 40) & 0xFF));
	EmitByte(static_cast<uint8_t>((value >> 48) & 0xFF));
	EmitByte(static_cast<uint8_t>((value >> 56) & 0xFF));
}

void BytecodeBuilder::EmitOpcode(Engine::Opcode opcode) 
{
	const auto& opcodeInfo = Engine::GetOpcodeInfo(opcode);
	Logger::Info("BytecodeBuilder", "    명령어 생성: " + std::string(opcodeInfo.mnemonic) + " (0x" + 
	             std::to_string(static_cast<int>(opcode)) + ")");
	EmitByte(static_cast<uint8_t>(opcode));
}

// 노드 처리 메서드들
void BytecodeBuilder::ProcessNode(const ASTNode* node) 
{
	if (!node)
	{
		Logger::Warning("BytecodeBuilder", "null 노드 처리 건너뜀");
		return;
	}
	
	switch (node->GetType()) 
	{
		case NodeType::Block:
			Logger::Info("BytecodeBuilder", "블록 노드 처리");
			ProcessBlock(dynamic_cast<const BlockNode*>(node));
			break;
			
		case NodeType::IntegerLiteral:
			ProcessIntegerLiteral(dynamic_cast<const IntegerLiteralNode*>(node));
			break;
			
		case NodeType::Variable:
			ProcessVariable(dynamic_cast<const VariableNode*>(node));
			break;
			
		case NodeType::VariableDecl:
			ProcessVariableDecl(dynamic_cast<const VariableDeclNode*>(node));
			break;
			
		case NodeType::BinaryOp:
			ProcessBinaryOp(dynamic_cast<const BinaryOpNode*>(node));
			break;
			
		default:
			Logger::Error("BytecodeBuilder", "지원하지 않는 AST 노드 타입: " + std::to_string(static_cast<int>(node->GetType())));
			throw std::runtime_error("지원하지 않는 AST 노드 타입: " + std::to_string(static_cast<int>(node->GetType())));
	}
}

void BytecodeBuilder::ProcessBlock(const BlockNode* node) 
{
	// 블록 내의 모든 문장을 순차적으로 처리
	Logger::Info("BytecodeBuilder", "  블록 내 " + std::to_string(node->GetStatements().size()) + "개 문장 처리 시작");
	for (size_t i = 0; i < node->GetStatements().size(); ++i) 
	{
		Logger::Info("BytecodeBuilder", "  문장 " + std::to_string(i + 1) + "/" + std::to_string(node->GetStatements().size()) + " 처리");
		ProcessNode(node->GetStatements()[i].get());
	}
	Logger::Info("BytecodeBuilder", "  블록 처리 완료");
}

void BytecodeBuilder::ProcessIntegerLiteral(const IntegerLiteralNode* node) 
{
	int64_t value = node->GetValue();
	Logger::Info("BytecodeBuilder", "  정수 리터럴 처리: " + std::to_string(value));
	
	// 값의 크기에 따라 적절한 PUSH 명령어 선택
	if (value >= 0 && value <= 255) 
	{
		Logger::Info("BytecodeBuilder", "    8비트 값으로 처리");
		EmitOpcode(Engine::Opcode::PUSH8);
		EmitByte(static_cast<uint8_t>(value));
	} 
	else if (value >= -32768 && value <= 32767) 
	{
		Logger::Info("BytecodeBuilder", "    16비트 값으로 처리");
		EmitOpcode(Engine::Opcode::PUSH16);
		EmitInt16(static_cast<int16_t>(value));
	} 
	else if (value >= -2147483648LL && value <= 2147483647LL) 
	{
		Logger::Info("BytecodeBuilder", "    32비트 값으로 처리");
		EmitOpcode(Engine::Opcode::PUSH32);
		EmitInt32(static_cast<int32_t>(value));
	} 
	else 
	{
		Logger::Info("BytecodeBuilder", "    64비트 값으로 처리");
		EmitOpcode(Engine::Opcode::PUSH64);
		EmitInt64(value);
	}
}

void BytecodeBuilder::ProcessVariable(const VariableNode* node) 
{
	// 변수 참조 처리
	const std::string& name = node->GetName();
	Logger::Info("BytecodeBuilder", "  변수 참조 처리: " + name);
	
	// 심볼 테이블에서 변수 주소 찾기
	size_t address = GetVariableAddress(name);
	Logger::Info("BytecodeBuilder", "    변수 주소: 0x" + std::to_string(address));
	
	// 변수 주소를 스택에 푸시
	EmitOpcode(Engine::Opcode::PUSH64);
	EmitInt64(static_cast<int64_t>(address));
	
	// 주소에서 값 로드
	EmitOpcode(Engine::Opcode::LOAD64);
}

void BytecodeBuilder::ProcessVariableDecl(const VariableDeclNode* node) 
{
	// 변수 선언 처리
	const std::string& name = node->GetName();
	const std::string& type = node->GetType();
	Logger::Info("BytecodeBuilder", "  변수 선언 처리: " + type + " " + name);
	
	// 변수를 심볼 테이블에 등록
	RegisterVariable(name, type);
	Logger::Info("BytecodeBuilder", "    변수 등록 완료: 주소 0x" + std::to_string(_symbolTable[name].address));
	
	// 초기화 값이 있으면 처리
	if (const ASTNode* initializer = node->GetInitializer()) 
	{
		Logger::Info("BytecodeBuilder", "    초기화 값 처리 시작");
		
		// 초기값 계산하여 스택에 푸시
		ProcessNode(initializer);
		
		Logger::Info("BytecodeBuilder", "    변수에 값 저장");
		// 변수 주소를 스택에 푸시
		EmitOpcode(Engine::Opcode::PUSH64);
		EmitInt64(static_cast<int64_t>(_symbolTable[name].address));
		
		// 스택 맨 위 값을 해당 주소에 저장
		EmitOpcode(Engine::Opcode::STORE64);
	}
}

void BytecodeBuilder::ProcessBinaryOp(const BinaryOpNode* node) 
{
	// 이항 연산자 처리
	std::string opName;
	switch (node->GetOpType()) 
	{
		case BinaryOpType::Add:      opName = "+"; break;
		case BinaryOpType::Subtract: opName = "-"; break;
		case BinaryOpType::Multiply: opName = "*"; break;
		case BinaryOpType::Divide:   opName = "/"; break;
		case BinaryOpType::Modulo:   opName = "%"; break;
		default:                     opName = "unknown"; break;
	}
	Logger::Info("BytecodeBuilder", "  이항 연산 처리: " + opName);
	
	Logger::Info("BytecodeBuilder", "    왼쪽 피연산자 처리");
	// 왼쪽 피연산자 처리 (결과는 스택에 푸시됨)
	ProcessNode(node->GetLeft());
	
	Logger::Info("BytecodeBuilder", "    오른쪽 피연산자 처리");
	// 오른쪽 피연산자 처리 (결과는 스택에 푸시됨)
	ProcessNode(node->GetRight());
	
	Logger::Info("BytecodeBuilder", "    연산자 명령어 생성");
	// 연산자에 따른 명령어 추가
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
			
		// 비교 연산자는 추가적인 처리 필요 (VM에 직접적인 비교 연산이 없을 경우)
		case BinaryOpType::Equal:
			// 두 값을 뺀 후 0과 비교하는 방식 또는 직접 지원하는 VM 명령어 사용
			EmitOpcode(Engine::Opcode::SUB);
			EmitOpcode(Engine::Opcode::PUSH8); // 0 푸시
			EmitByte(0);
			// 여기서는 제어흐름 명령어로 처리하지 않았지만, 실제로는 JZ 등을 사용할 수 있음
			break;
			
		// 다른 연산자도 유사하게 처리...
		default:
			throw std::runtime_error("지원하지 않는 연산자 타입");
	}
}

void BytecodeBuilder::RegisterVariable(const std::string& name, const std::string& type) 
{
	// 이미 등록된 변수인지 확인
	if (_symbolTable.find(name) != _symbolTable.end()) 
	{
		throw std::runtime_error("변수 '" + name + "' 이미 선언됨");
	}
	
	// 새 변수 등록 (8바이트 공간 할당 - int64_t 기준)
	_symbolTable.emplace(name, SymbolInfo(name, type, _currentAddress));
	
	// 주소 업데이트 (다음 변수를 위한 공간 확보)
	_currentAddress += 8;
}

size_t BytecodeBuilder::GetVariableAddress(const std::string& name) 
{
	auto it = _symbolTable.find(name);
	if (it == _symbolTable.end()) 
	{
		throw std::runtime_error("선언되지 않은 변수 '" + name + "' 참조");
	}
	
	return it->second.address;
}

} // namespace Translator
} // namespace DarkMatterVM
