# DarkMatterVM

## 프로젝트 개요
- **이름**: DarkMatterVM  
- **목적**: 코드 가상화 / 난독화 도구  
- **접근**:  
  1. C++ 소스 코드를 커스텀 스택 기반 VM 바이트코드로 변환 (Translator)  
  2. 변환된 바이트코드를 실행 바이너리에 패킹 (Packer)  
  3. 실행 시 바이트코드를 로드·인터프리팅하여 원본 로직을 수행 (Loader → Interpreter Engine)  

## 네이밍 컨벤션
- **디렉토리**: kebab-case (예: `control-flow/`)  
- **파일**: PascalCase (예: `ControlFlowManager.cpp`)  
- **네임스페이스**: `DarkMatterVM::<Module>` (예: `namespace DarkMatterVM::Engine`)  

## 아키텍처 개요

### Translator  
- **입력**: C++ AST  
- **출력**: VM 바이트코드  
- **서브모듈**:  
  - `AST` (ASTNode 정의)  
  - `CodeGen` (BytecodeBuilder)  
  - `Optimizer` (ConstantFolding 등)  
  - `Assembler` (어셈블리 코드를 바이트코드로 변환, 개발/테스트 도구)
    - Parser (어셈블리 구문 파싱)
    - SymbolTable (레이블, 심볼 관리)
    - CodeEmitter (바이트코드 생성)

### Packer  
- **역할**: 바이트코드를 실행 파일의 커스텀 섹션에 삽입  
- **서브모듈**: SectionBuilder  

### Loader  
- **역할**: 실행 시 파일에서 바이트코드 읽기 → VM 메모리 초기화  
- **서브모듈**: BytecodeReader  

### HostInterface  
- **역할**: VM 바이트코드에서 요구하는 호스트 API 호출 중계  
- **예시**: 스레드 생성 (`std::thread`), I/O, 랜덤, 암호화 라이브러리  

### Obfuscation  
- **역할**: 바이트코드 난독화 기법 적용  
- **서브모듈**:  
  - ControlFlowFlattener (제어 흐름 평탄화)  
  - ObfuscationUtils (junk code 삽입 등)  

### Engine (Interpreter)  
- **역할**: 바이트코드 fetch → decode → execute  
- **구성**:  
  - `Opcode.h` (명령어 값, operand 크기 정의)  
  - **Decoder**  
    - BytecodeParser  
    - OpcodeDecoder  
  - **Executor**  
    - ArithmeticExec (ADD, SUB, MUL …)  
    - FlowControlExec (JMP, CJMP, CALL, RET)  
    - HostCallExec (HOSTCALL, THREAD_CREATE 등)  
  - Interpreter (메인 루프)  

### Memory  
- **역할**: VM 스택·콜 스택·힙 메모리 관리  
- **서브모듈**:  
  - StackManager  
  - HeapManager  
  - MemoryManager (통합)  

### ControlFlow  
- **역할**: CALL/RET, 분기(조건·무조건) 흐름 관리  
- **서브모듈**:  
  - FrameLayout (스택 프레임 레이아웃 정의)  
  - ControlFlowManager (IP 조정, 콜 스택)  

## 명령어 집합 (Instruction Set)

