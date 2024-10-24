#pragma once

#include <string>
#include <utility>

#include "UpdateCore/ErrorCode.hpp"

class ReturnWrapper {
public:
	ReturnWrapper(const bool status) : m_Status(status) {}

	ReturnWrapper(const bool status, const ErrorCode errorCode,
				  std::string errorMessage)
		: m_Status(status),
		  m_ErrorCode(errorCode),
		  m_ErrorMessage(std::move(errorMessage)) {}

	[[nodiscard]] bool getStatus() const { return m_Status; }

	[[nodiscard]] ErrorCode getErrorNumber() const { return m_ErrorCode; }

	[[nodiscard]] std::string getErrorMessage() const { return m_ErrorMessage; }

private:
	bool m_Status{false};
	ErrorCode m_ErrorCode{ErrorCode::NoError};
	std::string m_ErrorMessage{"DefaultErrorMessage"};
};

#ifdef WIN32
constexpr const char* hpatchzExecuable = "hpatchz.exe";
#elif defined __linux
constexpr const char* hpatchzExecuable = "./hpatchz";
#endif
