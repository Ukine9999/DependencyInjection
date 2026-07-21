#pragma once

#include "Di/Provide/DiScope.hpp"
#include "Di/Provide/DiServiceProvider.hpp"
#include "Di/Registry/Abstraction/IDiRegistry.hpp"
#include "Di/Scheme/Factory/DiSchemeFactory.hpp"
#include "Di/Services/Abstraction/IDiServiceProvider.hpp"

#include <memory>

namespace Di::Container
{
    class DiContainer
    {
    public:
        DiContainer();
        explicit DiContainer(const ::Di::Registry::Abstraction::IDiRegistry& registry);

        [[nodiscard]] ::Di::Services::Abstraction::IDiServiceProvider& Provider();

        [[nodiscard]] std::unique_ptr<::Di::Provide::DiScope> CreateScope();

    private:
        ::Di::Scheme::Factory::DiSchemeFactory            m_SchemeFactory;
        std::unique_ptr<::Di::Provide::DiServiceProvider> m_Provider;
    };
}
