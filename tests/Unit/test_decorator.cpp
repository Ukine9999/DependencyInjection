#include "Di/Container/DiContainer.hpp"
#include "Di/Registry/DiDecorate.hpp"
#include "Di/Registry/DiDescribe.hpp"
#include "Di/Registry/DiRegistrationRow.hpp"
#include "Di/Registry/DiRegistry.hpp"

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <string>
#include <utility>

namespace Deco
{
    struct IController
    {
        virtual ~IController()                           = default;
        [[nodiscard]] virtual std::string Handle() const = 0;
    };

    struct BaseController final : IController
    {
        [[nodiscard]] std::string Handle() const override { return "base"; }
    };

    class LoggingController final : public IController
    {
    public:
        explicit LoggingController(std::shared_ptr<IController> inner) : m_Inner(std::move(inner)) {}

        [[nodiscard]] std::string Handle() const override { return "log(" + m_Inner->Handle() + ")"; }

    private:
        std::shared_ptr<IController> m_Inner;
    };

    class MetricsController final : public IController
    {
    public:
        explicit MetricsController(std::shared_ptr<IController> inner) : m_Inner(std::move(inner)) {}

        [[nodiscard]] std::string Handle() const override { return "metrics(" + m_Inner->Handle() + ")"; }

    private:
        std::shared_ptr<IController> m_Inner;
    };
}

DI_DESCRIBE(Deco::BaseController)
{
    DI_SERVICE(Deco::IController);
    DI_LIFETIME(Singleton);
};

DI_DECORATE(Deco::LoggingController)
{
    DI_SERVICE(Deco::IController);
};

DI_DECORATE(Deco::MetricsController)
{
    DI_SERVICE(Deco::IController);
};

TEST_CASE("decorators wrap the resolved service in registration order")
{
    Di::Registry::DiRegistry registry;
    registry.Add(Di::Registry::DiRegistrationRow::From<Deco::BaseController>());
    registry.Decorate<Deco::LoggingController>();
    registry.Decorate<Deco::MetricsController>();

    Di::Container::DiContainer container(registry);

    REQUIRE(container.Provider().ResolveService<Deco::IController>()->Handle() == "metrics(log(base))");
}
