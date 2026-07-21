#pragma once

#include "Di/Services/Abstraction/DiServiceId.hpp"

#include <memory>
#include <utility>
#include <vector>

namespace Di::Services::Abstraction
{
    class IDiServiceProvider
    {
    public:
        virtual ~IDiServiceProvider() = default;

        [[nodiscard]] virtual bool                               IsImplement(const DiServiceId& id, bool ignoreKeys) const = 0;
        [[nodiscard]] virtual std::vector<std::shared_ptr<void>> ResolveServices(const DiServiceId& id, bool ignoreKeys)   = 0;
        [[nodiscard]] virtual std::shared_ptr<void>              ResolveService(const DiServiceId& id, bool ignoreKeys)    = 0;

        template <class TService> [[nodiscard]] std::shared_ptr<TService> ResolveService()
        {
            return std::static_pointer_cast<TService>(ResolveService(DiServiceId::FromType<TService>(), true));
        }

        template <class TService> [[nodiscard]] std::shared_ptr<TService> ResolveService(DiServiceKey key)
        {
            return std::static_pointer_cast<TService>(ResolveService(DiServiceId::FromType<TService>(std::move(key)), false));
        }

        template <class TService> [[nodiscard]] std::shared_ptr<TService> ResolveKeyedService(DiServiceKey key)
        {
            return std::static_pointer_cast<TService>(ResolveService(DiServiceId::FromType<TService>(std::move(key)), false));
        }

        template <class TService> [[nodiscard]] std::vector<std::shared_ptr<TService>> ResolveServices()
        {
            return Cast<TService>(ResolveServices(DiServiceId::FromType<TService>(), true));
        }

        template <class TService> [[nodiscard]] std::vector<std::shared_ptr<TService>> ResolveKeyedServices(DiServiceKey key)
        {
            return Cast<TService>(ResolveServices(DiServiceId::FromType<TService>(std::move(key)), false));
        }

        template <class TService> [[nodiscard]] bool IsImplemented()
        {
            return IsImplement(DiServiceId::FromType<TService>(), true);
        }

    private:
        template <class TService> static std::vector<std::shared_ptr<TService>> Cast(std::vector<std::shared_ptr<void>> items)
        {
            std::vector<std::shared_ptr<TService>> result;
            result.reserve(items.size());

            for (std::shared_ptr<void>& item : items)
                result.push_back(std::static_pointer_cast<TService>(item));

            return result;
        }
    };
}
