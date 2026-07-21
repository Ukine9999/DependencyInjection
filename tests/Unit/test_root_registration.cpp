#include "Di/Container/DiContainer.hpp"
#include "Di/Engine/Attributes/DiParameters.hpp"
#include "Di/Registry/DiRegistry.hpp"
#include "Di/Scheme/Abstraction/EDiServiceLifetime.hpp"
#include "Di/Services/Abstraction/IDiServiceProvider.hpp"

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <utility>

namespace Root13
{
    struct IExternal
    {
        virtual ~IExternal()                    = default;
        [[nodiscard]] virtual int Value() const = 0;
    };

    class External final : public IExternal
    {
    public:
        explicit External(int value) : m_Value(value) {}

        [[nodiscard]] int Value() const override { return m_Value; }

    private:
        int m_Value;
    };

    struct IConsumer
    {
        virtual ~IConsumer()                  = default;
        [[nodiscard]] virtual int Sum() const = 0;
    };

    class Consumer final : public IConsumer
    {
    public:
        explicit Consumer(std::shared_ptr<IExternal> external) : m_External(std::move(external)) {}

        [[nodiscard]] int Sum() const override { return m_External->Value() + 1; }

    private:
        std::shared_ptr<IExternal> m_External;
    };
}

TEST_CASE("root registration wires an instance and an explicitly-typed service without DI_DESCRIBE")
{
    Di::Registry::DiRegistry registry;
    registry.AddInstance<Root13::IExternal>(std::make_shared<Root13::External>(41));
    registry.AddType<Root13::Consumer, Root13::IConsumer, Di::Service<Root13::IExternal>>();

    Di::Container::DiContainer container(registry);

    REQUIRE(container.Provider().ResolveService<Root13::IConsumer>()->Sum() == 42);
}

TEST_CASE("root registration wires a factory closure")
{
    Di::Registry::DiRegistry registry;
    registry.AddFactory<Root13::IExternal>(Di::Scheme::Abstraction::EDiServiceLifetime::Singleton,
                                           [](Di::Services::Abstraction::IDiServiceProvider&) { return std::make_shared<Root13::External>(7); });

    Di::Container::DiContainer container(registry);

    REQUIRE(container.Provider().ResolveService<Root13::IExternal>()->Value() == 7);
}
