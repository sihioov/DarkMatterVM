#pragma once

#include <string>
#include <fstream>
#include <iostream>

namespace DarkMatterVM 
{

/**
 * @brief 로그 레벨 열거형
 */
enum class LogLevel 
{
	DEBUG,    ///< 디버그 정보 (가장 상세한 로그)
	INFO,     ///< 일반 정보
	WARNING,  ///< 경고 (오류는 아니지만 주의 필요)
	ERROR,    ///< 오류 (작업 실패)
	FATAL     ///< 치명적 오류 (프로그램 중단 필요)
};

/**
 * @brief 로깅 유틸리티 클래스
 * 
 * 로그 메시지를 콘솔 및/또는 파일에 출력하는 정적 메서드 제공
 */
class Logger 
{
public:
	/**
	 * @brief 로거 초기화
	 * 
	 * @param level 로그 레벨 (기본값: INFO)
	 * @param toConsole 콘솔 출력 여부 (기본값: true)
	 * @param logFilePath 로그 파일 경로 (빈 문자열이면 파일 로깅 비활성화)
	 */
	static void Initialize(LogLevel level = LogLevel::INFO, bool toConsole = true, const std::string& logFilePath = "");
	
	static void SetLevel(LogLevel level);
	
	static void Debug(const std::string& component, const std::string& message);
	
	static void Info(const std::string& component, const std::string& message);
	
	static void Warning(const std::string& component, const std::string& message);
	
	static void Error(const std::string& component, const std::string& message);
	
	static void Fatal(const std::string& component, const std::string& message);
	
	static void Cleanup();
	
private:
	/**
	 * @brief 로그 메시지 출력
	 * 
	 * @param level 로그 레벨
	 * @param component 컴포넌트 이름
	 * @param message 로그 메시지
	 */
	static void Log(LogLevel level, const std::string& component, const std::string& message);
	
	/// 현재 로그 레벨
	static LogLevel _currentLevel;
	
	/// 로그 파일 스트림
	static std::ofstream _logFile;
	
	/// 콘솔 출력 활성화 여부
	static bool _toConsole;
	
	/// 파일 출력 활성화 여부
	static bool _toFile;
};

} // namespace DarkMatterVM
