#include "Support/Registry.hpp"
#include "Support/Wiring.hpp"

#include "Di/Registry/DiDescribe.hpp"

#include <memory>
#include <utility>

#include <catch2/catch_test_macros.hpp>

namespace LifetimeComposition
{
    struct IShared
    {
        virtual ~IShared() = default;
    };

    struct Shared final : IShared
    {
    };

    struct IJob
    {
        virtual ~IJob()                                      = default;
        [[nodiscard]] virtual std::shared_ptr<IShared> Dep() const = 0;
    };

    class Job final : public IJob
    {
    public:
        explicit Job(std::shared_ptr<IShared> shared) : m_Shared(std::move(shared)) {}

        [[nodiscard]] std::shared_ptr<IShared> Dep() const override { return m_Shared; }

    private:
        std::shared_ptr<IShared> m_Shared;
    };
}

DI_DESCRIBE(LifetimeComposition::Shared)
{
    DI_SERVICE(LifetimeComposition::IShared);
    DI_LIFETIME(Singleton);
};

DI_DESCRIBE(LifetimeComposition::Job)
{
    DI_SERVICE(LifetimeComposition::IJob);
    DI_DEPENDS(Di::Service<LifetimeComposition::IShared>);
    DI_LIFETIME(Transient);
};

TEST_CASE("two transient resolves are distinct instances yet share the one singleton they depend on")
{
    auto provider = DiTests::BuildFromDescriptors<LifetimeComposition::Shared, LifetimeComposition::Job>();

    auto first  = DiTests::Resolve<LifetimeComposition::IJob>(*provider);
    auto second = DiTests::Resolve<LifetimeComposition::IJob>(*provider);

    REQUIRE(first.get() != second.get());
    REQUIRE(first->Dep() != nullptr);
    REQUIRE(first->Dep().get() == second->Dep().get());
}
