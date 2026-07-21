#pragma once

#include <functional>
#include <memory>

namespace Di::Services::Abstraction
{
    using DiDisposer = std::function<void(const std::shared_ptr<void>&)>;
}
