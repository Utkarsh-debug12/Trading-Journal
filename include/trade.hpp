#pragma once
#include <optional>
#include <ostream>
#include <string>

// ---------------------------------------------------------------------------
// Trade – represents a single completed trade.
// ---------------------------------------------------------------------------

class Trade {
public:
    /// Direction of the position.
    enum class Direction { Long, Short };

    /// Construct a fully-populated trade record.
    Trade(int id, int userId, std::string ticker, Direction dir,
          double entry, double exit, int qty, double fees,
          std::string tradeDate, std::string notes = "");

    // -- const accessors (no setters: trades are immutable once built) ------

    int id() const {return id_;}
    int userId() const {return userId_;}
    const std::string& ticker() const {return ticker_;}
    Direction direction() const {return direction_;}
    double entry() const {return entry_;}
    double exit() const {return exit_;}
    int quantity() const {return quantity_;}
    double fees() const {return fees_;}
    const std::string& tradeDate() const {return tradeDate_;}
    const std::string& notes() const {return notes_;}

    // -- computed properties ------------------------------------------------

    /// Net profit of the trade.
    ///   Long:  (exit - entry) × quantity - fees
    ///   Short: (entry - exit) × quantity - fees
    double profit() const;

    /// A trade is a win when its profit is strictly positive.
    bool isWin() const {return profit() > 0;}

    // -- operator overloading -----------------------------------------------

    /// Sort by trade date.
    bool operator<(const Trade& o) const {return tradeDate_ < o.tradeDate_;}

    /// Two trades are equal when all fields match.
    bool operator==(const Trade& o) const;

    /// Stream insertion for logging and GoogleTest output.
    friend std::ostream& operator<<(std::ostream& os, const Trade& t);

    // -- static helpers for the Direction enum ------------------------------

    /// Returns "Long" or "Short".
    static std::string toString(Direction d);

    /// Parses "Long" or "Short" (case-sensitive); throws ValidationError
    /// if the string is invalid.
    static Direction fromString(const std::string& s);

private:
    int id_;
    int userId_;
    int quantity_;
    std::string ticker_;
    std::string tradeDate_;
    std::string notes_;
    Direction direction_;
    double entry_;
    double exit_;
    double fees_;
};

// ---------------------------------------------------------------------------
// TradeFilter 
// ---------------------------------------------------------------------------

struct TradeFilter {
    std::optional<std::string> ticker;     ///< filter by ticker symbol
    std::optional<bool>        winOnly;    ///< true = wins, false = losses, empty = all
};
