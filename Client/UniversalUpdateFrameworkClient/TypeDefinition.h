#pragma once

#include <string>

enum class UpdateMode {
    eFullPackageUpdate,
    eDifferencePackageUpdate
};

class ReturnWrapper {
public:
    ReturnWrapper(bool status)
            : m_Status(status) {

    }

    ReturnWrapper(bool status, int32_t errorNumber, std::string errorMessage)
            : m_Status(status), m_ErrorNumber(errorNumber), m_ErrorMessage(errorMessage) {

    }

    [[nodiscard]] bool getStatus() const {
        return m_Status;
    }

    [[nodiscard]] int32_t getErrorNumber() const {
        return m_ErrorNumber;
    }

    [[nodiscard]] std::string getErrorMessage() const {
        return m_ErrorMessage;
    }

private:
    bool m_Status{false};
    int32_t m_ErrorNumber{-1};
    std::string m_ErrorMessage{"DefaultErrorMessage"};
};
