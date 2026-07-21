#include "Support/TestTypes.hpp"
#include "Support/Wiring.hpp"

#include "Di/Engine/Attributes/DiParameters.hpp"
#include "Di/Engine/Template/DiEngineTemplate.hpp"
#include "Di/Registry/DiBinding.hpp"
#include "Di/Registry/DiDependencies.hpp"
#include "Di/Scheme/DiFactoryServiceScheme.hpp"

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <typeindex>
#include <vector>

using Di::Engine::Attributes::Service;
using Di::Engine::Template::DiEngineTemplate;
using Di::Registry::DiDependencies;
using Di::Registry::MakeCaster;
using Di::Scheme::DiFactoryServiceScheme;
using Di::Scheme::Abstraction::EDiServiceLifetime;
using Di::Scheme::Abstraction::IDiServiceScheme;
using Di::Services::Abstraction::DiServiceId;
using Di::Services::Abstraction::DiServiceKey;

namespace
{
    template <class TService, class TImplementation, class TDependencies>
    std::shared_ptr<IDiServiceScheme> Describe(const DiEngineTemplate& engine, TDependencies dependencies)
    {
        std::vector<IDiServiceScheme::Binding> bindings;
        bindings.emplace_back(DiServiceId::FromType<TService>(), MakeCaster<TImplementation, TService>());

        return std::make_shared<DiFactoryServiceScheme>(EDiServiceLifetime::Singleton, engine.CreateConstructor<TImplementation>(dependencies),
                                                        std::type_index(typeid(TService)), DiServiceKey {}, std::move(bindings),
                                                        Di::Services::Abstraction::DiDisposer {});
    }
}

TEST_CASE("three-level dependency chain is injected through the engine")
{
    const DiEngineTemplate engine;

    DiTests::SchemeList schemes;
    schemes.push_back(Describe<DiTests::IService1, DiTests::Service1>(engine, DiDependencies<> {}));
    schemes.push_back(Describe<DiTests::IService0, DiTests::Service0>(engine, DiDependencies<Service<DiTests::IService1>> {}));
    schemes.push_back(Describe<DiTests::IRegistered, DiTests::Registered>(engine, DiDependencies<Service<DiTests::IService0>> {}));

    auto provider   = DiTests::BuildProvider(schemes);
    auto registered = DiTests::Resolve<DiTests::IRegistered>(*provider);

    REQUIRE(registered != nullptr);
    REQUIRE(registered->Chain() == 7);
}
