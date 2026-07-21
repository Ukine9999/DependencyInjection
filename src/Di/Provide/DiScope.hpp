#pragma once

#include "Di/Services/Abstraction/DiDisposer.hpp"
#include "Di/Services/Abstraction/DiServiceId.hpp"
#include "Di/Services/Abstraction/IDiServiceProvider.hpp"

#include <cstddef>
#include <memory>
#include <utility>
#include <vector>

namespace Di::Provide
{
    using Services::Abstraction::DiDisposer;
    using Services::Abstraction::DiServiceId;
    using Services::Abstraction::IDiServiceProvider;

    class DiServiceProvider;

    class DiScope final : public IDiServiceProvider
    {
    public:
        explicit DiScope(DiServiceProvider& owner);
        ~DiScope() override;

        DiScope(const DiScope&)            = delete;
        DiScope& operator=(const DiScope&) = delete;

        [[nodiscard]] bool                               IsImplement(const DiServiceId& id, bool ignoreKeys) const override;
        [[nodiscard]] std::vector<std::shared_ptr<void>> ResolveServices(const DiServiceId& id, bool ignoreKeys) override;
        [[nodiscard]] std::shared_ptr<void>              ResolveService(const DiServiceId& id, bool ignoreKeys) override;

        using IDiServiceProvider::ResolveService;
        using IDiServiceProvider::ResolveServices;

    private:
        friend class DiServiceProvider;

        [[nodiscard]] std::shared_ptr<void> ResolveBinding(std::size_t bindingIndex);
        [[nodiscard]] std::shared_ptr<void> BuildScoped(std::size_t schemeIndex);
        void                                Track(std::shared_ptr<void> instance, const DiDisposer& disposer);

        DiServiceProvider&                                        m_Owner;
        std::vector<std::shared_ptr<void>>                        m_Scoped;
        std::vector<std::pair<std::shared_ptr<void>, DiDisposer>> m_Disposables;
        std::vector<DiServiceId>                                  m_Resolving;
        int                                                       m_BuildingSingleton = 0;
    };
}
