#include "Support/Registry.hpp"

#include "Di/Registry/DiDescribe.hpp"
#include "Di/Services/Abstraction/IDiDisposable.hpp"

#include <catch2/catch_test_macros.hpp>

#include <memory>

namespace ScopeIsolation
{
    inline int g_Disposed = 0;

    struct IUnit
    {
        virtual ~IUnit() = default;
    };

    struct Unit final : IUnit, Di::Services::Abstraction::IDiDisposable
    {
        void Dispose() override { ++g_Disposed; }
    };
}

DI_DESCRIBE(ScopeIsolation::Unit)
{
    DI_SERVICE(ScopeIsolation::IUnit);
    DI_LIFETIME(Scoped);
};

TEST_CASE("ending one scope disposes only its own scoped instance and leaves a sibling scope untouched")
{
    ScopeIsolation::g_Disposed = 0;

    auto provider = DiTests::BuildFromDescriptors<ScopeIsolation::Unit>();

    auto scopeA = provider->CreateScope();
    auto scopeB = provider->CreateScope();

    auto unitA = scopeA->ResolveService<ScopeIsolation::IUnit>();
    auto unitB = scopeB->ResolveService<ScopeIsolation::IUnit>();

    REQUIRE(unitA.get() != unitB.get());
    REQUIRE(ScopeIsolation::g_Disposed == 0);

    scopeA.reset();
    REQUIRE(ScopeIsolation::g_Disposed == 1);

    scopeB.reset();
    REQUIRE(ScopeIsolation::g_Disposed == 2);
}
