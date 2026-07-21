#include "Support/Registry.hpp"

#include "Di/Registry/DiDescribe.hpp"

#include <catch2/catch_test_macros.hpp>

namespace Ext11
{
    struct IThing
    {
        virtual ~IThing()                   = default;
        [[nodiscard]] virtual int N() const = 0;
    };

    struct Thing final : IThing
    {
        [[nodiscard]] int N() const override { return 5; }
    };
}

DI_DESCRIBE(Ext11::Thing)
{
    DI_SERVICE(Ext11::IThing);
    DI_LIFETIME(Singleton);
};

TEST_CASE("provider generic resolve members mirror the C# extensions")
{
    auto provider = DiTests::BuildFromDescriptors<Ext11::Thing>();

    REQUIRE(provider->IsImplemented<Ext11::IThing>());
    REQUIRE(provider->ResolveService<Ext11::IThing>()->N() == 5);
    REQUIRE(provider->ResolveServices<Ext11::IThing>().size() == 1);
}
