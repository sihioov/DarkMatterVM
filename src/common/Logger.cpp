#include "Logger.h"
#include <chrono>
#include <iomanip>
#include <sstream>

namespace DarkMatterVM 
{

// 현재 시간을 문자열로 반환하는 헬퍼 함수 선언
static std::string GetTimestamp();

// 정적 멤버 변수 초기화
LogLevel Logger::_currentLevel = LogLevel::INFO;
std::ofstream Logger::_logFile;
bool Logger::_toConsole = true;
bool Logger::_toFile = false;

void Logger::Initialize(LogLevel level, bool toConsole, const std::string& logFilePath) 
{
	_currentLevel = level;
	_toConsole = toConsole;
	
	// 이미 열린 파일이 있으면 닫음
	if (_logFile.is_open()) 
	{
		_logFile.close();
	}
	
	_toFile = !logFilePath.empty();
	
	if (_toFile) 
	{
		_logFile.open(logFilePath, std::ios::app);
		if (!_logFile.is_open()) 
		{
			std::cerr << "로그 파일을 열 수 없습니다: " << logFilePath << std::endl;
			_toFile = false;
		}
		else 
		{
			// 시작 로그 메시지
			_logFile << "\n=== DarkMatterVM 로그 시작 (" 
					<< GetTimestamp() << ") ===\n" << std::endl;
		}
	}
}

void Logger::SetLevel(LogLevel level) 
{
	_currentLevel = level;
}

void Logger::Debug(const std::string& component, const std::string& message) 
{
	Log(LogLevel::DEBUG, component, message);
}

void Logger::Info(const std::string& component, const std::string& message) 
{
	Log(LogLevel::INFO, component, message);
}

void Logger::Warning(const std::string& component, const std::string& message) 
{
	Log(LogLevel::WARNING, component, message);
}

void Logger::Error(const std::string& component, const std::string& message) 
{
	Log(LogLevel::ERROR, component, message);
}

void Logger::Fatal(const std::string& component, const std::string& message) 
{
	Log(LogLevel::FATAL, component, message);
}

void Logger::Cleanup() 
{
	if (_logFile.is_open()) 
	{
		// 종료 로그 메시지
		_logFile << "\n=== DarkMatterVM 로그 종료 (" 
				<< GetTimestamp() << ") ===\n" << std::endl;
		
		_logFile.close();
	}
}

void Logger::Log(LogLevel level, const std::string& component, const std::string& message) 
{
	// 설정된 레벨보다 낮은 로그는 무시
	if (level < _currentLevel) 
	{
		return;
	}
	
	// 타임스탬프 생성
	std::string timestamp = GetTimestamp();
	
	// 로그 레벨 문자열
	std::string levelStr;
	switch (level) 
	{
		case LogLevel::DEBUG:
			levelStr = "DEBUG";
			break;
		case LogLevel::INFO:
			levelStr = "INFO ";
			break;
		case LogLevel::WARNING:
			levelStr = "WARN ";
			break;
		case LogLevel::ERROR:
			levelStr = "ERROR";
			break;
		case LogLevel::FATAL:
			levelStr = "FATAL";
			break;
	}
	
	// 로그 메시지 포맷팅
	std::ostringstream logStream;
	logStream << timestamp << " [" << levelStr << "] [" << component << "] " << message;
	std::string logMessage = logStream.str();
	
	// 콘솔에 출력
	if (_toConsole) 
	{
		// 에러 및 치명적 오류는 stderr에, 나머지는 stdout에 출력
		std::ostream& out = (level >= LogLevel::ERROR) ? std::cerr : std::cout;
		out << logMessage << std::endl;
	}
	
	// 파일에 출력
	if (_toFile && _logFile.is_open()) 
	{
		_logFile << logMessage << std::endl;
		_logFile.flush(); // 즉시 파일에 기록
	}
}

// 현재 시간을 문자열로 반환하는 헬퍼 메서드 구현
static std::string GetTimestamp() 
{
	auto now = std::chrono::system_clock::now();
	auto time = std::chrono::system_clock::to_time_t(now);
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
		now.time_since_epoch()) % 1000;
	
	std::tm localTime;
	
#ifdef _WIN32
	localtime_s(&localTime, &time);
#else
	localtime_r(&time, &localTime);
#endif
	
	std::ostringstream oss;
	oss << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S")
		<< '.' << std::setfill('0') << std::setw(3) << ms.count();
	
	return oss.str();
}

} // namespace DarkMatterVM
