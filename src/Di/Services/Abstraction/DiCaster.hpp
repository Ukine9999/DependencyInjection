#pragma once

#include <functional>
#include <memory>

namespace Di::Services::Abstraction
{
    using DiCaster = std::function<std::shared_ptr<void>(const std::shared_ptr<void>&)>;
}
