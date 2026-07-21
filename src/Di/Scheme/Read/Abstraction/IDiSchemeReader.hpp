#pragma once

#include "Di/Registry/Abstraction/IDiRegistry.hpp"
#include "Di/Scheme/Abstraction/IDiServiceScheme.hpp"

#include <memory>
#include <vector>

namespace Di::Scheme::Read::Abstraction
{
    class IDiSchemeReader
    {
    public:
        virtual ~IDiSchemeReader() = default;

        virtual void Read(const ::Di::Registry::Abstraction::IDiRegistry& registry) = 0;

        [[nodiscard]] virtual std::vector<std::shared_ptr<::Di::Scheme::Abstraction::IDiServiceScheme>> EndRead() = 0;
    };
}
