#include "metrics.hpp"
#include <algorithm>

double WinRate::compute(const std::vector<Trade>& trades) const {
    if (trades.empty()) {
        return 0.0;
    }
    std::size_t wins = 0;
    for (const auto& t : trades) {
        if (t.isWin()) {
            ++wins;
        }
    }
    return (static_cast<double>(wins) / static_cast<double>(trades.size())) * 100.0;
}

double ProfitFactor::compute(const std::vector<Trade>& trades) const {
    if (trades.empty()) {
        return 0.0;
    }

    double grossProfit = 0.0;
    double grossLoss = 0.0;

    for (const auto& t : trades) {
        double p = t.profit();
        if (p > 0.0) {
            grossProfit += p;
        } else if (p < 0.0) {
            grossLoss += (-p);
        }
    }

    // Contract: return -1.0 when there are no losses (represents N/A)
    if (grossLoss == 0.0) {
        return -1.0;
    }

    return grossProfit / grossLoss;
}

double MaxDrawdown::compute(const std::vector<Trade>& trades) const {
    if (trades.empty()) {
        return 0.0;
    }

    double equity = 0.0;
    double peak = 0.0;
    double maxDd = 0.0;

    for (const auto& t : trades) {
        equity += t.profit();
        peak = std::max(peak, equity);
        maxDd = std::max(maxDd, peak - equity);
    }

    return maxDd;
}

double TotalPnL::compute(const std::vector<Trade>& trades) const {
    double total = 0.0;
    for (const auto& t : trades) {
        total += t.profit();
    }
    return total;
}
