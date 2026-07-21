#pragma once

#include "Di/Services/Abstraction/DiServiceId.hpp"
#include "Di/Services/Abstraction/IDiServiceProvider.hpp"

#include <functional>
#include <memory>

namespace Di::Registry
{
    struct DiDecorator
    {
        ::Di::Services::Abstraction::DiServiceId                                                                      Target;
        std::function<std::shared_ptr<void>(std::shared_ptr<void>, ::Di::Services::Abstraction::IDiServiceProvider&)> Apply;
    };
}
