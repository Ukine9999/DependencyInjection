include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.7.1
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(Catch2)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)

function(di_add_test testName)
    add_executable(${testName} ${ARGN})
    target_include_directories(${testName} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(${testName} PRIVATE Di::DependencyInjection Catch2::Catch2WithMain)
    catch_discover_tests(${testName})
endfunction()

di_add_test(test_core
    Unit/test_resolve_singleton_no_deps.cpp
    Unit/test_singleton_identity.cpp
    Unit/test_transient_distinct.cpp
    Unit/test_lazy_singleton.cpp
    Unit/test_dependency_chain.cpp
    Unit/test_keyed.cpp
    Unit/test_resolve_all.cpp
    Unit/test_order.cpp
    Unit/test_missing_dependency.cpp
    Unit/test_multibinding.cpp
    Unit/test_extensions.cpp
    Unit/test_cycle.cpp
    Unit/test_factory_lazy.cpp
    Unit/test_validate.cpp
    Unit/test_root_registration.cpp
    Unit/test_scope.cpp
    Unit/test_decorator.cpp
    Unit/test_disposal_order.cpp
    Unit/test_factory_semantics.cpp
    Unit/test_lazy_deferral.cpp
    Unit/test_absent_dependencies.cpp
    Unit/test_expose_identity.cpp
    Unit/test_lazy_cycle_runtime.cpp
    Unit/test_decorator_deps.cpp
    Unit/test_keyed_injection.cpp
    Unit/test_instance_ownership.cpp
    Unit/test_lifetime_composition.cpp
    Unit/test_resolve_selection.cpp
    Unit/test_scope_isolation.cpp)

foreach(moduleName IN ITEMS ModuleA ModuleB ModuleC)
    di_add_module(${moduleName} SOURCES modules/${moduleName}/${moduleName}Anchor.cpp)
    target_include_directories(${moduleName} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/modules)
endforeach()

add_executable(test_modules modules/test_modular.cpp)
target_include_directories(test_modules PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/modules)
target_link_libraries(test_modules PRIVATE Di::DependencyInjection Catch2::Catch2WithMain)
di_module_link(test_modules ModuleA ModuleB ModuleC)
catch_discover_tests(test_modules)
