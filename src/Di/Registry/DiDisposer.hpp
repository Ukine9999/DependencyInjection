#pragma once

#include "Di/Services/Abstraction/DiDisposer.hpp"
#include "Di/Services/Abstraction/IDiDisposable.hpp"

#include <memory>
#include <type_traits>

namespace Di::Registry
{
    template <class TImplementation>::Di::Services::Abstraction::DiDisposer MakeDisposer()
    {
        if constexpr (std::is_base_of_v<::Di::Services::Abstraction::IDiDisposable, TImplementation>)
            return [](const std::shared_ptr<void>& mostDerived) { static_cast<TImplementation*>(mostDerived.get())->Dispose(); };
        else
            return {};
    }
}
