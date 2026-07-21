#include "Di/Registry/DiRegistryValidator.hpp"

#include "Di/Registry/DiRegistrationRow.hpp"

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>

namespace Di::Registry
{
    using ::Di::Services::Abstraction::DiServiceId;

    std::vector<DiServiceId> FindMissingDependencies(const Abstraction::IDiRegistry& registry)
    {
        std::vector<DiServiceId> provided;
        for (const DiRegistrationRow& row : registry.Rows())
            for (const auto& binding : row.Bindings)
                provided.push_back(binding.first);

        std::vector<DiServiceId> missing;
        for (const DiRegistrationRow& row : registry.Rows())
            for (const auto& dependency : row.Dependencies)
            {
                if (!dependency.Required)
                    continue;

                bool found = false;
                for (const DiServiceId& id : provided)
                    if (id.Equals(dependency.Id, false))
                    {
                        found = true;
                        break;
                    }

                if (!found)
                    missing.push_back(dependency.Id);
            }

        return missing;
    }

    namespace
    {
        std::vector<std::size_t> ProvidersOf(const std::vector<DiRegistrationRow>& rows, const DiServiceId& dependency)
        {
            std::vector<std::size_t> providers;
            for (std::size_t index = 0; index < rows.size(); ++index)
                for (const auto& binding : rows[index].Bindings)
                    if (binding.first.Equals(dependency, false))
                    {
                        providers.push_back(index);
                        break;
                    }

            return providers;
        }

        bool WalkForCycle(const std::vector<DiRegistrationRow>& rows, std::size_t node, std::vector<int>& colors, std::vector<std::size_t>& path,
                          std::vector<std::size_t>& cycle)
        {
            colors[node] = 1;
            path.push_back(node);

            for (const auto& dependency : rows[node].Dependencies)
            {
                if (dependency.Defers)
                    continue;

                for (std::size_t next : ProvidersOf(rows, dependency.Id))
                {
                    if (colors[next] == 1)
                    {
                        const auto start = std::find(path.begin(), path.end(), next);
                        cycle.assign(start, path.end());
                        cycle.push_back(next);
                        return true;
                    }

                    if (colors[next] == 0 && WalkForCycle(rows, next, colors, path, cycle))
                        return true;
                }
            }

            path.pop_back();
            colors[node] = 2;
            return false;
        }
    }

    std::vector<DiServiceId> FindDependencyCycle(const Abstraction::IDiRegistry& registry)
    {
        const std::vector<DiRegistrationRow>& rows = registry.Rows();

        std::vector<int>         colors(rows.size(), 0);
        std::vector<std::size_t> path;
        std::vector<std::size_t> cycle;

        for (std::size_t index = 0; index < rows.size(); ++index)
            if (colors[index] == 0 && WalkForCycle(rows, index, colors, path, cycle))
                break;

        std::vector<DiServiceId> result;
        for (std::size_t index : cycle)
            result.push_back(rows[index].Bindings.front().first);

        return result;
    }

    void ValidateOrThrow(const Abstraction::IDiRegistry& registry)
    {
        const std::vector<DiServiceId> missing = FindMissingDependencies(registry);
        const std::vector<DiServiceId> cycle   = FindDependencyCycle(registry);

        if (missing.empty() && cycle.empty())
            return;

        std::string message = "DI validation failed";

        if (!missing.empty())
        {
            message += "\n  unresolved required dependencies:";
            for (const DiServiceId& id : missing)
                message += "\n    " + to_string(id);
        }

        if (!cycle.empty())
        {
            message += "\n  dependency cycle:";
            for (const DiServiceId& id : cycle)
                message += "\n    " + to_string(id);
        }

        throw std::runtime_error(message);
    }
}
