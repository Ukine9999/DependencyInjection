#include "ModuleC/ModuleC.hpp"

#include "Di/Container/DiContainer.hpp"
#include "Di/Registry/DiModuleAnchor.hpp"
#include "Di/Services/Abstraction/DiServiceId.hpp"

#include <catch2/catch_test_macros.hpp>

#include <memory>

DI_USE_MODULE(ModuleA)
DI_USE_MODULE(ModuleB)
DI_USE_MODULE(ModuleC)

TEST_CASE("cross-module self-registration survives static-library linking")
{
    Di::Container::DiContainer container;

    auto serviceC = std::static_pointer_cast<ModuleC::IServiceC>(
        container.Provider().ResolveService(Di::Services::Abstraction::DiServiceId::FromType<ModuleC::IServiceC>(), false));

    REQUIRE(serviceC != nullptr);
    REQUIRE(serviceC->Chain() == 43);
}
