#include "ModuleB/ModuleB.hpp"

#include "Di/Registry/DiDescribe.hpp"
#include "Di/Registry/DiModuleAnchor.hpp"

DI_MODULE_ANCHOR(ModuleB)

DI_DESCRIBE(ModuleB::ServiceB)
{
    DI_SERVICE(ModuleB::IServiceB);
    DI_DEPENDS(Di::Service<ModuleA::IServiceA>);
    DI_LIFETIME(Singleton);
    DI_ORDER(20);
    DI_MODULE(ModuleB);
};
