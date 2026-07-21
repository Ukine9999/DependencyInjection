#include "Support/Registry.hpp"
#include "Support/Wiring.hpp"

#include "Di/Registry/DiDescribe.hpp"

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <utility>

namespace LazyCycleRuntime
{
    struct IA
    {
        virtual ~IA()                = default;
        [[nodiscard]] virtual int ReachB() = 0;
    };

    struct IB
    {
        virtual ~IB()                                    = default;
        [[nodiscard]] virtual int                 Value() const = 0;
        [[nodiscard]] virtual std::shared_ptr<IA> HeldA() const = 0;
    };

    class A final : public IA
    {
    public:
        explicit A(Di::DiLazy<IB> b) : m_B(std::move(b)) {}

        [[nodiscard]] int ReachB() override { return m_B->Value(); }

    private:
        Di::DiLazy<IB> m_B;
    };

    class B final : public IB
    {
    public:
        explicit B(std::shared_ptr<IA> a) : m_A(std::move(a)) {}

        [[nodiscard]] int                 Value() const override { return 11; }
        [[nodiscard]] std::shared_ptr<IA> HeldA() const override { return m_A; }

    private:
        std::shared_ptr<IA> m_A;
    };
}

DI_DESCRIBE(LazyCycleRuntime::A)
{
    DI_SERVICE(LazyCycleRuntime::IA);
    DI_DEPENDS(Di::Lazy<LazyCycleRuntime::IB>);
    DI_LIFETIME(Singleton);
};

DI_DESCRIBE(LazyCycleRuntime::B)
{
    DI_SERVICE(LazyCycleRuntime::IB);
    DI_DEPENDS(Di::Service<LazyCycleRuntime::IA>);
    DI_LIFETIME(Singleton);
};

TEST_CASE("a singleton cycle closed only through a Lazy<> edge builds and both sides wire to each other")
{
    auto provider = DiTests::BuildFromDescriptors<LazyCycleRuntime::A, LazyCycleRuntime::B>();

    auto a = DiTests::Resolve<LazyCycleRuntime::IA>(*provider);
    auto b = DiTests::Resolve<LazyCycleRuntime::IB>(*provider);

    REQUIRE(a->ReachB() == 11);
    REQUIRE(b->HeldA().get() == a.get());
}
