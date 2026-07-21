#include "Support/Registry.hpp"
#include "Support/Wiring.hpp"

#include "Di/Registry/DiDescribe.hpp"

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <vector>

namespace ResolveSelection
{
    struct IStage
    {
        virtual ~IStage()                     = default;
        [[nodiscard]] virtual int Tag() const = 0;
    };

    template <int Value> struct Stage final : IStage
    {
        [[nodiscard]] int Tag() const override { return Value; }
    };
}

DI_DESCRIBE(ResolveSelection::Stage<30>)
{
    DI_SERVICE(ResolveSelection::IStage);
    DI_LIFETIME(Singleton);
    DI_ORDER(30);
};

DI_DESCRIBE(ResolveSelection::Stage<10>)
{
    DI_SERVICE(ResolveSelection::IStage);
    DI_LIFETIME(Singleton);
    DI_ORDER(10);
};

DI_DESCRIBE(ResolveSelection::Stage<20>)
{
    DI_SERVICE(ResolveSelection::IStage);
    DI_LIFETIME(Singleton);
    DI_ORDER(20);
};

TEST_CASE("a single resolve returns the lowest-Order registration, while the collection preserves ascending Order")
{
    auto provider = DiTests::BuildFromDescriptors<ResolveSelection::Stage<30>, ResolveSelection::Stage<10>, ResolveSelection::Stage<20>>();

    REQUIRE(DiTests::Resolve<ResolveSelection::IStage>(*provider)->Tag() == 10);

    std::vector<int> tags;
    for (const auto& stage : provider->ResolveServices<ResolveSelection::IStage>())
        tags.push_back(stage->Tag());

    REQUIRE(tags == std::vector<int> {10, 20, 30});
}
