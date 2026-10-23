#pragma once
#include "trade.hpp"
#include "errors.hpp"
#include <string>

// ---------------------------------------------------------------------------
// FormData – represents raw string inputs from web form (Owner: Member A).
// ---------------------------------------------------------------------------

struct FormData {
    std::string ticker;
    std::string direction;
    std::string entryPrice;
    std::string exitPrice;
    std::string quantity;
    std::string fees;
    std::string tradeDate;
    std::string notes;
};

// ---------------------------------------------------------------------------
// Validators – server-side validation rules for trade forms (Owner: Member A).
// Throws ValidationError with the offending field name on failure.
// ---------------------------------------------------------------------------

class Validators {
public:
    static Trade validateTradeForm(const FormData& form, int userId, int tradeId = 0);
    static bool isValidDateFormat(const std::string& dateStr);
};
