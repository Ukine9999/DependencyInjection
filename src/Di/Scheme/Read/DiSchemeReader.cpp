#include "Di/Scheme/Read/DiSchemeReader.hpp"

#include "Di/Scheme/DiFunctionServiceScheme.hpp"
#include "Di/Services/Abstraction/IDiServiceProvider.hpp"

#include <algorithm>
#include <functional>
#include <utility>

namespace Di::Scheme::Read
{
    namespace
    {
        using ::Di::Scheme::Abstraction::IDiServiceScheme;
        using DecoratorApply = std::function<std::shared_ptr<void>(std::shared_ptr<void>, ::Di::Services::Abstraction::IDiServiceProvider&)>;

        std::shared_ptr<IDiServiceScheme> Decorate(const std::shared_ptr<IDiServiceScheme>& base, std::vector<DecoratorApply> applies)
        {
            IDiServiceScheme::Constructor                  baseConstruct = base->GetServiceConstructor();
            ::Di::Services::Abstraction::DiCaster          baseCaster    = base->GetBindings().front().second;
            const ::Di::Services::Abstraction::DiServiceId id            = base->GetServiceId();

            IDiServiceScheme::Constructor wrapped = [baseConstruct, baseCaster, applies](::Di::Services::Abstraction::IDiServiceProvider& provider)
            {
                std::shared_ptr<void> instance = baseCaster(baseConstruct(provider));

                for (const DecoratorApply& apply : applies)
                    instance = apply(instance, provider);

                return instance;
            };

            return std::make_shared<::Di::Scheme::DiFunctionServiceScheme>(base->Lifetime(), std::move(wrapped), id.Type(), id.Key());
        }
    }

    DiSchemeReader::DiSchemeReader(::Di::Scheme::Factory::Abstraction::IDiSchemeFactory& factory) : m_Factory(factory) {}

    void DiSchemeReader::Read(const ::Di::Registry::Abstraction::IDiRegistry& registry)
    {
        for (const ::Di::Registry::DiRegistrationRow& row : registry.Rows())
            m_Rows.push_back(row);

        for (const ::Di::Registry::DiDecorator& decorator : registry.Decorators())
            m_Decorators.push_back(decorator);
    }

    std::vector<std::shared_ptr<IDiServiceScheme>> DiSchemeReader::EndRead()
    {
        std::stable_sort(m_Rows.begin(), m_Rows.end(), [](const ::Di::Registry::DiRegistrationRow& left, const ::Di::Registry::DiRegistrationRow& right)
                         { return left.Order < right.Order; });

        std::vector<std::shared_ptr<IDiServiceScheme>> schemes;
        schemes.reserve(m_Rows.size());

        for (const ::Di::Registry::DiRegistrationRow& row : m_Rows)
        {
            std::shared_ptr<IDiServiceScheme> scheme = m_Factory.Create(row);

            std::vector<DecoratorApply> applies;
            for (const ::Di::Registry::DiDecorator& decorator : m_Decorators)
                if (decorator.Target.Equals(scheme->GetServiceId(), false))
                    applies.push_back(decorator.Apply);

            if (!applies.empty())
                scheme = Decorate(scheme, std::move(applies));

            schemes.push_back(std::move(scheme));
        }

        m_Rows.clear();
        m_Decorators.clear();
        return schemes;
    }
}
