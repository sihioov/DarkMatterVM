#include "SymbolTable.h"

namespace DarkMatterVM {
namespace Translator {
namespace Assembler {

SymbolTable::SymbolTable() {
    Clear();
}

void SymbolTable::Clear() {
    _symbols.clear();
    _errorMessage.clear();
}

bool SymbolTable::AddLabel(const std::string& name, size_t offset) {
    // 이미 존재하는 심볼인지 확인
    auto it = _symbols.find(name);
    if (it != _symbols.end()) {
        // 이미 정의된 레이블인 경우 오류
        if (it->second.isDefined) {
            _SetError("Label '" + name + "' is already defined");
            return false;
        }
        
        // 정의되지 않은 레이블을 이제 정의하는 경우
        it->second.offset = offset;
        it->second.isDefined = true;
        return true;
    }
    
    // 새 레이블 추가
    SymbolInfo info;
    info.type = SymbolType::LABEL;
    info.offset = offset;
    info.value = 0;
    info.isDefined = true;
    
    _symbols[name] = info;
    return true;
}

bool SymbolTable::AddConstant(const std::string& name, uint64_t value) {
    // 이미 존재하는 심볼인지 확인
    if (_symbols.find(name) != _symbols.end()) {
        _SetError("Symbol '" + name + "' is already defined");
        return false;
    }
    
    // 새 상수 추가
    SymbolInfo info;
    info.type = SymbolType::CONSTANT;
    info.offset = 0;
    info.value = value;
    info.isDefined = true;
    
    _symbols[name] = info;
    return true;
}

const SymbolInfo* SymbolTable::GetSymbol(const std::string& name) const {
    auto it = _symbols.find(name);
    if (it != _symbols.end()) {
        return &(it->second);
    }
    return nullptr;
}

bool SymbolTable::HasSymbol(const std::string& name) const {
    return _symbols.find(name) != _symbols.end();
}

size_t SymbolTable::GetUndefinedCount() const {
    size_t count = 0;
    for (const auto& pair : _symbols) {
        if (!pair.second.isDefined) {
            count++;
        }
    }
    return count;
}

void SymbolTable::_SetError(const std::string& message) {
    _errorMessage = message;
}

} // namespace Assembler
} // namespace Translator
} // namespace DarkMatterVM
