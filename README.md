# Trading Journal: A C++ Web Application (Mini Project)

## 📖 Project Overview
The Trading Journal is a small web application that lets a trader **log trades, review their history, and see performance analytics**. The whole backend is written in **C++17**: web server, routing, authentication, database access, and analytics. The browser side is a thin layer of HTML, CSS, and a little JavaScript (Chart.js) for drawing charts.

This is a **mini project**. It is scoped to be finished by a team of 3 students in about **4 weeks** (see the Team Structure section for the module split), and it focuses on showing good C++ and OOP practice rather than building a full commercial product.

**Language split:** ~85% C++, ~15% HTML/CSS/JS.

**Team balance:** each member owns roughly **950–1000 lines of C++**, a distinct set of core C++ concepts, their own tests, and one small web-facing slice, so everyone touches the full stack and no one is left with only HTML or only SQL.

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
 │ Service / Domain Layer                  │  Trade, Journal + IMetric strategies,
 │                                         │  PositionCalculator, Validators,
 │                                         │  AuthService, CsvService
 ├────────────────────────────────────────┤
 │ Data Layer                              │  Database (RAII), UserRepository,
 │                                         │  ITradeRepository (+ 2 impls)
 └────────────────────────────────────────┘
        │
   trading_journal.db  (SQLite file)
