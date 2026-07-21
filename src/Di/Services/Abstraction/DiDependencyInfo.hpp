#pragma once

#include "Di/Services/Abstraction/DiServiceId.hpp"

namespace Di::Services::Abstraction
{
    struct DiDependencyInfo
    {
        DiServiceId Id;
        bool        Required;
        bool        Defers;
    };
}
