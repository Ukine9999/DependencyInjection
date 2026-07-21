#pragma once

#include "Di/Registry/DiRegistrationRow.hpp"
#include "Di/Scheme/Abstraction/IDiServiceScheme.hpp"
#include "Di/Scheme/Factory/Abstraction/IDiSchemeFactory.hpp"

#include <memory>

namespace Di::Scheme::Factory
{
    class DiSchemeFactory final : public Abstraction::IDiSchemeFactory
    {
    public:
        [[nodiscard]] std::shared_ptr<::Di::Scheme::Abstraction::IDiServiceScheme> Create(const ::Di::Registry::DiRegistrationRow& row) override;
    };
}
