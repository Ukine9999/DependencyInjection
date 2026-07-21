#pragma once

#include "Di/Scheme/DiByServiceKeyScheme.hpp"

#include <typeindex>
#include <utility>

namespace Di::Scheme
{
    class DiByServiceTypeScheme : public DiByServiceKeyScheme
    {
    public:
        DiByServiceTypeScheme(EDiServiceLifetime lifetime, std::type_index serviceType, DiServiceKey key)
            : DiByServiceKeyScheme(lifetime, std::move(key)), m_ServiceType(serviceType)
        {
        }

    protected:
        [[nodiscard]] DiServiceId GetServiceIdInternal(const DiServiceKey& key) const final
        {
            return DiServiceId::FromTypeIndex(m_ServiceType, key);
        }

    private:
        std::type_index m_ServiceType;
    };
}
