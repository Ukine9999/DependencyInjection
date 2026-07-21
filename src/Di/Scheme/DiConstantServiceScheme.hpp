#pragma once

#include "Di/Scheme/DiByServiceTypeScheme.hpp"

#include <memory>
#include <typeindex>
#include <utility>

namespace Di::Scheme
{
    class DiConstantServiceScheme final : public DiByServiceTypeScheme
    {
    public:
        DiConstantServiceScheme(EDiServiceLifetime lifetime, std::shared_ptr<void> instance, std::type_index serviceType, DiServiceKey key)
            : DiByServiceTypeScheme(lifetime, serviceType, std::move(key)), m_Instance(std::move(instance))
        {
        }

        [[nodiscard]] Constructor GetServiceConstructor() const override
        {
            std::shared_ptr<void> instance = m_Instance;
            return [instance](IDiServiceProvider&) { return instance; };
        }

    private:
        std::shared_ptr<void> m_Instance;
    };
}
