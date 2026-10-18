#include "../th08_web/cpp/game/BulletState.hpp"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <memory>
#include <vector>
struct ImVec2 {float x,y;};
struct ImDrawList {
    struct Rect {ImVec2 a,b;};std::vector<Rect> rectangles;
    std::vector<std::vector<ImVec2>> quads;int clips=0;
    void PushClipRect(ImVec2 a,ImVec2 b){assert(a.x==32&&a.y==16&&b.x==416&&b.y==464);++clips;}
    void PopClipRect(){--clips;}
    void AddRectFilled(ImVec2 a,ImVec2 b,unsigned){rectangles.push_back({a,b});}
    void AddRect(ImVec2,ImVec2,unsigned,float){}
    void AddQuad(ImVec2 a,ImVec2 b,ImVec2 c,ImVec2 d,unsigned){quads.push_back({a,b,c,d});}
    void AddQuadFilled(ImVec2,ImVec2,ImVec2,ImVec2,unsigned){}
};
namespace th08 {
struct BrowserRuntime {
    struct App {
        struct Session {struct Practice {bool show_bullet_hitbox=true;} practice;} session;
        struct Game {
            struct Globals {u32 game_flags=1;} globals;
            struct Player {struct Motion {struct Movement {Vec3 bounds[6];} movement;} motion;} player_state;
            BulletManagerState projectile_pool;
        } game;
        bool in_game()const{return true;}
    } app;
};
#include "../th08_web/cpp/sdl/PracticeHitbox.inc"
}
int main(){
    auto runtime=std::make_unique<th08::BrowserRuntime>();auto& game=runtime->app.game;
    game.player_state.motion.movement.bounds[0].x=99;
    game.player_state.motion.movement.bounds[1].x=101;
    auto& bullet=game.projectile_pool.bullets[0];bullet.position={100,200,0};bullet.sprites.hitbox={8,6,0};
    bullet.since_fired.current=1;ImDrawList draw;th08::RenderBtHitbox(&draw,*runtime);
    assert(draw.clips==0&&draw.rectangles.size()==1);
    assert(draw.rectangles[0].a.x==127&&draw.rectangles[0].a.y==212);
    assert(draw.rectangles[0].b.x==137&&draw.rectangles[0].b.y==220);
    bullet.since_fired.current=0;auto& laser=game.projectile_pool.lasers[0];
    laser.in_use=1;laser.state=1;laser.position={100,200,0};laser.start_offset=10;laser.end_offset=100;laser.width=8;
    draw={};th08::RenderBtHitbox(&draw,*runtime);assert(draw.rectangles.empty()&&draw.quads.size()==1&&draw.clips==0);
    assert(draw.quads[0][0].x==141&&draw.quads[0][0].y==213);
    assert(draw.quads[0][2].x==233&&draw.quads[0][2].y==219);
    for(int state:{0,2}){laser.state=state;laser.start=60;laser.hitbox_start=30;laser.stop=60;laser.hitbox_stop=30;laser.timer.current=15;draw={};th08::RenderBtHitbox(&draw,*runtime);assert(draw.quads.size()==(state==0?0:1));}
    runtime->app.session.practice.show_bullet_hitbox=false;draw={};th08::RenderBtHitbox(&draw,*runtime);assert(draw.quads.empty()&&draw.clips==0);
    runtime->app.session.practice.show_bullet_hitbox=true;game.globals.game_flags=0;draw={};th08::RenderBtHitbox(&draw,*runtime);assert(draw.quads.empty()&&draw.clips==0);
}
