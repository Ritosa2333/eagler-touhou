#pragma once
#include "../lifecycle.hpp"
namespace th20::source::title {
inline LifecycleEnvironment* oracle_lifecycle_environment=nullptr;
LifecycleEnvironment& lifecycle_environment(){
    if(!oracle_lifecycle_environment)throw std::logic_error("Title lifecycle fixture is not installed");
    return *oracle_lifecycle_environment;
}
}
