#pragma once

#include "Di/Engine/Attributes/DiParameters.hpp"
#include "Di/Registry/DiDescriptor.hpp"
#include "Di/Registry/DiRegistrationRow.hpp"
#include "Di/Registry/DiRegistry.hpp"

#include <string_view>

namespace Di::Registry
{
    using ::Di::Engine::Attributes::All;
    using ::Di::Engine::Attributes::DiKey;
    using ::Di::Engine::Attributes::Keyed;
    using ::Di::Engine::Attributes::Optional;
    using ::Di::Engine::Attributes::Service;

    template <class TImplementation> struct DiAutoRegistrar
    {
        explicit DiAutoRegistrar(std::string_view = {}) { DiRegistry::Process().Add(DiRegistrationRow::From<TImplementation>()); }
    };
}

#define DI_DESCRIBE(TYPE) template <> struct Di::Registry::DiDescriptor<TYPE> : Di::Registry::DiDescriptorDefaults<TYPE>

#define DI_SERVICE(...) using Service = __VA_ARGS__
#define DI_DEPENDS(...) using Depends = ::Di::Registry::DiDependencies<__VA_ARGS__>
#define DI_EXPOSE(...) using Exposes = ::Di::Registry::DiExposes<__VA_ARGS__>
#define DI_LIFETIME(NAME) static constexpr ::Di::Scheme::Abstraction::EDiServiceLifetime Lifetime = ::Di::Scheme::Abstraction::EDiServiceLifetime::NAME
#define DI_KEY(TEXT) static constexpr ::std::string_view Key = (TEXT)
#define DI_ORDER(VALUE) static constexpr int Order = (VALUE)
#define DI_MODULE(NAME)                                                              \
    static inline const ::Di::Registry::DiAutoRegistrar<Implementation> DiRegistrar_ \
    {                                                                                \
        #NAME                                                                        \
    }
