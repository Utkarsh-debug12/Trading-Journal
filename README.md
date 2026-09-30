# Trading Journal: A C++ Web Application (Mini Project)

## 📖 Project Overview
The Trading Journal is a small web application that lets a trader **log trades, review their history, and see performance analytics**. The whole backend is written in **C++17**: web server, routing, authentication, database access, and analytics. The browser side is a thin layer of HTML, CSS, and a little JavaScript (Chart.js) for drawing charts.

This is a **mini project**. It is scoped to be finished by a team of 1–3 students in about **4 weeks**, and it focuses on showing good C++ and OOP practice rather than building a full commercial product.

**Language split:** ~85% C++, ~15% HTML/CSS/JS.

## 🎯 Objectives
1. Apply object-oriented C++ (classes, inheritance, polymorphism, RAII, STL) to a real problem.
2. Build a working web application in C++ with a database.
3. Implement financial analytics (P&L, win rate, drawdown, etc.) with unit-tested, correct math.
4. Practise secure coding basics: password hashing, prepared statements, per-user data isolation.

## ✨ Features

### Included in this project
| # | Feature | Description |
|---|---|---|
| 1 | **User Accounts** | Register, log in, log out. Passwords are hashed; each user sees only their own trades. |
| 2 | **Trade Management** | Add, view, edit, and delete trades (ticker, direction, entry/exit price, quantity, fees, date, notes). |
| 3 | **Tags** | Tag each trade with a strategy (e.g. "Breakout") and an emotion (e.g. "FOMO", "Disciplined"). |
| 4 | **Search & Filter** | Filter the trade list by ticker, date range, direction, win/loss, strategy, or emotion. |
| 5 | **Analytics Dashboard** | Total P&L, Win Rate, Profit Factor, Expectancy, Average Win/Loss, Maximum Drawdown, longest win/loss streak. |
| 6 | **Charts** | Equity curve and P&L per trade using Chart.js, fed by a JSON endpoint. |
| 7 | **Performance by Emotion/Strategy** | Table showing average P&L for each tag, to link mindset with results. |
| 8 | **Position-Size Calculator** | Enter account size, risk %, entry and stop-loss to get the recommended quantity and risk/reward ratio. |
| 9 | **CSV Export / Import** | Export trades to CSV and import a CSV with duplicate detection (ticker + timestamp + quantity). |

### Out of scope (listed under Future Scope)
Live market data, price alerts, notifications, 2FA, payments, backtesting, PDF reports, multi-currency, options/futures, PWA, REST API tokens, admin panel, and backups. These are good extensions but too large for a mini project.

## 🛠️ Technology Stack

