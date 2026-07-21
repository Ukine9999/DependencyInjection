#pragma once

#include "Di/Registry/DiDecorator.hpp"
#include "Di/Registry/DiRegistrationRow.hpp"

#include <vector>

namespace Di::Registry::Abstraction
{
    class IDiRegistry
    {
    public:
        virtual ~IDiRegistry() = default;

        [[nodiscard]] virtual const std::vector<DiRegistrationRow>& Rows() const       = 0;
        [[nodiscard]] virtual const std::vector<DiDecorator>&       Decorators() const = 0;
    };
}
