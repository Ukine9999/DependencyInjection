#include "Support/Registry.hpp"

#include "Di/Registry/DiDescribe.hpp"

#include <catch2/catch_test_macros.hpp>

#include <vector>

namespace Order8
{
    inline std::vector<int> g_ConstructionOrder;

    struct IMarker
    {
        virtual ~IMarker() = default;
    };

    template <int Tag> struct Marker final : IMarker
    {
        Marker() { g_ConstructionOrder.push_back(Tag); }
    };
}

DI_DESCRIBE(Order8::Marker<20>)
{
    DI_SERVICE(Order8::IMarker);
    DI_LIFETIME(Singleton);
    DI_ORDER(20);
};

DI_DESCRIBE(Order8::Marker<10>)
{
    DI_SERVICE(Order8::IMarker);
    DI_LIFETIME(Singleton);
    DI_ORDER(10);
};

DI_DESCRIBE(Order8::Marker<30>)
{
    DI_SERVICE(Order8::IMarker);
    DI_LIFETIME(Singleton);
    DI_ORDER(30);
};

TEST_CASE("eager singletons are constructed in ascending Order")
{
    Order8::g_ConstructionOrder.clear();

    auto provider = DiTests::BuildFromDescriptors<Order8::Marker<20>, Order8::Marker<10>, Order8::Marker<30>>();

    REQUIRE(provider != nullptr);
    REQUIRE(Order8::g_ConstructionOrder == std::vector<int> {10, 20, 30});
}
