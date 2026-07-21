#pragma once

#include "Di/Registry/DiDecoratorDescriptor.hpp"
#include "Di/Registry/DiRegistry.hpp"

#include <string_view>

namespace Di::Registry
{
    template <class TDecorator> struct DiDecoratorRegistrar
    {
        explicit DiDecoratorRegistrar(std::string_view = {}) { DiRegistry::Process().AddDecorator(MakeDecorator<TDecorator>()); }
    };
}

#define DI_DECORATE(TYPE) template <> struct Di::Registry::DiDecoratorDescriptor<TYPE> : Di::Registry::DiDecoratorDescriptorDefaults<TYPE>

#define DI_DECORATOR(NAME)                                                                         \
    static inline const ::Di::Registry::DiDecoratorRegistrar<Implementation> DiDecoratorRegistrar_ \
    {                                                                                              \
        #NAME                                                                                      \
    }
