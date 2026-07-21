#include "Support/Registry.hpp"
#include "Support/Wiring.hpp"

#include "Di/Registry/DiDescribe.hpp"

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <stdexcept>
#include <utility>

namespace Missing9
{
    struct IMissing
    {
        virtual ~IMissing() = default;
    };

    struct INeedy
    {
        virtual ~INeedy() = default;
    };

    struct Needy final : INeedy
    {
        explicit Needy(std::shared_ptr<IMissing>) {}
    };

    struct IOptionalNeedy
    {
        virtual ~IOptionalNeedy()                        = default;
        [[nodiscard]] virtual bool HasDependency() const = 0;
    };

    class OptionalNeedy final : public IOptionalNeedy
    {
    public:
        explicit OptionalNeedy(std::shared_ptr<IMissing> dependency) : m_Dependency(std::move(dependency)) {}

        [[nodiscard]] bool HasDependency() const override { return m_Dependency != nullptr; }

    private:
        std::shared_ptr<IMissing> m_Dependency;
    };
}

DI_DESCRIBE(Missing9::Needy)
{
    DI_SERVICE(Missing9::INeedy);
    DI_DEPENDS(Di::Service<Missing9::IMissing>);
    DI_LIFETIME(Singleton);
};

DI_DESCRIBE(Missing9::OptionalNeedy)
{
    DI_SERVICE(Missing9::IOptionalNeedy);
    DI_DEPENDS(Di::Optional<Missing9::IMissing>);
    DI_LIFETIME(Singleton);
};

TEST_CASE("missing required dependency throws loudly")
{
    REQUIRE_THROWS_AS(DiTests::BuildFromDescriptors<Missing9::Needy>(), std::runtime_error);
}

TEST_CASE("optional missing dependency resolves to null")
{
    auto provider = DiTests::BuildFromDescriptors<Missing9::OptionalNeedy>();
    auto needy    = DiTests::Resolve<Missing9::IOptionalNeedy>(*provider);

    REQUIRE(needy != nullptr);
    REQUIRE(needy->HasDependency() == false);
}
