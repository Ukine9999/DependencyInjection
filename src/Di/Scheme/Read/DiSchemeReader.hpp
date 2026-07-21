#pragma once

#include "Di/Registry/Abstraction/IDiRegistry.hpp"
#include "Di/Registry/DiDecorator.hpp"
#include "Di/Registry/DiRegistrationRow.hpp"
#include "Di/Scheme/Abstraction/IDiServiceScheme.hpp"
#include "Di/Scheme/Factory/Abstraction/IDiSchemeFactory.hpp"
#include "Di/Scheme/Read/Abstraction/IDiSchemeReader.hpp"

#include <memory>
#include <vector>

namespace Di::Scheme::Read
{
    class DiSchemeReader final : public Abstraction::IDiSchemeReader
    {
    public:
        explicit DiSchemeReader(::Di::Scheme::Factory::Abstraction::IDiSchemeFactory& factory);

        void Read(const ::Di::Registry::Abstraction::IDiRegistry& registry) override;

        [[nodiscard]] std::vector<std::shared_ptr<::Di::Scheme::Abstraction::IDiServiceScheme>> EndRead() override;

    private:
        ::Di::Scheme::Factory::Abstraction::IDiSchemeFactory& m_Factory;
        std::vector<::Di::Registry::DiRegistrationRow>        m_Rows;
        std::vector<::Di::Registry::DiDecorator>              m_Decorators;
    };
}
