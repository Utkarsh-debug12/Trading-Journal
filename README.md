# Trading Journal: High-Performance C++17 Web Application
**Object-Oriented Programming (OOPS) Mini-Project**

---

## Project Team & Responsibilities

| Role | Name | Roll Number | Subsystem / Key Responsibilities |
| :--- | :--- | :--- | :--- |
| **Member A** | **Utkarsh Kumar** | **25008288 (41)** | **Domain & Analytics Engine**: Trade entity, business validation, metric strategy pattern (`IMetric`, `WinRate`, `ProfitFactor`, `MaxDrawdown`, `TotalPnL`), move-only `Journal`, equity curve calculation, template algorithms, analytics routes & interactive dashboard |
| **Member B** | **Pinkesh Kumar Mishra** | **25008253 (07)** | **Data Persistence Layer**: SQLite3 C-API encapsulation, RAII wrapper classes (`Database`, `Statement`), Repository pattern (`ITradeRepository`, `SqliteTradeRepository`, `InMemoryTradeRepository`), database schema design & foreign-key enforcement |
| **Member C** | **Rabeekant** | **25008259 (13)** | **Web Infrastructure & Security**: Crow HTTP server orchestration, Argon2id cryptographic password hashing (libsodium), session authentication middleware (`AuthGuard`), trade CRUD endpoints, RFC 4180 CSV export & view templates |

---

## 1. Project Overview

The **Trading Journal** is a high-performance web application designed for active financial market participants to record, audit, and analyze their trading history. The platform delivers real-time statistical metrics—such as win rate, profit factor, maximum drawdown, and equity curves—enabling traders to measure edge and identify behavioral strengths and weaknesses.

From a software engineering perspective, this project serves as a comprehensive demonstration of **modern C++17 object-oriented programming (OOP)** and systems architecture. The backend is implemented in native C++ using the Crow microframework and SQLite3, avoiding heavy runtime environments while providing type safety, memory efficiency, and deterministic latency. The frontend employs a clean, responsive interface powered by Bootstrap 5 and Chart.js.

### Key Objectives
1. **Object-Oriented Design**: Apply principles of encapsulation, abstraction, runtime polymorphism, and strict const-correctness to real-world financial analytics.
2. **Resource Management**: Implement **RAII (Resource Acquisition Is Initialization)** and modern move semantics to guarantee zero memory or resource leaks without a garbage collector.
3. **Decoupled Architecture**: Structure the codebase into three isolated layers (Presentation, Domain, and Data) following the Dependency Inversion Principle.
4. **Security & Data Isolation**: Guarantee safe multi-user operation through Argon2id cryptographic password hashing, parameterized SQL prepared statements, and session-guarded route middleware.
5. **Rigorous Verification**: Validate core mathematical formulas and business logic through automated unit testing and address sanitization.

---

## 2. System Architecture

The application is engineered as a decoupled, multi-tier system:

```
                      +-------------------------------------------------+
                      |           Client Browser (Presentation)         |
                      |    HTML5 / Bootstrap 5 / Chart.js Canvas        |
                      +-------------------------------------------------+
                                              |
                                              | HTTP Requests (REST / HTML)
                                              v
+---------------------------------------------------------------------------------------+
|  Web & Routing Layer (Crow Framework)                                    [Member C]   |
|  - Route Dispatchers (/trades, /login, /register, /export, /)                         |
|  - Middleware: AuthGuard (Session verification, HTTP-only cookies)                    |
|  - Template Engine: Crow Mustache view rendering                                      |
+---------------------------------------------------------------------------------------+
                                              |
                                              v
+---------------------------------------------------------------------------------------+
|  Domain & Business Logic Layer                                           [Member A]   |
|  - Core Entities: Trade (Value Object, operator overloads)                            |
|  - Validation Engine: Form & range validation with custom ValidationError             |
|  - Analytics Engine: Move-only Journal aggregating IMetric polymorphic strategies      |
|  - Metrics: WinRate, ProfitFactor, MaxDrawdown, TotalPnL, Equity Curve                |
|  - Generic Algorithms: Templated sumProfit with lambda predicates                     |
+---------------------------------------------------------------------------------------+
                                              |
                                              v
+---------------------------------------------------------------------------------------+
|  Data Access & Persistence Layer                                         [Member B]   |
|  - Repository Abstraction: ITradeRepository (Interface)                               |
|  - Implementations: SqliteTradeRepository (Production) & InMemoryTradeRepository (Mock)|
|  - RAII Wrappers: Database (sqlite3*), Statement (sqlite3_stmt*)                     |
|  - User Data Access: UserRepository                                                   |
+---------------------------------------------------------------------------------------+
                                              |
                                              v
                      +-------------------------------------------------+
                      |             SQLite Database Storage             |
                      |        trading_journal.db (Foreign Keys ON)     |
                      +-------------------------------------------------+
```

