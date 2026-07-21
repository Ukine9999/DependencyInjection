#pragma once

#include "ModuleA/ModuleA.hpp"

#include <memory>
#include <utility>

namespace ModuleB
{
    struct IServiceB
    {
        virtual ~IServiceB()                      = default;
        [[nodiscard]] virtual int Doubled() const = 0;
    };

    class ServiceB final : public IServiceB
    {
    public:
        explicit ServiceB(std::shared_ptr<ModuleA::IServiceA> serviceA) : m_ServiceA(std::move(serviceA)) {}

        [[nodiscard]] int Doubled() const override { return m_ServiceA->Value() * 2; }

    private:
        std::shared_ptr<ModuleA::IServiceA> m_ServiceA;
    };
}
