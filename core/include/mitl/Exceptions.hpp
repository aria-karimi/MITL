#pragma once
#include <stdexcept>
#include <string>

namespace mitl {

class MitlError : public std::runtime_error {
public:
    explicit MitlError(const std::string& message) : std::runtime_error(message) {}
};

class AllocationError : public MitlError {
public:
    explicit AllocationError(const std::string& message) : MitlError("Allocation Error: " + message) {}
};

class ConversionError : public MitlError {
public:
    explicit ConversionError(const std::string& message) : MitlError("Conversion Error: " + message) {}
};

} // namespace mitl