| Opcode | Mnemonic   | Operands | 설명                                  |
|:------:|:----------:|:--------:|:--------------------------------------|
| 0x01   | PUSH8      | imm8     | 스택에 1바이트 상수 푸시               |
| 0x02   | PUSH16     | imm16    | 스택에 2바이트 상수 푸시               |
| 0x03   | PUSH32     | imm32    | 스택에 4바이트 상수 푸시               |
| 0x04   | PUSH64     | imm64    | 스택에 8바이트 상수 푸시               |
| 0x05   | POP        | —        | 스택에서 값 제거                       |
| 0x06   | DUP        | —        | 스택 최상위 값 복제                    |
| 0x07   | SWAP       | —        | 스택 최상위 두 값 교환                 |
| 0x10   | ADD        | —        | 스택 두 값 꺼내 더한 뒤 결과 푸시      |
| 0x11   | SUB        | —        | 스택 두 값 꺼내 뺀 뒤 결과 푸시        |
| 0x12   | MUL        | —        | 스택 두 값 꺼내 곱한 뒤 결과 푸시      |
| 0x13   | DIV        | —        | 스택 두 값 꺼내 나눈 뒤 결과 푸시      |
| 0x14   | MOD        | —        | 스택 두 값 꺼내 나머지 계산 후 푸시    |
| 0x15   | AND        | —        | 비트 AND 연산                         |
| 0x16   | OR         | —        | 비트 OR 연산                          |
| 0x17   | XOR        | —        | 비트 XOR 연산                         |
| 0x18   | NOT        | —        | 비트 NOT 연산                         |
| 0x19   | SHL        | —        | 왼쪽 비트 시프트                       |
| 0x1A   | SHR        | —        | 오른쪽 비트 시프트                     |
| 0x20   | LOAD8      | —        | 메모리에서 1바이트 로드                |
| 0x21   | LOAD16     | —        | 메모리에서 2바이트 로드                |
| 0x22   | LOAD32     | —        | 메모리에서 4바이트 로드                |
| 0x23   | LOAD64     | —        | 메모리에서 8바이트 로드                |
| 0x24   | STORE8     | —        | 메모리에 1바이트 저장                  |
| 0x25   | STORE16    | —        | 메모리에 2바이트 저장                  |
| 0x26   | STORE32    | —        | 메모리에 4바이트 저장                  |
| 0x27   | STORE64    | —        | 메모리에 8바이트 저장                  |
| 0x30   | JMP        | rel16    | 상대 분기 (오프셋 ±2바이트)            |
| 0x31   | JZ         | rel16    | 조건 분기 (스택 팝 값 = 0일 때 분기)   |
| 0x32   | JNZ        | rel16    | 조건 분기 (스택 팝 값 ≠ 0일 때 분기)   |
| 0x33   | JG         | rel16    | 조건 분기 (값1 > 값2일 때 분기)        |
| 0x34   | JL         | rel16    | 조건 분기 (값1 < 값2일 때 분기)        |
| 0x35   | JGE        | rel16    | 조건 분기 (값1 >= 값2일 때 분기)       |
| 0x36   | JLE        | rel16    | 조건 분기 (값1 <= 값2일 때 분기)       |
| 0x40   | CALL       | rel16    | 함수 호출 (리턴 주소 푸시)             |
| 0x41   | RET        | —        | 함수 반환 (리턴 주소 팝)               |
| 0x50   | ALLOC      | size     | 힙에 메모리 할당 (주소 스택에 푸시)     |
| 0x51   | FREE       | —        | 할당된 메모리 해제                     |
| 0x60   | HOSTCALL   | id       | 호스트 함수 호출                       |
| 0x61   | THREAD     | —        | 새 스레드 생성                         |
| 0xFF   | HALT       | —        | VM 실행 종료                          |

- **Endian**: Little-endian  
- **인코딩**: `[1B opcode] + [operand bytes…]`  
- **암호화**: 추후 블록 단위 XOR 등 추가 예정  
- **데이터 타입**: 기본적으로 부호 없는 정수로 처리, 필요시 명령어로 타입 변환

## 메모리 레이아웃
- **연산 스택**: 임시 데이터(정수, 주소 등) 보관  
- **콜 스택**: 반환 주소, 이전 FP, 파라미터·로컬 변수 영역  
- **힙**: 동적 할당(추가 계획)  

## 전체 디렉토리 구조

