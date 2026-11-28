#include "game/PlayerShotHost.hpp"
#include <cassert>
#include <fstream>
#include <iterator>
#include <vector>

std::vector<unsigned char> read(const char* name){std::ifstream file(name,std::ios::binary);assert(file);return {(std::istreambuf_iterator<char>(file)),{}};}
int main(int argc,char** argv){
    assert(argc==3);
    using namespace th128::playershot;
    const auto shot=read(argv[1]),animation=read(argv[2]);
    for(const float horizontal:{-184.f,184.f,-141.34f,141.34f})for(const float vertical:{300.f,400.f,432.f}){
        PlayerShotHost moving;std::string reason;assert(moving.load(shot,animation,reason)&&moving.setCallbackBindings({0,0,0,0}));
        assert(moving.setSoundHost([](int,float,std::string&){return true;})&&moving.emitCycle(0,1,0,{horizontal,vertical,.1f},reason));
        for(int tick=0;tick<35;++tick){if(!moving.tickUnit(reason)){std::fprintf(stderr,"moving %f,%f tick%d: %s\n",horizontal,vertical,tick,reason.c_str());for(const auto& live:moving.snapshots())std::fprintf(stderr,"shot%d mode%d x%g y%g sx%g sy%g rot%g size%g,%g offset%g,%g local%g,%g\n",live.id,live.state.mode,live.state.engineX,live.state.engineY,live.state.sx,live.state.sy,live.state.rotation,live.sprite.width,live.sprite.height,live.state.offsetX,live.state.offsetY,live.state.x,live.state.y);return 9;}assert(moving.tickAnimationsUnit(reason));}
    }
    PlayerShotHost host;std::string error;
    assert(host.load(shot,animation,error));
    host.setCallbackBindings({0,0,0,0});
    unsigned sounds=0;
    host.setSoundHost([&](int identifier,float position,std::string&){assert(identifier==0&&position<0);++sounds;return true;});
    assert(host.emitCycle(0,1,0,{0,400,.1f},error));
    auto shots=host.snapshots();assert(shots.size()==2&&sounds==1);
    assert(shots[0].damage==13&&shots[0].script==5&&shots[0].state.sprite==28&&shots[0].state.ticks==1);
    assert(shots[0].velocity.y==-24&&shots[1].velocity.y==-24);
    assert(host.emitCycle(0,1,1,{0,400,.1f},error)&&host.snapshots().size()==2);
    assert(host.emitCycle(0,1,3,{0,400,.1f},error)&&host.snapshots().size()==4);
    const auto born=host.snapshots();
    assert(host.tickUnit(error));
    shots=host.snapshots();
    assert(shots.size()==4&&shots[0].age==1&&shots[0].position.y==born[0].position.y-24);
    assert(shots[0].state.engineX==320+shots[0].position.x);
    assert(shots[0].state.engineY==16+shots[0].position.y&&shots[0].state.engineZ==.1f);
    assert(shots[0].state.ticks==1);
    assert(host.tickAnimationsUnit(error));
    assert(host.snapshots()[0].state.ticks==2);
    for(unsigned tick=0;tick<30;++tick){assert(host.tickUnit(error));assert(host.tickAnimationsUnit(error));}
    assert(host.snapshots().empty());
    assert(host.emitCycle(0,1,0,{0,400,.1f},error));
    assert(host.snapshots().size()==2&&host.snapshots()[0].slot==0&&host.snapshots()[0].age==0);
    assert(host.tickUnit(error)&&host.tickAnimationsUnit(error));
    std::array<std::uint32_t,th128::shotenemy::DamageSourceCount> emptySources{};
    auto query=host.queryEnemy({{1000,100,0},{24,24}},0,1,emptySources,error);
    assert(query&&query->continuation==th128::shotenemy::QueryContinuation::Complete&&query->completedDamage==0);
    assert(host.snapshots()[0].hitMarker==0);
    const auto contact=host.snapshots()[0].position;
    query=host.queryEnemy({{contact.x,contact.y,contact.z},{24,24}},0,1,emptySources,error);
    assert(query&&query->continuation==th128::shotenemy::QueryContinuation::NeedsSelectedContact);
    assert(query->selectedContact.required.callSite==0x43ec8a&&query->selectedContact.damagePrefix==13);
    assert(host.snapshots()[0].hitMarker==1);
    assert(!host.tickUnit(error)&&error.find("0x43ec8a")!=std::string::npos);
    PlayerShotHost resolved;
    assert(resolved.load(shot,animation,error));resolved.setCallbackBindings({0,0,0,0});
    resolved.setSoundHost([](int,float,std::string&){return true;});
    assert(resolved.enableContactAnimations());
    unsigned impactCalls=0;
    assert(resolved.setImpactHost([&](std::string&){++impactCalls;return true;}));
    assert(resolved.emitCycle(0,1,0,{0,400,.1f},error));
    assert(resolved.tickUnit(error)&&resolved.tickAnimationsUnit(error));
    const auto shotPosition=resolved.snapshots()[0].position;
    const auto oldHandle=resolved.snapshots()[0].animationHandle;
    const auto completed=resolved.queryEnemy({{0,shotPosition.y,0},{64,64}},0,1,emptySources,error);
    assert(completed&&completed->continuation==th128::shotenemy::QueryContinuation::Complete&&completed->completedDamage==26);
    assert(impactCalls==1&&resolved.snapshots().size()==2);
    assert(resolved.snapshots()[0].script==6&&resolved.snapshots()[0].active==2&&resolved.snapshots()[0].speed==3);
    assert(resolved.snapshots()[0].state.ticks==1&&resolved.snapshots()[0].state.sprite==29);
    assert(resolved.snapshots()[0].animationHandle!=oldHandle);
    const auto second=resolved.queryEnemy({{0,shotPosition.y,0},{64,64}},0,1,emptySources,error);
    assert(second&&second->completedDamage==0&&impactCalls==1);
    for(unsigned tick=0;tick<33;++tick)assert(resolved.tickUnit(error)&&resolved.tickAnimationsUnit(error));
    assert(resolved.snapshots().empty());
    PlayerShotHost impactFailure;
    assert(impactFailure.load(shot,animation,error));impactFailure.setCallbackBindings({0,0,0,0});
    impactFailure.setSoundHost([](int,float,std::string&){return true;});
    assert(impactFailure.enableContactAnimations());
    assert(impactFailure.emitCycle(0,1,0,{0,400,.1f},error));
    const auto unresolvedImpact=impactFailure.queryEnemy({{0,416,0},{64,64}},0,1,emptySources,error);
    assert(!unresolvedImpact&&error.find("0x43fcb3")!=std::string::npos);
    assert(impactFailure.snapshots()[0].active==2&&impactFailure.snapshots()[0].script==6);
    assert(!impactFailure.queryEnemy({{0,416,0},{64,64}},0,1,emptySources,error));
    PlayerShotHost impactDiagnostic;
    assert(impactDiagnostic.load(shot,animation,error));impactDiagnostic.setCallbackBindings({0,0,0,0});
    impactDiagnostic.setSoundHost([](int,float,std::string&){return true;});
    assert(impactDiagnostic.enableContactAnimations());
    assert(impactDiagnostic.setImpactHost([](std::string& reason){reason="required impact continuation";return true;}));
    assert(impactDiagnostic.emitCycle(0,1,0,{0,400,.1f},error));
    assert(!impactDiagnostic.queryEnemy({{0,416,0},{64,64}},0,1,emptySources,error));
    assert(error=="required impact continuation");
    PlayerShotHost sourceOrder;
    assert(sourceOrder.load(shot,animation,error));sourceOrder.setCallbackBindings({0,0,0,0});
    sourceOrder.setSoundHost([](int,float,std::string&){return true;});
    assert(sourceOrder.enableContactAnimations());
    std::array<std::uint32_t,th128::shotenemy::DamageSourceCount> changingSources{};
    assert(sourceOrder.setImpactHost([&](std::string&){changingSources[0]=1;return true;}));
    assert(sourceOrder.emitCycle(0,1,0,{0,400,.1f},error));
    const auto afterImpact=sourceOrder.queryEnemy({{0,416,0},{64,64}},0,1,changingSources,error);
    assert(afterImpact&&afterImpact->continuation==th128::shotenemy::QueryContinuation::NeedsDamageSourceContact);
    assert(afterImpact->completedDamage==0&&afterImpact->required.slot==0);
    PlayerShotHost withField;
    assert(withField.load(shot,animation,error));withField.setCallbackBindings({0,0,0,0});
    withField.setSoundHost([](int,float,std::string&){return true;});
    assert(withField.enableContactAnimations());
    th128::damagefield::DamageFieldPool fields;
    const std::vector<th128::damagefield::Image> emptyFields(th128::damagefield::Capacity);
    assert(fields.configure(emptyFields,0,error));
    unsigned fieldQueries=0,scoreCalls=0;int score=0;
    assert(withField.setDamageSourceHost([&](const th128::shotenemy::EnemyContactShape& shape,std::uint32_t flag,std::string& reason){
        ++fieldQueries;return fields.query({shape.combinedPosition[0],shape.combinedPosition[1],shape.combinedPosition[2]},flag,reason);
    }));
    assert(withField.setScoreHost([&](int damage,std::string&){
        ++scoreCalls;const auto result=th128::damagefield::finishDamageQuery(0,damage,score);assert(result);score=result->score;return true;
    }));
    assert(withField.setImpactHost([&](std::string& reason){
        assert(fieldQueries==0);return bool(fields.spawn({0,416,0},32,16,30,150,1,reason));
    }));
    assert(withField.emitCycle(0,1,0,{0,400,.1f},error));
    const auto gated=withField.queryEnemy({{0,416,0},{64,64}},8,8,emptySources,error);
    assert(gated&&gated->completedDamage==0&&fieldQueries==0&&scoreCalls==0);
    const auto combined=withField.queryEnemy({{0,416,0},{64,64}},8,9,emptySources,error);
    assert(combined&&combined->continuation==th128::shotenemy::QueryContinuation::Complete);
    assert(combined->completedDamage==176&&combined->outFlag==2&&score==2&&scoreCalls==1&&fieldQueries==1);
    assert(withField.queryEnemy({{0,416,0},{64,64}},8,9,emptySources,error)->completedDamage==150);
    assert(fieldQueries==2&&scoreCalls==2&&score==4);
    PlayerShotHost failedField;
    assert(failedField.load(shot,animation,error));failedField.setCallbackBindings({0,0,0,0});
    unsigned rejectedQueries=0;
    assert(failedField.setDamageSourceHost([&](const th128::shotenemy::EnemyContactShape&,std::uint32_t,std::string& reason)
        ->std::optional<th128::damagefield::PoolContact>{++rejectedQueries;reason="field continuation rejected";return std::nullopt;}));
    assert(!failedField.queryEnemy({{0,416,0},{64,64}},0,1,emptySources,error));
    assert(error=="field continuation rejected"&&rejectedQueries==1);
    assert(!failedField.queryEnemy({{0,416,0},{64,64}},0,1,emptySources,error)&&rejectedQueries==1);
    PlayerShotHost unknown;
    assert(unknown.load(shot,animation,error));
    assert(!unknown.emitCycle(0,1,0,{0,400,.1f},error)&&error.find("callback")!=std::string::npos);
    assert(unknown.snapshots().empty());
    unknown.setCallbackBindings({0x43ffb0,0,0,0});
    assert(!unknown.emitCycle(0,1,0,{0,400,.1f},error)&&error.find("callback")!=std::string::npos);
    return 0;
}
