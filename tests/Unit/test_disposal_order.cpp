#include "Support/Registry.hpp"

#include "Di/Registry/DiDescribe.hpp"
#include "Di/Services/Abstraction/IDiDisposable.hpp"

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <vector>

namespace SingletonDispose
{
    inline std::vector<int> g_Disposed;

    struct ILeaf
    {
        virtual ~ILeaf() = default;
    };

    struct Leaf final : ILeaf, Di::Services::Abstraction::IDiDisposable
    {
        void Dispose() override { g_Disposed.push_back(1); }
    };

    struct IRoot
    {
        virtual ~IRoot() = default;
    };

    struct Root final : IRoot, Di::Services::Abstraction::IDiDisposable
    {
        explicit Root(std::shared_ptr<ILeaf>) {}

        void Dispose() override { g_Disposed.push_back(2); }
    };
}

DI_DESCRIBE(SingletonDispose::Leaf)
{
    DI_SERVICE(SingletonDispose::ILeaf);
    DI_LIFETIME(Singleton);
    DI_ORDER(1);
};

DI_DESCRIBE(SingletonDispose::Root)
{
    DI_SERVICE(SingletonDispose::IRoot);
    DI_DEPENDS(Di::Service<SingletonDispose::ILeaf>);
    DI_LIFETIME(Singleton);
    DI_ORDER(0);
};

TEST_CASE("singleton disposables are disposed in reverse creation order, so a dependency outlives its dependent")
{
    SingletonDispose::g_Disposed.clear();

    auto provider = DiTests::BuildFromDescriptors<SingletonDispose::Root, SingletonDispose::Leaf>();
    REQUIRE(SingletonDispose::g_Disposed.empty());

    provider.reset();

    REQUIRE(SingletonDispose::g_Disposed == std::vector<int> {2, 1});
}

namespace TransientDispose
{
    inline std::vector<int> g_Disposed;
    inline int              g_Next = 0;

    struct IWork
    {
        virtual ~IWork() = default;
    };

    struct Work final : IWork, Di::Services::Abstraction::IDiDisposable
    {
        int Id = ++g_Next;

        void Dispose() override { g_Disposed.push_back(Id); }
    };
}

DI_DESCRIBE(TransientDispose::Work)
{
    DI_SERVICE(TransientDispose::IWork);
    DI_LIFETIME(Transient);
};

TEST_CASE("each transient resolve is tracked and disposed in reverse creation order when its scope ends")
{
    TransientDispose::g_Disposed.clear();
    TransientDispose::g_Next = 0;

    auto provider = DiTests::BuildFromDescriptors<TransientDispose::Work>();

    {
        auto scope  = provider->CreateScope();
        auto first  = scope->ResolveService<TransientDispose::IWork>();
        auto second = scope->ResolveService<TransientDispose::IWork>();
        auto third  = scope->ResolveService<TransientDispose::IWork>();

        REQUIRE(first.get() != second.get());
        REQUIRE(second.get() != third.get());
        REQUIRE(first.get() != third.get());
        REQUIRE(TransientDispose::g_Disposed.empty());
    }

    REQUIRE(TransientDispose::g_Disposed == std::vector<int> {3, 2, 1});
}
