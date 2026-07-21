#include "Support/Registry.hpp"
#include "Support/Wiring.hpp"

#include "Di/Registry/DiDescribe.hpp"

#include <catch2/catch_test_macros.hpp>

#include <memory>

namespace ExposeIdentity
{
    struct IPrimary
    {
        virtual ~IPrimary()                     = default;
        [[nodiscard]] virtual int Value() const = 0;
    };

    struct IExposed
    {
        virtual ~IExposed() = default;
    };

    struct Both final : public IPrimary, public IExposed
    {
        [[nodiscard]] int Value() const override { return 5; }
    };
}

DI_DESCRIBE(ExposeIdentity::Both)
{
    DI_SERVICE(ExposeIdentity::IPrimary);
    DI_EXPOSE(ExposeIdentity::IExposed);
    DI_LIFETIME(Singleton);
};

TEST_CASE("a service and its exposed interface resolve to the very same singleton instance")
{
    auto provider = DiTests::BuildFromDescriptors<ExposeIdentity::Both>();

    auto viaPrimary = DiTests::Resolve<ExposeIdentity::IPrimary>(*provider);
    auto viaExposed = DiTests::Resolve<ExposeIdentity::IExposed>(*provider);

    REQUIRE(viaPrimary != nullptr);
    REQUIRE(viaExposed != nullptr);
    REQUIRE(viaPrimary->Value() == 5);
    REQUIRE(std::static_pointer_cast<ExposeIdentity::Both>(viaPrimary).get() == std::static_pointer_cast<ExposeIdentity::Both>(viaExposed).get());
}
