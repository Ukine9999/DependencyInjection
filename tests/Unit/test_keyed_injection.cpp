#include "Support/Registry.hpp"
#include "Support/Wiring.hpp"

#include "Di/Registry/DiDescribe.hpp"

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <string>
#include <utility>

namespace KeyedInjection
{
    struct IEngine
    {
        virtual ~IEngine()                        = default;
        [[nodiscard]] virtual std::string Name() const = 0;
    };

    struct EcoEngine final : IEngine
    {
        [[nodiscard]] std::string Name() const override { return "eco"; }
    };

    struct TurboEngine final : IEngine
    {
        [[nodiscard]] std::string Name() const override { return "turbo"; }
    };

    struct ICar
    {
        virtual ~ICar()                                 = default;
        [[nodiscard]] virtual std::string EngineName() const = 0;
    };

    class Car final : public ICar
    {
    public:
        explicit Car(std::shared_ptr<IEngine> engine) : m_Engine(std::move(engine)) {}

        [[nodiscard]] std::string EngineName() const override { return m_Engine->Name(); }

    private:
        std::shared_ptr<IEngine> m_Engine;
    };
}

DI_DESCRIBE(KeyedInjection::EcoEngine)
{
    DI_SERVICE(KeyedInjection::IEngine);
    DI_KEY("eco");
    DI_LIFETIME(Singleton);
};

DI_DESCRIBE(KeyedInjection::TurboEngine)
{
    DI_SERVICE(KeyedInjection::IEngine);
    DI_KEY("turbo");
    DI_LIFETIME(Singleton);
};

DI_DESCRIBE(KeyedInjection::Car)
{
    DI_SERVICE(KeyedInjection::ICar);
    DI_DEPENDS(Di::Keyed<KeyedInjection::IEngine, Di::DiKey("turbo")>);
    DI_LIFETIME(Singleton);
};

TEST_CASE("a Keyed<> dependency selects the implementation whose key matches, among several of the interface")
{
    auto provider = DiTests::BuildFromDescriptors<KeyedInjection::EcoEngine, KeyedInjection::TurboEngine, KeyedInjection::Car>();
    auto car      = DiTests::Resolve<KeyedInjection::ICar>(*provider);

    REQUIRE(car->EngineName() == "turbo");
}
