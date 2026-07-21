#include "Support/Registry.hpp"
#include "Support/Wiring.hpp"

#include "Di/Registry/DiDescribe.hpp"

#include <catch2/catch_test_macros.hpp>

#include <cstddef>
#include <memory>
#include <utility>
#include <vector>

namespace OptionalDep
{
    struct IPresent
    {
        virtual ~IPresent()                     = default;
        [[nodiscard]] virtual int Value() const = 0;
    };

    struct Present final : IPresent
    {
        [[nodiscard]] int Value() const override { return 7; }
    };

    struct IAbsent
    {
        virtual ~IAbsent() = default;
    };

    struct IConsumer
    {
        virtual ~IConsumer()                          = default;
        [[nodiscard]] virtual int  PresentValue() const = 0;
        [[nodiscard]] virtual bool AbsentIsNull() const = 0;
    };

    class Consumer final : public IConsumer
    {
    public:
        Consumer(std::shared_ptr<IPresent> present, std::shared_ptr<IAbsent> absent) : m_Present(std::move(present)), m_Absent(std::move(absent)) {}

        [[nodiscard]] int  PresentValue() const override { return m_Present ? m_Present->Value() : -1; }
        [[nodiscard]] bool AbsentIsNull() const override { return m_Absent == nullptr; }

    private:
        std::shared_ptr<IPresent> m_Present;
        std::shared_ptr<IAbsent>  m_Absent;
    };
}

DI_DESCRIBE(OptionalDep::Present)
{
    DI_SERVICE(OptionalDep::IPresent);
    DI_LIFETIME(Singleton);
};

DI_DESCRIBE(OptionalDep::Consumer)
{
    DI_SERVICE(OptionalDep::IConsumer);
    DI_DEPENDS(Di::Optional<OptionalDep::IPresent>, Di::Optional<OptionalDep::IAbsent>);
    DI_LIFETIME(Singleton);
};

TEST_CASE("Optional<> injects a registered dependency and null for an unregistered one without throwing")
{
    auto provider = DiTests::BuildFromDescriptors<OptionalDep::Present, OptionalDep::Consumer>();
    auto consumer = DiTests::Resolve<OptionalDep::IConsumer>(*provider);

    REQUIRE(consumer->PresentValue() == 7);
    REQUIRE(consumer->AbsentIsNull());
}

namespace EmptyAll
{
    struct IPlugin
    {
        virtual ~IPlugin() = default;
    };

    struct IHost
    {
        virtual ~IHost()                                = default;
        [[nodiscard]] virtual std::size_t Count() const = 0;
    };

    class Host final : public IHost
    {
    public:
        explicit Host(std::vector<std::shared_ptr<IPlugin>> plugins) : m_Plugins(std::move(plugins)) {}

        [[nodiscard]] std::size_t Count() const override { return m_Plugins.size(); }

    private:
        std::vector<std::shared_ptr<IPlugin>> m_Plugins;
    };
}

DI_DESCRIBE(EmptyAll::Host)
{
    DI_SERVICE(EmptyAll::IHost);
    DI_DEPENDS(Di::All<EmptyAll::IPlugin>);
    DI_LIFETIME(Singleton);
};

TEST_CASE("All<> injects an empty collection when nothing is registered, and the consumer still builds")
{
    auto provider = DiTests::BuildFromDescriptors<EmptyAll::Host>();
    auto host     = DiTests::Resolve<EmptyAll::IHost>(*provider);

    REQUIRE(host->Count() == 0);
}
