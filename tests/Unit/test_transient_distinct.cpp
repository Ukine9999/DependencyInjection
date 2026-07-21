#include "Support/TestTypes.hpp"
#include "Support/Wiring.hpp"

#include <catch2/catch_test_macros.hpp>

using Di::Scheme::Abstraction::EDiServiceLifetime;
using DiTests::IValue;
using DiTests::Value;

TEST_CASE("transient resolves to a fresh instance every time")
{
    auto provider = DiTests::BuildProvider({DiTests::Provide<IValue, Value>(EDiServiceLifetime::Transient)});

    auto first  = DiTests::Resolve<IValue>(*provider);
    auto second = DiTests::Resolve<IValue>(*provider);

    REQUIRE(first != nullptr);
    REQUIRE(second != nullptr);
    REQUIRE(first.get() != second.get());
}
