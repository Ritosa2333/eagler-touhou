#pragma once
#include "EnemyDropRules.hpp"
#include <functional>
#include <optional>
#include <string>

namespace th128::enemy {
struct DropPosition {float x=0,y=0,z=0;};
struct DropEmission {int kind=0;DropPosition position;float angle=0,speed=0;};
float initialDropAngle(std::uint32_t random) noexcept;
std::optional<float> advanceDropAngle(float angle,std::uint32_t random) noexcept;
std::optional<DropPosition> scatterDrop(DropPosition position,float angle,float width,float height,std::uint32_t random) noexcept;
class AdditionalDropOperation {
public:
    using RandomHost=std::function<std::uint32_t()>;
    using SpawnHost=std::function<bool(const DropEmission&,std::string&)>;
    AdditionalDropOperation(DropState state,DropPosition position);
    bool run(RandomHost random,SpawnHost spawn,std::string& error);
    const DropState& state() const;
private:
    DropState state_;
    DropPosition position_;
    bool started_=false,executing_=false;
    std::string failure_;
    bool fail(std::string message,std::string& error);
};
}
