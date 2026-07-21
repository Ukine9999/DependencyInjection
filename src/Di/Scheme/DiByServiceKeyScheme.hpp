#pragma once

#include "Di/Scheme/DiServiceScheme.hpp"
#include "Di/Services/Abstraction/DiServiceKey.hpp"

#include <utility>

namespace Di::Scheme
{
    using Services::Abstraction::DiServiceKey;

    class DiByServiceKeyScheme : public DiServiceScheme
    {
    public:
        DiByServiceKeyScheme(EDiServiceLifetime lifetime, DiServiceKey key) : DiServiceScheme(lifetime), m_Key(std::move(key)) {}

        [[nodiscard]] DiServiceId GetServiceId() const final { return GetServiceIdInternal(m_Key); }

    protected:
        [[nodiscard]] virtual DiServiceId GetServiceIdInternal(const DiServiceKey& key) const = 0;

    private:
        DiServiceKey m_Key;
    };
}
