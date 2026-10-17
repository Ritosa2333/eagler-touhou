#pragma once
#include "EnemyState.hpp"
#include "EclContext.hpp"
#include "LaserScene.hpp"
namespace th15 {
struct LaserEmissionHost {virtual ~LaserEmissionHost()=default;virtual bool emit(const MovingLaserRequest&)=0;virtual bool emit(const StationaryLaserRequest&)=0;virtual bool emit(const CurveLaserRequest&){return false;}virtual bool emit(const SegmentedLaserRequest&){return false;}};
class LaserCommands {
    EnemyState& enemy;
public:
    LaserEmissionHost* host=nullptr;LaserScene* scene=nullptr;
    explicit LaserCommands(EnemyState& state):enemy(state){}
    int execute(EclContext&,u16 opcode);
};
}
