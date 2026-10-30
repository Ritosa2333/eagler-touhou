#pragma once
#include "../../cpp/game/ScreenGridObject.hpp"
struct ScreenGridFixture {ScreenGrid grid;EnemyDistortionState state;ScreenGridViewport viewport;Vec3 position;std::vector<AnmGeometryVertex> strip;ScreenGridFixture(){grid.initialize(17,17);state.active=true;}};
API(screen_grid_create) ScreenGridFixture* screen_grid_create(){return new ScreenGridFixture;}
API(screen_grid_delete) void screen_grid_delete(ScreenGridFixture* p){delete p;}
API(screen_grid_field) void* screen_grid_field(ScreenGridFixture* p,u32 kind){switch(kind){case 0:return &p->state;case 1:return &p->viewport;case 2:return &p->position;case 3:return p->grid.vertices.data();case 4:return p->grid.sampling.data();default:return p->strip.data();}}
API(screen_grid_update) u32 screen_grid_update(ScreenGridFixture* p,float rate){return p->grid.radial(p->state,p->position,rate,p->viewport);}
API(screen_grid_strip) u32 screen_grid_strip(ScreenGridFixture* p,u32 column){return p->grid.strip(column,p->strip);}
