#pragma once

#include "Di/Registry/DiRegistrationRow.hpp"
#include "Di/Scheme/Abstraction/IDiServiceScheme.hpp"

#include <memory>

namespace Di::Scheme::Factory::Abstraction
{
    class IDiSchemeFactory
    {
    public:
        virtual ~IDiSchemeFactory() = default;

        [[nodiscard]] virtual std::shared_ptr<::Di::Scheme::Abstraction::IDiServiceScheme> Create(const ::Di::Registry::DiRegistrationRow& row) = 0;
    };
}
