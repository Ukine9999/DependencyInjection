#include "Di/Provide/DiScope.hpp"

#include "Di/Provide/DiServiceProvider.hpp"
#include "Di/Scheme/Abstraction/EDiServiceLifetime.hpp"

#include <stdexcept>
#include <utility>

namespace Di::Provide
{
    using Scheme::Abstraction::EDiServiceLifetime;

    DiScope::DiScope(DiServiceProvider& owner) : m_Owner(owner), m_Scoped(owner.m_Schemes.size()) {}

    DiScope::~DiScope()
    {
        for (auto disposable = m_Disposables.rbegin(); disposable != m_Disposables.rend(); ++disposable)
            disposable->second(disposable->first);
    }

    bool DiScope::IsImplement(const DiServiceId& id, bool ignoreKeys) const
    {
        const auto bucket = m_Owner.m_BindingsByType.find(id.Type());
        if (bucket == m_Owner.m_BindingsByType.end())
            return false;

        for (std::size_t index : bucket->second)
            if (m_Owner.m_Bindings[index].Id.Equals(id, ignoreKeys))
                return true;

        return false;
    }

    std::vector<std::shared_ptr<void>> DiScope::ResolveServices(const DiServiceId& id, bool ignoreKeys)
    {
        std::vector<std::shared_ptr<void>> result;

        const auto bucket = m_Owner.m_BindingsByType.find(id.Type());
        if (bucket == m_Owner.m_BindingsByType.end())
            return result;

        for (std::size_t index : bucket->second)
            if (m_Owner.m_Bindings[index].Id.Equals(id, ignoreKeys))
                result.push_back(ResolveBinding(index));

        return result;
    }

    std::shared_ptr<void> DiScope::ResolveService(const DiServiceId& id, bool ignoreKeys)
    {
        const auto bucket = m_Owner.m_BindingsByType.find(id.Type());
        if (bucket != m_Owner.m_BindingsByType.end())
            for (std::size_t index : bucket->second)
                if (m_Owner.m_Bindings[index].Id.Equals(id, ignoreKeys))
                    return ResolveBinding(index);

        throw std::runtime_error("DiServiceProvider: no service registered for " + to_string(id));
    }

    std::shared_ptr<void> DiScope::ResolveBinding(std::size_t bindingIndex)
    {
        const DiServiceProvider::Binding& binding = m_Owner.m_Bindings[bindingIndex];
        const DiServiceProvider::Scheme&  scheme  = m_Owner.m_Schemes[binding.SchemeIndex];

        for (const DiServiceId& active : m_Resolving)
            if (active.Equals(binding.Id, false))
                throw std::runtime_error("DiServiceProvider: dependency cycle detected at " + to_string(binding.Id));

        m_Resolving.push_back(binding.Id);

        struct ResolvingGuard
        {
            std::vector<DiServiceId>& stack;
            ~ResolvingGuard() { stack.pop_back(); }
        } guard {m_Resolving};

        std::shared_ptr<void> mostDerived;

        switch (scheme.Lifetime)
        {
        case EDiServiceLifetime::Singleton:
        case EDiServiceLifetime::LazySingleton:
            mostDerived = m_Owner.GetOrCreateSingleton(binding.SchemeIndex);
            break;

        case EDiServiceLifetime::Scoped:
            if (m_BuildingSingleton > 0)
                throw std::runtime_error("DiServiceProvider: captive dependency, a singleton must not depend on scoped " + to_string(binding.Id));

            mostDerived = BuildScoped(binding.SchemeIndex);
            break;

        case EDiServiceLifetime::Transient:
            mostDerived = scheme.Construct(*this);
            Track(mostDerived, scheme.Disposer);
            break;

        default:
            throw std::logic_error("DiServiceProvider: unsupported lifetime for " + to_string(binding.Id));
        }

        return binding.Cast(mostDerived);
    }

    std::shared_ptr<void> DiScope::BuildScoped(std::size_t schemeIndex)
    {
        if (m_Scoped[schemeIndex] != nullptr)
            return m_Scoped[schemeIndex];

        const DiServiceProvider::Scheme& scheme = m_Owner.m_Schemes[schemeIndex];

        std::shared_ptr<void> instance = scheme.Construct(*this);
        m_Scoped[schemeIndex]          = instance;
        Track(instance, scheme.Disposer);

        return instance;
    }

    void DiScope::Track(std::shared_ptr<void> instance, const DiDisposer& disposer)
    {
        if (disposer)
            m_Disposables.emplace_back(std::move(instance), disposer);
    }
}
