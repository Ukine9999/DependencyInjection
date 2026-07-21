#include "Support/Registry.hpp"
#include "Support/Wiring.hpp"

#include "Di/Registry/DiDescribe.hpp"
#include "Di/Services/Abstraction/DiServiceId.hpp"

#include <catch2/catch_test_macros.hpp>

namespace Multi10
{
    struct IMulti
    {
        virtual ~IMulti()                     = default;
        [[nodiscard]] virtual int Tag() const = 0;
    };

    struct MultiA final : IMulti
    {
        [[nodiscard]] int Tag() const override { return 1; }
    };

    struct MultiB final : IMulti
    {
        [[nodiscard]] int Tag() const override { return 2; }
    };
}

DI_DESCRIBE(Multi10::MultiA)
{
    DI_SERVICE(Multi10::IMulti);
    DI_LIFETIME(Singleton);
    DI_ORDER(0);
};

DI_DESCRIBE(Multi10::MultiB)
{
    DI_SERVICE(Multi10::IMulti);
    DI_LIFETIME(Singleton);
    DI_ORDER(1);
};

TEST_CASE("multiple registrations of one service coexist and single resolve returns the first")
{
    auto provider = DiTests::BuildFromDescriptors<Multi10::MultiA, Multi10::MultiB>();

    auto all = provider->ResolveServices(Di::Services::Abstraction::DiServiceId::FromType<Multi10::IMulti>(), true);
    REQUIRE(all.size() == 2);

    auto first = DiTests::Resolve<Multi10::IMulti>(*provider);
    REQUIRE(first->Tag() == 1);
}
