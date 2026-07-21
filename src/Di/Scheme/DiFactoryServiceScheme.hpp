#pragma once

#include "Di/Scheme/DiByServiceTypeScheme.hpp"

#include <typeindex>
#include <utility>
#include <vector>

namespace Di::Scheme
{
    class DiFactoryServiceScheme final : public DiByServiceTypeScheme
    {
    public:
        DiFactoryServiceScheme(EDiServiceLifetime lifetime, Constructor constructor, std::type_index serviceType, DiServiceKey key,
                               std::vector<Binding> bindings, Disposer disposer)
            : DiByServiceTypeScheme(lifetime, serviceType, std::move(key)), m_Constructor(std::move(constructor)), m_Bindings(std::move(bindings)),
              m_Disposer(std::move(disposer))
        {
        }

        [[nodiscard]] Constructor GetServiceConstructor() const override { return m_Constructor; }

        [[nodiscard]] std::vector<Binding> GetBindings() const override { return m_Bindings; }

        [[nodiscard]] Disposer GetDisposer() const override { return m_Disposer; }

    private:
        Constructor          m_Constructor;
        std::vector<Binding> m_Bindings;
        Disposer             m_Disposer;
    };
}