| Layer | Technology | Why |
|---|---|---|
| **Language** | C++17 | Course requirement; modern features (`std::optional`, structured bindings) |
| **Build** | CMake | Standard, cross-platform |
| **Web framework** | [Crow](https://github.com/CrowCpp/Crow) (single header `crow_all.h`) | Flask-like routing, built-in sessions/cookies and Mustache templates, minimal setup |
| **Database** | SQLite 3 (C API wrapped in your own RAII `Database` class) | Zero server setup, single file; the wrapper demonstrates RAII |
| **JSON** | nlohmann/json | Clean JSON for chart endpoints |
| **Password hashing** | libsodium (`crypto_pwhash_str`, Argon2id) | Secure and only two function calls |
| **Templates** | Crow Mustache (`.html` files) | Server-side rendering |
| **Frontend** | Bootstrap 5 + Chart.js (loaded from CDN) | No build tools needed |
| **Testing** | GoogleTest (or plain `assert` if you prefer) | Verify analytics math |

**Dependencies to install: only 4** (Crow header, SQLite, libsodium, nlohmann/json) plus Asio, which Crow needs.

## 🏗️ System Architecture
A simple layered design:

```
 Browser (HTML + Bootstrap + Chart.js)
        │  HTTP
        ▼
 ┌────────────────────────────────────────┐
 │ Web Layer (Crow)                        │  routes, sessions, templates
 ├────────────────────────────────────────┤
 │ Domain Layer                            │  Trade, Journal (analytics),
 │                                         │  PositionCalculator, CsvService
 ├────────────────────────────────────────┤
 │ Data Layer                              │  Database (RAII), UserRepository,
 │                                         │  TradeRepository
 └────────────────────────────────────────┘
        │
   trading_journal.db  (SQLite file)
```

The **Domain Layer knows nothing about Crow or SQLite**, so it can be unit-tested on its own.

### Key Classes
| Class | Responsibility |
|---|---|
| `Trade` | One trade. `const` methods: `profit()`, `isWin()`, `riskRewardRatio()`. |
| `Journal` | Holds `std::vector<Trade>`. Computes win rate, profit factor, expectancy, max drawdown, equity curve, streaks. |
| `PositionCalculator` | Position size and risk/reward from account size, risk %, entry, stop. |
| `CsvService` | Reads/writes trades as CSV (file I/O, string parsing, duplicate check). |
| `Database` | RAII wrapper over `sqlite3*` (opens in constructor, closes in destructor); prepared-statement helper. |
| `UserRepository` | `create()`, `findByUsername()`. |
| `TradeRepository` | `add()`, `update()`, `remove()`, `findById()`, `search(filter)`, all scoped by `user_id`. |
| `AuthService` | Hash/verify passwords with libsodium; session helpers. |

### Database Schema (2 tables)

**`users`**
| Column | Type |
|---|---|
| `id` | INTEGER PRIMARY KEY |
| `username` | TEXT UNIQUE NOT NULL |
| `password_hash` | TEXT NOT NULL |
| `created_at` | DATETIME DEFAULT CURRENT_TIMESTAMP |

**`trades`**
| Column | Type |
|---|---|
| `id` | INTEGER PRIMARY KEY |
| `user_id` | INTEGER NOT NULL, FK → users(id) |
| `ticker` | TEXT NOT NULL |
| `direction` | TEXT NOT NULL (`Long` / `Short`) |
| `entry_price` | REAL NOT NULL |
| `exit_price` | REAL NOT NULL |
| `quantity` | INTEGER NOT NULL |
| `fees` | REAL DEFAULT 0 |
| `stop_loss` | REAL (nullable) |
| `strategy_tag` | TEXT |
| `emotion_tag` | TEXT |
| `trade_date` | DATETIME NOT NULL |
| `notes` | TEXT |

### Analytics Formulas
* **Profit (Long):** `(exit − entry) × quantity − fees`
* **Profit (Short):** `(entry − exit) × quantity − fees`
* **Win Rate:** `winning trades ÷ total trades × 100`
* **Profit Factor:** `gross profit ÷ gross loss` (report "∞" if there are no losses)
* **Expectancy:** `(win rate × avg win) − (loss rate × avg loss)`
* **Equity Curve:** running cumulative sum of profits ordered by date
* **Maximum Drawdown:** largest drop from a running peak of the equity curve to a later low

### Web Routes
| Method | Route | Purpose |
|---|---|---|
| GET/POST | `/register` | Create account |
| GET/POST | `/login` | Log in |
| GET | `/logout` | Log out |
| GET | `/` | Dashboard (metrics + charts) |
| GET | `/trades` | List with filters |
| GET/POST | `/trades/new` | Add trade |
| GET/POST | `/trades/<id>/edit` | Edit trade |
| POST | `/trades/<id>/delete` | Delete trade |
| GET | `/analytics/by-tag` | Emotion/strategy breakdown |
| GET | `/calculator` | Position-size calculator |
| GET | `/export` | Download CSV |
| POST | `/import` | Upload CSV |
| GET | `/api/equity-curve` | JSON for Chart.js |
| GET | `/api/pnl-per-trade` | JSON for Chart.js |

## 📂 Project File Structure

```
trading_journal/
├── CMakeLists.txt
├── README.md
├── third_party/
│   └── crow/crow_all.h            # single-header Crow
│
├── include/
│   ├── trade.hpp
│   ├── journal.hpp
│   ├── position_calculator.hpp
│   ├── csv_service.hpp
│   ├── database.hpp
│   ├── user_repository.hpp
│   ├── trade_repository.hpp
│   └── auth_service.hpp
│
├── src/
│   ├── main.cpp                    # sets up Crow app, registers routes, starts server
│   ├── trade.cpp
│   ├── journal.cpp
│   ├── position_calculator.cpp
│   ├── csv_service.cpp
│   ├── database.cpp
│   ├── user_repository.cpp
│   ├── trade_repository.cpp
│   ├── auth_service.cpp
│   └── routes.cpp                  # all route handlers
│
├── templates/                      # Mustache HTML
│   ├── base.html   login.html   register.html
│   ├── dashboard.html   trades.html   trade_form.html
│   ├── by_tag.html   calculator.html
│
├── static/
│   ├── css/style.css
│   └── js/charts.js
│
├── sql/
│   └── schema.sql                  # CREATE TABLE statements (run at startup if DB is empty)
│
├── sample_data/
│   └── sample_trades.csv           # demo data for the viva
│
└── tests/
    ├── CMakeLists.txt
    ├── test_trade.cpp
    ├── test_journal.cpp
    └── test_position_calculator.cpp
```

## 🚀 Installation and Setup

### Option A: Ubuntu / Debian / WSL (easiest)
```bash
sudo apt update
sudo apt install build-essential cmake libsqlite3-dev libsodium-dev \
                 libasio-dev nlohmann-json3-dev libgtest-dev
```
Download `crow_all.h` from the [Crow releases page](https://github.com/CrowCpp/Crow/releases) and place it in `third_party/crow/`.

### Option B: Windows / macOS with vcpkg
```bash
vcpkg install sqlite3 libsodium asio nlohmann-json gtest
```
Then pass `-DCMAKE_TOOLCHAIN_FILE=<vcpkg-root>/scripts/buildsystems/vcpkg.cmake` to CMake.

### Build
```bash
git clone <your-repo-url> trading_journal
cd trading_journal
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

### Run
```bash
./build/trading_journal
```
Open **http://localhost:18080**. The SQLite file `trading_journal.db` is created automatically on first run using `sql/schema.sql`. Register an account, then import `sample_data/sample_trades.csv` to see the dashboard populated.

### Run Tests
```bash
cd build && ctest --output-on-failure
```

### Minimal `CMakeLists.txt`
```cmake
cmake_minimum_required(VERSION 3.16)
project(trading_journal LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SQLite3 REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(Threads REQUIRED)
find_library(SODIUM_LIB sodium REQUIRED)

# Domain + data code as a library, so tests can link to it
add_library(tj_core
    src/trade.cpp src/journal.cpp src/position_calculator.cpp
    src/csv_service.cpp src/database.cpp src/user_repository.cpp
    src/trade_repository.cpp src/auth_service.cpp)
target_include_directories(tj_core PUBLIC include third_party/crow)
target_link_libraries(tj_core PUBLIC SQLite::SQLite3 nlohmann_json::nlohmann_json
                      ${SODIUM_LIB} Threads::Threads)

add_executable(trading_journal src/main.cpp src/routes.cpp)
target_link_libraries(trading_journal PRIVATE tj_core)

enable_testing()
add_subdirectory(tests)
```

## 💻 Code Samples

**`trade.hpp`**
```cpp
#pragma once
#include <string>

class Trade {
public:
    enum class Direction { Long, Short };

    Trade(int id, std::string ticker, Direction dir, double entry,
          double exit, int qty, double fees);

    double profit() const;                 // const-correct: does not modify the object
    bool   isWin() const { return profit() > 0; }
    const std::string& ticker() const { return ticker_; }

private:
    int id_;
    std::string ticker_;
    Direction direction_;
    double entry_, exit_, fees_;
    int quantity_;
};
```

**`journal.cpp` (drawdown)**
```cpp
double Journal::maxDrawdown() const {
    double equity = 0.0, peak = 0.0, maxDd = 0.0;
    for (const auto& t : trades_) {          // trades_ sorted by date
        equity += t.profit();
        peak = std::max(peak, equity);
        maxDd = std::max(maxDd, peak - equity);
    }
    return maxDd;
}
```

**A Crow route**
```cpp
CROW_ROUTE(app, "/api/equity-curve")([&](const crow::request& req) {
    int userId = currentUserId(req);             // from session
    auto trades = tradeRepo.search(userId, {});  // empty filter = all trades
    Journal journal(trades);

    nlohmann::json out = journal.equityCurve();  // vector<double> -> JSON array
    return crow::response(200, "application/json", out.dump());
});
```

**A unit test**
```cpp
TEST(JournalTest, ProfitFactorAndDrawdown) {
    Journal j({ Trade(1,"AAPL",Trade::Direction::Long, 100,110,10,0),   // +100
                Trade(2,"TSLA",Trade::Direction::Long, 200,190,5, 0),   // -50
                Trade(3,"INFY",Trade::Direction::Short,50, 40, 10,0) });// +100
    EXPECT_DOUBLE_EQ(j.profitFactor(), 4.0);   // 200 / 50
    EXPECT_DOUBLE_EQ(j.maxDrawdown(), 50.0);
}
```

## 🧪 Testing Plan
* **Unit tests (GoogleTest):** `Trade::profit()` for long/short with and without fees; `Journal` metrics on hand-calculated datasets; edge cases (no trades, no losses, all losses); `PositionCalculator`.
* **Manual test checklist:**
  - Register with a duplicate username → error shown
  - Wrong password → login fails
  - User A cannot open User B's trade by changing the URL ID
  - Add / edit / delete a trade updates the dashboard
  - Filters return the correct rows
  - Importing the same CSV twice does not create duplicates
  - Exported CSV re-imports cleanly
* **Memory checks:** build once with `-fsanitize=address,undefined` and click through every page; optionally run Valgrind.

## 🔒 Security Basics Implemented
* Passwords hashed with **Argon2id** (libsodium); plaintext is never stored.
* **Prepared statements** everywhere, so no SQL injection.
* Every trade query includes `WHERE user_id = ?`, so users are isolated from each other.
* Session cookies are set HTTP-only; protected routes redirect to `/login` when there is no session.
* All user-provided text is HTML-escaped by Mustache (`{{ }}`) to prevent XSS.

## 🧠 C++ / OOP Concepts Demonstrated (for report & viva)
| Concept | Where |
|---|---|
| Classes & encapsulation | `Trade`, `Journal`, repositories |
| `const` correctness | `Trade::profit() const`, `Journal` getters |
| RAII | `Database` (opens/closes `sqlite3*`), prepared-statement wrapper |
| Smart pointers | `std::unique_ptr<Database>` shared via services; no raw `new`/`delete` |
| STL containers & algorithms | `std::vector<Trade>`, `std::sort` by date, `std::accumulate`, `std::count_if`, `std::unordered_map` for tag grouping |
| Enums / `enum class` | `Trade::Direction` |
| `std::optional` | `findById()` and `findByUsername()` return `optional` |
| Exception handling | DB errors and CSV parse errors thrown and caught in routes |
| File I/O & string parsing | `CsvService` |
| Lambdas | Filtering and sorting predicates |
| Templates *(optional bonus)* | Generic `groupBy()` helper for tag analytics |
| Inheritance & polymorphism *(optional bonus)* | `Repository` base interface → `SqliteTradeRepository` |

## 📅 4-Week Timeline
| Week | Milestone |
|---|---|
| **1** | Environment + CMake working; `Trade` and `Journal` classes with unit tests; Crow "Hello World" |
| **2** | `Database` RAII wrapper, schema, repositories; trade CRUD pages |
| **3** | Registration/login/sessions; per-user isolation; filters; dashboard metrics; Chart.js charts |
| **4** | Position calculator, tag analytics, CSV import/export, polish UI, testing, report, demo prep |

**Suggested split for a team of 3:**
* *Member 1:* Domain layer (`Trade`, `Journal`, `PositionCalculator`) and tests
* *Member 2:* Data layer (`Database`, repositories, CSV service) and auth
* *Member 3:* Web layer (routes, templates, charts, styling)

## ⚠️ Limitations
* Uses SQLite, so it is suited to a few users, not heavy concurrent load.
* Runs over plain HTTP on localhost; production would need HTTPS and CSRF tokens.
* No live market data; all prices are entered manually.
* Single currency, equities-style P&L only.

## 🔮 Future Scope
* Live market data, price alerts, and email/push notifications
* Advanced statistics (Sharpe, Sortino, R-multiples), calendar P&L heatmap
* Pre-trade checklists, trade replay timeline, chart annotations
* Options/futures support, multi-currency, tax-lot accounting (FIFO/LIFO)
* Strategy backtesting and correlation/exposure analysis
* PDF reports, dark mode, PWA support, i18n
* Token-based REST API, webhooks, 2FA, admin panel, encrypted backups
* Migration to PostgreSQL, Docker + CI/CD, migration to the Drogon framework
* AI-assisted trade review

## 📄 Submission Checklist
- [ ] Source code with clear commit history
- [ ] `README.md` (this file) and a short project report
- [ ] `sample_data/sample_trades.csv` for the demo
- [ ] Screenshots of dashboard, trade list, and analytics
- [ ] Unit tests passing (`ctest`)
- [ ] Demo script: register → import sample CSV → view dashboard → filter → export

## 📜 License
MIT
