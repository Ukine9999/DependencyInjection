#include "Support/Registry.hpp"
#include "Support/Wiring.hpp"

#include "Di/Registry/DiDescribe.hpp"

#include <catch2/catch_test_macros.hpp>

#include <functional>
#include <memory>
#include <utility>

namespace FactorySemantics
{
    struct ITransient
    {
        virtual ~ITransient() = default;
    };

    struct TransientThing final : ITransient
    {
    };

    struct ISingleton
    {
        virtual ~ISingleton() = default;
    };

    struct SingletonThing final : ISingleton
    {
    };

    struct ITransientProbe
    {
        virtual ~ITransientProbe()                 = default;
        [[nodiscard]] virtual bool YieldsDistinct() = 0;
    };

    class TransientProbe final : public ITransientProbe
    {
    public:
        explicit TransientProbe(std::function<std::shared_ptr<ITransient>()> factory) : m_Factory(std::move(factory)) {}

        [[nodiscard]] bool YieldsDistinct() override { return m_Factory().get() != m_Factory().get(); }

    private:
        std::function<std::shared_ptr<ITransient>()> m_Factory;
    };

    struct ISingletonProbe
    {
        virtual ~ISingletonProbe()             = default;
        [[nodiscard]] virtual bool YieldsSame() = 0;
    };

    class SingletonProbe final : public ISingletonProbe
    {
    public:
        explicit SingletonProbe(std::function<std::shared_ptr<ISingleton>()> factory) : m_Factory(std::move(factory)) {}

        [[nodiscard]] bool YieldsSame() override { return m_Factory().get() == m_Factory().get(); }

    private:
        std::function<std::shared_ptr<ISingleton>()> m_Factory;
    };
}

DI_DESCRIBE(FactorySemantics::TransientThing)
{
    DI_SERVICE(FactorySemantics::ITransient);
    DI_LIFETIME(Transient);
};

DI_DESCRIBE(FactorySemantics::SingletonThing)
{
    DI_SERVICE(FactorySemantics::ISingleton);
    DI_LIFETIME(Singleton);
};

DI_DESCRIBE(FactorySemantics::TransientProbe)
{
    DI_SERVICE(FactorySemantics::ITransientProbe);
    DI_DEPENDS(Di::Factory<FactorySemantics::ITransient>);
    DI_LIFETIME(Singleton);
};

DI_DESCRIBE(FactorySemantics::SingletonProbe)
{
    DI_SERVICE(FactorySemantics::ISingletonProbe);
    DI_DEPENDS(Di::Factory<FactorySemantics::ISingleton>);
    DI_LIFETIME(Singleton);
};

TEST_CASE("Factory<> of a transient service produces a fresh instance on every invocation")
{
    auto provider = DiTests::BuildFromDescriptors<FactorySemantics::TransientThing, FactorySemantics::TransientProbe>();
    auto probe    = DiTests::Resolve<FactorySemantics::ITransientProbe>(*provider);

    REQUIRE(probe->YieldsDistinct());
}

TEST_CASE("Factory<> of a singleton service returns the same instance on every invocation")
{
    auto provider = DiTests::BuildFromDescriptors<FactorySemantics::SingletonThing, FactorySemantics::SingletonProbe>();
    auto probe    = DiTests::Resolve<FactorySemantics::ISingletonProbe>(*provider);

    REQUIRE(probe->YieldsSame());
}
