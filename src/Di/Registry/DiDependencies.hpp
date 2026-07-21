#pragma once

namespace Di::Registry
{
    template <class... TDependencies> struct DiDependencies
    {
    };

    template <class... TInterfaces> struct DiExposes
    {
    };
}
