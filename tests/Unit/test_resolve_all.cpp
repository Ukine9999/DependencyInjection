#include "Support/Registry.hpp"
#include "Support/Wiring.hpp"

#include "Di/Registry/DiDescribe.hpp"

#include <catch2/catch_test_macros.hpp>

#include <cstddef>
#include <memory>
#include <utility>
#include <vector>

namespace All7
{
    struct IInit
    {
        virtual ~IInit()                      = default;
        [[nodiscard]] virtual int Tag() const = 0;
    };

    struct InitA final : IInit
    {
        [[nodiscard]] int Tag() const override { return 1; }
    };

    struct InitB final : IInit
    {
        [[nodiscard]] int Tag() const override { return 2; }
    };

    struct IDerived : IInit
    {
    };

    struct InitC final : IDerived
    {
        [[nodiscard]] int Tag() const override { return 3; }
    };

    struct IAggregate
    {
        virtual ~IAggregate()                           = default;
        [[nodiscard]] virtual std::size_t Count() const = 0;
    };

    class Aggregate final : public IAggregate
    {
    public:
        explicit Aggregate(std::vector<std::shared_ptr<IInit>> items) : m_Items(std::move(items)) {}

        [[nodiscard]] std::size_t Count() const override { return m_Items.size(); }

    private:
        std::vector<std::shared_ptr<IInit>> m_Items;
    };
}

DI_DESCRIBE(All7::InitA)
{
    DI_SERVICE(All7::IInit);
    DI_LIFETIME(Singleton);
};

DI_DESCRIBE(All7::InitB)
{
    DI_SERVICE(All7::IInit);
    DI_LIFETIME(Singleton);
};

DI_DESCRIBE(All7::InitC)
{
    DI_SERVICE(All7::IDerived);
    DI_EXPOSE(All7::IInit);
    DI_LIFETIME(Singleton);
};

DI_DESCRIBE(All7::Aggregate)
{
    DI_SERVICE(All7::IAggregate);
    DI_DEPENDS(Di::All<All7::IInit>);
    DI_LIFETIME(LazySingleton);
    DI_ORDER(100);
};

TEST_CASE("All<> collects direct and exposed implementations")
{
    auto provider  = DiTests::BuildFromDescriptors<All7::InitA, All7::InitB, All7::InitC, All7::Aggregate>();
    auto aggregate = DiTests::Resolve<All7::IAggregate>(*provider);

    REQUIRE(aggregate != nullptr);
    REQUIRE(aggregate->Count() == 3);
}
