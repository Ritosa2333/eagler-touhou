#include "game/BulletManager.hpp"

#include <cassert>
#include <bit>
#include <fstream>
#include <iterator>
#include <limits>
#include <vector>

int main(int argc, char** argv) {
    using namespace th128::bullet;
    assert(argc == 2);
    std::ifstream stream(argv[1], std::ios::binary);
    const std::vector<unsigned char> profileBytes((std::istreambuf_iterator<char>(stream)), {});
    Profile profile;
    std::string error;
    assert(profile.load(profileBytes.data(), profileBytes.size(), error));
    assert(profile.type == 8 && profile.script == 43 && profile.secondaryScript == 0);
    assert(profile.tail.hitboxSize == 2.4f && profile.tail.auxiliarySize == 16 && profile.tail.value == 4);
    assert(profile.remap(10, 1) == 328 && profile.remap(10, 2) == 346);
    assert(!profile.remap(16, 1) && !profile.remap(0, 4));
    assert(!profile.load(profileBytes.data(), profileBytes.size() - 1, error));
    assert(profile.script == 43);
    Manager manager;
    auto emitter = resetEmitter();
    emitter.type = 8;
    emitter.color = 10;
    emitter.speed = 1;
    emitter.endSpeed = 1;
    emitter.positionX = -128;
    emitter.positionY = -32;
    emitter.extension0.type = 2;
    emitter.extension0.parameterA = 1;
    emitter.extension0.parameterB = -999999;
    emitter.extension0.parameterR = -999999;
    emitter.extension0.parameterS = -999999;
    emitter.extension0.control = 1;
    assert(!manager.emit(emitter, profile, {0, 400, 0}, 0, error));
    assert(manager.activeCount() == 0 && error.find("constructor") != std::string::npos);
    unsigned calls = 0;
    unsigned heard=0;
    assert(manager.setSoundSink([&](const SoundRequest& request,std::string&){assert(request.identifier==22 && request.positionX==-128);++heard;return true;}));
    manager.setConstructor([&calls](const Birth& birth, std::string&) {
        ++calls;
        assert(birth.type == 8 && birth.color == 10 && birth.script == 43);
        assert(birth.state.animationInterrupt == 8 && birth.state.extensionIndex == 1);
        return true;
    });
    assert(manager.emit(emitter, profile, {0, 400, 0}, 0, error));
    assert(calls == 1 && heard==1 && manager.activeCount() == 1 && manager.birthCount() == 1 && manager.cursor() == 1);
    const auto first = manager.snapshots().at(0);
    assert(first.slot == 0 && first.id == 1 && first.state.motionState == 2);
    assert(first.state.position.z == .1f && first.state.velocity.z == 0);
    const auto aim = targetAnglePrefix({-128, -32, 0}, {0, 400, 0});
    const auto geometry = emissionPrefix(emitter, 0, 0, *aim);
    assert(first.state.velocity.x == geometry->velocityX && first.state.velocity.y == geometry->velocityY);
    assert(manager.sounds().empty());
    assert(!manager.emit(emitter, profile, {0, 400, 0}, 1, error));
    assert(manager.activeCount() == 1 && manager.birthCount() == 1);
    emitter.extension0.type = 4;
    assert(!manager.emit(emitter, profile, {0, 400, 0}, 0, error));
    assert(manager.activeCount() == 1);
    manager.reset();
    assert(manager.activeCount() == 0 && manager.cursor() == 0 && manager.sounds().empty());
    manager.setConstructor([](const Birth&, std::string&) { return true; });
    emitter.extension0.type = 2;
    for (unsigned index = 0; index < Manager::Capacity; ++index) {
        assert(manager.emit(emitter, profile, {0, 400, 0}, 0, error));
    }
    assert(manager.cursor() == 0 && manager.activeCount() == Manager::Capacity);
    assert(manager.emit(emitter, profile, {0, 400, 0}, 0, error));
    assert(manager.saturated() && manager.birthCount() == Manager::Capacity);
    assert(manager.release(5));
    assert(manager.emit(emitter, profile, {0, 400, 0}, 0, error));
    assert(!manager.saturated() && manager.cursor() == 6 && manager.birthCount() == Manager::Capacity + 1);
    auto empty = emitter;
    empty.count = 0;
    empty.layers = 32767;
    const auto sounds = manager.sounds().size();
    assert(manager.emit(empty, profile, {0, 400, 0}, 0, error));
    assert(manager.sounds().size() == sounds + 1);
    empty.aimMode = 12;
    assert(!manager.emit(empty, profile, {0, 400, 0}, 0, error));
    assert(!manager.release(Manager::Capacity));
    manager.reset();
    manager.setConstructor([](const Birth&,std::string&){return true;});
    emitter.count=1;emitter.layers=1;emitter.aimMode=0;
    assert(manager.emit(emitter,profile,{0,400,0},0,error));
    assert(!manager.tick(error)&&manager.snapshots().at(0).age==0);
    unsigned updates=0;
    assert(manager.setUpdater([&updates](Birth& birth,bool& removed,std::string&){
        ++updates;
        assert(birth.age==0);
        birth.state.position.y+=1;
        removed=false;
        return true;
    }));
    assert(manager.tick(error)&&updates==1&&manager.snapshots().at(0).age==1);
    unsigned cancellations=0;
    assert(manager.cancelAll([&](Birth& birth,std::string&){
        ++cancellations;assert(birth.state.motionState==3&&birth.age==1);return true;
    },error));
    assert(cancellations==1&&manager.snapshots().at(0).age==0&&manager.activeCount()==1);
    assert(manager.cancelAll([&](Birth&,std::string&){++cancellations;return true;},error)&&cancellations==1);
    assert(manager.setUpdater([](Birth&,bool& removed,std::string&){removed=true;return true;}));
    assert(manager.tick(error)&&manager.activeCount()==0);
    assert(manager.emit(emitter,profile,{0,400,0},0,error));
    unsigned rejects=0;
    assert(!manager.cancelAll([&](Birth&,std::string& reason){++rejects;reason="required cancellation rejected";return false;},error));
    assert(error=="required cancellation rejected"&&rejects==1);
    assert(!manager.cancelAll([&](Birth&,std::string&){++rejects;return true;},error)&&rejects==1);
    assert(!manager.tick(error)&&error=="required cancellation rejected");
    manager.reset();manager.setConstructor([](const Birth&,std::string&){return true;});
    ExtensionRaw later;later.type=16;later.parameterA=60;later.parameterB=1;later.parameterC=2;later.parameterS=-999;
    assert(setExtendedExtension(emitter,1,later));
    assert(manager.emit(emitter,profile,{0,400,0},0,error));
    const auto extended=manager.snapshots().at(0);
    assert(!extended.extensionListEmpty&&extended.state.extensionIndex==1&&extended.state.laterExtensions[0].type==16);
    manager.reset();assert(manager.setConstructor([](const Birth&,std::string&){return true;}));
    emitter.positionX=10;emitter.positionY=100;emitter.count=1;emitter.layers=1;emitter.extension0={};emitter.laterExtensions={};
    assert(manager.emit(emitter,profile,{0,400,0},0,error));
    const auto beforeRadius=manager.snapshots()[0];unsigned radiusCalls=0;
    assert(manager.setUpdater([](Birth& birth,bool&,std::string&){birth.lifeClock=9;birth.lifeTime=9;return true;})&&manager.tick(error));
    assert(manager.cancelRadius({1000,100,0},1,[&](Birth&,std::string&){++radiusCalls;return true;},error)&&radiusCalls==0);
    assert(manager.cancelRadius({10,100,0},640,[&](Birth& birth,std::string&){++radiusCalls;assert(birth.state.motionState==1);return true;},error));
    const auto afterRadius=manager.snapshots()[0];assert(radiusCalls==1&&afterRadius.lifeClock==9&&afterRadius.lifeTime==9&&afterRadius.state.motionState==4);
    assert(afterRadius.state.position.x==float(double(beforeRadius.state.position.x)+float(double(beforeRadius.state.velocity.x)*.5)));
    assert(afterRadius.state.position.y==float(double(beforeRadius.state.position.y)+float(double(beforeRadius.state.velocity.y)*.5)));
    return 0;
}