```

The **Domain classes (`Trade`, `Journal`, `PositionCalculator`, validators) know nothing about Crow or SQLite**, so they can be unit-tested on their own.

### Key Classes
| Class | Owner | Responsibility |
|---|---|---|
| `Trade` | A | One trade. `const` methods: `profit()`, `isWin()`, `riskRewardRatio()`. |
| `IMetric` | A | Abstract strategy interface for one analytics metric: `name()`, `compute(const std::vector<Trade>&) const`. |
| `WinRate`, `ProfitFactor`, `Expectancy`, `MaxDrawdown` | A | Concrete metrics derived from `IMetric`; each holds one formula from the Analytics Formulas section. |
| `Journal` | A | Holds `std::vector<Trade>` and a `std::vector<std::unique_ptr<IMetric>>`. Runs every metric polymorphically; also computes equity curve and streaks. Move-only because it owns `unique_ptr`s. |
| `TagAnalytics` | A | Template `groupBy(trades, keyFn)` for average P&L and count per tag. |
| `PositionCalculator` | A | Position size and risk/reward from account size, risk %, entry, stop. |
| `Validators` | A | Server-side input rules (prices > 0, quantity > 0, valid date, direction). Throw `ValidationError`. |
| `Database` | B | RAII wrapper over `sqlite3*` (opens in constructor, closes in destructor); non-copyable, movable. |
| `Statement` | B | RAII wrapper over `sqlite3_stmt*`; bind, step, typed column getters. |
| `UserRepository` | B | `create()`, `findByUsername()`. |
| `ITradeRepository` | B | Abstract interface for trade storage. |
| `SqliteTradeRepository` | B | Real implementation: `add()`, `update()`, `remove()`, `findById()`, `search(filter)`, `existsDuplicate()`, all scoped by `user_id`. |
| `InMemoryTradeRepository` | B | `std::map`-based implementation of the same interface for early development and tests. |
| `AuthService` | C | Hash/verify passwords with libsodium; username/password rules. |
| `CsvService` | C | Reads/writes trades as CSV (file I/O, string parsing, row-level error report). |
| `AuthGuard` (middleware) | C | Redirects to `/login` when there is no session; `currentUserId(req)` helper. |

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
| Method | Route | Purpose | Route file (owner) |
|---|---|---|---|
| GET/POST | `/register` | Create account | `routes_auth.cpp` (C) |
| GET/POST | `/login` | Log in | `routes_auth.cpp` (C) |
| GET | `/logout` | Log out | `routes_auth.cpp` (C) |
| GET | `/trades` | List with filters | `routes_trades.cpp` (C) |
| GET/POST | `/trades/new` | Add trade | `routes_trades.cpp` (C) |
| GET/POST | `/trades/<id>/edit` | Edit trade | `routes_trades.cpp` (C) |
| POST | `/trades/<id>/delete` | Delete trade | `routes_trades.cpp` (C) |
| GET | `/export` | Download CSV | `routes_io.cpp` (C) |
| POST | `/import` | Upload CSV | `routes_io.cpp` (C) |
| GET | `/` | Dashboard (metrics + charts) | `routes_analytics.cpp` (A) |
| GET | `/analytics/by-tag` | Emotion/strategy breakdown | `routes_analytics.cpp` (A) |
| GET | `/calculator` | Position-size calculator | `routes_analytics.cpp` (A) |
| GET | `/api/equity-curve` | JSON for Chart.js | `routes_analytics.cpp` (A) |
| GET | `/api/pnl-per-trade` | JSON for Chart.js | `routes_analytics.cpp` (A) |

Each route file exposes one registration function (declared in `routes.hpp`), e.g. `registerAuthRoutes(app, deps)`, and `main.cpp` calls all four.

## 📂 Project File Structure
Owner of each file is shown in brackets: **[A]** Domain & Analytics, **[B]** Persistence, **[C]** Services & Web.

```
trading_journal/
├── CMakeLists.txt                       [C]
├── README.md
├── third_party/
│   └── crow/crow_all.h                  # single-header Crow
│
├── include/
│   ├── errors.hpp                       [A]  (DatabaseError, ValidationError, CsvParseError)
│   ├── trade.hpp                        [A]  (Trade, TradeFilter)
│   ├── metrics.hpp                      [A]  (IMetric + WinRate, ProfitFactor, Expectancy, MaxDrawdown)
│   ├── journal.hpp                      [A]
│   ├── tag_analytics.hpp                [A]  (template groupBy)
│   ├── position_calculator.hpp          [A]
│   ├── validators.hpp                   [A]
│   ├── view_models_analytics.hpp        [A]
│   ├── database.hpp                     [B]  (RAII sqlite3*)
│   ├── statement.hpp                    [B]  (RAII sqlite3_stmt*)
│   ├── user_repository.hpp              [B]
│   ├── trade_repository.hpp             [B]  (ITradeRepository + SqliteTradeRepository)
│   ├── in_memory_trade_repository.hpp   [B]  (stub for early development/tests)
│   ├── auth_service.hpp                 [C]
│   ├── csv_service.hpp                  [C]
│   ├── middleware.hpp                   [C]  (AuthGuard, session helpers)
│   └── routes.hpp                       [C]  (register*Routes declarations)
│
├── src/
│   ├── trade.cpp                        [A]
│   ├── metrics.cpp                      [A]
│   ├── journal.cpp                      [A]
│   ├── tag_analytics.cpp                [A]
│   ├── position_calculator.cpp          [A]
│   ├── validators.cpp                   [A]
│   ├── view_models_analytics.cpp        [A]
│   ├── routes_analytics.cpp             [A]  (/, /analytics/by-tag, /calculator, /api/*)
│   ├── database.cpp                     [B]
│   ├── statement.cpp                    [B]
│   ├── user_repository.cpp              [B]
│   ├── trade_repository.cpp             [B]
│   ├── in_memory_trade_repository.cpp   [B]
│   ├── auth_service.cpp                 [C]
│   ├── csv_service.cpp                  [C]
│   ├── middleware.cpp                   [C]
│   ├── routes_auth.cpp                  [C]
│   ├── routes_trades.cpp                [C]
│   ├── routes_io.cpp                    [C]  (/export, /import)
│   └── main.cpp                         [C]  sets up Crow app, wires dependencies, starts server
│
├── templates/                           Mustache HTML
│   ├── dashboard.html  by_tag.html  calculator.html            [A]
│   ├── base.html  login.html  register.html                    [C]
│   ├── trades.html  trade_form.html  404.html  500.html        [C]
│
├── static/
│   ├── css/style.css                    [C]
│   └── js/charts.js                     [A]
│
├── sql/
│   └── schema.sql                       [B]
│
├── sample_data/
│   └── sample_trades.csv                [A]
│
└── tests/
    ├── CMakeLists.txt
    ├── test_trade.cpp                   [A]
    ├── test_journal.cpp                 [A]
    ├── test_position_calculator.cpp     [A]
    ├── test_validators.cpp              [A]
    ├── test_repository.cpp              [B]
    ├── test_csv.cpp                     [C]
    ├── test_auth.cpp                    [C]
    └── test_routes.cpp                  [C]
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

# Domain + data + services as a library, so tests can link to it
add_library(tj_core
    # Member A
    src/trade.cpp src/metrics.cpp src/journal.cpp src/tag_analytics.cpp
    src/position_calculator.cpp src/validators.cpp
    # Member B
    src/database.cpp src/statement.cpp src/user_repository.cpp
    src/trade_repository.cpp src/in_memory_trade_repository.cpp
    # Member C
    src/auth_service.cpp src/csv_service.cpp)
target_include_directories(tj_core PUBLIC include third_party/crow)
target_link_libraries(tj_core PUBLIC SQLite::SQLite3 nlohmann_json::nlohmann_json
                      ${SODIUM_LIB} Threads::Threads)

add_executable(trading_journal
    src/main.cpp src/middleware.cpp
    src/routes_auth.cpp src/routes_trades.cpp src/routes_io.cpp   # C
    src/routes_analytics.cpp src/view_models_analytics.cpp)       # A
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

**`metrics.hpp` / `metrics.cpp` (strategy interface + one derived metric)**
```cpp
// metrics.hpp
class IMetric {
public:
    virtual ~IMetric() = default;                       // virtual destructor: safe delete via base pointer
    virtual std::string name() const = 0;
    virtual double compute(const std::vector<Trade>& trades) const = 0;
};

class MaxDrawdown : public IMetric {
public:
    std::string name() const override { return "Max Drawdown"; }
    double compute(const std::vector<Trade>& trades) const override;
};

// metrics.cpp
double MaxDrawdown::compute(const std::vector<Trade>& trades) const {
    double equity = 0.0, peak = 0.0, maxDd = 0.0;
    for (const auto& t : trades) {            // trades sorted by date
        equity += t.profit();
        peak = std::max(peak, equity);
        maxDd = std::max(maxDd, peak - equity);
    }
    return maxDd;
}
```

**`journal.hpp` / `journal.cpp` (owns metrics through smart pointers)**
```cpp
class Journal {
public:
    explicit Journal(std::vector<Trade> trades);        // registers the 4 default metrics
    Journal(Journal&&) = default;                       // move-only: owns unique_ptrs
    Journal& operator=(Journal&&) = default;

    void addMetric(std::unique_ptr<IMetric> m) { metrics_.push_back(std::move(m)); }
    std::vector<std::pair<std::string,double>> computeAll() const;

    double maxDrawdown() const;                         // convenience: looks up the MaxDrawdown metric
    // winRate(), profitFactor(), expectancy() work the same way

private:
    std::vector<Trade> trades_;
    std::vector<std::unique_ptr<IMetric>> metrics_;
};

Journal::Journal(std::vector<Trade> trades) : trades_(std::move(trades)) {
    metrics_.push_back(std::make_unique<WinRate>());
    metrics_.push_back(std::make_unique<ProfitFactor>());
    metrics_.push_back(std::make_unique<Expectancy>());
    metrics_.push_back(std::make_unique<MaxDrawdown>());
}

std::vector<std::pair<std::string,double>> Journal::computeAll() const {
    std::vector<std::pair<std::string,double>> out;
    for (const auto& m : metrics_)                      // runtime polymorphism
        out.emplace_back(m->name(), m->compute(trades_));
    return out;
}
```

**A Crow route (`routes_analytics.cpp`)**
```cpp
CROW_ROUTE(app, "/api/equity-curve")([&](const crow::request& req) {
    int userId = currentUserId(req);             // from session (C's middleware)
    auto trades = tradeRepo.search(userId, {});  // empty filter = all trades (B's interface)
    Journal journal(trades);                     // A's domain class

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
* **Unit tests (GoogleTest), by owner:**
  - **A:** `Trade::profit()` for long/short with and without fees; each `IMetric` subclass on hand-calculated datasets, called both directly and through `Journal::computeAll()` (polymorphism check); `Journal` with a custom metric added via `addMetric()`; edge cases (no trades, no losses, all losses); `PositionCalculator`; validators (bad price, zero quantity, bad date, bad direction).
  - **B:** repository CRUD on in-memory SQLite (`:memory:`); user isolation ("User A cannot read User B's trade"); `search()` filters; `existsDuplicate()`; transaction rollback on failed bulk insert; both repository implementations pass the same test suite.
  - **C:** CSV (quoted fields, bad numbers, missing columns, round-trip export→import); `AuthService` (hash verifies, wrong password fails, duplicate username rejected); route integration tests for key flows.
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
| Concept | Where | Owner |
|---|---|---|
| Classes & encapsulation | `Trade`, `Journal`, repositories | A, B |
| `const` correctness | `Trade::profit() const`, `Journal` getters | A |
| `enum class` | `Trade::Direction`, `Result` | A |
| STL containers & algorithms | `std::vector<Trade>`, `std::sort` by date, `std::accumulate`, `std::count_if`, `std::unordered_map` for tag grouping | A |
| Lambdas | Filtering and sorting predicates, `groupBy` key functions | A |
| Templates | Generic `groupBy()` helper for tag analytics | A |
| Custom exception hierarchy | `errors.hpp`: `DatabaseError`, `ValidationError`, `CsvParseError` (all derive from `std::runtime_error`) | A (defined), all (used) |
| RAII | `Database` (opens/closes `sqlite3*`), `Statement` (finalizes `sqlite3_stmt*`) | B |
| Move semantics | `Database` and `Statement` are non-copyable, movable | B |
| Smart pointers | `Journal` owns `std::vector<std::unique_ptr<IMetric>>` (A); `std::unique_ptr<ITradeRepository>` / `std::shared_ptr<Database>` (B, C); no raw `new`/`delete` | A, B, C |
| `std::optional` | `findById()`, `findByUsername()`, `login()` return `optional` | B, C |
| Inheritance & polymorphism | `IMetric` → `WinRate`, `ProfitFactor`, `Expectancy`, `MaxDrawdown` (strategy pattern, virtual destructor, `override`) | A |
| Inheritance & polymorphism | `ITradeRepository` → `SqliteTradeRepository`, `InMemoryTradeRepository` | B |
| Transactions & C-library interop | `BEGIN`/`COMMIT`/`ROLLBACK`, sqlite3 C API | B |
| File I/O & string parsing | `CsvService` | C |
| C-library interop (crypto) | libsodium Argon2id in `AuthService` | C |
| Middleware pattern | `AuthGuard`, session helpers | C |
| Dependency injection | `main.cpp` constructs repositories/services and passes them to the route files | C |
| `std::function` | Route handler wrappers and middleware callbacks | C |

## 👥 Team Structure & Module Division (3 Members)

The project is split **vertically by layer**, and the original split has been **rebalanced** so every member writes about the same amount of C++ (~900–1000 lines), owns a different set of core C++ concepts, and still has one small web-facing slice.

### Summary table

| | **Member A: Domain & Analytics** | **Member B: Persistence** | **Member C: Services & Web** |
|---|---|---|---|
| **Theme** | The "brain": trade math, analytics, validation, analytics pages | The "memory": SQLite, repositories, the storage interface | The "face": auth, CSV, middleware, HTTP server, trade CRUD pages |
| **C++ files** | `errors.hpp`, `trade.*`, `metrics.*`, `journal.*`, `tag_analytics.*`, `position_calculator.*`, `validators.*`, `view_models_analytics.*`, `routes_analytics.cpp` | `database.*`, `statement.*`, `user_repository.*`, `trade_repository.*` (`ITradeRepository` + `SqliteTradeRepository`), `in_memory_trade_repository.*`, `sql/schema.sql` | `auth_service.*`, `csv_service.*`, `middleware.*`, `routes.hpp`, `main.cpp`, `routes_auth.cpp`, `routes_trades.cpp`, `routes_io.cpp` |
| **Approx. C++ size** | ~1000 lines | ~1000 lines | ~950 lines |
| **Core concepts** | `const` correctness, `enum class`, STL algorithms, lambdas, **templates**, exceptions, **inheritance & polymorphism** (`IMetric`), **smart pointers** (`unique_ptr` in `Journal`) | **RAII**, **move semantics**, smart pointers, `std::optional`, **inheritance & polymorphism**, transactions | File I/O, string parsing, **middleware pattern**, **dependency injection**, `std::function`, libsodium interop |
| **Frontend** | `dashboard.html`, `by_tag.html`, `calculator.html`, `charts.js` | None (heaviest C++) | `base`, `login`, `register`, `trades`, `trade_form`, `404`, `500`, `style.css` |
| **Tests** | `test_trade`, `test_journal`, `test_position_calculator`, `test_validators` | `test_repository` | `test_csv`, `test_auth`, `test_routes` |
| **Depends on** | Nobody for core logic; uses B's `ITradeRepository` and C's `currentUserId()` only in `routes_analytics.cpp` | A's `Trade` / `TradeFilter` / `errors.hpp` | A's `Trade` and validators; B's `ITradeRepository` and `UserRepository` |

### What changed from the original split

| Moved item | From → To | Why |
|---|---|---|
| `validators.*` | C → A | Pure C++ logic with exceptions; a natural fit for the domain layer |
| Analytics routes, analytics view models, dashboard / by-tag / calculator templates, `charts.js` | C → A | A owns the feature end to end, from math to page |
| `InMemoryTradeRepository` | C → B | B owns `ITradeRepository`, so both implementations sit together (stronger polymorphism demo) |
| `AuthService` | B → C | Pairs with sessions and middleware; still shows C-library interop (libsodium) |
| `CsvService` | A → C | File I/O and parsing pair with import/export routes; C calls B's `existsDuplicate()` |
| `routes.cpp` | one file → four files | One route file per owner/feature avoids merge conflicts |
| `IMetric` hierarchy and `unique_ptr` ownership in `Journal` | new, A | Gives A real inheritance, polymorphism and smart-pointer ownership, so OOP concepts are spread more evenly |

### 🅰️ Member A: Domain & Analytics
**Mission:** all financial logic and input validation, with correct and tested results, plus the analytics pages built on top of it.

Responsibilities:
1. **`errors.hpp`**: exception hierarchy (`DatabaseError`, `ValidationError`, `CsvParseError`) derived from `std::runtime_error`.
2. **`Trade`** and **`TradeFilter`**: immutable-style class with getters, `profit()` (long/short, fees), `isWin()`, `riskRewardRatio()`; filter struct of `std::optional` fields.
3. **`IMetric` strategy hierarchy** (`metrics.*`): abstract `IMetric` (`name()`, `compute(trades)`, virtual destructor) with derived `WinRate`, `ProfitFactor`, `Expectancy`, `MaxDrawdown`, all marked `override`.
4. **`Journal`**: owns a `std::vector<Trade>` and a `std::vector<std::unique_ptr<IMetric>>` (move-only). Provides `computeAll()`, `addMetric()`, convenience getters `winRate()`, `profitFactor()`, `expectancy()`, `maxDrawdown()` that delegate to the metric objects, plus `totalPnL()`, `avgWin()`, `avgLoss()`, `equityCurve()`, `longestWinStreak()`, `longestLossStreak()`, and `filterByResult(Win|Loss)`.
5. **`TagAnalytics`**: generic `groupBy(trades, keyFn)` **template** to compute average P&L and count per strategy/emotion tag.
6. **`PositionCalculator`**: position size from account size, risk %, entry, stop-loss; risk/reward ratio; input validation with exceptions.
7. **`validators.*`**: server-side form rules (prices > 0, quantity > 0, valid ISO date, direction ∈ {Long, Short}) throwing `ValidationError` with clear messages.
8. **`routes_analytics.cpp` + `view_models_analytics.*`**: `/`, `/analytics/by-tag`, `/calculator`, `/api/equity-curve`, `/api/pnl-per-trade`; convert `Journal` results into Mustache contexts and JSON.
9. **Frontend slice**: `dashboard.html`, `by_tag.html`, `calculator.html`, `charts.js`.
10. **Unit tests** for everything above, using hand-calculated expected values.

Viva topics: why `profit()` is `const`; how drawdown is computed; why `IMetric` needs a virtual destructor and how `Journal` calls metrics polymorphically; why `Journal` is move-only because of `unique_ptr`; how `groupBy<T>` is instantiated; how validators throw and where the exceptions are caught.

### 🅱️ Member B: Persistence
**Mission:** safe, correct persistence with all SQL hidden behind classes, plus the storage interface the rest of the team codes against.

Responsibilities:
1. **`Database`**: RAII wrapper over `sqlite3*` (open in constructor, close in destructor; non-copyable, movable); runs `schema.sql` on first start; enables `PRAGMA foreign_keys = ON`.
2. **`Statement`**: RAII wrapper over `sqlite3_stmt*` with `bind(...)`, `step()`, and typed column getters (prepared statements only; move-only).
3. **`UserRepository`**: `create()`, `findByUsername()` returning `std::optional`.
4. **`ITradeRepository` (abstract interface)** with two implementations:
   * **`SqliteTradeRepository`**: `add`, `update`, `remove`, `findById`, `search(userId, TradeFilter)` (dynamic `WHERE` built safely with bound parameters), and `existsDuplicate()` for CSV import. Every query is scoped by `user_id`.
   * **`InMemoryTradeRepository`**: `std::map`-based stub, delivered early in Week 1 so A and C are never blocked.
5. **Transactions** for bulk import (`BEGIN` / `COMMIT` / `ROLLBACK`) and indexes on `(user_id, trade_date)`.
6. **Error wrapping**: all SQLite failures surface as `DatabaseError`.
7. **Tests** using in-memory SQLite (`:memory:`); the same test suite runs against both repository implementations.

Viva topics: RAII and why the copy constructor is deleted while the move constructor is defined; how prepared statements stop SQL injection; how `search()` builds filters safely; why two repository classes share one interface.

### 🅲 Member C: Services & Web
**Mission:** authentication, CSV exchange, and the web plumbing that connects everything into a working app.

Responsibilities:
1. **`AuthService`**: `registerUser()`, `login()` using libsodium Argon2id (`crypto_pwhash_str` / `crypto_pwhash_str_verify`); username/password rules.
2. **`CsvService`**: `exportCsv(trades)` and `importCsv(stream, userId)` returning parsed trades plus a list of row errors (handles quotes, bad numbers, missing columns).
3. **`main.cpp`**: create the Crow app with `CookieParser` and `SessionMiddleware`, open the database, construct repositories/services, register all four route files (dependency injection).
4. **Middleware**: `AuthGuard` (redirect to `/login` when no session), `currentUserId(req)`, flash messages.
5. **Routes**: `routes_auth.cpp` (register, login, logout), `routes_trades.cpp` (CRUD, list with filters, using A's validators), `routes_io.cpp` (export, import with summary such as "18 imported, 2 duplicates, 1 invalid row").
6. **Frontend slice**: `base`, `login`, `register`, `trades`, `trade_form`, `404`, `500` templates and `style.css`.
7. **Tests**: `test_csv`, `test_auth`, `test_routes`, plus the final demo script.

Viva topics: how Argon2id hashing and verification work; how the middleware chain and sessions work; how dependency injection swaps the in-memory and SQLite repositories; how CSV parsing handles malformed rows.

---

## 🤝 Interface Contracts (agree in Phase 0, then don't change without telling everyone)

These headers are written **first** so all three members can work in parallel.

| Contract | Owner | Signature (summary) |
|---|---|---|
| `errors.hpp` | A | `DatabaseError`, `ValidationError`, `CsvParseError`, all derived from `std::runtime_error` |
| `Trade` | A | `Trade(int id, int userId, std::string ticker, Direction, double entry, double exit, int qty, double fees, std::string tradeDate, ...)`; getters; `double profit() const` |
| `TradeFilter` | A | `struct` of `std::optional` fields: `ticker`, `fromDate`, `toDate`, `direction`, `strategy`, `emotion` |
| `IMetric` | A | `virtual std::string name() const = 0`; `virtual double compute(const std::vector<Trade>&) const = 0`; virtual destructor |
| `Journal` | A | `explicit Journal(std::vector<Trade>)` (move-only); `computeAll()`, `addMetric(std::unique_ptr<IMetric>)`; `winRate()`, `profitFactor()`, `expectancy()`, `maxDrawdown()`, `equityCurve()`, `filterByResult(Result)` |
| `PositionCalculator` | A | `PositionResult calculate(double account, double riskPct, double entry, double stop)` |
| `Validators` | A | `Trade validateTradeForm(const FormData&, int userId)`; throws `ValidationError` |
| `ITradeRepository` | B | `int add(const Trade&)`; `bool update(const Trade&)`; `bool remove(int userId, int tradeId)`; `std::optional<Trade> findById(int userId, int id)`; `std::vector<Trade> search(int userId, const TradeFilter&)`; `bool existsDuplicate(int userId, const Trade&)`; `int addBulk(const std::vector<Trade>&)` (transactional) |
| `UserRepository` | B | `std::optional<int> create(user, hash)`; `std::optional<User> findByUsername(user)` |
| `AuthService` | C | `std::optional<int> registerUser(user, pass)`; `std::optional<int> login(user, pass)` (returns user id) |
| `CsvService` | C | `std::string exportCsv(const std::vector<Trade>&)`; `ImportResult importCsv(std::istream&, int userId)` |
| `routes.hpp` | C | `registerAuthRoutes`, `registerTradeRoutes`, `registerIoRoutes` (C); `registerAnalyticsRoutes` (A); each takes the Crow app and a `Deps` struct of references |

**Design decisions made up front:**
* Dates are stored and passed as ISO strings `"YYYY-MM-DD HH:MM:SS"`, which sort correctly as text.
* The win/loss filter is applied **in memory** by `Journal::filterByResult()`, not in SQL, to keep layers clean.
* All errors are reported with exceptions derived from `std::runtime_error` and are caught in the route layer.
* `CsvService` depends only on `Trade` and `ITradeRepository`; validators depend only on `Trade`.

---

## 🗺️ Phase-Wise Development Path (4 Weeks)

### Phase 0: Setup & Contracts (Days 1–3, everyone together)
| Step | Task | Who |
|---|---|---|
| 0.1 | Create the Git repo, branches `main`, `feature/domain`, `feature/data`, `feature/web`; protect `main` (merge via pull request only) | C |
| 0.2 | Install dependencies and get the **same CMake build** working on all 3 machines | All |
| 0.3 | Smoke tests: Crow "Hello World" (C), SQLite "open and create table" (B), libsodium "hash a string" (C) | C, B, C |
| 0.4 | Write `errors.hpp`, `Trade`, `TradeFilter` headers | A |
| 0.5 | Write headers with stub bodies: `ITradeRepository`, `UserRepository` (B); `AuthService`, `CsvService`, `routes.hpp` (C) | B, C |
| 0.6 | Create `sample_data/sample_trades.csv` (20–30 realistic trades, including losses and shorts) | A |
| 0.7 | Agree on coding style (naming, `clang-format`), commit message format, and review rotation (A reviews B, B reviews C, C reviews A) | All |

✅ **Checkpoint 0:** everyone can build; all headers are merged to `main`; each person knows their files.

---

### Phase 1: Foundations in Parallel (Week 1)
| | Tasks |
|---|---|
| **A** | 1. `Trade` with `profit()` (long/short, fees), `isWin()`, `riskRewardRatio()`. 2. `IMetric` interface and the first derived metric `WinRate`. 3. Basic `Journal` holding `std::vector<std::unique_ptr<IMetric>>`: `totalPnL`, `avgWin`, `avgLoss`, `computeAll()`. 4. First GoogleTest cases (long win, short win, loss, fees, empty journal, `WinRate` via the interface). |
| **B** | 1. `Database` RAII class (non-copyable, movable) + `schema.sql` auto-run + foreign keys on. 2. `Statement` wrapper (bind/step/columns, move-only). 3. `UserRepository` with `create` / `findByUsername`. 4. **`InMemoryTradeRepository`** (push by mid-week so others can use it). 5. Tests on `:memory:`. |
| **C** | 1. Crow app skeleton in `main.cpp` with static files, `CookieParser`, `SessionMiddleware`. 2. `AuthService` (Argon2id hash/verify, username/password rules) + `test_auth`. 3. `base.html`, `login.html`, `register.html`. 4. Route stubs for every URL; wire to `InMemoryTradeRepository` once B pushes it (until then, a hard-coded vector). |

✅ **Checkpoint 1 (end of Week 1):** `ctest` passes for A, B, and auth tests; the app runs with stub routes and login pages. Merge all branches to `main` and confirm it still builds.

---

### Phase 2: Core Features (Week 2)
| | Tasks |
|---|---|
| **A** | 1. Derived metrics `ProfitFactor`, `Expectancy`, `MaxDrawdown` (register all four in `Journal`); `equityCurve`. 2. Win/loss streaks and `filterByResult`. 3. `PositionCalculator` with exception-based validation. 4. `validators.*` (price/quantity/date/direction rules) + `test_validators`. 5. Edge-case tests (no losses, all losses, single trade, flat equity). |
| **B** | 1. `SqliteTradeRepository`: `add`, `update`, `remove`, `findById`, all scoped by `user_id`. 2. `search()` with dynamic filters using bound parameters. 3. Indexes on `(user_id, trade_date)`. 4. Repository tests (including "User A cannot read User B's trade"), run against both implementations. |
| **C** | 1. `/register`, `/login`, `/logout` in `routes_auth.cpp`; session cookie handling. 2. `AuthGuard` middleware and `currentUserId()`. 3. Trade CRUD routes, `trades.html`, `trade_form.html`, using A's validators. 4. **Switch to `SqliteTradeRepository`** once B's tests pass (one-line change in `main.cpp`). |

✅ **Checkpoint 2 (end of Week 2):** a user can register, log in, add/edit/delete trades stored in SQLite, and see only their own trades.

---

### Phase 3: Analytics, Charts & Data Exchange (Week 3)
| | Tasks |
|---|---|
| **A** | 1. `TagAnalytics` with template `groupBy`. 2. `routes_analytics.cpp`: `/`, `/analytics/by-tag`, `/calculator`, `/api/equity-curve`, `/api/pnl-per-trade`. 3. `view_models_analytics.*`. 4. `dashboard.html`, `by_tag.html`, `calculator.html`, `charts.js`. |
| **B** | 1. `existsDuplicate()` (ticker + timestamp + quantity). 2. Transactional bulk insert (`BEGIN` / `COMMIT` / `ROLLBACK`) + rollback test. 3. Wrap all SQLite errors in `DatabaseError`. 4. Connect `search()` to C's filter form; fix integration bugs. |
| **C** | 1. `CsvService`: `exportCsv`, `importCsv` with row-level error list (quotes, bad numbers, missing columns). 2. `routes_io.cpp`: `/export`, `/import` with summary ("18 imported, 2 duplicates, 1 invalid row"). 3. Filter form on `/trades`. 4. `test_csv` including round-trip export→import. |

✅ **Checkpoint 3 (end of Week 3, feature-complete):** import the sample CSV → dashboard and charts show correct numbers → export → re-import creates no duplicates.

---

### Phase 4: Testing, Hardening & Submission (Week 4)
| Step | Task | Who |
|---|---|---|
| 4.1 | Run the **manual test checklist** (see Testing Plan) and log bugs as GitHub issues | All |
| 4.2 | Build with `-fsanitize=address,undefined`, click through every page, fix leaks and undefined behaviour | Each fixes own module |
| 4.3 | Cross-review (A→B→C→A): clarity, comments, and `const`/RAII correctness | All |
| 4.4 | Security pass: confirm prepared statements everywhere, `user_id` checks on every query, Mustache escaping, cookie flags | B leads |
| 4.5 | UI polish, error pages (404/500), flash messages | C (A polishes own pages) |
| 4.6 | Integration tests and demo script (register → import → dashboard → filter → export) | C |
| 4.7 | Write the project report: each member writes their own module's chapter (design, classes, tests) | All |
| 4.8 | Take screenshots, rehearse the demo and viva (each member demos *their own* module) | All |
| 4.9 | Tag release `v1.0`, final README update | C |

✅ **Final checkpoint:** all items in the Submission Checklist are ticked.

---

## 🔀 Team Workflow Rules
* **Ownership:** only the owner edits their files. Need a change in someone else's file? Ask them, or open a small pull request for them to review.
* **Branches:** each member works on their own `feature/*` branch and merges to `main` through a pull request reviewed by the next member in the rotation (A→B→C→A).
* **Sync points:** merge to `main` at every checkpoint; meet for 15 minutes twice a week to confirm interfaces and unblock each other.
* **Never break `main`:** it must always build and pass `ctest`.
* **If you're blocked,** use a stub: B's `InMemoryTradeRepository` (ready mid-Week 1), A's hard-coded `Journal`, or a hard-coded vector of trades in route stubs.

## ⚖️ Workload Balance at a Glance
| Area | A | B | C |
|---|:---:|:---:|:---:|
| Algorithms / logic | ●●● | ●○○ | ●●○ |
| OOP design (classes, interfaces) | ●●● | ●●● | ●●○ |
| Memory management / RAII / smart pointers | ●●○ | ●●● | ●○○ |
| Templates / STL | ●●● | ●○○ | ●○○ |
| Exceptions & validation | ●●● | ●●○ | ●●○ |
| File I/O & parsing | ○○○ | ○○○ | ●●● |
| Database / SQL | ○○○ | ●●● | ○○○ |
| Security | ●○○ | ●●● | ●●● |
| Networking / HTTP | ●○○ | ○○○ | ●●● |
| Testing | ●●● | ●●○ | ●●● |
| Frontend (HTML/JS) | ●●○ | ○○○ | ●●○ |

## 🚧 Risks & Mitigation
| Risk | Mitigation |
|---|---|
| Build differs across machines (Asio/Crow setup) | Fix this in Phase 0; document the exact commands that worked |
| One member is blocked waiting for another | Interfaces agreed on Day 1–3; B ships `InMemoryTradeRepository` in Week 1; A's core tests need nobody |
| Merge conflicts | Strict file ownership; one route file per owner; small, frequent pull requests |
| Interface changes late in the project | Freeze contracts after Phase 0; changes require agreement from all three |
| B is the critical path for the database switch | B delivers `SqliteTradeRepository` CRUD by early Week 2; C stays on the in-memory repo until B's tests pass |
| Running out of time | The calculator (A) and CSV (C) are the first to cut; they are independent, so cutting one doesn't unbalance the workload. Core CRUD + analytics + charts must be done by Checkpoint 3 |

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
- [ ] Source code with clear commit history (each member's commits visible)
- [ ] `README.md` (this file) and a short project report
- [ ] `sample_data/sample_trades.csv` for the demo
- [ ] Screenshots of dashboard, trade list, and analytics
- [ ] Unit tests passing (`ctest`)
- [ ] Demo script: register → import sample CSV → view dashboard → filter → export

## 📜 License
MIT
