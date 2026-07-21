#pragma once

namespace Di::Services::Abstraction
{
    struct IDiDisposable
    {
        virtual ~IDiDisposable() = default;
        virtual void Dispose()   = 0;
    };
}
