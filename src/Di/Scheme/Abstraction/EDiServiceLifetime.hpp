#pragma once

#include <cstdint>

namespace Di::Scheme::Abstraction
{
    enum class EDiServiceLifetime : std::uint8_t
    {
        None,
        Singleton,
        LazySingleton,
        Scoped,
        Transient
    };
}
