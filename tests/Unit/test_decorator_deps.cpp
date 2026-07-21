#include "Di/Container/DiContainer.hpp"
#include "Di/Registry/DiDecorate.hpp"
#include "Di/Registry/DiDescribe.hpp"
#include "Di/Registry/DiRegistrationRow.hpp"
#include "Di/Registry/DiRegistry.hpp"

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <string>
#include <utility>

namespace DecoratorDeps
{
    struct IPrefix
    {
        virtual ~IPrefix()                             = default;
        [[nodiscard]] virtual std::string Text() const = 0;
    };

    struct Prefix final : IPrefix
    {
        [[nodiscard]] std::string Text() const override { return "log"; }
    };

    struct IHandler
    {
        virtual ~IHandler()                              = default;
        [[nodiscard]] virtual std::string Handle() const = 0;
    };

    struct BaseHandler final : IHandler
    {
        [[nodiscard]] std::string Handle() const override { return "base"; }
    };

    class LoggingHandler final : public IHandler
    {
    public:
        LoggingHandler(std::shared_ptr<IHandler> inner, std::shared_ptr<IPrefix> prefix) : m_Inner(std::move(inner)), m_Prefix(std::move(prefix)) {}

        [[nodiscard]] std::string Handle() const override { return m_Prefix->Text() + "(" + m_Inner->Handle() + ")"; }

    private:
        std::shared_ptr<IHandler> m_Inner;
        std::shared_ptr<IPrefix>  m_Prefix;
    };
}

DI_DESCRIBE(DecoratorDeps::BaseHandler)
{
    DI_SERVICE(DecoratorDeps::IHandler);
    DI_LIFETIME(Singleton);
};

DI_DESCRIBE(DecoratorDeps::Prefix)
{
    DI_SERVICE(DecoratorDeps::IPrefix);
    DI_LIFETIME(Singleton);
};

DI_DECORATE(DecoratorDeps::LoggingHandler)
{
    DI_SERVICE(DecoratorDeps::IHandler);
    DI_DEPENDS(Di::Service<DecoratorDeps::IPrefix>);
};

TEST_CASE("a decorator receives its own injected dependencies alongside the wrapped service")
{
    Di::Registry::DiRegistry registry;
    registry.Add(Di::Registry::DiRegistrationRow::From<DecoratorDeps::BaseHandler>());
    registry.Add(Di::Registry::DiRegistrationRow::From<DecoratorDeps::Prefix>());
    registry.Decorate<DecoratorDeps::LoggingHandler>();

    Di::Container::DiContainer container(registry);

    REQUIRE(container.Provider().ResolveService<DecoratorDeps::IHandler>()->Handle() == "log(base)");
}
