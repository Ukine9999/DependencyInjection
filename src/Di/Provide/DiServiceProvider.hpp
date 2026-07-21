#pragma once

#include "Di/Provide/DiScope.hpp"
#include "Di/Scheme/Abstraction/EDiServiceLifetime.hpp"
#include "Di/Scheme/Abstraction/IDiServiceScheme.hpp"
#include "Di/Services/Abstraction/DiCaster.hpp"
#include "Di/Services/Abstraction/DiDisposer.hpp"
#include "Di/Services/Abstraction/DiServiceId.hpp"
#include "Di/Services/Abstraction/IDiServiceProvider.hpp"

#include <cstddef>
#include <memory>
#include <mutex>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

namespace Di::Provide
{
    using Scheme::Abstraction::IDiServiceScheme;
    using Services::Abstraction::DiCaster;
    using Services::Abstraction::DiDisposer;
    using Services::Abstraction::DiServiceId;
    using Services::Abstraction::IDiServiceProvider;

    class DiServiceProvider final : public IDiServiceProvider
    {
    public:
        explicit DiServiceProvider(const std::vector<std::shared_ptr<IDiServiceScheme>>& schemes);
        ~DiServiceProvider() override;

        DiServiceProvider(const DiServiceProvider&)            = delete;
        DiServiceProvider& operator=(const DiServiceProvider&) = delete;

        [[nodiscard]] bool                               IsImplement(const DiServiceId& id, bool ignoreKeys) const override;
        [[nodiscard]] std::vector<std::shared_ptr<void>> ResolveServices(const DiServiceId& id, bool ignoreKeys) override;
        [[nodiscard]] std::shared_ptr<void>              ResolveService(const DiServiceId& id, bool ignoreKeys) override;

        using IDiServiceProvider::ResolveService;
        using IDiServiceProvider::ResolveServices;

        [[nodiscard]] std::unique_ptr<DiScope> CreateScope();

    private:
        friend class DiScope;

        struct Scheme
        {
            ::Di::Scheme::Abstraction::EDiServiceLifetime Lifetime;
            IDiServiceScheme::Constructor                 Construct;
            DiDisposer                                    Disposer;
        };

        struct Binding
        {
            DiServiceId Id;
            std::size_t SchemeIndex;
            DiCaster    Cast;
        };

        std::shared_ptr<void> GetOrCreateSingleton(std::size_t schemeIndex);

        std::vector<Scheme>                                          m_Schemes;
        std::vector<Binding>                                         m_Bindings;
        std::unordered_map<std::type_index, std::vector<std::size_t>> m_BindingsByType;
        std::vector<std::shared_ptr<void>>                           m_Singletons;
        std::vector<bool>                                            m_SingletonBuilding;
        std::vector<std::pair<std::shared_ptr<void>, DiDisposer>>    m_SingletonDisposables;
        std::unique_ptr<DiScope>                                     m_RootScope;
        std::recursive_mutex                                        m_Mutex;
    };
}
