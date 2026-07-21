#include "Di/Registry/DiRegistry.hpp"

#include <utility>

namespace Di::Registry
{
    DiRegistry& DiRegistry::Process()
    {
        static DiRegistry instance;
        return instance;
    }

    void DiRegistry::Add(DiRegistrationRow row)
    {
        m_Rows.push_back(std::move(row));
    }

    void DiRegistry::AddDecorator(DiDecorator decorator)
    {
        m_Decorators.push_back(std::move(decorator));
    }

    const std::vector<DiRegistrationRow>& DiRegistry::Rows() const
    {
        return m_Rows;
    }

    const std::vector<DiDecorator>& DiRegistry::Decorators() const
    {
        return m_Decorators;
    }
}
