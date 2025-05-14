#include "Interpreter.h"
#include <iostream>
#include <iomanip>
#include <sstream>

namespace DarkMatterVM {
namespace Engine {

// 정적 멤버 변수 초기화
const std::unordered_map<uint8_t, Interpreter::OpcodeHandler> Interpreter::_opcodeHandlers = 
    Interpreter::_InitializeOpcodeHandlers();

Interpreter::Interpreter(size_t codeSize, size_t stackSize, size_t heapSize)
    : _ip(0), _running(false), _returnValue(0)
{
    // 메모리 관리자 생성
    _memory = std::make_unique<Memory::MemoryManager>(codeSize, stackSize, heapSize);
}

void Interpreter::LoadBytecode(const uint8_t* bytecode, size_t size)
{
    // 기존 상태 리셋
    Reset();
    
    // 바이트코드를 코드 세그먼트에 로드
    _memory->InitializeCode(bytecode, size);
}

void Interpreter::Reset()
{
    // 명령어 포인터 초기화
    _ip = 0;
    
    // 실행 상태 초기화
    _running = false;
    
    // 반환 값 초기화
    _returnValue = 0;
    
    // 스택 포인터 초기화 (스택 세그먼트 크기로 설정)
    auto& stackSegment = _memory->GetSegment(Memory::MemorySegmentType::STACK);
    _memory->SetStackPointer(stackSegment.GetSize());
}

int Interpreter::Execute(size_t startAddress)
{
    // 시작 주소 설정
    _ip = startAddress;
    
    // 실행 플래그 설정
    _running = true;
    
    // 바이트코드 실행 루프
    while (_running) {
        if (!Step()) {
            break;
        }
    }
    
    return 0; // 성공적으로 실행 완료
}

bool Interpreter::Step()
{
    if (!_running) {
        return false;
    }
    
    try {
        // 명령어 가져오기 (fetch)
        uint8_t opcode = _FetchByte();
        
        // 핸들러 찾기 (decode)
        auto it = _opcodeHandlers.find(opcode);
        if (it == _opcodeHandlers.end()) {
            std::stringstream ss;
            ss << "알 수 없는 명령어: 0x" << std::hex << static_cast<int>(opcode);
            throw std::runtime_error(ss.str());
        }
        
        // 명령어 실행 (execute)
        it->second(this);
        
        return true;
    }
    catch (const Memory::MemoryAccessException& e) {
        std::cerr << "메모리 접근 오류: " << e.what() << std::endl;
        _running = false;
        return false;
    }
    catch (const std::exception& e) {
        std::cerr << "VM 실행 오류: " << e.what() << std::endl;
        _running = false;
        return false;
    }
}

void Interpreter::DumpState() const
{
    std::cout << "===== VM 상태 덤프 =====" << std::endl;
    std::cout << "IP: 0x" << std::hex << _ip << std::endl;
    std::cout << "실행 중: " << (_running ? "예" : "아니오") << std::endl;
    std::cout << "반환 값: 0x" << std::hex << _returnValue << " (" << std::dec << _returnValue << ")" << std::endl;
    std::cout << "스택 포인터: 0x" << std::hex << _memory->GetStackPointer() << std::endl;
    std::cout << "=========================" << std::endl;
}

void Interpreter::PushParameter(uint64_t value)
{
    _memory->PushUInt64(value);
}

uint64_t Interpreter::GetReturnValue() const
{
    return _returnValue;
}

uint8_t Interpreter::_FetchByte()
{
    // 현재 IP 위치에서 바이트 읽기
    auto& codeSegment = _memory->GetSegment(Memory::MemorySegmentType::CODE);
    uint8_t byte = codeSegment.ReadByte(_ip);
    
    // IP 증가
    _ip++;
    
    return byte;
}

int16_t Interpreter::_FetchInt16()
{
    // 현재 IP 위치에서 2바이트 읽기
    auto& codeSegment = _memory->GetSegment(Memory::MemorySegmentType::CODE);
    int16_t value = static_cast<int16_t>(codeSegment.ReadUInt16(_ip));
    
    // IP 증가
    _ip += 2;
    
    return value;
}

int32_t Interpreter::_FetchInt32()
{
    // 현재 IP 위치에서 4바이트 읽기
    auto& codeSegment = _memory->GetSegment(Memory::MemorySegmentType::CODE);
    int32_t value = static_cast<int32_t>(codeSegment.ReadUInt32(_ip));
    
    // IP 증가
    _ip += 4;
    
    return value;
}

int64_t Interpreter::_FetchInt64()
{
    // 현재 IP 위치에서 8바이트 읽기
    auto& codeSegment = _memory->GetSegment(Memory::MemorySegmentType::CODE);
    int64_t value = static_cast<int64_t>(codeSegment.ReadUInt64(_ip));
    
    // IP 증가
    _ip += 8;
    
    return value;
}

std::unordered_map<uint8_t, Interpreter::OpcodeHandler> Interpreter::_InitializeOpcodeHandlers()
{
    std::unordered_map<uint8_t, OpcodeHandler> handlers;
    
    // 스택 연산
    handlers[static_cast<uint8_t>(Opcode::PUSH8)] = [](Interpreter* interpreter) { interpreter->_Handle_PUSH8(); };
    handlers[static_cast<uint8_t>(Opcode::PUSH16)] = [](Interpreter* interpreter) { interpreter->_Handle_PUSH16(); };
    handlers[static_cast<uint8_t>(Opcode::PUSH32)] = [](Interpreter* interpreter) { interpreter->_Handle_PUSH32(); };
    handlers[static_cast<uint8_t>(Opcode::PUSH64)] = [](Interpreter* interpreter) { interpreter->_Handle_PUSH64(); };
    handlers[static_cast<uint8_t>(Opcode::POP)] = [](Interpreter* interpreter) { interpreter->_Handle_POP(); };
    handlers[static_cast<uint8_t>(Opcode::DUP)] = [](Interpreter* interpreter) { interpreter->_Handle_DUP(); };
    handlers[static_cast<uint8_t>(Opcode::SWAP)] = [](Interpreter* interpreter) { interpreter->_Handle_SWAP(); };
    
    // 산술 연산
    handlers[static_cast<uint8_t>(Opcode::ADD)] = [](Interpreter* interpreter) { interpreter->_Handle_ADD(); };
    handlers[static_cast<uint8_t>(Opcode::SUB)] = [](Interpreter* interpreter) { interpreter->_Handle_SUB(); };
    handlers[static_cast<uint8_t>(Opcode::MUL)] = [](Interpreter* interpreter) { interpreter->_Handle_MUL(); };
    handlers[static_cast<uint8_t>(Opcode::DIV)] = [](Interpreter* interpreter) { interpreter->_Handle_DIV(); };
    handlers[static_cast<uint8_t>(Opcode::MOD)] = [](Interpreter* interpreter) { interpreter->_Handle_MOD(); };
    
    // 비트 연산
    handlers[static_cast<uint8_t>(Opcode::AND)] = [](Interpreter* interpreter) { interpreter->_Handle_AND(); };
    handlers[static_cast<uint8_t>(Opcode::OR)] = [](Interpreter* interpreter) { interpreter->_Handle_OR(); };
    handlers[static_cast<uint8_t>(Opcode::XOR)] = [](Interpreter* interpreter) { interpreter->_Handle_XOR(); };
    handlers[static_cast<uint8_t>(Opcode::NOT)] = [](Interpreter* interpreter) { interpreter->_Handle_NOT(); };
    handlers[static_cast<uint8_t>(Opcode::SHL)] = [](Interpreter* interpreter) { interpreter->_Handle_SHL(); };
    handlers[static_cast<uint8_t>(Opcode::SHR)] = [](Interpreter* interpreter) { interpreter->_Handle_SHR(); };
    
    // 메모리 연산
    handlers[static_cast<uint8_t>(Opcode::LOAD8)] = [](Interpreter* interpreter) { interpreter->_Handle_LOAD8(); };
    handlers[static_cast<uint8_t>(Opcode::LOAD16)] = [](Interpreter* interpreter) { interpreter->_Handle_LOAD16(); };
    handlers[static_cast<uint8_t>(Opcode::LOAD32)] = [](Interpreter* interpreter) { interpreter->_Handle_LOAD32(); };
    handlers[static_cast<uint8_t>(Opcode::LOAD64)] = [](Interpreter* interpreter) { interpreter->_Handle_LOAD64(); };
    handlers[static_cast<uint8_t>(Opcode::STORE8)] = [](Interpreter* interpreter) { interpreter->_Handle_STORE8(); };
    handlers[static_cast<uint8_t>(Opcode::STORE16)] = [](Interpreter* interpreter) { interpreter->_Handle_STORE16(); };
    handlers[static_cast<uint8_t>(Opcode::STORE32)] = [](Interpreter* interpreter) { interpreter->_Handle_STORE32(); };
    handlers[static_cast<uint8_t>(Opcode::STORE64)] = [](Interpreter* interpreter) { interpreter->_Handle_STORE64(); };
    
    // 제어 흐름
    handlers[static_cast<uint8_t>(Opcode::JMP)] = [](Interpreter* interpreter) { interpreter->_Handle_JMP(); };
    handlers[static_cast<uint8_t>(Opcode::JZ)] = [](Interpreter* interpreter) { interpreter->_Handle_JZ(); };
    handlers[static_cast<uint8_t>(Opcode::JNZ)] = [](Interpreter* interpreter) { interpreter->_Handle_JNZ(); };
    handlers[static_cast<uint8_t>(Opcode::JG)] = [](Interpreter* interpreter) { interpreter->_Handle_JG(); };
    handlers[static_cast<uint8_t>(Opcode::JL)] = [](Interpreter* interpreter) { interpreter->_Handle_JL(); };
    handlers[static_cast<uint8_t>(Opcode::JGE)] = [](Interpreter* interpreter) { interpreter->_Handle_JGE(); };
    handlers[static_cast<uint8_t>(Opcode::JLE)] = [](Interpreter* interpreter) { interpreter->_Handle_JLE(); };
    
    // 함수 호출
    handlers[static_cast<uint8_t>(Opcode::CALL)] = [](Interpreter* interpreter) { interpreter->_Handle_CALL(); };
    handlers[static_cast<uint8_t>(Opcode::RET)] = [](Interpreter* interpreter) { interpreter->_Handle_RET(); };
    
    // 힙 관리
    handlers[static_cast<uint8_t>(Opcode::ALLOC)] = [](Interpreter* interpreter) { interpreter->_Handle_ALLOC(); };
    handlers[static_cast<uint8_t>(Opcode::FREE)] = [](Interpreter* interpreter) { interpreter->_Handle_FREE(); };
    
    // 호스트 인터페이스
    handlers[static_cast<uint8_t>(Opcode::HOSTCALL)] = [](Interpreter* interpreter) { interpreter->_Handle_HOSTCALL(); };
    handlers[static_cast<uint8_t>(Opcode::THREAD)] = [](Interpreter* interpreter) { interpreter->_Handle_THREAD(); };
    
    // 시스템
    handlers[static_cast<uint8_t>(Opcode::HALT)] = [](Interpreter* interpreter) { interpreter->_Handle_HALT(); };
    
    return handlers;
}

// 스택 연산 핸들러 구현
void Interpreter::_Handle_PUSH8()
{
    // 1바이트 상수 가져오기
    uint8_t value = _FetchByte();
    
    // 스택에 푸시 (64비트로 확장)
    _memory->PushUInt64(value);
}

void Interpreter::_Handle_PUSH16()
{
    // 2바이트 상수 가져오기
    uint16_t value = static_cast<uint16_t>(_FetchInt16());
    
    // 스택에 푸시 (64비트로 확장)
    _memory->PushUInt64(value);
}

void Interpreter::_Handle_PUSH32()
{
    // 4바이트 상수 가져오기
    uint32_t value = static_cast<uint32_t>(_FetchInt32());
    
    // 스택에 푸시 (64비트로 확장)
    _memory->PushUInt64(value);
}

void Interpreter::_Handle_PUSH64()
{
    // 8바이트 상수 가져오기
    uint64_t value = static_cast<uint64_t>(_FetchInt64());
    
    // 스택에 푸시
    _memory->PushUInt64(value);
}

void Interpreter::_Handle_POP()
{
    // 스택에서 값 제거
    _memory->PopUInt64();
}

void Interpreter::_Handle_DUP()
{
    // 스택 최상위 값 읽기
    uint64_t value = _memory->PopUInt64();
    
    // 두 번 푸시하여 복제
    _memory->PushUInt64(value);
    _memory->PushUInt64(value);
}

void Interpreter::_Handle_SWAP()
{
    // 스택 최상위 두 값 교환
    uint64_t a = _memory->PopUInt64();
    uint64_t b = _memory->PopUInt64();
    
    _memory->PushUInt64(a);
    _memory->PushUInt64(b);
}

// 산술 연산 핸들러 구현
void Interpreter::_Handle_ADD()
{
    uint64_t b = _memory->PopUInt64();
    uint64_t a = _memory->PopUInt64();
    _memory->PushUInt64(a + b);
}

void Interpreter::_Handle_SUB()
{
    uint64_t b = _memory->PopUInt64();
    uint64_t a = _memory->PopUInt64();
    _memory->PushUInt64(a - b);
}

void Interpreter::_Handle_MUL()
{
    uint64_t b = _memory->PopUInt64();
    uint64_t a = _memory->PopUInt64();
    _memory->PushUInt64(a * b);
}

void Interpreter::_Handle_DIV()
{
    uint64_t b = _memory->PopUInt64();
    uint64_t a = _memory->PopUInt64();
    
    if (b == 0) {
        throw std::runtime_error("0으로 나누기 시도");
    }
    
    _memory->PushUInt64(a / b);
}

void Interpreter::_Handle_MOD()
{
    uint64_t b = _memory->PopUInt64();
    uint64_t a = _memory->PopUInt64();
    
    if (b == 0) {
        throw std::runtime_error("0으로 나누기 시도 (나머지 연산)");
    }
    
    _memory->PushUInt64(a % b);
}

// 비트 연산 핸들러 구현
void Interpreter::_Handle_AND()
{
    uint64_t b = _memory->PopUInt64();
    uint64_t a = _memory->PopUInt64();
    _memory->PushUInt64(a & b);
}

void Interpreter::_Handle_OR()
{
    uint64_t b = _memory->PopUInt64();
    uint64_t a = _memory->PopUInt64();
    _memory->PushUInt64(a | b);
}

void Interpreter::_Handle_XOR()
{
    uint64_t b = _memory->PopUInt64();
    uint64_t a = _memory->PopUInt64();
    _memory->PushUInt64(a ^ b);
}

void Interpreter::_Handle_NOT()
{
    uint64_t a = _memory->PopUInt64();
    _memory->PushUInt64(~a);
}

void Interpreter::_Handle_SHL()
{
    uint64_t b = _memory->PopUInt64();
    uint64_t a = _memory->PopUInt64();
    
    // 시프트 양이 64 이상이면 결과는 0
    if (b >= 64) {
        _memory->PushUInt64(0);
    } else {
        _memory->PushUInt64(a << b);
    }
}

void Interpreter::_Handle_SHR()
{
    uint64_t b = _memory->PopUInt64();
    uint64_t a = _memory->PopUInt64();
    
    // 시프트 양이 64 이상이면 결과는 0
    if (b >= 64) {
        _memory->PushUInt64(0);
    } else {
        _memory->PushUInt64(a >> b);
    }
}

// 메모리 연산 핸들러 - 간단한 구현
void Interpreter::_Handle_LOAD8()
{
    // 주소를 스택에서 가져옴
    uint64_t address = _memory->PopUInt64();
    
    // 힙 메모리에서 바이트 읽기
    auto& heapSegment = _memory->GetSegment(Memory::MemorySegmentType::HEAP);
    uint8_t value = heapSegment.ReadByte(static_cast<size_t>(address));
    
    // 결과를 스택에 푸시
    _memory->PushUInt64(value);
}

void Interpreter::_Handle_STORE8()
{
    // 값을 스택에서 가져옴
    uint64_t value = _memory->PopUInt64();
    
    // 주소를 스택에서 가져옴
    uint64_t address = _memory->PopUInt64();
    
    // 힙 메모리에 바이트 쓰기
    auto& heapSegment = _memory->GetSegment(Memory::MemorySegmentType::HEAP);
    heapSegment.WriteByte(static_cast<size_t>(address), static_cast<uint8_t>(value));
}

// 나머지 LOAD/STORE 핸들러는 비슷한 패턴으로 구현

// 제어 흐름 핸들러 구현
void Interpreter::_Handle_JMP()
{
    // 점프 오프셋 가져오기
    int16_t offset = _FetchInt16();
    
    // IP 조정 (IP는 이미 opcode와 offset을 읽은 후 증가되어 있음)
    _ip += offset;
}

void Interpreter::_Handle_JZ()
{
    // 조건 가져오기
    uint64_t condition = _memory->PopUInt64();
    
    // 점프 오프셋 가져오기
    int16_t offset = _FetchInt16();
    
    // 조건이 0이면 점프
    if (condition == 0) {
        _ip += offset;
    }
}

void Interpreter::_Handle_JNZ()
{
    // 조건 가져오기
    uint64_t condition = _memory->PopUInt64();
    
    // 점프 오프셋 가져오기
    int16_t offset = _FetchInt16();
    
    // 조건이 0이 아니면 점프
    if (condition != 0) {
        _ip += offset;
    }
}

// 나머지 조건부 점프 핸들러는 비슷한 패턴으로 구현

// HALT 핸들러
void Interpreter::_Handle_HALT()
{
    // 마지막 스택 값을 반환 값으로 설정 (있는 경우)
    if (_memory->GetStackPointer() < _memory->GetSegment(Memory::MemorySegmentType::STACK).GetSize()) {
        _returnValue = _memory->PopUInt64();
    }
    
    // 실행 중지
    _running = false;
}

// 아직 미구현된 핸들러들 - 기본 구현만 제공
void Interpreter::_Handle_LOAD16() { /* TODO */ }
void Interpreter::_Handle_LOAD32() { /* TODO */ }
void Interpreter::_Handle_LOAD64() { /* TODO */ }
void Interpreter::_Handle_STORE16() { /* TODO */ }
void Interpreter::_Handle_STORE32() { /* TODO */ }
void Interpreter::_Handle_STORE64() { /* TODO */ }
void Interpreter::_Handle_JG() { /* TODO */ }
void Interpreter::_Handle_JL() { /* TODO */ }
void Interpreter::_Handle_JGE() { /* TODO */ }
void Interpreter::_Handle_JLE() { /* TODO */ }
void Interpreter::_Handle_CALL() { /* TODO */ }
void Interpreter::_Handle_RET() { /* TODO */ }
void Interpreter::_Handle_ALLOC() { /* TODO */ }
void Interpreter::_Handle_FREE() { /* TODO */ }
void Interpreter::_Handle_HOSTCALL() { /* TODO */ }
void Interpreter::_Handle_THREAD() { /* TODO */ }

} // namespace Engine
} // namespace DarkMatterVM
