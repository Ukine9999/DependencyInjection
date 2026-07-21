#pragma once

namespace ModuleA
{
    struct IServiceA
    {
        virtual ~IServiceA()                    = default;
        [[nodiscard]] virtual int Value() const = 0;
    };

    class ServiceA final : public IServiceA
    {
    public:
        [[nodiscard]] int Value() const override { return 21; }
    };
}
