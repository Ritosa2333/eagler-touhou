#pragma once
#include "BulletRules.hpp"
namespace th128::bullet {
struct Extension1 {bool active=false;int previous=-1,time=0;float fraction=0;};
struct Extension1Step {Extension1 next;Position velocity;bool finished=false;};
std::optional<Extension1> initializeExtension1(const ExtensionRaw& record) noexcept;
std::optional<Extension1Step> stepExtension1(const Extension1& state,float speed,float angle,Position velocity) noexcept;
}
