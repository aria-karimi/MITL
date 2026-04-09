#pragma once
#include <stdexcept>
#include <string>

namespace mitl {

/** @brief Base MITL exception type. */
class MitlError : public std::runtime_error {
public:
    /** @brief Constructs exception with a message. */
    explicit MitlError(const std::string& message) : std::runtime_error(message) {}
};

/** @brief Exception for allocation failures and memory-related errors. */
class AllocationError : public MitlError {
public:
    /** @brief Constructs allocation exception with contextual text. */
    explicit AllocationError(const std::string& message) : MitlError("Allocation Error: " + message) {}
};

/** @brief Exception for conversion failures between numeric formats. */
class ConversionError : public MitlError {
public:
    /** @brief Constructs conversion exception with contextual text. */
    explicit ConversionError(const std::string& message) : MitlError("Conversion Error: " + message) {}
};

} // namespace mitl