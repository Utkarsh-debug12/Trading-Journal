#include "trade.hpp"
#include "errors.hpp"
#include <iomanip>

Trade::Trade(int id, int userId, std::string ticker, Direction dir,
             double entry, double exit, int qty, double fees,
             std::string tradeDate, std::string notes)
    : id_(id),
      userId_(userId),
      quantity_(qty),
      ticker_(std::move(ticker)),
      tradeDate_(std::move(tradeDate)),
      notes_(std::move(notes)),
      direction_(dir),
      entry_(entry),
      exit_(exit),
      fees_(fees) {}

double Trade::profit() const {
    if (direction_ == Direction::Long) {
        return (exit_ - entry_) * quantity_ - fees_;
    } else {
        return (entry_ - exit_) * quantity_ - fees_;
    }
}

bool Trade::operator==(const Trade& o) const {
    return id_ == o.id_ &&
           userId_ == o.userId_ &&
           quantity_ == o.quantity_ &&
           ticker_ == o.ticker_ &&
           tradeDate_ == o.tradeDate_ &&
           notes_ == o.notes_ &&
           direction_ == o.direction_ &&
           entry_ == o.entry_ &&
           exit_ == o.exit_ &&
           fees_ == o.fees_;
}

std::ostream& operator<<(std::ostream& os, const Trade& t) {
    os << "Trade(id=" << t.id_
       << ", userId=" << t.userId_
       << ", ticker=" << t.ticker_
       << ", dir=" << Trade::toString(t.direction_)
       << ", entry=" << t.entry_
       << ", exit=" << t.exit_
       << ", qty=" << t.quantity_
       << ", fees=" << t.fees_
       << ", date=" << t.tradeDate_
       << ", profit=" << t.profit()
       << ")";
    return os;
}

std::string Trade::toString(Direction d) {
    switch (d) {
        case Direction::Long:
            return "Long";
        case Direction::Short:
            return "Short";
    }
    return "Unknown";
}

Trade::Direction Trade::fromString(const std::string& s) {
    if (s == "Long") {
        return Direction::Long;
    }
    if (s == "Short") {
        return Direction::Short;
    }
    throw ValidationError("direction", "Invalid direction '" + s + "': must be 'Long' or 'Short'");
}
