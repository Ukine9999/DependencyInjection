#pragma once

#include "Di/Services/Abstraction/DiCaster.hpp"

#include <memory>
#include <type_traits>

namespace Di::Registry
{
    template <class TImplementation, class TTarget> Di::Services::Abstraction::DiCaster MakeCaster()
    {
        static_assert(std::is_base_of_v<TTarget, TImplementation> || std::is_same_v<TTarget, TImplementation>,
                      "DI service/exposed type must be the implementation itself or one of its base classes");

        return [](const std::shared_ptr<void>& mostDerived) -> std::shared_ptr<void>
        {
            auto* implementation = static_cast<TImplementation*>(mostDerived.get());
            return std::shared_ptr<void>(mostDerived, static_cast<TTarget*>(implementation));
        };
    }
}
