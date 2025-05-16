#include "CodeEmitter.h"
#include <map>
#include <algorithm>

namespace DarkMatterVM 
{
namespace Translator 
{
namespace Assembler 
{

// 명령어 니모닉과 Opcode 매핑
static const std::map<std::string, Engine::Opcode> s_opcodeMap = 
{
    {"PUSH8", Engine::Opcode::PUSH8},
    {"PUSH16", Engine::Opcode::PUSH16},
    {"PUSH32", Engine::Opcode::PUSH32},
    {"PUSH64", Engine::Opcode::PUSH64},
    {"POP", Engine::Opcode::POP},
    {"DUP", Engine::Opcode::DUP},
    {"SWAP", Engine::Opcode::SWAP},
    
    {"ADD", Engine::Opcode::ADD},
    {"SUB", Engine::Opcode::SUB},
    {"MUL", Engine::Opcode::MUL},
    {"DIV", Engine::Opcode::DIV},
    {"MOD", Engine::Opcode::MOD},
    
    {"AND", Engine::Opcode::AND},
    {"OR", Engine::Opcode::OR},
    {"XOR", Engine::Opcode::XOR},
    {"NOT", Engine::Opcode::NOT},
    {"SHL", Engine::Opcode::SHL},
    {"SHR", Engine::Opcode::SHR},
    
    {"LOAD8", Engine::Opcode::LOAD8},
    {"LOAD16", Engine::Opcode::LOAD16},
    {"LOAD32", Engine::Opcode::LOAD32},
    {"LOAD64", Engine::Opcode::LOAD64},
    {"STORE8", Engine::Opcode::STORE8},
    {"STORE16", Engine::Opcode::STORE16},
    {"STORE32", Engine::Opcode::STORE32},
    {"STORE64", Engine::Opcode::STORE64},
    
    {"JMP", Engine::Opcode::JMP},
    {"JZ", Engine::Opcode::JZ},
    {"JNZ", Engine::Opcode::JNZ},
    {"JG", Engine::Opcode::JG},
    {"JL", Engine::Opcode::JL},
    {"JGE", Engine::Opcode::JGE},
    {"JLE", Engine::Opcode::JLE},
    
    {"CALL", Engine::Opcode::CALL},
    {"RET", Engine::Opcode::RET},
    
    {"ALLOC", Engine::Opcode::ALLOC},
    {"FREE", Engine::Opcode::FREE},
    
    {"HOSTCALL", Engine::Opcode::HOSTCALL},
    {"THREAD", Engine::Opcode::THREAD},
    
    {"HALT", Engine::Opcode::HALT}
};

CodeEmitter::CodeEmitter(SymbolTable& symbolTable)
    : _symbolTable(symbolTable), _currentTokenIndex(0), _tokens(nullptr), _hasError(false) 
{
    Initialize();
}

void CodeEmitter::Initialize() 
{
    _bytecode.clear();
    _fixups.clear();
    _errorMessage.clear();
    _currentTokenIndex = 0;
    _hasError = false;
}

bool CodeEmitter::EmitCode(const std::vector<Token>& tokens) 
{
    if (tokens.empty()) 
    {
        return true;
    }
    
    _tokens = &tokens;
    _currentTokenIndex = 0;
    
    while (_currentTokenIndex < _tokens->size()) 
    {
        const Token& token = _CurrentToken();
        
        // 파일 끝 도달
        if (token.type == TokenType::END) 
        {
            break;
        }
        
        // 토큰 유형에 따른 처리
        switch (token.type) 
        {
            case TokenType::LABEL: 
            {
                // 레이블 처리: 현재 위치를 심볼 테이블에 기록
                if (!_symbolTable.AddLabel(token.text, GetCurrentOffset())) 
                {
                    _SetError("Failed to add label: " + _symbolTable.GetErrorMessage(), &token);
                    return false;
                }
                _NextToken();
                break;
            }
            
            case TokenType::MNEMONIC: 
            {
                // 명령어 처리
                auto it = s_opcodeMap.find(token.text);
                if (it == s_opcodeMap.end()) 
                {
                    _SetError("Unknown mnemonic: " + token.text, &token);
                    return false;
                }
                
                Engine::Opcode opcode = it->second;
                _EmitByte(static_cast<uint8_t>(opcode));
                
                _NextToken();
                
                // 오퍼랜드 처리
                if (!_ProcessInstruction(opcode)) 
                {
                    return false;
                }
                
                break;
            }
            
            case TokenType::DIRECTIVE: 
            {
                // 지시자 처리 (예: .DB, .DW 등)
                // TODO: 지시자 구현
                _NextToken();
                break;
            }
            
            case TokenType::EOL: 
            {
                // 줄 끝 처리
                _NextToken();
                break;
            }
            
            default: 
            {
                _SetError("Unexpected token type", &token);
                return false;
            }
        }
    }
    
    // 레이블 수정(fix-up) 적용
    return _ApplyFixups();
}

bool CodeEmitter::_ProcessInstruction(Engine::Opcode opcode) 
{
    const Engine::OpcodeInfo& info = Engine::GetOpcodeInfo(opcode);
    
    // 오퍼랜드가 필요 없는 경우 바로 리턴
    if (info.operandSize == 0) 
    {
        return true;
    }
    
    // 오퍼랜드 토큰이 없는 경우 오류
    if (_currentTokenIndex >= _tokens->size() || 
        (_CurrentToken().type != TokenType::NUMBER && _CurrentToken().type != TokenType::LABEL)) 
    {
        _SetError("Expected operand for instruction " + std::string(info.mnemonic));
        return false;
    }
    
    const Token& operandToken = _CurrentToken();
    
    // 점프/호출 명령어 처리 (레이블 관련)
    if (info.modifiesIP && (opcode == Engine::Opcode::JMP || opcode == Engine::Opcode::JZ || 
                           opcode == Engine::Opcode::JNZ || opcode == Engine::Opcode::JG || 
                           opcode == Engine::Opcode::JL || opcode == Engine::Opcode::JGE || 
                           opcode == Engine::Opcode::JLE || opcode == Engine::Opcode::CALL)) 
    {
        
        if (operandToken.type == TokenType::LABEL) 
        {
            // 레이블로 점프하는 경우
            const SymbolInfo* symbolInfo = _symbolTable.GetSymbol(operandToken.text);
            
            if (symbolInfo && symbolInfo->isDefined) 
            {
                // 이미 정의된 레이블인 경우, 상대 주소 계산
                int32_t relativeOffset = static_cast<int32_t>(symbolInfo->offset) - 
                                         static_cast<int32_t>(_bytecode.size() + info.operandSize);
                
                // 2바이트 상대 주소 저장
                _EmitUInt16(static_cast<uint16_t>(relativeOffset));
            } 
            else 
            {
                // 아직 정의되지 않은 레이블인 경우, 수정 목록에 추가
                Fixup fixup;
                fixup.offset = _bytecode.size();
                fixup.targetLabel = operandToken.text;
                fixup.size = info.operandSize;
                fixup.isRelative = true;
                
                _fixups.push_back(fixup);
                
                // 임시 값 추가 (0으로 채움)
                for (uint8_t i = 0; i < info.operandSize; i++) 
                {
                    _EmitByte(0);
                }
            }
        } 
        else if (operandToken.type == TokenType::NUMBER) 
        {
            // 상수 값으로 점프하는 경우 (절대 주소나 상대 주소)
            if (info.operandSize == 2) 
            {
                _EmitUInt16(static_cast<uint16_t>(operandToken.value));
            } 
            else 
            {
                _SetError("Unsupported operand size for jump instruction", &operandToken);
                return false;
            }
        } 
        else 
        {
            _SetError("Invalid operand type for jump instruction", &operandToken);
            return false;
        }
    } 
    else 
    {
        // 일반 명령어 처리 (PUSH 등)
        if (operandToken.type == TokenType::NUMBER) 
        {
            switch (info.operandSize) 
            {
                case 1:
                    _EmitByte(static_cast<uint8_t>(operandToken.value));
                    break;
                case 2:
                    _EmitUInt16(static_cast<uint16_t>(operandToken.value));
                    break;
                case 4:
                    _EmitUInt32(static_cast<uint32_t>(operandToken.value));
                    break;
                case 8:
                    _EmitUInt64(operandToken.value);
                    break;
                default:
                    _SetError("Unsupported operand size", &operandToken);
                    return false;
            }
        } 
        else 
        {
            _SetError("Invalid operand type for instruction", &operandToken);
            return false;
        }
    }
    
    _NextToken();
    return true;
}

bool CodeEmitter::_ApplyFixups() 
{
    // 모든 수정(fix-up) 항목 적용
    for (const Fixup& fixup : _fixups) 
    {
        const SymbolInfo* symbolInfo = _symbolTable.GetSymbol(fixup.targetLabel);
        
        if (!symbolInfo) 
        {
            _SetError("Undefined label referenced: " + fixup.targetLabel);
            return false;
        }
        
        if (!symbolInfo->isDefined) 
        {
            _SetError("Forward reference to undefined label: " + fixup.targetLabel);
            return false;
        }
        
        // 상대 주소 또는 절대 주소 계산
        uint64_t value;
        if (fixup.isRelative) 
        {
            // 상대 주소 계산 (목표 위치 - (현재 위치 + 오퍼랜드 크기))
            int32_t relativeOffset = static_cast<int32_t>(symbolInfo->offset) - 
                                     static_cast<int32_t>(fixup.offset + fixup.size);
            value = static_cast<uint64_t>(relativeOffset);
        } 
        else 
        {
            // 절대 주소
            value = static_cast<uint64_t>(symbolInfo->offset);
        }
        
        // 바이트코드에 적용
        switch (fixup.size) 
        {
            case 1:
                _bytecode[fixup.offset] = static_cast<uint8_t>(value);
                break;
                
            case 2:
                _bytecode[fixup.offset] = static_cast<uint8_t>(value & 0xFF);
                _bytecode[fixup.offset + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
                break;
                
            case 4:
                _bytecode[fixup.offset] = static_cast<uint8_t>(value & 0xFF);
                _bytecode[fixup.offset + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
                _bytecode[fixup.offset + 2] = static_cast<uint8_t>((value >> 16) & 0xFF);
                _bytecode[fixup.offset + 3] = static_cast<uint8_t>((value >> 24) & 0xFF);
                break;
                
            case 8:
                _bytecode[fixup.offset] = static_cast<uint8_t>(value & 0xFF);
                _bytecode[fixup.offset + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
                _bytecode[fixup.offset + 2] = static_cast<uint8_t>((value >> 16) & 0xFF);
                _bytecode[fixup.offset + 3] = static_cast<uint8_t>((value >> 24) & 0xFF);
                _bytecode[fixup.offset + 4] = static_cast<uint8_t>((value >> 32) & 0xFF);
                _bytecode[fixup.offset + 5] = static_cast<uint8_t>((value >> 40) & 0xFF);
                _bytecode[fixup.offset + 6] = static_cast<uint8_t>((value >> 48) & 0xFF);
                _bytecode[fixup.offset + 7] = static_cast<uint8_t>((value >> 56) & 0xFF);
                break;
                
            default:
                _SetError("Unsupported fixup size: " + std::to_string(fixup.size));
                return false;
        }
    }
    
    return true;
}

void CodeEmitter::_EmitByte(uint8_t value) 
{
    _bytecode.push_back(value);
}

void CodeEmitter::_EmitUInt16(uint16_t value) 
{
    _bytecode.push_back(static_cast<uint8_t>(value & 0xFF));
    _bytecode.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
}

void CodeEmitter::_EmitUInt32(uint32_t value) 
{
    _bytecode.push_back(static_cast<uint8_t>(value & 0xFF));
    _bytecode.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
    _bytecode.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
    _bytecode.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
}

void CodeEmitter::_EmitUInt64(uint64_t value) 
{
    _bytecode.push_back(static_cast<uint8_t>(value & 0xFF));
    _bytecode.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
    _bytecode.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
    _bytecode.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
    _bytecode.push_back(static_cast<uint8_t>((value >> 32) & 0xFF));
    _bytecode.push_back(static_cast<uint8_t>((value >> 40) & 0xFF));
    _bytecode.push_back(static_cast<uint8_t>((value >> 48) & 0xFF));
    _bytecode.push_back(static_cast<uint8_t>((value >> 56) & 0xFF));
}

void CodeEmitter::_SetError(const std::string& message, const Token* token) 
{
    if (token) 
    {
        _errorMessage = "Line " + std::to_string(token->line) + ", Column " + 
                        std::to_string(token->column) + ": " + message;
    } 
    else 
    {
        _errorMessage = message;
    }
    _hasError = true;
}

const Token& CodeEmitter::_CurrentToken() const 
{
    return (*_tokens)[_currentTokenIndex];
}

void CodeEmitter::_NextToken() 
{
    if (_currentTokenIndex < _tokens->size()) 
    {
        _currentTokenIndex++;
    }
}

} // namespace Assembler
} // namespace Translator
} // namespace DarkMatterVM