### Architectural Benefits
- **Testability**: The Domain and Analytics classes (`Trade`, `Journal`, `IMetric`) have no dependencies on Crow or SQLite, permitting isolated unit testing in microseconds.
- **Interchangeability**: The Repository pattern permits swapping SQLite for an in-memory test database or a distributed SQL engine without modifying a single line of business logic.
- **Fail-Safe Resource Handling**: Database handles and prepared statements are scoped to stack frames, ensuring automatic deallocation even when exceptions are thrown.

---

## 3. Core Features

| Feature | Subsystem | Description |
| :--- | :--- | :--- |
| **Authentication & Accounts** | Web & Security | User registration, login, and logout. Passwords hashed using memory-hard Argon2id. |
| **Trade Ledger & CRUD** | Web & Data | Complete trade management: Ticker, Direction (Long/Short), Entry/Exit Price, Quantity, Fees, Date, and Notes. |
| **Multi-Criteria Filtering** | Domain & Web | Filter trades dynamically by ticker symbol or trade outcome (Wins vs. Losses). |
| **Performance Dashboard** | Analytics & Domain | Real-time calculation of Total P&L, Win Rate, Profit Factor, Max Drawdown, Average Win, and Average Loss. |
| **Interactive Equity Curve** | Analytics & Frontend | Time-series cumulative equity curve plotted using Chart.js from a JSON API endpoint. |
| **RFC 4180 CSV Export** | Domain & Web | Export user trades into standardized CSV files compatible with Excel, Google Sheets, or R. |

---

## 4. Mathematical & Financial Formulas

The analytics engine processes sorted trade sequences to compute standard financial risk-and-return metrics:

1. **Trade Profit & Loss ($P$)**:
   $$\text{Profit}_{\text{Long}} = (\text{Exit Price} - \text{Entry Price}) \times \text{Quantity} - \text{Fees}$$
   $$\text{Profit}_{\text{Short}} = (\text{Entry Price} - \text{Exit Price}) \times \text{Quantity} - \text{Fees}$$

2. **Win Rate ($WR$)**:
   $$WR = \left(\frac{N_{\text{wins}}}{N_{\text{total}}}\right) \times 100\%$$

3. **Profit Factor ($PF$)**:
   $$PF = \frac{\sum \text{Gross Profits}}{\sum |\text{Gross Losses}|}$$
   *(Returns $-1.0$ / "N/A" if total gross loss equals zero to avoid division by zero).*

4. **Equity Curve ($E_k$)**:
   $$E_k = \sum_{i=1}^{k} P_i \quad \text{for } k = 1, 2, \dots, N \quad (E_0 = 0)$$

5. **Maximum Drawdown ($MDD$)**:
   $$\text{Peak}_k = \max_{1 \le i \le k} (E_i)$$
   $$DD_k = \text{Peak}_k - E_k$$
   $$MDD = \max_{1 \le k \le N} (DD_k)$$

---

## 5. Technology Stack & Dependencies

| Component | Technology | Rationale & Selection Criteria |
| :--- | :--- | :--- |
| **Core Language** | **C++17** | Modern language idioms (`std::optional`, `std::unique_ptr`, structured bindings, lambdas, type safety). |
| **Web Microframework**| **Crow** (C++17) | Lightweight routing, HTTP/1.1 parsing, cookie session middleware, Mustache template engine. |
| **Database Engine** | **SQLite 3** | Serverless, zero-configuration, transactional relational database accessible via C-API. |
| **Password Hashing** | **libsodium** | Cryptographically secure Argon2id implementation (`crypto_pwhash_str`) resistant to GPU/ASIC attacks. |
| **Build System** | **CMake** (>= 3.16) | Standard cross-platform configuration supporting Linux, macOS, and Windows. |
| **Unit Testing** | **GoogleTest (GTest)**| Industrial-grade xUnit testing framework for C++. |
| **Frontend Styling** | **Bootstrap 5** | Clean responsive layouts, dark/light styling, modal alerts, and tables. |
| **Data Visualization**| **Chart.js** | Client-side HTML5 canvas charting for interactive time-series equity curves. |

