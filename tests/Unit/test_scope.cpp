#include "Support/Registry.hpp"

#include "Di/Registry/DiDescribe.hpp"
#include "Di/Services/Abstraction/IDiDisposable.hpp"

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <stdexcept>
#include <vector>

namespace ScopeTest
{
    struct IScoped
    {
        virtual ~IScoped() = default;
    };

    struct Scoped final : IScoped
    {
    };

    struct ISingle
    {
        virtual ~ISingle() = default;
    };

    struct Single final : ISingle
    {
    };

    inline std::vector<int> g_Disposed;

    struct IDisposableService
    {
        virtual ~IDisposableService() = default;
    };

    template <int Tag> struct DisposableService final : IDisposableService, Di::Services::Abstraction::IDiDisposable
    {
        void Dispose() override { g_Disposed.push_back(Tag); }
    };

    struct ICaptor
    {
        virtual ~ICaptor() = default;
    };

    struct Captor final : ICaptor
    {
        explicit Captor(std::shared_ptr<IScoped>) {}
    };
}

DI_DESCRIBE(ScopeTest::Scoped)
{
    DI_SERVICE(ScopeTest::IScoped);
    DI_LIFETIME(Scoped);
};

DI_DESCRIBE(ScopeTest::Single)
{
    DI_SERVICE(ScopeTest::ISingle);
    DI_LIFETIME(Singleton);
};

DI_DESCRIBE(ScopeTest::DisposableService<1>)
{
    DI_SERVICE(ScopeTest::IDisposableService);
    DI_LIFETIME(Scoped);
};

DI_DESCRIBE(ScopeTest::DisposableService<2>)
{
    DI_SERVICE(ScopeTest::IDisposableService);
    DI_LIFETIME(Scoped);
};

DI_DESCRIBE(ScopeTest::Captor)
{
    DI_SERVICE(ScopeTest::ICaptor);
    DI_DEPENDS(Di::Service<ScopeTest::IScoped>);
    DI_LIFETIME(Singleton);
};

TEST_CASE("scoped services are one instance per scope")
{
    auto provider = DiTests::BuildFromDescriptors<ScopeTest::Scoped>();
    auto first    = provider->CreateScope();
    auto second   = provider->CreateScope();

    REQUIRE(first->ResolveService<ScopeTest::IScoped>().get() == first->ResolveService<ScopeTest::IScoped>().get());
    REQUIRE(first->ResolveService<ScopeTest::IScoped>().get() != second->ResolveService<ScopeTest::IScoped>().get());
}

TEST_CASE("singletons are shared across scopes")
{
    auto provider = DiTests::BuildFromDescriptors<ScopeTest::Single>();
    auto first    = provider->CreateScope();
    auto second   = provider->CreateScope();

    REQUIRE(first->ResolveService<ScopeTest::ISingle>().get() == second->ResolveService<ScopeTest::ISingle>().get());
}

TEST_CASE("scoped disposables are disposed in reverse creation order when the scope ends")
{
    ScopeTest::g_Disposed.clear();

    auto provider = DiTests::BuildFromDescriptors<ScopeTest::DisposableService<1>, ScopeTest::DisposableService<2>>();

    {
        auto scope = provider->CreateScope();
        REQUIRE(scope->ResolveServices<ScopeTest::IDisposableService>().size() == 2);
        REQUIRE(ScopeTest::g_Disposed.empty());
    }

    REQUIRE(ScopeTest::g_Disposed == std::vector<int> {2, 1});
}

TEST_CASE("a singleton depending on a scoped service is a captive dependency and throws")
{
    REQUIRE_THROWS_AS((DiTests::BuildFromDescriptors<ScopeTest::Scoped, ScopeTest::Captor>()), std::runtime_error);
}
