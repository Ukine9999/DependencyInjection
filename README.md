# DependencyInjection

> An attribute-style dependency-injection container for modern C++.

![C++20](https://img.shields.io/badge/C%2B%2B-20-00599C?logo=cplusplus)
![CMake](https://img.shields.io/badge/CMake-%E2%89%A5%203.24-064F8C?logo=cmake)
![Tests](https://img.shields.io/badge/tests-Catch2-brightgreen)

This is a port of a DI container I've used in EmptySolutions, rebuilt for C++. A lot of the API is a direct translation of the original.

```cpp
Di::Container::DiContainer container;                       // discovers every registered service
auto greeter = container.Provider().ResolveService<IGreeter>();
std::cout << greeter->Greet() << '\n';
```

## Features

- Constructor injection — interfaces stay plain abstract classes, no DI annotations on them
- Four lifetimes: `Singleton`, `LazySingleton`, `Scoped`, `Transient`
- Dependency wrappers for the common cases: required, keyed, optional, collection, factory, lazy
- Keyed registrations and multiple implementations of one interface
- Decorators for cross-cutting concerns (logging, caching, retry), without touching the implementation or its callers
- Scopes with deterministic disposal in reverse creation order
- Construction-free validation of the whole graph, plus always-on cycle detection
- Module/anchor discovery that survives static-library boundaries
- A manual registration API for third-party types you can't annotate

## Quick start

An interface is a plain abstract class — it's never annotated. The descriptor goes on the implementation instead.

```cpp
// IClock.hpp
struct IClock
{
    virtual ~IClock() = default;
    virtual std::string Now() const = 0;
};

// IGreeter.hpp
struct IGreeter
{
    virtual ~IGreeter() = default;
    virtual std::string Greet() const = 0;
};
```

```cpp
// SystemClock.hpp / Greeter.hpp — implementations
class SystemClock final : public IClock
{
public:
    std::string Now() const override { return "12:00"; }
};

class Greeter final : public IGreeter
{
public:
    explicit Greeter(std::shared_ptr<IClock> clock) : m_Clock(std::move(clock)) {}
    std::string Greet() const override { return "Hello at " + m_Clock->Now(); }

private:
    std::shared_ptr<IClock> m_Clock;
};
```

Describe each implementation once, in its registration translation unit:

```cpp
// AppModule.cpp
#include "Di/Registry/DiDescribe.hpp"
#include "Di/Registry/DiModuleAnchor.hpp"

DI_MODULE_ANCHOR(App)

DI_DESCRIBE(SystemClock)
{
    DI_SERVICE(IClock);
    DI_LIFETIME(Singleton);
    DI_MODULE(App);
};

DI_DESCRIBE(Greeter)
{
    DI_SERVICE(IGreeter);
    DI_DEPENDS(Di::Service<IClock>);   // constructor parameters, in order
    DI_LIFETIME(Singleton);
    DI_MODULE(App);
};
```

Resolve from the composition root:

```cpp
// main.cpp
#include "Di/Container/DiContainer.hpp"

DI_USE_MODULE(App)

int main()
{
    Di::Container::DiContainer container;
    auto greeter = container.Provider().ResolveService<IGreeter>();
    std::cout << greeter->Greet() << '\n';   // Hello at 12:00
}
```

Descriptor macros, for reference:

| Macro | Meaning | Default |
|---|---|---|
| `DI_SERVICE(I)` | interface the type is registered as | the type itself |
| `DI_DEPENDS(...)` | constructor parameters, as wrappers | none |
| `DI_EXPOSE(...)` | extra interfaces the type also answers for | none |
| `DI_LIFETIME(name)` | `Singleton` / `LazySingleton` / `Scoped` / `Transient` | `Transient` |
| `DI_KEY("text")` | named registration | none |
| `DI_ORDER(n)` | ordering hint | `0` |
| `DI_MODULE(Name)` | self-register into module `Name` | — |

## Installation

Builds as a static library target, `Di::DependencyInjection`. Simplest integration is a subdirectory:

```cmake
add_subdirectory(third_party/DependencyInjection)
target_link_libraries(my_app PRIVATE Di::DependencyInjection)
```

Or with `FetchContent`:

```cmake
include(FetchContent)
FetchContent_Declare(DependencyInjection
    GIT_REPOSITORY https://github.com/Ukine9999/DI.git
    GIT_TAG        main)
FetchContent_MakeAvailable(DependencyInjection)
target_link_libraries(my_app PRIVATE Di::DependencyInjection)
```

Modules built as their own static libraries should use the CMake helpers below so their self-registrations survive the linker (see [Modules and anchors](#modules-and-anchors)).

## Lifetimes

| Lifetime | Instances | Built |
|---|---|---|
| `Singleton` | one, shared with the root | eagerly, when the container is created |
| `LazySingleton` | one, shared with the root | on first resolve |
| `Scoped` | one per scope | on first resolve within that scope |
| `Transient` | a fresh instance every resolve | on every resolve |

```cpp
DI_DESCRIBE(Cache)    { DI_SERVICE(ICache);   DI_LIFETIME(Singleton);     DI_MODULE(App); };
DI_DESCRIBE(Session)  { DI_SERVICE(ISession); DI_LIFETIME(Scoped);        DI_MODULE(App); };
DI_DESCRIBE(Command)  { DI_SERVICE(ICommand); DI_LIFETIME(Transient);     DI_MODULE(App); };
```

## Constructor dependencies

`DI_DEPENDS` lists one wrapper per constructor parameter, in order:

| Wrapper | Constructor parameter | Meaning |
|---|---|---|
| `Di::Service<T>` | `std::shared_ptr<T>` | required dependency |
| `Di::Keyed<T, DiKey("k")>` | `std::shared_ptr<T>` | required, resolved by key |
| `Di::All<T>` | `std::vector<std::shared_ptr<T>>` | every registered / exposed `T` |
| `Di::Optional<T>` | `std::shared_ptr<T>` (may be null) | resolved if present, else null |
| `Di::Factory<T>` | `std::function<std::shared_ptr<T>()>` | on-demand creator |
| `Di::Lazy<T>` | `Di::DiLazy<T>` | deferred first-`Get()`; also breaks cycles |

```cpp
class Report final : public IReport
{
public:
    Report(std::shared_ptr<IClock> clock,
           std::vector<std::shared_ptr<ISection>> sections,
           Di::DiLazy<IRenderer> renderer);
};

DI_DESCRIBE(Report)
{
    DI_SERVICE(IReport);
    DI_DEPENDS(Di::Service<IClock>, Di::All<ISection>, Di::Lazy<IRenderer>);
    DI_MODULE(App);
};
```

## Keyed services and multiple implementations

Several implementations of one interface can coexist, told apart by key:

```cpp
DI_DESCRIBE(ConsoleLogger) { DI_SERVICE(ILogger); DI_KEY("Console"); DI_LIFETIME(Singleton); DI_MODULE(App); };
DI_DESCRIBE(StreamLogger)  { DI_SERVICE(ILogger); DI_KEY("Stream");  DI_LIFETIME(Singleton); DI_MODULE(App); };
```

```cpp
auto stream = provider.ResolveService<ILogger>("Stream");    // -> StreamLogger
auto all    = provider.ResolveServices<ILogger>();           // -> both, keys ignored
```

`DI_EXPOSE` lets a type answer for an extra base interface too, so `Di::All<>` picks it up even when its primary `DI_SERVICE` is something more specific:

```cpp
DI_DESCRIBE(FileLogger) { DI_SERVICE(IFileLogger); DI_EXPOSE(ILogger); DI_LIFETIME(Singleton); DI_MODULE(App); };
// A consumer with DI_DEPENDS(Di::All<ILogger>) receives FileLogger among all ILoggers.
```

This is the same pattern the original C# version used for startup hooks, validators, and message handlers: register many implementations of one interface (keyed or not), then pull the whole set with `Di::All<T>` or `ResolveServices<T>()` — while `Di::Service<T>` / `Di::Keyed<T, ...>` still bind one specific registration, so the same implementations stay individually addressable by key when you need just one.

```cpp
struct IInitializable
{
    virtual ~IInitializable() = default;
    virtual void Initialize() = 0;
};

DI_DESCRIBE(DatabaseWarmup) { DI_SERVICE(IInitializable); DI_KEY("Database"); DI_LIFETIME(Singleton); DI_MODULE(App); };
DI_DESCRIBE(CacheWarmup)    { DI_SERVICE(IInitializable); DI_KEY("Cache");    DI_LIFETIME(Singleton); DI_MODULE(App); };

for (const auto& initializable : container.Provider().ResolveServices<IInitializable>())
    initializable->Initialize();

auto cacheWarmup = container.Provider().ResolveService<IInitializable>("Cache");   // still addressable by key
```

## Decorators

A decorator wraps a resolved service in another implementation of the same interface. The decorator's first constructor parameter is the wrapped service; `DI_DEPENDS` lists anything else it needs:

```cpp
class LoggingController final : public IController
{
public:
    LoggingController(std::shared_ptr<IController> inner, std::shared_ptr<ILogger> logger);
};

DI_DECORATE(LoggingController)
{
    DI_SERVICE(IController);           // the interface being decorated
    DI_DEPENDS(Di::Service<ILogger>);  // extra dependencies, after the wrapped service
    DI_DECORATOR(App);
};
```

Decorators apply in registration order — last registered is outermost. In a manual composition root they can also be added with `registry.Decorate<LoggingController>()`. A decorated service shouldn't also use `DI_EXPOSE`.

## Scopes and disposal

A scope is a child provider with its own lifetime for `Scoped` services. Singletons stay shared with the root; transients are still fresh on every resolve.

```cpp
{
    auto scope = container.CreateScope();
    auto unitOfWork = scope->ResolveService<IUnitOfWork>();   // one per scope
}   // scope ends here
```

When a scope (or the container itself, for singletons) is destroyed, every created service implementing `Di::Services::Abstraction::IDiDisposable` has `Dispose()` called in reverse creation order:

```cpp
class UnitOfWork final : public IUnitOfWork, public Di::Services::Abstraction::IDiDisposable
{
public:
    void Dispose() override { m_Transaction.Rollback(); }
};

DI_DESCRIBE(UnitOfWork) { DI_SERVICE(IUnitOfWork); DI_LIFETIME(Scoped); DI_MODULE(App); };
```

## Modules and anchors

Self-registration lives in static initializers. In a static library, the linker drops object files nothing references — taking their registrations with them. Group a module's descriptors in one registration translation unit that anchors itself, and pull it in from the composition root:

```cpp
// App/AppModule.cpp — the module's registration translation unit
#include "Di/Registry/DiModuleAnchor.hpp"

DI_MODULE_ANCHOR(App)

DI_DESCRIBE(Greeter) { /* ... */ DI_MODULE(App); };
DI_DESCRIBE(Clock)   { /* ... */ DI_MODULE(App); };
```

```cpp
// main.cpp — composition root
DI_USE_MODULE(App)
```

`DI_USE_MODULE(App)` references the module's anchor symbol through a dynamic initializer, forcing the linker to keep the whole translation unit. When each module is its own static library, wire them with the CMake helpers so retention is guaranteed:

```cmake
di_add_module(App SOURCES App/AppModule.cpp)   # requires exactly one *Anchor.cpp
di_module_link(my_app App)                     # honours the per-module whole-archive fallback
```

## Registering third-party types

For types you can't annotate, register them into a `DiRegistry` and build a container from it:

```cpp
Di::Registry::DiRegistry registry;
registry.AddInstance<IClock>(std::make_shared<SystemClock>());               // a pre-built instance
registry.AddType<Greeter, IGreeter, Di::Service<IClock>>();                  // a type with explicit deps
registry.AddFactory<IConnection>(EDiServiceLifetime::Singleton,
    [](Di::Services::Abstraction::IDiServiceProvider&) { return OpenConnection(); });
registry.Decorate<LoggingController>();

Di::Container::DiContainer container(registry);
```

The same `DiRegistry` API backs the attribute macros, so annotated and manual registrations mix freely.

## Validation and safety

Building a `DiContainer` validates the whole graph before constructing anything, and throws `std::runtime_error` with a precise message if it finds:

- a required dependency nothing provides (this covers lazy/scoped/transient services that eager singleton construction would never reach on its own);
- a dependency cycle — edges through `Di::Factory<T>` and `Di::Lazy<T>` are treated as cycle-breaking, since they defer the resolve, so a graph that only closes a loop through one of those validates clean, matching the runtime behavior exactly.

This runs automatically whether the container is built from auto-discovery (`DiContainer container;`) or an explicit registry (`DiContainer container(registry);`), and constructs no instances, so it's cheap.

Two more guarantees hold independent of validation: cycle detection is always on at runtime (an `A → B → A` chain throws instead of overflowing the stack), and a singleton depending on a scoped service is rejected as a captive dependency (a singleton would otherwise outlive the per-scope instance it captured).

You can also validate a registry on its own:

```cpp
Di::Registry::ValidateOrThrow(Di::Registry::DiRegistry::Process());
```

## How it works

Registration and resolution are split into three phases. `DI_DESCRIBE` specializes a `DiDescriptor<T>` trait, and a static registrar records a `DiRegistrationRow` (service type, key, lifetime, constructor, bindings, dependency list) — that's the *describe* phase. A reader then sorts rows by order, applies decorators, and builds an `IDiServiceScheme` per registration — *compile*. The service provider owns instances per lifetime, dispatches by scheme, and hands back correctly-typed `shared_ptr`s — *resolve*.

The C# original leans on reflection to walk constructors and read attributes at runtime; there's no equivalent at compile time in C++, so this port replaces it with dependency wrappers folded over a variadic pack (`Di::Service<T>`, `Di::Keyed<T, ...>`, and so on each expose the same static interface) to synthesize the constructor call, plus explicit template specialization instead of attribute reflection to attach a descriptor to a type. The core stays type-erased (`shared_ptr<void>` + `std::function`) so the machinery itself doesn't need to know concrete types, with typed casts only at the edges where a caller actually asks for `T`. The source under [`src/Di`](src/Di) is organized to match these three phases.

## Building and testing

```bash
cmake --preset ninja-debug
cmake --build --preset ninja-debug
ctest --preset ninja-debug
```

Presets for `ninja-release` and Visual Studio (`msvc`) are provided too. Tests use [Catch2](https://github.com/catchorg/Catch2), fetched automatically. Build without tests with `-DDI_BUILD_TESTS=OFF`. `-DDI_WARNINGS_AS_ERRORS=ON` turns the strict warning set (`/W4 /permissive-`, or `-Wall -Wextra -Wpedantic`) into errors.

## Requirements

- A C++20 compiler — MSVC 2022, Clang, or GCC
- CMake ≥ 3.24
