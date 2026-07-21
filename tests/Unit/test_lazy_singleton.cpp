#include "Support/TestTypes.hpp"
#include "Support/Wiring.hpp"

#include <catch2/catch_test_macros.hpp>

using Di::Scheme::Abstraction::EDiServiceLifetime;
using DiTests::Counted;
using DiTests::ICounted;

TEST_CASE("lazy singleton constructs on first resolve and caches")
{
    DiTests::g_CountedConstructions = 0;

    auto provider = DiTests::BuildProvider({DiTests::Provide<ICounted, Counted>(EDiServiceLifetime::LazySingleton)});

    REQUIRE(DiTests::g_CountedConstructions == 0);

    auto first = DiTests::Resolve<ICounted>(*provider);
    REQUIRE(DiTests::g_CountedConstructions == 1);

    auto second = DiTests::Resolve<ICounted>(*provider);
    REQUIRE(DiTests::g_CountedConstructions == 1);
    REQUIRE(first.get() == second.get());
}
