#pragma once

#include <string>
#include <utility>
#include "ErrorCode.hpp"

enum class UpdateMode {
    Unknown,
    FullPackageUpdate,
    DifferencePackageUpdate,
    MultiVersionDifferencePackageUpdate,
    DifferenceUpdate
};

class ReturnWrapper {
public:
    ReturnWrapper(bool status)
        : m_Status(status) {
    }

    ReturnWrapper(bool status, ErrorCode errorCode, std::string errorMessage)
        : m_Status(status), m_ErrorCode(errorCode), m_ErrorMessage(std::move(errorMessage)) {
    }

    [[nodiscard]] bool getStatus() const {
        return m_Status;
    }

    [[nodiscard]] ErrorCode getErrorNumber() const {
        return m_ErrorCode;
    }

    [[nodiscard]] std::string getErrorMessage() const {
        return m_ErrorMessage;
    }

private:
    bool m_Status{false};
    ErrorCode m_ErrorCode{ErrorCode::NoError};
    std::string m_ErrorMessage{"DefaultErrorMessage"};
};
