#pragma once

#include "Di/Provide/DiServiceProvider.hpp"
#include "Di/Scheme/Abstraction/EDiServiceLifetime.hpp"
#include "Di/Scheme/Abstraction/IDiServiceScheme.hpp"
#include "Di/Scheme/DiFunctionServiceScheme.hpp"
#include "Di/Services/Abstraction/DiServiceId.hpp"
#include "Di/Services/Abstraction/DiServiceKey.hpp"

#include <memory>
#include <typeindex>
#include <utility>
#include <vector>

namespace DiTests
{
    using SchemeList = std::vector<std::shared_ptr<Di::Scheme::Abstraction::IDiServiceScheme>>;

    template <class TService, class TImplementation>
    std::shared_ptr<Di::Scheme::Abstraction::IDiServiceScheme> Provide(Di::Scheme::Abstraction::EDiServiceLifetime lifetime,
                                                                       Di::Services::Abstraction::DiServiceKey     key = {})
    {
        return std::make_shared<Di::Scheme::DiFunctionServiceScheme>(
            lifetime, [](Di::Services::Abstraction::IDiServiceProvider&) -> std::shared_ptr<void> { return std::make_shared<TImplementation>(); },
            std::type_index(typeid(TService)), std::move(key));
    }

    inline std::shared_ptr<Di::Provide::DiServiceProvider> BuildProvider(const SchemeList& schemes)
    {
        return std::make_shared<Di::Provide::DiServiceProvider>(schemes);
    }

    template <class TService> std::shared_ptr<TService> Resolve(Di::Provide::DiServiceProvider& provider, bool ignoreKeys = false)
    {
        return std::static_pointer_cast<TService>(provider.ResolveService(Di::Services::Abstraction::DiServiceId::FromType<TService>(), ignoreKeys));
    }
}