---

## 6. Database Schema Design

The SQLite relational database (`trading_journal.db`) enforces relational integrity via `PRAGMA foreign_keys = ON`:

```sql
-- Users Table
CREATE TABLE IF NOT EXISTS users (
    id            INTEGER PRIMARY KEY AUTOINCREMENT,
    username      TEXT UNIQUE NOT NULL,
    password_hash TEXT NOT NULL,
    created_at    DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- Trades Table
CREATE TABLE IF NOT EXISTS trades (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id     INTEGER NOT NULL,
    ticker      TEXT NOT NULL,
    direction   TEXT NOT NULL CHECK(direction IN ('Long', 'Short')),
    entry_price REAL NOT NULL CHECK(entry_price > 0),
    exit_price  REAL NOT NULL CHECK(exit_price > 0),
    quantity    INTEGER NOT NULL CHECK(quantity > 0),
    fees        REAL DEFAULT 0.0 CHECK(fees >= 0),
    trade_date  TEXT NOT NULL, -- Stored as ISO-8601 (YYYY-MM-DD) for alphabetical sorting
    notes       TEXT,
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS idx_trades_user_date ON trades(user_id, trade_date);
```

---

## 7. Project File Structure

```
trading_journal/
├── CMakeLists.txt                         [Build] CMake build configuration & dependency linking
├── demo.sh                                [Script] Automated build, seed, and live demo launcher
├── README.md                              [Docs]  Complete project documentation & evaluation guide
├── .gitignore                             [Config] Git build artifact ignore rules
│
├── include/                               [C++ Header Files / Contracts]
│   ├── errors.hpp                         [A] Custom exception hierarchy (AppError, ValidationError, DatabaseError)
│   ├── trade.hpp                          [A] Trade entity, Direction enum, TradeFilter value struct
│   ├── metrics.hpp                        [A] IMetric strategy interface + WinRate, ProfitFactor, MaxDrawdown, TotalPnL
│   ├── journal.hpp                        [A] Move-only Journal class and templated sumProfit algorithm
│   ├── validators.hpp                     [A] Business input validation rules and sanitation
│   ├── database.hpp                       [B] RAII sqlite3* connection wrapper
│   ├── statement.hpp                      [B] RAII sqlite3_stmt* prepared statement wrapper
│   ├── user_repository.hpp                [B] UserRepository interface and SQLite implementation
│   ├── trade_repository.hpp               [B] ITradeRepository interface and SqliteTradeRepository
│   ├── in_memory_trade_repository.hpp     [B] Mock trade repository for automated testing
│   ├── auth_service.hpp                   [C] Argon2id password hashing and validation service
│   ├── middleware.hpp                     [C] AuthGuard session middleware and user extraction
│   └── routes.hpp                         [C] Route handler signatures and dependency injection container (Deps)
│
├── src/                                   [C++ Implementation Files]
│   ├── trade.cpp                          [A] Trade calculations, operator< (date sort), operator<<, string conversion
│   ├── metrics.cpp                        [A] Mathematical financial metric algorithm implementations
│   ├── journal.cpp                        [A] Journal analytics, polymorphic computeAll, and equity curve logic
│   ├── validators.cpp                     [A] Server-side field validation implementation
│   ├── routes_analytics.cpp               [A] Web routes for dashboard metrics and equity-curve JSON API
│   ├── database.cpp                       [B] SQLite database connection lifecycle and schema setup
│   ├── statement.cpp                      [B] Statement binding, step execution, and typed column extraction
│   ├── user_repository.cpp                [B] User query execution and optional retrieval
│   ├── trade_repository.cpp               [B] SQLite trade queries with parameterized user isolation
│   ├── in_memory_trade_repository.cpp     [B] In-memory mock storage implementation
│   ├── auth_service.cpp                   [C] Argon2id cryptographic hashing via libsodium
│   ├── middleware.cpp                     [C] Session verification and unauthenticated redirection
│   ├── routes_auth.cpp                    [C] Login, user registration, and logout web handlers
│   ├── routes_trades.cpp                  [C] Trade CRUD endpoints, filtering, and CSV export
│   └── main.cpp                           [C] Entry point, dependency composition, and Crow server start
│
├── templates/                             [HTML5 / Mustache View Templates]
│   ├── base.html                          [C] Main layout skeleton, navigation bar, and responsive containers
│   ├── login.html                         [C] User authentication form
│   ├── register.html                      [C] User registration form
│   ├── trades.html                        [C] Filterable trade history table with badges
│   ├── trade_form.html                    [C] Add and edit trade input form
│   └── dashboard.html                     [A] Analytics overview with KPI metric cards and chart canvas
│
├── static/                                [Static Assets]
│   ├── css/style.css                      [C] Custom styling, dark mode accents, card elevations
│   └── js/charts.js                       [A] Chart.js configuration, asynchronous fetch, and rendering
│
├── sql/                                   [Database Scripts]
│   ├── schema.sql                         [B] DDL schema definition for users and trades
│   └── seed.sql                           [A] 20 realistic historical trades for testing & demonstration
│
└── third_party/
    └── crow/crow_all.h                    [External] Header-only Crow web framework
```