```text
DarkMatterVM/
├── CMakeLists.txt
├── cmake/
│   └── FindXYZ.cmake
├── docs/
│   ├── design.md
│   └── api.md
├── include/
│   └── DarkMatterVM/
│       ├── Translator/
│       │   ├── AST/
│       │   │   └── ASTNode.h
│       │   ├── CodeGen/
│       │   │   └── BytecodeBuilder.h
│       │   ├── Optimizer/
│       │   │   └── ConstantFolding.h
│       │   ├── Assembler/
│       │   │   ├── Parser.h
│       │   │   ├── SymbolTable.h
│       │   │   └── CodeEmitter.h
│       │   └── Translator.h
│       ├── Packer/
│       │   ├── SectionBuilder.h
│       │   └── Packer.h
│       ├── Loader/
│       │   ├── BytecodeReader.h
│       │   └── Loader.h
│       ├── HostInterface/
│       │   ├── ThreadAPI.h
│       │   └── HostInterface.h
│       ├── Obfuscation/
│       │   ├── ControlFlowFlattener.h
│       │   └── ObfuscationUtils.h
│       ├── Engine/
│       │   ├── Opcode.h
│       │   ├── Decoder/
│       │   │   ├── BytecodeParser.h
│       │   │   └── OpcodeDecoder.h
│       │   ├── Executor/
│       │   │   ├── ArithmeticExec.h
│       │   │   ├── FlowControlExec.h
│       │   │   └── HostCallExec.h
│       │   └── Interpreter.h
│       ├── Memory/
│       │   ├── StackManager.h
│       │   ├── HeapManager.h
│       │   └── MemoryManager.h
│       └── ControlFlow/
│           ├── FrameLayout.h
│           └── ControlFlowManager.h
├── src/
│   ├── translator/
│   │   ├── ast/
│   │   │   ├── base/
│   │   │   │   ├── ASTNode.h
│   │   │   │   ├── ASTNode.cpp
│   │   │   │   └── OperatorTypes.h
│   │   │   ├── nodes/
│   │   │   │   ├── LiteralNodes.h
│   │   │   │   ├── LiteralNodes.cpp
│   │   │   │   ├── VariableNodes.h
│   │   │   │   ├── VariableNodes.cpp
│   │   │   │   ├── OperatorNodes.h
│   │   │   │   ├── OperatorNodes.cpp
│   │   │   │   ├── ContainerNodes.h
│   │   │   │   └── ContainerNodes.cpp
│   │   │   ├── visitor/
│   │   │   │   ├── ASTVisitor.h
│   │   │   │   ├── BytecodeGeneratorVisitor.h
│   │   │   │   ├── BytecodeGeneratorVisitor.cpp
│   │   │   │   └── README.md
│   │   │   ├── ASTNodeFactory.h
│   │   │   └── ASTNodeFactory.cpp
│   │   ├── codegen/
│   │   │   └── BytecodeBuilder.cpp
│   │   ├── optimizer/
│   │   │   └── ConstantFolding.cpp
│   │   ├── assembler/
│   │   │   ├── Parser.cpp
│   │   │   ├── SymbolTable.cpp
│   │   │   └── CodeEmitter.cpp
│   │   └── Translator.cpp
│   ├── packer/
│   │   ├── sections/
│   │   │   └── SectionBuilder.cpp
│   │   └── Packer.cpp
│   ├── loader/
│   │   ├── reader/
│   │   │   └── BytecodeReader.cpp
│   │   └── Loader.cpp
│   ├── host-interface/
│   │   ├── thread/
│   │   │   └── ThreadAPI.cpp
│   │   └── HostInterface.cpp
│   ├── obfuscation/
│   │   ├── control-flow/
│   │   │   └── ControlFlowFlattener.cpp
│   │   └── ObfuscationUtils.cpp
│   ├── engine/
│   │   ├── decoder/
│   │   │   ├── BytecodeParser.cpp
│   │   │   └── OpcodeDecoder.cpp
│   │   ├── executor/
│   │   │   ├── ArithmeticExec.cpp
│   │   │   ├── FlowControlExec.cpp
│   │   │   └── HostCallExec.cpp
│   │   └── Interpreter.cpp
│   ├── memory/
│   │   ├── StackManager.cpp
│   │   ├── HeapManager.cpp
│   │   └── MemoryManager.cpp
│   └── control-flow/
│       ├── FrameLayout.cpp
│       └── ControlFlowManager.cpp
├── tests/
│   ├── translator/
│   │   ├── TestTranslator.cpp
│   │   └── TestAssembler.cpp
│   ├── engine/
│   │   └── TestInterpreter.cpp
│   └── memory/
│       └── TestMemoryManager.cpp
└── examples/
    ├── sample-protected-app/
    │   ├── CMakeLists.txt
    │   └── Main.cpp
    └── multithread-vm/
        ├── CMakeLists.txt
        └── Main.cpp
