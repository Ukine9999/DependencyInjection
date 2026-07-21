#include "Di/Container/DiContainer.hpp"
#include "Di/Registry/DiRegistry.hpp"
#include "Di/Services/Abstraction/IDiDisposable.hpp"

#include <catch2/catch_test_macros.hpp>

#include <memory>

namespace InstanceOwnership
{
    inline int g_DisposeCalls = 0;

    struct IResource
    {
        virtual ~IResource()                     = default;
        [[nodiscard]] virtual int Handle() const = 0;
    };

    struct Resource final : IResource, Di::Services::Abstraction::IDiDisposable
    {
        [[nodiscard]] int Handle() const override { return 77; }

        void Dispose() override { ++g_DisposeCalls; }
    };
}

TEST_CASE("an externally supplied instance is handed back as-is and its lifetime stays with the caller")
{
    InstanceOwnership::g_DisposeCalls = 0;

    auto external = std::make_shared<InstanceOwnership::Resource>();

    {
        Di::Registry::DiRegistry registry;
        registry.AddInstance<InstanceOwnership::IResource>(external);

        Di::Container::DiContainer container(registry);
        auto                      resolved = container.Provider().ResolveService<InstanceOwnership::IResource>();

        REQUIRE(resolved.get() == external.get());
        REQUIRE(resolved->Handle() == 77);
    }

    REQUIRE(InstanceOwnership::g_DisposeCalls == 0);
    REQUIRE(external.use_count() == 1);
}
