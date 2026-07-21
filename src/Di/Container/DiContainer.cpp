#include "Di/Container/DiContainer.hpp"

#include "Di/Registry/DiRegistry.hpp"
#include "Di/Registry/DiRegistryValidator.hpp"
#include "Di/Scheme/Read/DiSchemeReader.hpp"

#include <memory>

namespace Di::Container
{
    DiContainer::DiContainer() : DiContainer(::Di::Registry::DiRegistry::Process()) {}

    DiContainer::DiContainer(const ::Di::Registry::Abstraction::IDiRegistry& registry)
    {
        ::Di::Registry::ValidateOrThrow(registry);

        ::Di::Scheme::Read::DiSchemeReader reader(m_SchemeFactory);
        reader.Read(registry);
        m_Provider = std::make_unique<::Di::Provide::DiServiceProvider>(reader.EndRead());
    }

    ::Di::Services::Abstraction::IDiServiceProvider& DiContainer::Provider()
    {
        return *m_Provider;
    }

    std::unique_ptr<::Di::Provide::DiScope> DiContainer::CreateScope()
    {
        return m_Provider->CreateScope();
    }
}
