#pragma once

#include "Di/Scheme/Abstraction/EDiServiceLifetime.hpp"
#include "Di/Services/Abstraction/DiCaster.hpp"
#include "Di/Services/Abstraction/DiDisposer.hpp"
#include "Di/Services/Abstraction/DiServiceId.hpp"
#include "Di/Services/Abstraction/IDiServiceProvider.hpp"

#include <functional>
#include <memory>
#include <utility>
#include <vector>

namespace Di::Scheme::Abstraction
{
    using Services::Abstraction::DiCaster;
    using Services::Abstraction::DiDisposer;
    using Services::Abstraction::DiServiceId;
    using Services::Abstraction::IDiServiceProvider;

    class IDiServiceScheme
    {
    public:
        using Constructor = std::function<std::shared_ptr<void>(IDiServiceProvider&)>;
        using Binding     = std::pair<DiServiceId, DiCaster>;
        using Disposer    = DiDisposer;

        virtual ~IDiServiceScheme() = default;

        [[nodiscard]] virtual EDiServiceLifetime Lifetime() const              = 0;
        [[nodiscard]] virtual DiServiceId        GetServiceId() const          = 0;
        [[nodiscard]] virtual Constructor        GetServiceConstructor() const = 0;

        [[nodiscard]] virtual std::vector<Binding> GetBindings() const
        {
            return {Binding {GetServiceId(), [](const std::shared_ptr<void>& instance) { return instance; }}};
        }

        [[nodiscard]] virtual Disposer GetDisposer() const { return {}; }
    };
}
