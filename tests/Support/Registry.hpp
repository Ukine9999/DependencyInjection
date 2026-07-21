#pragma once

#include "Di/Provide/DiServiceProvider.hpp"
#include "Di/Registry/DiRegistrationRow.hpp"
#include "Di/Registry/DiRegistry.hpp"
#include "Di/Scheme/Factory/DiSchemeFactory.hpp"
#include "Di/Scheme/Read/DiSchemeReader.hpp"
#include "Di/Services/Abstraction/DiServiceId.hpp"
#include "Di/Services/Abstraction/DiServiceKey.hpp"

#include <memory>
#include <string>

namespace DiTests
{
    template <class... TImplementations> std::shared_ptr<Di::Provide::DiServiceProvider> BuildFromDescriptors()
    {
        Di::Registry::DiRegistry registry;
        (registry.Add(Di::Registry::DiRegistrationRow::From<TImplementations>()), ...);

        Di::Scheme::Factory::DiSchemeFactory schemeFactory;
        Di::Scheme::Read::DiSchemeReader     reader(schemeFactory);
        reader.Read(registry);

        return std::make_shared<Di::Provide::DiServiceProvider>(reader.EndRead());
    }

    template <class TService> std::shared_ptr<TService> ResolveKeyed(Di::Provide::DiServiceProvider& provider, const std::string& key)
    {
        const auto id = Di::Services::Abstraction::DiServiceId::FromType<TService>(Di::Services::Abstraction::DiServiceKey {key});
        return std::static_pointer_cast<TService>(provider.ResolveService(id, false));
    }
}
