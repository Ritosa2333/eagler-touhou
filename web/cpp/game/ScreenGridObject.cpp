#include "ScreenGridObject.hpp"
namespace th15 {
bool ScreenGridObject::retire(AnmManager& a){bool ok=a.retire(capture_handle);for(auto& handle:strip_handles)ok=a.retire(handle)&&ok;grid=ScreenGrid{};return ok;}
bool ScreenGridObject::initialize(AnmManager& a,i32 bank,u32 rows){
 if(!retire(a)||!grid.initialize(17,rows))return false;
 auto create=[&](u32 count)->u32{u32 handle=a.create(bank,59,33,0);auto* vm=a.registry.find(handle);if(!vm)return 0;if(!vm->geometry.allocate(i32(count),false)){a.retire(handle);return 0;}if(count<3)vm->visual.flags&=0xc1ffffffu;else{vm->variables.integers[0]=i32(count);vm->visual.flags=(vm->visual.flags&0xd9ffffffu)|0x18000000u;}return handle;};
 capture_handle=create(2);if(!capture_handle)return false;
 for(auto& handle:strip_handles){handle=create(rows);auto* vm=a.registry.find(handle);if(!vm)return false;vm->visual.flags&=0xfffffe1fu;vm->visual.render_flags&=0xfff3ffffu;}return true;
}
bool ScreenGridObject::update(AnmManager& a,EnemyDistortionState& state,const Vec3& position,float rate,const ScreenGridViewport& view){
 if(!state.active)return true;if(!grid.column_count()||!grid.radial(state,position,rate,view))return false;
 for(u32 column=0;column<strip_handles.size();column++){auto* vm=a.registry.find(strip_handles[column]);if(!vm||!grid.strip(column,vm->geometry.screen_vertices))return false;}return true;
}
}
