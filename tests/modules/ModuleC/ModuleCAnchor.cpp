#include "ModuleC/ModuleC.hpp"

#include "Di/Registry/DiDescribe.hpp"
#include "Di/Registry/DiModuleAnchor.hpp"

DI_MODULE_ANCHOR(ModuleC)

DI_DESCRIBE(ModuleC::ServiceC)
{
    DI_SERVICE(ModuleC::IServiceC);
    DI_DEPENDS(Di::Service<ModuleB::IServiceB>);
    DI_LIFETIME(Singleton);
    DI_ORDER(30);
    DI_MODULE(ModuleC);
};
