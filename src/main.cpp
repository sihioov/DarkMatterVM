#include <iostream>
#include "engine/Interpreter.h"
#include "translator/Translator.h"
#include <common/Logger.h>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif

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

void RunCppParserExample()
{
	std::cout << "\n=== C++ 파서 테스트 시작 ===" << std::endl;
	
	// Translator 생성
	DarkMatterVM::Translator::Translator translator;
	
	// 테스트할 C++ 코드
	std::string cppCode = R"(
		int x = 42;
		int y = 13;
		int sum = x + y;
	)";
	
	std::cout << "변환할 C++ 코드:" << std::endl;
	std::cout << cppCode << std::endl;
	
	// C++ 코드를 바이트코드로 변환
	auto result = translator.TranslateFromCpp(cppCode, "test_module");
	
	if (result == DarkMatterVM::Translator::TranslationResult::Success) 
	{
		std::cout << "✅ C++ 파싱 및 변환 성공!" << std::endl;
		
		// 생성된 바이트코드 정보 출력
		const auto& bytecode = translator.GetBytecode();
		std::cout << "생성된 바이트코드 크기: " << bytecode.size() << " 바이트" << std::endl;
		
		// 바이트코드 덤프 출력
		std::cout << "\n생성된 바이트코드:" << std::endl;
		std::cout << translator.DumpBytecode() << std::endl;
		
		// 생성된 바이트코드를 VM에서 실행 테스트
		std::cout << "\n=== 생성된 바이트코드 실행 테스트 ===" << std::endl;
		DarkMatterVM::Engine::Interpreter interpreter;
		interpreter.LoadBytecode(bytecode.data(), bytecode.size());
		interpreter.Execute();
		
		std::cout << "VM 실행 완료!" << std::endl;
	} 
	else 
	{
		std::cout << "❌ C++ 파싱 실패: " << translator.GetLastError() << std::endl;
	}
}

int main()
{
	// Windows 콘솔에서 UTF-8 한글 표시를 위한 설정
#ifdef _WIN32
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);
#endif
	
	// Logger 초기화 - DEBUG 레벨로 설정하여 모든 로그 출력
	DarkMatterVM::Logger::Initialize(DarkMatterVM::LogLevel::DEBUG, true);
	
	std::cout << "=== DarkMatterVM 테스트 시작 ===" << std::endl;
	
	// 간단한 덧셈 예제 실행
	RunSimpleAdditionExample();
	
	// 메모리 예제 실행
	RunMemoryExample();
	
	// C++ 파서 테스트 실행
	RunCppParserExample();

	// Logger 정리
	DarkMatterVM::Logger::Cleanup();

	return 0;
}