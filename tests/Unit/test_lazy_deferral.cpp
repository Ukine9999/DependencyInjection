#include "Support/Registry.hpp"
#include "Support/Wiring.hpp"

#include "Di/Registry/DiDescribe.hpp"

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <utility>

namespace LazyDeferral
{
    inline int g_Constructions = 0;

    struct IDep
    {
        virtual ~IDep()                         = default;
        [[nodiscard]] virtual int Value() const = 0;
    };

    struct Dep final : IDep
    {
        Dep() { ++g_Constructions; }

        [[nodiscard]] int Value() const override { return 9; }
    };

    struct IUser
    {
        virtual ~IUser()             = default;
        [[nodiscard]] virtual int Peek() = 0;
    };

    class User final : public IUser
    {
    public:
        explicit User(Di::DiLazy<IDep> lazy) : m_Lazy(std::move(lazy)) {}

        [[nodiscard]] int Peek() override { return m_Lazy->Value(); }

    private:
        Di::DiLazy<IDep> m_Lazy;
    };
}

DI_DESCRIBE(LazyDeferral::Dep)
{
    DI_SERVICE(LazyDeferral::IDep);
    DI_LIFETIME(Transient);
};

DI_DESCRIBE(LazyDeferral::User)
{
    DI_SERVICE(LazyDeferral::IUser);
    DI_DEPENDS(Di::Lazy<LazyDeferral::IDep>);
    DI_LIFETIME(Singleton);
};

TEST_CASE("Lazy<> does not construct its target until the first Get and caches it thereafter")
{
    LazyDeferral::g_Constructions = 0;

    auto provider = DiTests::BuildFromDescriptors<LazyDeferral::Dep, LazyDeferral::User>();
    REQUIRE(LazyDeferral::g_Constructions == 0);

    auto user = DiTests::Resolve<LazyDeferral::IUser>(*provider);
    REQUIRE(LazyDeferral::g_Constructions == 0);

    REQUIRE(user->Peek() == 9);
    REQUIRE(LazyDeferral::g_Constructions == 1);

    REQUIRE(user->Peek() == 9);
    REQUIRE(LazyDeferral::g_Constructions == 1);
}
