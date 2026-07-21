#pragma once

#include "ModuleB/ModuleB.hpp"

#include <memory>
#include <utility>

namespace ModuleC
{
    struct IServiceC
    {
        virtual ~IServiceC()                    = default;
        [[nodiscard]] virtual int Chain() const = 0;
    };

    class ServiceC final : public IServiceC
    {
    public:
        explicit ServiceC(std::shared_ptr<ModuleB::IServiceB> serviceB) : m_ServiceB(std::move(serviceB)) {}

        [[nodiscard]] int Chain() const override { return m_ServiceB->Doubled() + 1; }

    private:
        std::shared_ptr<ModuleB::IServiceB> m_ServiceB;
    };
}
