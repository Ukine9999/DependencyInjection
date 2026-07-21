#pragma once

#include "Di/Registry/DiDependencies.hpp"
#include "Di/Services/Abstraction/IDiServiceProvider.hpp"

#include <functional>
#include <memory>

namespace Di::Engine::Template
{
    using Services::Abstraction::IDiServiceProvider;

    class DiEngineTemplate
    {
    public:
        using Constructor = std::function<std::shared_ptr<void>(IDiServiceProvider&)>;

        explicit DiEngineTemplate(bool ignoreKeys = false) : m_IgnoreKeys(ignoreKeys) {}

        template <class TImplementation, class... TDependencies>
        [[nodiscard]] Constructor CreateConstructor(::Di::Registry::DiDependencies<TDependencies...>) const
        {
            const bool ignoreKeys = m_IgnoreKeys;

            return [ignoreKeys](IDiServiceProvider& provider) -> std::shared_ptr<void>
            { return std::static_pointer_cast<void>(std::make_shared<TImplementation>(TDependencies::Resolve(provider, ignoreKeys)...)); };
        }

    private:
        bool m_IgnoreKeys;
    };
}
