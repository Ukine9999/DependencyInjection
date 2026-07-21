#pragma once

#include "Di/Services/Abstraction/DiServiceKey.hpp"

#include <string>
#include <typeindex>
#include <typeinfo>
#include <utility>

namespace Di::Services::Abstraction
{
    class DiServiceId
    {
    public:
        DiServiceId(std::type_index type, DiServiceKey key) : m_Type(type), m_Key(std::move(key)) {}

        [[nodiscard]] std::type_index     Type() const { return m_Type; }
        [[nodiscard]] const DiServiceKey& Key() const { return m_Key; }

        [[nodiscard]] bool Equals(const DiServiceId& other, bool ignoreKeys) const { return (ignoreKeys || m_Key == other.m_Key) && m_Type == other.m_Type; }

        [[nodiscard]] bool Equals(const DiServiceId& other) const { return Equals(other, false); }

        template <class TService> static DiServiceId FromType() { return FromType<TService>(DiServiceKey {}); }

        template <class TService> static DiServiceId FromType(DiServiceKey key) { return DiServiceId(std::type_index(typeid(TService)), std::move(key)); }

        static DiServiceId FromTypeIndex(std::type_index type, DiServiceKey key) { return DiServiceId(type, std::move(key)); }

        friend bool operator==(const DiServiceId& left, const DiServiceId& right) { return left.Equals(right, false); }

        friend bool operator!=(const DiServiceId& left, const DiServiceId& right) { return !left.Equals(right, false); }

    private:
        std::type_index m_Type;
        DiServiceKey    m_Key;
    };

    [[nodiscard]] inline std::string to_string(const DiServiceId& id)
    {
        return std::string("(") + id.Type().name() + ", " + (id.Key().has_value() ? *id.Key() : std::string("null")) + ")";
    }
}
