#pragma once

#include "Di/Scheme/Abstraction/EDiServiceLifetime.hpp"
#include "Di/Scheme/Abstraction/IDiServiceScheme.hpp"

namespace Di::Scheme
{
    using Abstraction::EDiServiceLifetime;
    using Abstraction::IDiServiceScheme;
    using Services::Abstraction::DiServiceId;
    using Services::Abstraction::IDiServiceProvider;

    class DiServiceScheme : public IDiServiceScheme
    {
    public:
        explicit DiServiceScheme(EDiServiceLifetime lifetime) : m_Lifetime(lifetime) {}

        [[nodiscard]] EDiServiceLifetime Lifetime() const override { return m_Lifetime; }

    private:
        EDiServiceLifetime m_Lifetime;
    };
}
