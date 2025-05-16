#include <iostream>
#include "engine/Interpreter.h"

void RunSimpleAdditionExample()
{
	// 인터프리터 생성 (기본 메모리 크기 사용)
	DarkMatterVM::Engine::Interpreter interpreter;
	
	// 간단한 바이트코드 예제:
	// 1. PUSH8 42 - 42를 스택에 넣음
	// 2. PUSH8 13 - 13을 스택에 넣음 
	// 3. ADD     - 두 값을 더함 (42 + 13 = 55)
	// 4. HALT    - 실행 종료 (결과값 반환)
	uint8_t bytecode[] = {
		static_cast<uint8_t>(DarkMatterVM::Engine::Opcode::PUSH8), 42,
		static_cast<uint8_t>(DarkMatterVM::Engine::Opcode::PUSH8), 13,
		static_cast<uint8_t>(DarkMatterVM::Engine::Opcode::ADD),
		static_cast<uint8_t>(DarkMatterVM::Engine::Opcode::HALT)
	};
	
	// 바이트코드 로드
	interpreter.LoadBytecode(bytecode, sizeof(bytecode));
	
	// 실행
	std::cout << "간단한 덧셈 예제 실행 시작..." << std::endl;
	interpreter.Execute();
	
	// 결과 확인
	uint64_t result = interpreter.GetReturnValue();
	std::cout << "실행 결과: " << result << std::endl;
	
	// 예상 결과는 55 (42 + 13)
	if (result == 55) 
	{
		std::cout << "성공: 예상 결과와 일치합니다." << std::endl;
	} 
	else 
	{
		std::cout << "오류: 예상 결과(55)와 다릅니다." << std::endl;
	}
}

void RunMemoryExample()
{
	// 인터프리터 생성
	DarkMatterVM::Engine::Interpreter interpreter;
	
	// 메모리 작업을 하는 바이트코드 예제:
	// 1. ALLOC 8 - 8바이트 메모리 할당 (주소를 스택에 푸시)
	// 2. DUP - 메모리 주소 복제
	// 3. PUSH8 123 - 값 123 푸시
	// 4. STORE64 - 주소에 64비트 값 저장
	// 5. LOAD64 - 주소에서 64비트 값 로드
	// 6. HALT - 실행 종료 (결과값 반환)
	uint8_t bytecode[] = {
		static_cast<uint8_t>(DarkMatterVM::Engine::Opcode::PUSH64), 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ALLOC의 인자 (8바이트)
		static_cast<uint8_t>(DarkMatterVM::Engine::Opcode::ALLOC),
		static_cast<uint8_t>(DarkMatterVM::Engine::Opcode::DUP),
		static_cast<uint8_t>(DarkMatterVM::Engine::Opcode::PUSH64), 0x7B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 값 123 (64비트)
		static_cast<uint8_t>(DarkMatterVM::Engine::Opcode::STORE64),
		static_cast<uint8_t>(DarkMatterVM::Engine::Opcode::LOAD64),
		static_cast<uint8_t>(DarkMatterVM::Engine::Opcode::HALT)
	};
	
	// 바이트코드 로드
	interpreter.LoadBytecode(bytecode, sizeof(bytecode));
	
	// 실행
	std::cout << "\n메모리 예제 실행 시작..." << std::endl;
	interpreter.Execute();
	
	// 결과 확인
	uint64_t result = interpreter.GetReturnValue();
	std::cout << "메모리 예제 실행 결과: " << result << std::endl;
	
	// 예상 결과는 123
	if (result == 123) 
	{
		std::cout << "성공: 메모리 작업이 정상적으로 수행되었습니다." << std::endl;
	} 
	else 
	{
		std::cout << "오류: 예상 결과(123)와 다릅니다." << std::endl;
	}
}

int main()
{
	// 간단한 덧셈 예제 실행
	RunSimpleAdditionExample();
	
	// 메모리 예제 실행
	RunMemoryExample();

	return 0;
}