---

## 8. C++ & Object-Oriented Concepts Matrix

This project deliberately incorporates core and advanced C++ concepts required in academic evaluation:

| OOP / C++ Concept | Applied In | Implementation Details | Contributor |
| :--- | :--- | :--- | :--- |
| **Classes & Encapsulation** | `Trade`, `Journal`, `Database` | Strict private state variables exposed only via `const` getters and controlled business methods. | Member A, B |
| **Const-Correctness** | `Trade::profit()`, `IMetric::compute()`, `Journal` | Methods that do not modify member variables are marked `const`, ensuring referential transparency. | Member A |
| **Enum Classes & Static Helpers**| `Trade::Direction` | Strongly-typed `enum class Direction { Long, Short }` with static conversion helpers. | Member A |
| **Abstract Interfaces** | `IMetric`, `ITradeRepository` | Pure virtual functions (`= 0`) defining strict API contracts without exposing implementation. | Member A, B |
| **Runtime Polymorphism** | Strategy Pattern in `Journal` | `Journal` executes heterogeneous metrics (`WinRate`, `ProfitFactor`, etc.) via `IMetric*` base pointers. | Member A |
| **Virtual Destructors** | `IMetric`, `ITradeRepository` | `virtual ~IMetric() = default;` prevents undefined behavior and memory leaks during polymorphic deletion. | Member A, B |
| **Smart Pointers & Ownership** | `Journal`, `Deps` | `std::unique_ptr<IMetric>` models exclusive ownership; `std::shared_ptr<Database>` models shared access. | Member A, B, C |
| **Rule of Five & Move Semantics**| `Journal`, `Database`, `Statement`| Copy constructors deleted to protect non-sharable resources; move constructors implemented for efficient transfer. | Member A, B |
| **Operator Overloading** | `Trade` | `operator<` (chronological date sorting for `std::sort`), `operator==`, and `operator<<` for logging. | Member A |
| **Function Templates & Lambdas**| `sumProfit()` in `journal.hpp` | Templated generic accumulator instantiated with lambda predicates to compute gross profits and losses. | Member A |
| **Exception Hierarchy** | `errors.hpp` | Custom domain exceptions (`AppError` -> `ValidationError`, `DatabaseError`) extending `std::runtime_error`. | Member A |
| **RAII (Resource Acquisition)** | `Database`, `Statement` | `sqlite3*` and `sqlite3_stmt*` handles are automatically closed in destructors when leaving scope. | Member B |
| **Modern Optional Return** | `UserRepository`, `TradeRepository` | `std::optional<Trade>` clearly models nullable queries without sentinel values or raw null pointers. | Member B, C |
| **C-Library Interoperability** | `Statement`, `AuthService` | Clean bridging of C APIs (`sqlite3_*`, libsodium `crypto_pwhash_*`) into exception-safe C++ paradigms. | Member B, C |
| **Dependency Injection** | `Deps` container in `main.cpp` | Repositories and services are instantiated once and injected into route handlers as interfaces. | Member C |

---

## 9. Key Code Implementations

