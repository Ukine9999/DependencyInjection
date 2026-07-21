#pragma once

#include "Di/Registry/Abstraction/IDiRegistry.hpp"
#include "Di/Services/Abstraction/DiServiceId.hpp"

#include <vector>

namespace Di::Registry
{
    [[nodiscard]] std::vector<::Di::Services::Abstraction::DiServiceId> FindMissingDependencies(const Abstraction::IDiRegistry& registry);

    [[nodiscard]] std::vector<::Di::Services::Abstraction::DiServiceId> FindDependencyCycle(const Abstraction::IDiRegistry& registry);

    void ValidateOrThrow(const Abstraction::IDiRegistry& registry);
}
