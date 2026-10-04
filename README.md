# Trading Journal: A C++ Web Application (Mini Project)

## 📖 Project Overview
The Trading Journal is a small web application that lets a trader **log trades, review their history, and see performance analytics**. The whole backend is written in **C++17**: web server, routing, authentication, database access, and analytics. The browser side is a thin layer of HTML, CSS, and a little JavaScript (Chart.js) for drawing charts.

This is a **mini project**. It is scoped to be finished by a team of 3 students in about **4 weeks** (see the Team Structure section for the module split), and it focuses on showing good C++ and OOP practice rather than building a full commercial product.

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
Owner of each file is shown in brackets: **[A]** Domain & Analytics, **[B]** Data & Security, **[C]** Web & Integration.

```
trading_journal/
├── CMakeLists.txt                       [C]
├── README.md
├── third_party/
│   └── crow/crow_all.h                  # single-header Crow
│
├── include/
│   ├── trade.hpp                        [A]  (Trade, TradeFilter)
│   ├── journal.hpp                      [A]
│   ├── tag_analytics.hpp                [A]  (template groupBy)
│   ├── position_calculator.hpp          [A]
│   ├── csv_service.hpp                  [A]
│   ├── database.hpp                     [B]  (RAII sqlite3*)
│   ├── statement.hpp                    [B]  (RAII sqlite3_stmt*)
│   ├── user_repository.hpp              [B]
│   ├── trade_repository.hpp             [B]  (ITradeRepository + SqliteTradeRepository)
│   ├── auth_service.hpp                 [B]
│   ├── in_memory_trade_repository.hpp   [C]  (stub for early development/tests)
│   ├── middleware.hpp                   [C]  (AuthGuard, session helpers)
│   ├── view_models.hpp                  [C]
│   └── validators.hpp                   [C]
│
├── src/
│   ├── main.cpp                         [C]  sets up Crow app, wires dependencies, starts server
│   ├── routes.cpp                       [C]
│   ├── middleware.cpp                   [C]
│   ├── view_models.cpp                  [C]
│   ├── validators.cpp                   [C]
│   ├── in_memory_trade_repository.cpp   [C]
│   ├── trade.cpp                        [A]
│   ├── journal.cpp                      [A]
│   ├── tag_analytics.cpp                [A]
│   ├── position_calculator.cpp          [A]
│   ├── csv_service.cpp                  [A]
│   ├── database.cpp                     [B]
│   ├── statement.cpp                    [B]
│   ├── user_repository.cpp              [B]
│   ├── trade_repository.cpp             [B]
│   └── auth_service.cpp                 [B]
│
├── templates/                           [C]  Mustache HTML
│   ├── base.html   login.html   register.html
│   ├── dashboard.html   trades.html   trade_form.html
│   ├── by_tag.html   calculator.html   404.html   500.html
│
├── static/                              [C]
│   ├── css/style.css
│   └── js/charts.js
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
    ├── test_csv.cpp                     [A]
    ├── test_repository.cpp              [B]
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

# Domain + data code as a library, so tests can link to it
add_library(tj_core
    src/trade.cpp src/journal.cpp src/tag_analytics.cpp src/position_calculator.cpp
    src/csv_service.cpp src/database.cpp src/statement.cpp src/user_repository.cpp
    src/trade_repository.cpp src/auth_service.cpp src/in_memory_trade_repository.cpp)
target_include_directories(tj_core PUBLIC include third_party/crow)
target_link_libraries(tj_core PUBLIC SQLite::SQLite3 nlohmann_json::nlohmann_json
                      ${SODIUM_LIB} Threads::Threads)

add_executable(trading_journal src/main.cpp src/routes.cpp src/middleware.cpp
    src/view_models.cpp src/validators.cpp)
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

## 👥 Team Structure & Module Division (3 Members)

The project is split **vertically by layer**, so every member writes a substantial, independent piece of C++ and nobody ends up doing only HTML or only SQL.

| | **Member A: Domain & Analytics Engine** | **Member B: Data & Security Layer** | **Member C: Web Layer & Integration** |
|---|---|---|---|
| **Owns** | The "brain": trade math, analytics, calculator, CSV | The "memory": SQLite, repositories, authentication | The "face": HTTP server, routes, sessions, templates, charts |
| **Files** | `trade.*`, `journal.*`, `position_calculator.*`, `csv_service.*`, `tag_analytics.*`, `tests/test_trade.cpp`, `tests/test_journal.cpp`, `tests/test_position_calculator.cpp`, `tests/test_csv.cpp` | `database.*`, `statement.*`, `user_repository.*`, `trade_repository.*`, `auth_service.*`, `sql/schema.sql`, `tests/test_repository.cpp` | `main.cpp`, `routes.cpp`, `middleware.*`, `view_models.*`, `in_memory_trade_repository.*`, `validators.*`, `templates/*`, `static/*`, `tests/test_routes.cpp` |
| **Approx. C++ size** | 700–900 lines | 800–1000 lines | 800–1000 lines (+ HTML/JS) |
| **Main C++ concepts** | `const` correctness, `enum class`, STL algorithms, lambdas, **templates** (`groupBy`), file I/O, string parsing, exceptions | **RAII**, **move semantics**, smart pointers, `std::optional`, **inheritance & polymorphism** (`ITradeRepository`), transactions, C-library interop (sqlite3, libsodium) | Lambdas / `std::function`, **middleware pattern**, **dependency injection**, JSON building, input validation, HTTP handling, integration testing |
| **Depends on** | Nobody (pure C++ logic) | Member A's `Trade` class | A's `Trade`/`Journal`/`CsvService` and B's repositories (uses a stub until ready) |

### 🅰️ Member A: Domain & Analytics Engine
**Mission:** all financial logic, with correct and tested results, and no knowledge of web or database.

Responsibilities:
1. **`Trade`**: immutable-style class with getters, `profit()` (long/short, fees), `isWin()`, `riskRewardRatio()`.
2. **`Journal`**: `std::vector<Trade>` wrapper with `totalPnL()`, `winRate()`, `profitFactor()`, `expectancy()`, `avgWin()`, `avgLoss()`, `maxDrawdown()`, `equityCurve()`, `longestWinStreak()`, `longestLossStreak()`, and `filterByResult(Win|Loss)`.
3. **`TagAnalytics`**: a generic `groupBy(trades, keyFn)` **template** to compute average P&L and count per strategy/emotion tag.
4. **`PositionCalculator`**: position size from account size, risk %, entry, stop-loss; risk/reward ratio; input validation with exceptions.
5. **`CsvService`**: `exportCsv(trades)` and `importCsv(stream)` returning parsed trades plus a list of row errors (handles quotes, bad numbers, missing columns).
6. **Unit tests** for everything above, using hand-calculated expected values.

Viva topics: why `profit()` is `const`; how drawdown is computed; how the template `groupBy` works; how CSV parsing handles malformed rows.

### 🅱️ Member B: Data & Security Layer
**Mission:** safe, correct persistence and authentication, with all SQL hidden behind classes.

Responsibilities:
1. **`Database`**: RAII wrapper over `sqlite3*` (open in constructor, close in destructor; non-copyable, movable); runs `schema.sql` on first start; enables `PRAGMA foreign_keys = ON`.
2. **`Statement`**: RAII wrapper over `sqlite3_stmt*` with `bind(...)`, `step()`, and typed column getters (prepared statements only).
3. **`UserRepository`**: `create()`, `findByUsername()` returning `std::optional`.
4. **`AuthService`**: `registerUser()`, `login()` using libsodium Argon2id (`crypto_pwhash_str` / `crypto_pwhash_str_verify`); validates username/password rules.
5. **`ITradeRepository` (abstract interface) + `SqliteTradeRepository`**: `add`, `update`, `remove`, `findById`, `search(userId, TradeFilter)` (dynamic `WHERE` built safely with bound parameters), and `existsDuplicate()` for CSV import. Every query is scoped by `user_id`.
6. **Transactions** for bulk import (`BEGIN` / `COMMIT` / `ROLLBACK`) and indexes on `(user_id, trade_date)`.
7. **Tests** using in-memory SQLite (`:memory:`).

Viva topics: RAII and why the copy constructor is deleted; how prepared statements stop SQL injection; Argon2id vs plain hashing; how `search()` builds filters safely.

### 🅲 Member C: Web Layer & Integration
**Mission:** connect everything into a working web app and own the end-to-end experience.

Responsibilities:
1. **`main.cpp`**: create the Crow app with `CookieParser` and `SessionMiddleware`, open the database, construct repositories/services, register routes (dependency injection).
2. **Middleware**: `AuthGuard` (redirect to `/login` when no session), helpers `currentUserId(req)`, flash messages.
3. **`routes.cpp`**: all routes in the route table (auth, trade CRUD, list with filters, calculator, tag analytics, export/import, JSON APIs).
4. **`view_models`**: functions that turn `Trade`/`Journal` objects into `crow::mustache::context` or JSON for the templates and charts.
5. **`validators`**: server-side validation of form input (prices > 0, quantity > 0, valid date, direction ∈ {Long, Short}) with clear error messages.
6. **`InMemoryTradeRepository`**: a stub implementing B's `ITradeRepository` using `std::map`, so C can build and test pages **before** the database is ready (a real use of polymorphism).
7. **Frontend**: Bootstrap templates (`base`, `login`, `register`, `dashboard`, `trades`, `trade_form`, `by_tag`, `calculator`) and `charts.js` (Chart.js).
8. **Integration tests** (`cpr`/`curl` scripts or GoogleTest) for key flows, plus the final demo script.

Viva topics: how the middleware chain and sessions work; how dependency injection lets us swap the in-memory and SQLite repositories; how user input is validated and escaped.

---

## 🤝 Interface Contracts (agree in Phase 0, then don't change without telling everyone)

These headers are written **first** so all three members can work in parallel.

| Contract | Owner | Signature (summary) |
|---|---|---|
| `Trade` | A | `Trade(int id, int userId, std::string ticker, Direction, double entry, double exit, int qty, double fees, std::string tradeDate, ...)`; getters; `double profit() const` |
| `TradeFilter` | A | `struct` of `std::optional` fields: `ticker`, `fromDate`, `toDate`, `direction`, `strategy`, `emotion` |
| `Journal` | A | `explicit Journal(std::vector<Trade>)`; `winRate()`, `profitFactor()`, `expectancy()`, `maxDrawdown()`, `equityCurve()`, `filterByResult(Result)` |
| `CsvService` | A | `std::string exportCsv(const std::vector<Trade>&)`; `ImportResult importCsv(std::istream&, int userId)` |
| `PositionCalculator` | A | `PositionResult calculate(double account, double riskPct, double entry, double stop)` |
| `ITradeRepository` | B | `int add(const Trade&)`; `bool update(const Trade&)`; `bool remove(int userId, int tradeId)`; `std::optional<Trade> findById(int userId, int id)`; `std::vector<Trade> search(int userId, const TradeFilter&)`; `bool existsDuplicate(int userId, const Trade&)` |
| `AuthService` | B | `std::optional<int> registerUser(user, pass)`; `std::optional<int> login(user, pass)` (returns user id) |

**Design decisions made up front:**
* Dates are stored and passed as ISO strings `"YYYY-MM-DD HH:MM:SS"`, which sort correctly as text.
* The win/loss filter is applied **in memory** by `Journal::filterByResult()`, not in SQL, to keep layers clean.
* All errors are reported with exceptions derived from `std::runtime_error` (e.g. `DatabaseError`, `ValidationError`, `CsvParseError`) and are caught in the route layer.

---

## 🗺️ Phase-Wise Development Path (4 Weeks)

### Phase 0: Setup & Contracts (Days 1–3, everyone together)
| Step | Task | Who |
|---|---|---|
| 0.1 | Create the Git repo, branches `main`, `feature/domain`, `feature/data`, `feature/web`; protect `main` (merge via pull request only) | C |
| 0.2 | Install dependencies and get the **same CMake build** working on all 3 machines | All |
| 0.3 | Run a Crow "Hello World" and a SQLite "open and create table" test | All |
| 0.4 | Write all header files (`.hpp`) from the contract table above, with empty or stub implementations | A leads, B & C review |
| 0.5 | Create `sample_data/sample_trades.csv` (20–30 realistic trades, including losses and shorts) | A |
| 0.6 | Agree on coding style (naming, `clang-format`), commit message format, and review rotation (A reviews B, B reviews C, C reviews A) | All |

✅ **Checkpoint 0:** everyone can build; headers are merged to `main`; each person knows their files.

---

### Phase 1: Foundations in Parallel (Week 1)
| | Tasks |
|---|---|
| **A** | 1. Implement `Trade` (`profit()` for long/short, fees). 2. Implement basic `Journal`: `totalPnL`, `winRate`, `avgWin`, `avgLoss`. 3. Write first GoogleTest cases (long win, short win, loss, fees, empty journal). |
| **B** | 1. `Database` RAII class + `schema.sql` auto-run. 2. `Statement` wrapper (bind/step/columns, move-only). 3. `UserRepository` with `create` / `findByUsername`. 4. Test with `:memory:` DB. |
| **C** | 1. Crow app skeleton with static files and Mustache `base.html`. 2. Build all pages as static mock-ups with **fake data**. 3. Write `InMemoryTradeRepository` implementing `ITradeRepository`. 4. Route stubs for every URL in the route table. |

✅ **Checkpoint 1 (end of Week 1):** `ctest` passes for A and B's classes; C's app runs and every page opens with fake data. Merge all branches to `main` and confirm it still builds.

---

### Phase 2: Core Features (Week 2)
| | Tasks |
|---|---|
| **A** | 1. `profitFactor`, `expectancy`, `maxDrawdown`, `equityCurve`. 2. Win/loss streaks and `filterByResult`. 3. `PositionCalculator` with validation. 4. Edge-case tests (no losses, all losses, single trade, flat equity). |
| **B** | 1. `AuthService` with Argon2id. 2. `SqliteTradeRepository`: `add`, `update`, `remove`, `findById`. 3. `search()` with dynamic filters using bound parameters. 4. Repository tests (including "User A cannot read User B's trade"). |
| **C** | 1. `/register`, `/login`, `/logout` using `AuthService`; session cookie handling. 2. `AuthGuard` middleware. 3. Trade CRUD routes and forms with server-side validation. 4. Still on the in-memory repository, then **switch to `SqliteTradeRepository`** once B's tests pass (one-line change in `main.cpp`). |

✅ **Checkpoint 2 (end of Week 2):** a user can register, log in, add/edit/delete trades stored in SQLite, and see only their own trades.

---

### Phase 3: Analytics, Charts & Data Exchange (Week 3)
| | Tasks |
|---|---|
| **A** | 1. `TagAnalytics` with template `groupBy`. 2. `CsvService` export and import (with row-level error reporting). 3. Tests for CSV (quoted fields, bad numbers, missing columns, round-trip export→import). |
| **B** | 1. `existsDuplicate()` and transactional bulk insert. 2. Indexes and `PRAGMA` tuning. 3. Wrap DB errors in `DatabaseError`. 4. Help C connect `search()` to the filter form; fix bugs found during integration. |
| **C** | 1. Dashboard: build `Journal` from the user's trades and render metrics. 2. `/api/equity-curve` and `/api/pnl-per-trade` + `charts.js`. 3. Filter form on `/trades`. 4. `/by-tag`, `/calculator`, `/export`, `/import` pages. 5. Show import summary ("18 imported, 2 duplicates, 1 invalid row"). |

✅ **Checkpoint 3 (end of Week 3):** feature-complete. Import the sample CSV → dashboard and charts show correct numbers → export → re-import creates no duplicates.

---

### Phase 4: Testing, Hardening & Submission (Week 4)
| Step | Task | Who |
|---|---|---|
| 4.1 | Run the **manual test checklist** (see Testing Plan) and log bugs as GitHub issues | All |
| 4.2 | Build with `-fsanitize=address,undefined`, click through every page, fix leaks and undefined behaviour | Each fixes own module |
| 4.3 | Cross-review: each member reviews another's module for clarity, comments, and `const`/RAII correctness | All |
| 4.4 | Security pass: confirm prepared statements everywhere, `user_id` checks on every query, Mustache escaping, cookie flags | B leads |
| 4.5 | UI polish, error pages (404/500), flash messages | C |
| 4.6 | Integration tests and demo script (register → import → dashboard → filter → export) | C |
| 4.7 | Write the project report: each member writes their own module's chapter (design, classes, tests) | All |
| 4.8 | Take screenshots, rehearse the demo and viva (each member demos *their own* module) | All |
| 4.9 | Tag release `v1.0`, final README update | C |

✅ **Final checkpoint:** all items in the Submission Checklist are ticked.

---

## 🔀 Team Workflow Rules
* **Ownership:** only the owner edits their files. Need a change in someone else's file? Ask them, or open a small pull request for them to review.
* **Branches:** each member works on their own `feature/*` branch and merges to `main` through a pull request reviewed by one teammate.
* **Sync points:** merge to `main` at every checkpoint; meet for 15 minutes twice a week to confirm interfaces and unblock each other.
* **Never break `main`:** it must always build and pass `ctest`.
* **If you're blocked,** use a stub (C's `InMemoryTradeRepository`, A's hard-coded `Journal`) and keep going.

## ⚖️ Workload Balance at a Glance
| Area | A | B | C |
|---|:---:|:---:|:---:|
| Algorithms / logic | ●●● | ●○○ | ●○○ |
| OOP design (classes, interfaces) | ●●○ | ●●● | ●●○ |
| Memory management / RAII | ●○○ | ●●● | ●○○ |
| Templates / STL | ●●● | ●○○ | ●○○ |
| File I/O & parsing | ●●● | ○○○ | ●○○ |
| Database / SQL | ○○○ | ●●● | ○○○ |
| Security | ○○○ | ●●● | ●●○ |
| Networking / HTTP | ○○○ | ○○○ | ●●● |
| Testing | ●●● | ●●○ | ●●○ |
| Frontend (HTML/JS) | ○○○ | ○○○ | ●●● |

## 🚧 Risks & Mitigation
| Risk | Mitigation |
|---|---|
| Build differs across machines (Asio/Crow setup) | Fix this in Phase 0; document the exact commands that worked |
| One member is blocked waiting for another | Interfaces agreed on Day 1–3; C uses `InMemoryTradeRepository`; A's tests need nobody |
| Merge conflicts | Strict file ownership; small, frequent pull requests |
| Interface changes late in the project | Freeze contracts after Phase 0; changes require agreement from all three |
| Running out of time | Features 8 (calculator) and 9 (CSV) are the first to cut; core CRUD + analytics + charts must be done by Checkpoint 3 |

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
