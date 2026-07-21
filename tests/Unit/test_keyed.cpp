#include "Support/Registry.hpp"

#include "Di/Registry/DiDescribe.hpp"

#include <catch2/catch_test_macros.hpp>

namespace Keyed6
{
    struct IKeyed
    {
        virtual ~IKeyed()                      = default;
        [[nodiscard]] virtual char Tag() const = 0;
    };

    struct KeyedX final : IKeyed
    {
        [[nodiscard]] char Tag() const override { return 'x'; }
    };

    struct KeyedY final : IKeyed
    {
        [[nodiscard]] char Tag() const override { return 'y'; }
    };
}

DI_DESCRIBE(Keyed6::KeyedX)
{
    DI_SERVICE(Keyed6::IKeyed);
    DI_KEY("x");
    DI_LIFETIME(Singleton);
};

DI_DESCRIBE(Keyed6::KeyedY)
{
    DI_SERVICE(Keyed6::IKeyed);
    DI_KEY("y");
    DI_LIFETIME(Singleton);
};

TEST_CASE("keyed services resolve by their key")
{
    auto provider = DiTests::BuildFromDescriptors<Keyed6::KeyedX, Keyed6::KeyedY>();

    REQUIRE(DiTests::ResolveKeyed<Keyed6::IKeyed>(*provider, "x")->Tag() == 'x');
    REQUIRE(DiTests::ResolveKeyed<Keyed6::IKeyed>(*provider, "y")->Tag() == 'y');
    REQUIRE_THROWS(DiTests::ResolveKeyed<Keyed6::IKeyed>(*provider, "missing"));
}
