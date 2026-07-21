#pragma once

#include "Di/Services/Abstraction/DiDependencyInfo.hpp"
#include "Di/Services/Abstraction/DiServiceId.hpp"
#include "Di/Services/Abstraction/DiServiceKey.hpp"
#include "Di/Services/Abstraction/IDiServiceProvider.hpp"

#include <cstddef>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace Di::Engine::Attributes
{
    using Services::Abstraction::DiDependencyInfo;
    using Services::Abstraction::DiServiceId;
    using Services::Abstraction::DiServiceKey;
    using Services::Abstraction::IDiServiceProvider;

    template <std::size_t N> struct DiKey
    {
        char Value[N] {};

        constexpr DiKey(const char (&text)[N])
        {
            for (std::size_t index = 0; index < N; ++index)
                Value[index] = text[index];
        }

        [[nodiscard]] std::string str() const { return std::string(Value, N - 1); }
    };

    template <class TService> struct Service
    {
        using Argument = std::shared_ptr<TService>;

        static Argument Resolve(IDiServiceProvider& provider, bool ignoreKeys)
        {
            return std::static_pointer_cast<TService>(provider.ResolveService(DiServiceId::FromType<TService>(), ignoreKeys));
        }

        static DiDependencyInfo Dependency() { return {DiServiceId::FromType<TService>(), true, false}; }
    };

    template <class TService, auto Key> struct Keyed
    {
        using Argument = std::shared_ptr<TService>;

        static Argument Resolve(IDiServiceProvider& provider, bool)
        {
            return std::static_pointer_cast<TService>(provider.ResolveService(DiServiceId::FromType<TService>(DiServiceKey {Key.str()}), false));
        }

        static DiDependencyInfo Dependency() { return {DiServiceId::FromType<TService>(DiServiceKey {Key.str()}), true, false}; }
    };

    template <class TService> struct All
    {
        using Argument = std::vector<std::shared_ptr<TService>>;

        static Argument Resolve(IDiServiceProvider& provider, bool)
        {
            Argument result;

            for (const std::shared_ptr<void>& item : provider.ResolveServices(DiServiceId::FromType<TService>(), true))
                result.push_back(std::static_pointer_cast<TService>(item));

            return result;
        }

        static DiDependencyInfo Dependency() { return {DiServiceId::FromType<TService>(), false, false}; }
    };

    template <class TService> struct Optional
    {
        using Argument = std::shared_ptr<TService>;

        static Argument Resolve(IDiServiceProvider& provider, bool ignoreKeys)
        {
            const DiServiceId id = DiServiceId::FromType<TService>();

            if (!provider.IsImplement(id, ignoreKeys))
                return nullptr;

            return std::static_pointer_cast<TService>(provider.ResolveService(id, ignoreKeys));
        }

        static DiDependencyInfo Dependency() { return {DiServiceId::FromType<TService>(), false, false}; }
    };

    template <class TService> class DiLazy
    {
    public:
        explicit DiLazy(std::function<std::shared_ptr<TService>()> factory) : m_Factory(std::move(factory)) {}

        [[nodiscard]] std::shared_ptr<TService> Get()
        {
            if (m_Instance == nullptr)
                m_Instance = m_Factory();

            return m_Instance;
        }

        [[nodiscard]] TService* operator->() { return Get().get(); }
        [[nodiscard]] TService& operator*() { return *Get(); }

    private:
        std::function<std::shared_ptr<TService>()> m_Factory;
        std::shared_ptr<TService>                  m_Instance;
    };

    template <class TService> struct Factory
    {
        using Argument = std::function<std::shared_ptr<TService>()>;

        static Argument Resolve(IDiServiceProvider& provider, bool)
        {
            return [&provider] { return std::static_pointer_cast<TService>(provider.ResolveService(DiServiceId::FromType<TService>(), true)); };
        }

        static DiDependencyInfo Dependency() { return {DiServiceId::FromType<TService>(), true, true}; }
    };

    template <class TService> struct Lazy
    {
        using Argument = DiLazy<TService>;

        static Argument Resolve(IDiServiceProvider& provider, bool)
        {
            return DiLazy<TService>([&provider]
                                    { return std::static_pointer_cast<TService>(provider.ResolveService(DiServiceId::FromType<TService>(), true)); });
        }

        static DiDependencyInfo Dependency() { return {DiServiceId::FromType<TService>(), true, true}; }
    };
}

namespace Di
{
    using Engine::Attributes::All;
    using Engine::Attributes::DiKey;
    using Engine::Attributes::DiLazy;
    using Engine::Attributes::Factory;
    using Engine::Attributes::Keyed;
    using Engine::Attributes::Lazy;
    using Engine::Attributes::Optional;
    using Engine::Attributes::Service;
}
