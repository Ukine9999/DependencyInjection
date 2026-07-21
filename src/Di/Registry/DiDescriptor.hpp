#pragma once

#include "Di/Registry/DiDependencies.hpp"
#include "Di/Scheme/Abstraction/EDiServiceLifetime.hpp"

#include <string_view>

namespace Di::Registry
{
    template <class TImplementation> struct DiDescriptorDefaults
    {
        using Implementation = TImplementation;
        using Service        = TImplementation;
        using Depends        = DiDependencies<>;
        using Exposes        = DiExposes<>;

        static constexpr ::Di::Scheme::Abstraction::EDiServiceLifetime Lifetime = ::Di::Scheme::Abstraction::EDiServiceLifetime::Transient;
        static constexpr std::string_view                              Key      = "";
        static constexpr int                                           Order    = 0;
    };

    template <class TImplementation> struct DiDescriptor;
}
