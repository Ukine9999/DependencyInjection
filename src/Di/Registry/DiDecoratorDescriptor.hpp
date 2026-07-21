#pragma once

#include "Di/Engine/Attributes/DiParameters.hpp"
#include "Di/Registry/DiDecorator.hpp"
#include "Di/Registry/DiDependencies.hpp"
#include "Di/Services/Abstraction/DiServiceId.hpp"
#include "Di/Services/Abstraction/IDiServiceProvider.hpp"

#include <memory>
#include <type_traits>
#include <utility>

namespace Di::Registry
{
    template <class TDecorator> struct DiDecoratorDescriptorDefaults
    {
        using Implementation = TDecorator;
        using Depends        = DiDependencies<>;
    };

    template <class TDecorator> struct DiDecoratorDescriptor;

    template <class TDecorator, class TService, class... TExtra>
    std::shared_ptr<void> BuildDecorator(DiDependencies<TExtra...>, std::shared_ptr<void> innerAtService,
                                         ::Di::Services::Abstraction::IDiServiceProvider& provider)
    {
        auto inner     = std::static_pointer_cast<TService>(std::move(innerAtService));
        auto decorated = std::make_shared<TDecorator>(inner, TExtra::Resolve(provider, false)...);
        return std::static_pointer_cast<void>(std::static_pointer_cast<TService>(decorated));
    }

    template <class TDecorator> DiDecorator MakeDecorator()
    {
        using Descriptor  = DiDecoratorDescriptor<TDecorator>;
        using ServiceType = typename Descriptor::Service;
        static_assert(std::is_base_of_v<ServiceType, TDecorator>, "a decorator must derive from the service it decorates");

        return DiDecorator {::Di::Services::Abstraction::DiServiceId::FromType<ServiceType>(),
                            [](std::shared_ptr<void> innerAtService, ::Di::Services::Abstraction::IDiServiceProvider& provider)
                            { return BuildDecorator<TDecorator, ServiceType>(typename Descriptor::Depends {}, std::move(innerAtService), provider); }};
    }
}
