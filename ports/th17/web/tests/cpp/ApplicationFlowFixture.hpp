#include "../../cpp/game/ApplicationFlow.hpp"
struct ApplicationFlowFixture final:ApplicationFlowServices {
 GameRun run;ApplicationFlow flow{run,*this};std::vector<i32> events;bool available=true;std::array<i32,9> values{};
 bool begin_loading(bool& out)override{events.push_back(1);out=available;return true;}bool shutdown()override{events.push_back(2);return true;}bool shutdown_delay(i32 n)override{events.insert(events.end(),{3,n});return true;}
 bool release_game()override{events.push_back(4);return true;}bool release_title()override{events.push_back(5);return true;}bool release_ending()override{events.push_back(6);return true;}
 bool begin_game(bool replay)override{events.insert(events.end(),{7,replay?1:0});return true;}bool begin_title()override{events.push_back(8);return true;}bool begin_ending()override{events.push_back(9);return true;}
};
API(app_flow_create) ApplicationFlowFixture* app_flow_create(){return new ApplicationFlowFixture;}
API(app_flow_delete) void app_flow_delete(ApplicationFlowFixture* p){delete p;}
API(app_flow_configure) void app_flow_configure(ApplicationFlowFixture* p,i32 current,i32 pending,i32 fresh,i32 retry,i32 stage,i32 initial,i32 replay,i32 available,i32 reason){p->flow.current=AppScene(current);p->flow.pending=AppScene(pending);p->flow.previous=AppScene::Uninitialized;p->flow.fresh=fresh;p->flow.retry_selection=retry;p->flow.title_return_reason=reason;p->flow.background=0;p->run.stage=stage;p->run.initial_stage=initial;p->run.practice_record=replay;p->available=available;}
API(app_flow_update) i32 app_flow_update(ApplicationFlowFixture* p){p->events.clear();return p->flow.update();}
API(app_flow_values) const i32* app_flow_values(ApplicationFlowFixture* p){p->values={i32(p->flow.current),i32(p->flow.pending),i32(p->flow.previous),p->flow.fresh,p->flow.retry_selection,signed_bits(p->flow.background),p->run.stage,p->run.initial_stage,p->flow.title_return_reason};return p->values.data();}
API(app_flow_events) const i32* app_flow_events(ApplicationFlowFixture* p){return p->events.data();}
API(app_flow_count) u32 app_flow_count(ApplicationFlowFixture* p){return p->events.size();}
