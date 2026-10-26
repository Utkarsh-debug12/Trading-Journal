#include "validators.hpp"
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <stdexcept>

namespace {
    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, (last - first + 1));
    }
}

bool Validators::isValidDateFormat(const std::string& s) {
    if (s.length() != 10) return false;
    if (s[4] != '-' || s[7] != '-') return false;

    for (size_t i = 0; i < 10; ++i) {
        if (i == 4 || i == 7) continue;
        if (!std::isdigit(static_cast<unsigned char>(s[i]))) return false;
    }

    int year = std::stoi(s.substr(0, 4));
    int month = std::stoi(s.substr(5, 2));
    int day = std::stoi(s.substr(8, 2));

    if (year < 1900 || year > 2100) return false;
    if (month < 1 || month > 12) return false;

    static const int daysInMonth[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    int maxDays = daysInMonth[month];
    if (month == 2) {
        bool isLeap = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
        if (isLeap) maxDays = 29;
    }
    if (day < 1 || day > maxDays) return false;

    return true;
}

Trade Validators::validateTradeForm(const FormData& form, int userId, int tradeId) {
    // 1. Ticker
    std::string ticker = trim(form.ticker);
    if (ticker.empty()) {
        throw ValidationError("ticker", "Ticker symbol cannot be empty");
    }
    for (char& c : ticker) {
        c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    }
    for (char c : ticker) {
        if (!std::isalnum(static_cast<unsigned char>(c))) {
            throw ValidationError("ticker", "Ticker symbol must be alphanumeric");
        }
    }

    // 2. Direction
    Trade::Direction dir;
    try {
        dir = Trade::fromString(trim(form.direction));
    } catch (const ValidationError&) {
        throw ValidationError("direction", "Direction must be 'Long' or 'Short'");
    }

    // 3. Entry Price
    double entry = 0.0;
    try {
        size_t pos = 0;
        std::string s = trim(form.entryPrice);
        entry = std::stod(s, &pos);
        if (pos != s.length() || entry <= 0.0) {
            throw std::invalid_argument("out of range");
        }
    } catch (...) {
        throw ValidationError("entryPrice", "Entry price must be a positive number");
    }

    // 4. Exit Price
    double exit = 0.0;
    try {
        size_t pos = 0;
        std::string s = trim(form.exitPrice);
        exit = std::stod(s, &pos);
        if (pos != s.length() || exit <= 0.0) {
            throw std::invalid_argument("out of range");
        }
    } catch (...) {
        throw ValidationError("exitPrice", "Exit price must be a positive number");
    }

    // 5. Quantity
    int quantity = 0;
    try {
        size_t pos = 0;
        std::string s = trim(form.quantity);
        quantity = std::stoi(s, &pos);
        if (pos != s.length() || quantity <= 0) {
            throw std::invalid_argument("out of range");
        }
    } catch (...) {
        throw ValidationError("quantity", "Quantity must be a positive integer");
    }

    // 6. Fees (optional, default 0.0)
    double fees = 0.0;
    std::string feesStr = trim(form.fees);
    if (!feesStr.empty()) {
        try {
            size_t pos = 0;
            fees = std::stod(feesStr, &pos);
            if (pos != feesStr.length() || fees < 0.0) {
                throw std::invalid_argument("out of range");
            }
        } catch (...) {
            throw ValidationError("fees", "Fees must be a non-negative number");
        }
    }

    // 7. Trade Date
    std::string dateStr = trim(form.tradeDate);
    if (!isValidDateFormat(dateStr)) {
        throw ValidationError("tradeDate", "Trade date must follow YYYY-MM-DD format");
    }

    std::string notes = trim(form.notes);

    return Trade(tradeId, userId, std::move(ticker), dir, entry, exit, quantity, fees, std::move(dateStr), std::move(notes));
}
