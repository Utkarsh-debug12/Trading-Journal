#include "in_memory_trade_repository.hpp"

int InMemoryTradeRepository::add(const Trade& trade) {
    std::lock_guard<std::mutex> lock(mutex_);
    int id = nextId_++;
    Trade stored(id, trade.userId(), trade.ticker(), trade.direction(),
                 trade.entry(), trade.exit(), trade.quantity(), trade.fees(),
                 trade.tradeDate(), trade.notes());
    trades_.emplace(id, std::move(stored));
    return id;
}

bool InMemoryTradeRepository::update(const Trade& trade) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = trades_.find(trade.id());
    if (it != trades_.end() && it->second.userId() == trade.userId()) {
        it->second = trade;
        return true;
    }
    return false;
}

bool InMemoryTradeRepository::remove(int userId, int tradeId) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = trades_.find(tradeId);
    if (it != trades_.end() && it->second.userId() == userId) {
        trades_.erase(it);
        return true;
    }
    return false;
}

std::optional<Trade> InMemoryTradeRepository::findById(int userId, int id) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = trades_.find(id);
    if (it != trades_.end() && it->second.userId() == userId) {
        return it->second;
    }
    return std::nullopt;
}

#include <algorithm>
#include <cctype>

namespace {
    bool iequals(const std::string& a, const std::string& b) {
        if (a.size() != b.size()) return false;
        for (size_t i = 0; i < a.size(); ++i) {
            if (std::tolower(static_cast<unsigned char>(a[i])) != std::tolower(static_cast<unsigned char>(b[i]))) {
                return false;
            }
        }
        return true;
    }
}

std::vector<Trade> InMemoryTradeRepository::search(int userId, const TradeFilter& filter) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<Trade> out;

    for (const auto& [id, trade] : trades_) {
        if (trade.userId() != userId) {
            continue;
        }

        if (filter.ticker.has_value() && !filter.ticker->empty() && !iequals(trade.ticker(), *filter.ticker)) {
            continue;
        }

        if (filter.direction.has_value() && trade.direction() != *filter.direction) {
            continue;
        }

        if (filter.startDate.has_value() && !filter.startDate->empty() && trade.tradeDate() < *filter.startDate) {
            continue;
        }

        if (filter.endDate.has_value() && !filter.endDate->empty() && trade.tradeDate() > *filter.endDate) {
            continue;
        }

        if (filter.winOnly.has_value()) {
            if (*filter.winOnly && !trade.isWin()) {
                continue;
            }
            if (!*filter.winOnly && trade.isWin()) {
                continue;
            }
        }

        out.push_back(trade);
    }

    std::sort(out.begin(), out.end());
    return out;
}

void InMemoryTradeRepository::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    trades_.clear();
    nextId_ = 1;
}

size_t InMemoryTradeRepository::size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return trades_.size();
}
