#pragma once

#include <memory>
#include <utility>

namespace DiTests
{
    struct IValue
    {
        virtual ~IValue()                     = default;
        [[nodiscard]] virtual int Get() const = 0;
    };

    class Value final : public IValue
    {
    public:
        [[nodiscard]] int Get() const override { return 42; }
    };

    struct ICounted
    {
        virtual ~ICounted() = default;
    };

    inline int g_CountedConstructions = 0;

    class Counted final : public ICounted
    {
    public:
        Counted() { ++g_CountedConstructions; }
    };

    struct IService1
    {
        virtual ~IService1()                    = default;
        [[nodiscard]] virtual int Value() const = 0;
    };

    class Service1 final : public IService1
    {
    public:
        [[nodiscard]] int Value() const override { return 7; }
    };

    struct IService0
    {
        virtual ~IService0()                       = default;
        [[nodiscard]] virtual int SubValue() const = 0;
    };

    class Service0 final : public IService0
    {
    public:
        explicit Service0(std::shared_ptr<IService1> sub) : m_Sub(std::move(sub)) {}

        [[nodiscard]] int SubValue() const override { return m_Sub->Value(); }

    private:
        std::shared_ptr<IService1> m_Sub;
    };

    struct IRegistered
    {
        virtual ~IRegistered()                  = default;
        [[nodiscard]] virtual int Chain() const = 0;
    };

    class Registered final : public IRegistered
    {
    public:
        explicit Registered(std::shared_ptr<IService0> service) : m_Service(std::move(service)) {}

        [[nodiscard]] int Chain() const override { return m_Service->SubValue(); }

    private:
        std::shared_ptr<IService0> m_Service;
    };
}
