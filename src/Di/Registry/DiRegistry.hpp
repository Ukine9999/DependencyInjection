#pragma once

#include "Di/Registry/Abstraction/IDiRegistry.hpp"
#include "Di/Registry/DiDecorator.hpp"
#include "Di/Registry/DiDecoratorDescriptor.hpp"
#include "Di/Registry/DiDisposer.hpp"
#include "Di/Registry/DiRegistrationRow.hpp"

#include <functional>
#include <memory>
#include <typeindex>
#include <typeinfo>
#include <utility>
#include <vector>

namespace Di::Registry
{
    class DiRegistry final : public Abstraction::IDiRegistry
    {
    public:
        static DiRegistry& Process();

        void Add(DiRegistrationRow row);
        void AddDecorator(DiDecorator decorator);

        template <class TService> void AddInstance(std::shared_ptr<TService> instance)
        {
            const auto id = ::Di::Services::Abstraction::DiServiceId::FromType<TService>();
            Add(DiRegistrationRow {std::type_index(typeid(TService)),
                                   ::Di::Services::Abstraction::DiServiceKey {},
                                   ::Di::Scheme::Abstraction::EDiServiceLifetime::Singleton,
                                   0,
                                   [instance](::Di::Services::Abstraction::IDiServiceProvider&) { return std::static_pointer_cast<void>(instance); },
                                   {{id, MakeCaster<TService, TService>()}},
                                   {},
                                   {}});
        }

        template <class TService>
        void AddFactory(::Di::Scheme::Abstraction::EDiServiceLifetime                                              lifetime,
                        std::function<std::shared_ptr<TService>(::Di::Services::Abstraction::IDiServiceProvider&)> factory)
        {
            const auto id = ::Di::Services::Abstraction::DiServiceId::FromType<TService>();
            Add(DiRegistrationRow {std::type_index(typeid(TService)),
                                   ::Di::Services::Abstraction::DiServiceKey {},
                                   lifetime,
                                   0,
                                   [factory = std::move(factory)](::Di::Services::Abstraction::IDiServiceProvider& provider)
                                   { return std::static_pointer_cast<void>(factory(provider)); },
                                   {{id, MakeCaster<TService, TService>()}},
                                   {},
                                   MakeDisposer<TService>()});
        }

        template <class TImplementation, class TService, class... TDependencies>
        void AddType(::Di::Scheme::Abstraction::EDiServiceLifetime lifetime = ::Di::Scheme::Abstraction::EDiServiceLifetime::Singleton,
                     ::Di::Services::Abstraction::DiServiceKey key = {}, int order = 0)
        {
            const auto id = ::Di::Services::Abstraction::DiServiceId::FromType<TService>(key);
            Add(DiRegistrationRow {std::type_index(typeid(TService)),
                                   key,
                                   lifetime,
                                   order,
                                   ::Di::Engine::Template::DiEngineTemplate {}.CreateConstructor<TImplementation>(DiDependencies<TDependencies...> {}),
                                   {{id, MakeCaster<TImplementation, TService>()}},
                                   CollectDependencies(DiDependencies<TDependencies...> {}),
                                   MakeDisposer<TImplementation>()});
        }

        template <class TDecorator> void Decorate() { AddDecorator(MakeDecorator<TDecorator>()); }

        [[nodiscard]] const std::vector<DiRegistrationRow>& Rows() const override;
        [[nodiscard]] const std::vector<DiDecorator>&       Decorators() const override;

    private:
        std::vector<DiRegistrationRow> m_Rows;
        std::vector<DiDecorator>       m_Decorators;
    };
}
