#pragma once
#include "trade.hpp"
#include "metrics.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

// ---------------------------------------------------------------------------
// Generic sumProfit template with lambda predicate support (Owner: Member A)
// ---------------------------------------------------------------------------

template <typename Pred>
double sumProfit(const std::vector<Trade>& trades, Pred keep) {
    double total = 0.0;
    for (const auto& t : trades) {
        if (keep(t)) {
            total += t.profit();
        }
    }
    return total;
}

// ---------------------------------------------------------------------------
// Journal – owns trades and metrics via smart pointers (Owner: Member A).
// Move-only class adhering to Rule of Five.
// ---------------------------------------------------------------------------

class Journal {
public:
    explicit Journal(std::vector<Trade> trades);

    // Rule of Five: move-only (owns unique_ptrs to IMetric)
    Journal(const Journal&) = delete;
    Journal& operator=(const Journal&) = delete;
    Journal(Journal&&) = default;
    Journal& operator=(Journal&&) = default;

    // Add metric polymorphically
    void addMetric(std::unique_ptr<IMetric> m);

    // Polymorphic computation of all registered metrics
    std::vector<std::pair<std::string, double>> computeAll() const;

    // Direct accessors & analytical calculations
    const std::vector<Trade>& trades() const { return trades_; }
    double totalPnL() const;
    double avgWin() const;
    double avgLoss() const;
    double winRate() const;
    double profitFactor() const;
    double maxDrawdown() const;

    // Equity curve: cumulative profit after each trade in chronological order
    std::vector<double> equityCurve() const;

    // Filter trades by win status (true = wins, false = losses)
    std::vector<Trade> filterByResult(bool wins) const;

private:
    std::vector<Trade> trades_;
    std::vector<std::unique_ptr<IMetric>> metrics_;
};
