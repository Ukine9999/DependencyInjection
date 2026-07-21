#include "Di/Scheme/Factory/DiSchemeFactory.hpp"

#include "Di/Scheme/DiFactoryServiceScheme.hpp"

#include <memory>

namespace Di::Scheme::Factory
{
    std::shared_ptr<::Di::Scheme::Abstraction::IDiServiceScheme> DiSchemeFactory::Create(const ::Di::Registry::DiRegistrationRow& row)
    {
        return std::make_shared<::Di::Scheme::DiFactoryServiceScheme>(row.Lifetime, row.Construct, row.Service, row.Key, row.Bindings, row.Disposer);
    }
}
