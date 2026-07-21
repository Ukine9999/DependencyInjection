#include "Support/TestTypes.hpp"
#include "Support/Wiring.hpp"

#include <catch2/catch_test_macros.hpp>

using Di::Scheme::Abstraction::EDiServiceLifetime;
using DiTests::IValue;
using DiTests::Value;

TEST_CASE("singleton resolves to the same instance every time")
{
    auto provider = DiTests::BuildProvider({DiTests::Provide<IValue, Value>(EDiServiceLifetime::Singleton)});

    auto first  = DiTests::Resolve<IValue>(*provider);
    auto second = DiTests::Resolve<IValue>(*provider);

    REQUIRE(first.get() == second.get());
}
