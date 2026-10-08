#pragma once
#include <stdexcept>
#include <string>

// base error class for our app
class AppError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

// thrown when form input is bad — also stores which field failed
class ValidationError : public AppError {
public:
    ValidationError(std::string field, const std::string& msg)
        : AppError(field + ": " + msg), field_(std::move(field)) {}

    const std::string& field() const noexcept { return field_; }

private:
    std::string field_;
};

// thrown when something goes wrong with the database
class DatabaseError : public AppError {
public:
    using AppError::AppError;
};
