#include "ModuleA/ModuleA.hpp"

#include "Di/Registry/DiDescribe.hpp"
#include "Di/Registry/DiModuleAnchor.hpp"

DI_MODULE_ANCHOR(ModuleA)

DI_DESCRIBE(ModuleA::ServiceA)
{
    DI_SERVICE(ModuleA::IServiceA);
    DI_LIFETIME(Singleton);
    DI_ORDER(10);
    DI_MODULE(ModuleA);
};