### A. Polymorphic Strategy Pattern (`include/metrics.hpp` & `src/metrics.cpp`)
```cpp
// Abstract Metric Strategy Interface
class IMetric {
public:
    virtual ~IMetric() = default; // Essential virtual destructor
    virtual std::string name() const = 0;
    virtual double compute(const std::vector<Trade>& trades) const = 0;
};

// Concrete Strategy: Maximum Drawdown Calculation
class MaxDrawdown final : public IMetric {
public:
    std::string name() const override { return "Max Drawdown"; }

    double compute(const std::vector<Trade>& trades) const override {
        double equity = 0.0, peak = 0.0, maxDd = 0.0;
        for (const auto& trade : trades) {
            equity += trade.profit();
            peak = std::max(peak, equity);
            maxDd = std::max(maxDd, peak - equity);
        }
        return maxDd;
    }
};
```

### B. Move-Only Resource Container (`include/journal.hpp`)
```cpp
class Journal {
public:
    explicit Journal(std::vector<Trade> trades);

    // Rule of Five: Move-only semantics
    Journal(const Journal&) = delete;
    Journal& operator=(const Journal&) = delete;
    Journal(Journal&&) noexcept = default;
    Journal& operator=(Journal&&) noexcept = default;
    ~Journal() = default;

    // Polymorphic computation across all registered metrics
    std::vector<std::pair<std::string, double>> computeAll() const {
        std::vector<std::pair<std::string, double>> results;
        for (const auto& metric : metrics_) {
            results.emplace_back(metric->name(), metric->compute(trades_));
        }
        return results;
    }

private:
    std::vector<Trade> trades_;
    std::vector<std::unique_ptr<IMetric>> metrics_;
};
```

### C. RAII SQLite Prepared Statement Wrapper (`include/statement.hpp`)
```cpp
class Statement {
public:
    Statement(sqlite3* db, const std::string& sql) {
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt_, nullptr) != SQLITE_OK) {
            throw DatabaseError(sqlite3_errmsg(db));
        }
    }

    // Destructor guarantees statement cleanup even if exceptions occur
    ~Statement() {
        if (stmt_) sqlite3_finalize(stmt_);
    }

    // Non-copyable, movable
    Statement(const Statement&) = delete;
    Statement& operator=(const Statement&) = delete;
    Statement(Statement&& other) noexcept : stmt_(other.stmt_) { other.stmt_ = nullptr; }

    void bindInt(int index, int value) { sqlite3_bind_int(stmt_, index, value); }
    void bindText(int index, const std::string& val) {
        sqlite3_bind_text(stmt_, index, val.c_str(), -1, SQLITE_TRANSIENT);
    }
    bool step() { return sqlite3_step(stmt_) == SQLITE_ROW; }

private:
    sqlite3_stmt* stmt_{nullptr};
};
```

---

## 10. Installation, Build & Demonstration Guide

### Prerequisites

#### Linux (Ubuntu / Debian / WSL)
```bash
sudo apt update
sudo apt install build-essential cmake libsqlite3-dev libsodium-dev libasio-dev libgtest-dev sqlite3
```

#### macOS (via Homebrew)
```bash
brew install cmake sqlite libsodium asio googletest
```

---

### Automated One-Click Build & Demo Script

The repository includes a self-contained execution script that compiles the project, initializes the SQLite database schema, loads 20 historical trades, and launches the web server:

```bash
./demo.sh
```

