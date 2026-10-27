#!/usr/bin/env bash
# ---------------------------------------------------------------------------
# Trading Journal Web Application - Demo & Run Script
# ---------------------------------------------------------------------------

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "================================================================="
echo "        Trading Journal Web Application (C++17 & Crow)          "
echo "================================================================="
echo

# 1. Build the application using CMake
echo "[1/3] Building application..."
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j4
echo "Build succeeded."
echo

# 2. Initialize SQLite database and load sample seed data
echo "[2/3] Initializing and seeding trading_journal.db..."
sqlite3 trading_journal.db < sql/schema.sql
sqlite3 trading_journal.db < sql/seed.sql
TRADE_COUNT=$(sqlite3 trading_journal.db "SELECT count(*) FROM trades WHERE user_id = 1;")
echo "Database ready with $TRADE_COUNT demo trades seeded for user 'demo'."
echo

# 3. Display demo summary and credentials
echo "================================================================="
echo "                    DEMO CREDENTIALS & INFO                      "
echo "================================================================="
echo "  URL:              http://localhost:18080"
echo "  Demo Username:    demo"
echo "  Demo Password:    Password123!"
echo
echo "  Key Features to Explore:"
echo "   - /             : Interactive Dashboard with KPIs & Equity Curve"
echo "   - /trades       : Trade History with Multi-field Search & Filters"
echo "   - /trades/new   : Add New Trade with Strict Field Validation"
echo "   - /export       : RFC 4180 CSV Export (honors active filters)"
echo "   - /api/equity-curve: Live Chart.js JSON Data Endpoint"
echo "================================================================="
echo
echo "Starting Trading Journal Web Server on port 18080..."
echo "Press Ctrl+C to terminate."
echo

exec ./build/trading_journal
