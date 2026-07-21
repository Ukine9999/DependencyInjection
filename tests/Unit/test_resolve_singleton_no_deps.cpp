#include "Support/TestTypes.hpp"
#include "Support/Wiring.hpp"

#include <catch2/catch_test_macros.hpp>

using Di::Scheme::Abstraction::EDiServiceLifetime;
using DiTests::IValue;
using DiTests::Value;

TEST_CASE("singleton with no dependencies resolves and works")
{
    auto provider = DiTests::BuildProvider({DiTests::Provide<IValue, Value>(EDiServiceLifetime::Singleton)});

    auto value = DiTests::Resolve<IValue>(*provider);

    REQUIRE(value != nullptr);
    REQUIRE(value->Get() == 42);
}