#### Demo Access Credentials
- **URL**: [http://localhost:18080](http://localhost:18080)
- **Demo Username**: `demo`
- **Demo Password**: `Password123!`

---

### Manual Compilation & Execution

If you prefer building manually via CMake:

```bash
# 1. Configure release build
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release

# 2. Build executable
cmake --build build -j4

# 3. Initialize SQLite database & seed demo data
sqlite3 trading_journal.db < sql/schema.sql
sqlite3 trading_journal.db < sql/seed.sql

# 4. Start the application
./build/trading_journal
```

---

## 11. Testing & Memory Verification

### Unit Testing Suite
The analytical core and business validators are tested using GoogleTest:

```bash
# Run tests via CTest
ctest --test-dir build --output-on-failure
```

#### Key Test Scenarios Covered
1. **Trade Domain Math**: Long vs. Short profit formulas, fee deductions, equality comparison, and chronological sorting.
2. **Metric Strategy Algorithms**: Correct mathematical outcomes on hand-calculated trading scenarios; division-by-zero protection in `ProfitFactor`.
3. **Polymorphic Execution**: Dynamically registering custom metrics into `Journal` using `addMetric()` and validating runtime computation.
4. **Validation Logic**: Verification that invalid quantities ($q \le 0$), negative prices, and bad date formats reliably throw `ValidationError` with targeted field identifiers.
5. **Persistence Isolation**: Verifying that queries for User 1 never leak records belonging to User 2.

### Memory Leak & Undefined Behavior Sanitization
To demonstrate industrial-grade memory safety, the build can be compiled with Clang / GCC AddressSanitizer (ASan) and UndefinedBehaviorSanitizer (UBSan):

```bash
cmake -B build_san -S . -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined -g"
cmake --build build_san -j4
./build_san/trading_journal
```
*Result: Zero memory leaks, zero heap-buffer overflows, and clean deallocation upon shutdown.*

---

## 12. Security Architecture

1. **Argon2id Password Storage**: User passwords are never saved in cleartext. Hashes are generated using libsodium's `crypto_pwhash_str`, which applies salt, memory-hardness, and time-cost iterations to prevent dictionary and rainbow-table attacks.
2. **SQL Injection Immunization**: Direct SQL string interpolation is strictly prohibited. All user inputs are bound to prepared statements using parameterized `sqlite3_bind_*` calls.
3. **Multi-Tenant Data Isolation**: Every database read, update, or deletion query enforces `WHERE user_id = ?`, guaranteeing that users cannot inspect or manipulate other traders' records.
4. **Session Security & XSS Mitigation**: Session authentication cookies are tagged with `HttpOnly` attributes. Dynamic variables rendered in Mustache templates are escaped by default (`{{ var }}`).

---

## 13. Team Ownership Breakdown

### Member A: Utkarsh Kumar (Roll: 25008288 / 41)
- **Assigned Module**: Domain Logic, Analytics Engine & Frontend Integration
- **Key Deliverables**:
  - `include/errors.hpp`: Complete project exception hierarchy (`AppError`, `ValidationError`, `DatabaseError`).
  - `include/trade.hpp` & `src/trade.cpp`: Value object `Trade`, `operator<`, `operator==`, and string conversions.
  - `include/metrics.hpp` & `src/metrics.cpp`: Abstract `IMetric` and concrete implementations (`WinRate`, `ProfitFactor`, `MaxDrawdown`, `TotalPnL`).
  - `include/journal.hpp` & `src/journal.cpp`: Move-only `Journal`, `computeAll()`, equity curve generation, and generic `sumProfit` template.
  - `include/validators.hpp` & `src/validators.cpp`: Input validators for all trade form inputs.
  - `src/routes_analytics.cpp`: Dashboard route handler and equity-curve JSON API.
  - `templates/dashboard.html` & `static/js/charts.js`: Interactive Chart.js analytics dashboard.
  - `sql/seed.sql`: 20 realistic historical trades for demonstration.

### Member B: Pinkesh Kumar Mishra (Roll: 25008253 / 07)
- **Assigned Module**: Data Persistence & RAII Infrastructure
- **Key Deliverables**:
  - `include/database.hpp` & `src/database.cpp`: RAII wrapper for `sqlite3*` connection lifecycle management.
  - `include/statement.hpp` & `src/statement.cpp`: RAII wrapper for `sqlite3_stmt*` prepared statement binding and row fetching.
  - `include/trade_repository.hpp` & `src/trade_repository.cpp`: `ITradeRepository` interface and `SqliteTradeRepository` implementation with strict user isolation.
  - `include/in_memory_trade_repository.hpp` & `src/in_memory_trade_repository.cpp`: Standalone mock repository for decoupled unit testing.
  - `include/user_repository.hpp` & `src/user_repository.cpp`: User retrieval and registration queries.
  - `sql/schema.sql`: Relational database schema with constraints and index creation.

### Member C: Rabeekant (Roll: 25008259 / 13)
- **Assigned Module**: Web Framework, Authentication & Application Lifecycle
- **Key Deliverables**:
  - `src/main.cpp`: Crow application orchestration, dependency container composition (`Deps`), and route binding.
  - `include/auth_service.hpp` & `src/auth_service.cpp`: User authentication and Argon2id password hashing via libsodium.
  - `include/middleware.hpp` & `src/middleware.cpp`: `AuthGuard` middleware and session cookie management.
  - `include/routes.hpp`: Unified route registrations and dependency injection structures.
  - `src/routes_auth.cpp`: Web handlers for user registration, login, and logout.
  - `src/routes_trades.cpp`: Trade CRUD endpoints, filtering, and RFC 4180 CSV export.
  - `templates/base.html`, `login.html`, `register.html`, `trades.html`, `trade_form.html`, and `static/css/style.css`.
  - `CMakeLists.txt` & `demo.sh`: Build scripts and system demonstration launcher.

---

## 14. Viva Defense & Evaluation Guide

This section outlines technical questions and rationales prepared for project viva examination:

#### Q1: Why is `profit()` marked as `const`, and why does `Trade` overload `operator<`?
> **Answer**: `profit()` calculates and returns a value based purely on existing attributes without mutating the object's internal state. Marking it `const` guarantees const-correctness and permits calling it on `const Trade&` references. `operator<` compares trades chronologically by date (`tradeDate_ < o.tradeDate_`), allowing `std::sort` to order trades prior to calculating time-series equity curves and maximum drawdowns.

#### Q2: Why does `IMetric` require a virtual destructor?
> **Answer**: `Journal` manages a collection of derived metric instances (`WinRate`, `ProfitFactor`, etc.) through base class pointers (`std::unique_ptr<IMetric>`). If the base class destructor were non-virtual, deleting a derived object via a base pointer would trigger undefined behavior and fail to invoke the derived destructor, resulting in potential resource leaks.

#### Q3: Why is the `Journal` class move-only (copy constructor deleted)?
> **Answer**: `Journal` owns a vector of `std::unique_ptr<IMetric>`. Because `std::unique_ptr` cannot be copied (it models exclusive ownership), copying a `Journal` is illegal. Following the C++ Rule of Five, we explicitly delete the copy constructor and copy assignment operator (`= delete`) and provide defaulted move operations (`= default`) to allow transferring ownership efficiently without heap duplication.

#### Q4: How does RAII in the `Database` and `Statement` classes prevent resource leaks?
> **Answer**: In C APIs like SQLite, developers must pair every `sqlite3_open` with `sqlite3_close` and every `sqlite3_prepare_v2` with `sqlite3_finalize`. If an exception is thrown mid-execution, manual cleanup code is bypassed. By encapsulating these handles within C++ classes whose destructors automatically invoke the cleanup functions, C++ stack unwinding guarantees that resources are released under all exit paths.

#### Q5: What is the benefit of the Strategy Pattern in financial metric computation?
> **Answer**: The Strategy Pattern satisfies the Open/Closed Principle (OCP). New metrics (such as Sharpe Ratio, Sortino Ratio, or Calmar Ratio) can be added simply by creating a new class deriving from `IMetric` without modifying the `Journal` class or touching existing calculation logic.

#### Q6: How does `InMemoryTradeRepository` help in development and testing?
> **Answer**: By coding to the `ITradeRepository` interface rather than a concrete SQLite class (Dependency Inversion Principle), business logic can be tested in memory without disk I/O, file locks, or external database setups. It also unblocked parallel development of the Domain and Web layers before the database layer was finalized.

---

## 15. Limitations & Future Scope

### Current Limitations
- **Single Currency Assumption**: All entries assume a unified base currency (e.g., USD or INR) without cross-currency exchange rate conversions.
- **Asset Scope**: Tailored for cash equities and linear instruments; complex non-linear derivatives (options Greeks, multi-leg spreads) are not currently modeled.
- **Server Deployment**: Designed for local execution (`localhost:18080`) over HTTP; production deployment would require TLS/HTTPS termination and CSRF protection.

### Future Scope
- **Advanced Quantitative Metrics**: Implementation of Sharpe Ratio, Sortino Ratio, Profit-to-MaxDD ratio, and consecutive win/loss streaks.
- **Emotional & Strategy Tagging**: Categorizing trades by psychological state (e.g., Disciplined, FOMO, Revenge) and strategy playbook.
- **Live WebSocket Market Feeds**: Automated real-time mark-to-market valuations using asynchronous WebSockets.
- **Database Scalability**: Migration path from SQLite to PostgreSQL for high-concurrency multi-user production environments.
- **Containerization**: Dockerfile and Docker Compose support for zero-dependency containerized deployments.

---

## 16. License

This project is developed for academic evaluation under the **MIT License**.
