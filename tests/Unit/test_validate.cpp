#include "Di/Registry/DiDescribe.hpp"
#include "Di/Registry/DiRegistrationRow.hpp"
#include "Di/Registry/DiRegistry.hpp"
#include "Di/Registry/DiRegistryValidator.hpp"

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <stdexcept>

namespace Validate12
{
    struct IMissing
    {
        virtual ~IMissing() = default;
    };

    struct IGood
    {
        virtual ~IGood() = default;
    };

    struct Good final : IGood
    {
    };

    struct INeedy
    {
        virtual ~INeedy() = default;
    };

    struct Needy final : INeedy
    {
        explicit Needy(std::shared_ptr<IMissing>) {}
    };
}

DI_DESCRIBE(Validate12::Good)
{
    DI_SERVICE(Validate12::IGood);
    DI_LIFETIME(Singleton);
};

DI_DESCRIBE(Validate12::Needy)
{
    DI_SERVICE(Validate12::INeedy);
    DI_DEPENDS(Di::Service<Validate12::IMissing>);
    DI_LIFETIME(Transient);
};

TEST_CASE("validate flags a missing required dependency without constructing anything")
{
    Di::Registry::DiRegistry registry;
    registry.Add(Di::Registry::DiRegistrationRow::From<Validate12::Needy>());

    REQUIRE_FALSE(Di::Registry::FindMissingDependencies(registry).empty());
    REQUIRE_THROWS_AS(Di::Registry::ValidateOrThrow(registry), std::runtime_error);
}

TEST_CASE("validate passes for a satisfied graph")
{
    Di::Registry::DiRegistry registry;
    registry.Add(Di::Registry::DiRegistrationRow::From<Validate12::Good>());

    REQUIRE(Di::Registry::FindMissingDependencies(registry).empty());
    Di::Registry::ValidateOrThrow(registry);
}

namespace ValidateCycle
{
    struct IA
    {
        virtual ~IA() = default;
    };

    struct IB
    {
        virtual ~IB() = default;
    };

    struct A final : IA
    {
        explicit A(std::shared_ptr<IB>) {}
    };

    struct B final : IB
    {
        explicit B(std::shared_ptr<IA>) {}
    };
}

DI_DESCRIBE(ValidateCycle::A)
{
    DI_SERVICE(ValidateCycle::IA);
    DI_DEPENDS(Di::Service<ValidateCycle::IB>);
    DI_LIFETIME(Transient);
};

DI_DESCRIBE(ValidateCycle::B)
{
    DI_SERVICE(ValidateCycle::IB);
    DI_DEPENDS(Di::Service<ValidateCycle::IA>);
    DI_LIFETIME(Transient);
};

TEST_CASE("validate detects a dependency cycle among transients without constructing anything")
{
    Di::Registry::DiRegistry registry;
    registry.Add(Di::Registry::DiRegistrationRow::From<ValidateCycle::A>());
    registry.Add(Di::Registry::DiRegistrationRow::From<ValidateCycle::B>());

    REQUIRE_FALSE(Di::Registry::FindDependencyCycle(registry).empty());
    REQUIRE_THROWS_AS(Di::Registry::ValidateOrThrow(registry), std::runtime_error);
}

namespace ValidateLazyBreak
{
    struct IA
    {
        virtual ~IA() = default;
    };

    struct IB
    {
        virtual ~IB() = default;
    };

    struct A final : IA
    {
        explicit A(Di::DiLazy<IB>) {}
    };

    struct B final : IB
    {
        explicit B(std::shared_ptr<IA>) {}
    };
}

DI_DESCRIBE(ValidateLazyBreak::A)
{
    DI_SERVICE(ValidateLazyBreak::IA);
    DI_DEPENDS(Di::Lazy<ValidateLazyBreak::IB>);
    DI_LIFETIME(Transient);
};

DI_DESCRIBE(ValidateLazyBreak::B)
{
    DI_SERVICE(ValidateLazyBreak::IB);
    DI_DEPENDS(Di::Service<ValidateLazyBreak::IA>);
    DI_LIFETIME(Transient);
};

TEST_CASE("validate treats a Lazy<> edge as cycle-breaking")
{
    Di::Registry::DiRegistry registry;
    registry.Add(Di::Registry::DiRegistrationRow::From<ValidateLazyBreak::A>());
    registry.Add(Di::Registry::DiRegistrationRow::From<ValidateLazyBreak::B>());

    REQUIRE(Di::Registry::FindDependencyCycle(registry).empty());
    Di::Registry::ValidateOrThrow(registry);
}
