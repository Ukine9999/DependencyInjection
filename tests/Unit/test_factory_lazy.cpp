#include "Support/Registry.hpp"
#include "Support/Wiring.hpp"

#include "Di/Registry/DiDescribe.hpp"

#include <catch2/catch_test_macros.hpp>

#include <functional>
#include <memory>
#include <utility>

namespace FactoryLazy
{
    struct IDep
    {
        virtual ~IDep()                         = default;
        [[nodiscard]] virtual int Value() const = 0;
    };

    struct Dep final : IDep
    {
        [[nodiscard]] int Value() const override { return 9; }
    };

    struct IUsesFactory
    {
        virtual ~IUsesFactory()                = default;
        [[nodiscard]] virtual int Make() const = 0;
    };

    class UsesFactory final : public IUsesFactory
    {
    public:
        explicit UsesFactory(std::function<std::shared_ptr<IDep>()> factory) : m_Factory(std::move(factory)) {}

        [[nodiscard]] int Make() const override { return m_Factory()->Value(); }

    private:
        std::function<std::shared_ptr<IDep>()> m_Factory;
    };

    struct IUsesLazy
    {
        virtual ~IUsesLazy()             = default;
        [[nodiscard]] virtual int Peek() = 0;
    };

    class UsesLazy final : public IUsesLazy
    {
    public:
        explicit UsesLazy(Di::DiLazy<IDep> lazy) : m_Lazy(std::move(lazy)) {}

        [[nodiscard]] int Peek() override { return m_Lazy->Value(); }

    private:
        Di::DiLazy<IDep> m_Lazy;
    };
}

DI_DESCRIBE(FactoryLazy::Dep)
{
    DI_SERVICE(FactoryLazy::IDep);
    DI_LIFETIME(Transient);
};

DI_DESCRIBE(FactoryLazy::UsesFactory)
{
    DI_SERVICE(FactoryLazy::IUsesFactory);
    DI_DEPENDS(Di::Factory<FactoryLazy::IDep>);
    DI_LIFETIME(Singleton);
};

DI_DESCRIBE(FactoryLazy::UsesLazy)
{
    DI_SERVICE(FactoryLazy::IUsesLazy);
    DI_DEPENDS(Di::Lazy<FactoryLazy::IDep>);
    DI_LIFETIME(Singleton);
};

TEST_CASE("Factory<> injects an on-demand creator")
{
    auto provider = DiTests::BuildFromDescriptors<FactoryLazy::Dep, FactoryLazy::UsesFactory>();
    auto user     = DiTests::Resolve<FactoryLazy::IUsesFactory>(*provider);

    REQUIRE(user->Make() == 9);
}

TEST_CASE("Lazy<> injects a deferred resolver")
{
    auto provider = DiTests::BuildFromDescriptors<FactoryLazy::Dep, FactoryLazy::UsesLazy>();
    auto user     = DiTests::Resolve<FactoryLazy::IUsesLazy>(*provider);

    REQUIRE(user->Peek() == 9);
}
