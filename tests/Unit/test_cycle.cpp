#include "Support/Registry.hpp"

#include "Di/Registry/DiDescribe.hpp"

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <stdexcept>

namespace Cycle1
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

DI_DESCRIBE(Cycle1::A)
{
    DI_SERVICE(Cycle1::IA);
    DI_DEPENDS(Di::Service<Cycle1::IB>);
    DI_LIFETIME(Singleton);
};

DI_DESCRIBE(Cycle1::B)
{
    DI_SERVICE(Cycle1::IB);
    DI_DEPENDS(Di::Service<Cycle1::IA>);
    DI_LIFETIME(Singleton);
};

TEST_CASE("dependency cycle is detected and reported instead of overflowing the stack")
{
    REQUIRE_THROWS_AS((DiTests::BuildFromDescriptors<Cycle1::A, Cycle1::B>()), std::runtime_error);
}
