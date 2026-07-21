#pragma once

#include "Di/Engine/Template/DiEngineTemplate.hpp"
#include "Di/Registry/DiBinding.hpp"
#include "Di/Registry/DiDependencies.hpp"
#include "Di/Registry/DiDescriptor.hpp"
#include "Di/Registry/DiDisposer.hpp"
#include "Di/Scheme/Abstraction/EDiServiceLifetime.hpp"
#include "Di/Scheme/Abstraction/IDiServiceScheme.hpp"
#include "Di/Services/Abstraction/DiDependencyInfo.hpp"
#include "Di/Services/Abstraction/DiDisposer.hpp"
#include "Di/Services/Abstraction/DiServiceId.hpp"
#include "Di/Services/Abstraction/DiServiceKey.hpp"

#include <string>
#include <typeindex>
#include <typeinfo>
#include <vector>

namespace Di::Registry
{
    struct DiRegistrationRow
    {
        std::type_index                                                   Service;
        ::Di::Services::Abstraction::DiServiceKey                         Key;
        ::Di::Scheme::Abstraction::EDiServiceLifetime                     Lifetime;
        int                                                               Order;
        ::Di::Scheme::Abstraction::IDiServiceScheme::Constructor          Construct;
        std::vector<::Di::Scheme::Abstraction::IDiServiceScheme::Binding> Bindings;
        std::vector<::Di::Services::Abstraction::DiDependencyInfo>        Dependencies;
        ::Di::Services::Abstraction::DiDisposer                           Disposer;

        template <class TImplementation> static DiRegistrationRow From();
    };

    template <class TImplementation, class TService, class... TExposed>
    std::vector<::Di::Scheme::Abstraction::IDiServiceScheme::Binding> BuildBindings(DiExposes<TExposed...>,
                                                                                    const ::Di::Services::Abstraction::DiServiceKey& key)
    {
        using ::Di::Services::Abstraction::DiServiceId;

        std::vector<::Di::Scheme::Abstraction::IDiServiceScheme::Binding> bindings;
        bindings.emplace_back(DiServiceId::FromTypeIndex(std::type_index(typeid(TService)), key), MakeCaster<TImplementation, TService>());
        (bindings.emplace_back(DiServiceId::FromTypeIndex(std::type_index(typeid(TExposed)), key), MakeCaster<TImplementation, TExposed>()), ...);

        return bindings;
    }

    template <class... TDependencies> std::vector<::Di::Services::Abstraction::DiDependencyInfo> CollectDependencies(DiDependencies<TDependencies...>)
    {
        return {TDependencies::Dependency()...};
    }

    template <class TImplementation> DiRegistrationRow DiRegistrationRow::From()
    {
        using Descriptor  = DiDescriptor<TImplementation>;
        using ServiceType = typename Descriptor::Service;

        ::Di::Services::Abstraction::DiServiceKey key =
            Descriptor::Key.empty() ? ::Di::Services::Abstraction::DiServiceKey {} : ::Di::Services::Abstraction::DiServiceKey {std::string(Descriptor::Key)};

        return DiRegistrationRow {std::type_index(typeid(ServiceType)),
                                  key,
                                  Descriptor::Lifetime,
                                  Descriptor::Order,
                                  ::Di::Engine::Template::DiEngineTemplate {}.CreateConstructor<TImplementation>(typename Descriptor::Depends {}),
                                  BuildBindings<TImplementation, ServiceType>(typename Descriptor::Exposes {}, key),
                                  CollectDependencies(typename Descriptor::Depends {}),
                                  MakeDisposer<TImplementation>()};
    }
}
