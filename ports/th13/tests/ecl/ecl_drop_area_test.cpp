#include "ecl_test_fixtures.hpp"
#include "game/StageSim.hpp"
using namespace th13;
namespace {
struct Drops : StageItemHost {
    struct Item {float x,y;int kind;};
    std::vector<Item> items;
    void spawn_item(float x,float y,float,i32 kind,float,float) override {items.push_back({x,y,kind});}
    void spawn_value_item(i32,float,float,float,float) override {}
    void on_et_clear(float,float,float) override {}
};
TEST(drop_clear_preserves_main_and_area_in_original_nonoverlapping_fields) {
    StageSim sim; StageEnemy e; EclContext c;
    sim.commands.sim=&sim;sim.current=&e;
    CHECK_EQ(e.drop_area[0],32.f);CHECK_EQ(e.drop_area[1],32.f);
    e.drop_main=2;e.drop_extra[0]=4;e.drop_extra[13]=7;
    e.drop_area[0]=48;e.drop_area[1]=16;
    std::vector<u8> ins;th13test::emit(ins,0,506,0,255,0,{});
    c.instruction=reinterpret_cast<const EclInstruction*>(ins.data());
    CHECK_EQ(sim.commands.execute(c,506),0);
    CHECK_EQ(e.drop_main,2);CHECK_EQ(e.drop_area[0],48.f);CHECK_EQ(e.drop_area[1],16.f);
    for(i32 value:e.drop_extra)CHECK_EQ(value,0);
}
TEST(default_extra_drops_scatter_and_emission_preserves_the_configured_area) {
    StageSim sim;StageEnemy e;Drops drops;sim.items=&drops;
    e.position[0]=10;e.position[1]=100;e.drop_main=2;e.drop_extra[0]=4;
    sim.drop_table(e);
    CHECK_EQ(drops.items.size(),size_t(5));
    if(drops.items.size()!=5)return;
    CHECK_EQ(drops.items[0].x,10.f);CHECK_EQ(drops.items[0].y,100.f);
    for(size_t i=1;i<5;++i){
        CHECK(drops.items[i].x!=10.f||drops.items[i].y!=100.f);
        CHECK(drops.items[i].x>=-22.f&&drops.items[i].x<=42.f);
        CHECK(drops.items[i].y>=68.f&&drops.items[i].y<=132.f);
    }
    CHECK_EQ(e.drop_main,0);CHECK_EQ(e.drop_extra[0],0);
    CHECK_EQ(e.drop_area[0],32.f);CHECK_EQ(e.drop_area[1],32.f);
}
}
int main(){return th13test::run_all("drop-area");}
