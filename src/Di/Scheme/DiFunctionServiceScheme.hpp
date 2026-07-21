#pragma once

#include "Di/Scheme/DiByServiceTypeScheme.hpp"

#include <typeindex>
#include <utility>

namespace Di::Scheme
{
    class DiFunctionServiceScheme final : public DiByServiceTypeScheme
    {
    public:
        DiFunctionServiceScheme(EDiServiceLifetime lifetime, Constructor constructor, std::type_index serviceType, DiServiceKey key)
            : DiByServiceTypeScheme(lifetime, serviceType, std::move(key)), m_Constructor(std::move(constructor))
        {
        }

        [[nodiscard]] Constructor GetServiceConstructor() const override { return m_Constructor; }

    private:
        Constructor m_Constructor;
    };
}
