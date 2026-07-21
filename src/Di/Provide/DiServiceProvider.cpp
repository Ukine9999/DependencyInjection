#include "Di/Provide/DiServiceProvider.hpp"

#include "Di/Provide/DiScope.hpp"
#include "Di/Scheme/Abstraction/EDiServiceLifetime.hpp"
#include "Di/Scheme/Abstraction/IDiServiceScheme.hpp"

#include <mutex>
#include <stdexcept>
#include <utility>

namespace Di::Provide
{
    using Scheme::Abstraction::EDiServiceLifetime;

    DiServiceProvider::DiServiceProvider(const std::vector<std::shared_ptr<IDiServiceScheme>>& schemes)
    {
        for (const std::shared_ptr<IDiServiceScheme>& scheme : schemes)
        {
            const std::size_t schemeIndex = m_Schemes.size();
            m_Schemes.push_back(Scheme {scheme->Lifetime(), scheme->GetServiceConstructor(), scheme->GetDisposer()});

            for (const IDiServiceScheme::Binding& binding : scheme->GetBindings())
                m_Bindings.push_back(Binding {binding.first, schemeIndex, binding.second});
        }

        for (std::size_t index = 0; index < m_Bindings.size(); ++index)
            m_BindingsByType[m_Bindings[index].Id.Type()].push_back(index);

        m_Singletons.resize(m_Schemes.size());
        m_SingletonBuilding.resize(m_Schemes.size(), false);
        m_RootScope = std::make_unique<DiScope>(*this);

        for (std::size_t index = 0; index < m_Schemes.size(); ++index)
            if (m_Schemes[index].Lifetime == EDiServiceLifetime::Singleton)
                GetOrCreateSingleton(index);
    }

    DiServiceProvider::~DiServiceProvider()
    {
        m_RootScope.reset();

        for (auto disposable = m_SingletonDisposables.rbegin(); disposable != m_SingletonDisposables.rend(); ++disposable)
            disposable->second(disposable->first);
    }

    bool DiServiceProvider::IsImplement(const DiServiceId& id, bool ignoreKeys) const
    {
        return m_RootScope->IsImplement(id, ignoreKeys);
    }

    std::vector<std::shared_ptr<void>> DiServiceProvider::ResolveServices(const DiServiceId& id, bool ignoreKeys)
    {
        std::lock_guard<std::recursive_mutex> lock(m_Mutex);
        return m_RootScope->ResolveServices(id, ignoreKeys);
    }

    std::shared_ptr<void> DiServiceProvider::ResolveService(const DiServiceId& id, bool ignoreKeys)
    {
        std::lock_guard<std::recursive_mutex> lock(m_Mutex);
        return m_RootScope->ResolveService(id, ignoreKeys);
    }

    std::unique_ptr<DiScope> DiServiceProvider::CreateScope()
    {
        return std::make_unique<DiScope>(*this);
    }

    std::shared_ptr<void> DiServiceProvider::GetOrCreateSingleton(std::size_t schemeIndex)
    {
        std::lock_guard<std::recursive_mutex> lock(m_Mutex);

        if (m_Singletons[schemeIndex] != nullptr)
            return m_Singletons[schemeIndex];

        if (m_SingletonBuilding[schemeIndex])
            throw std::runtime_error("DiServiceProvider: dependency cycle detected while building a singleton");

        const Scheme& scheme = m_Schemes[schemeIndex];

        struct BuildGuard
        {
            std::vector<bool>& building;
            std::size_t        index;
            int&               depth;

            BuildGuard(std::vector<bool>& b, std::size_t i, int& d) : building(b), index(i), depth(d)
            {
                building[index] = true;
                ++depth;
            }

            ~BuildGuard()
            {
                building[index] = false;
                --depth;
            }
        } guard {m_SingletonBuilding, schemeIndex, m_RootScope->m_BuildingSingleton};

        std::shared_ptr<void> instance = scheme.Construct(*m_RootScope);
        m_Singletons[schemeIndex]      = instance;

        if (scheme.Disposer)
            m_SingletonDisposables.emplace_back(instance, scheme.Disposer);

        return instance;
    }
}
