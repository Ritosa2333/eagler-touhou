#pragma once
#include "StageScript.hpp"
#include "ScreenGridObject.hpp"
namespace th15 {
// STD effects own capture/strip animations and column-major CPU vertices.
class StageDeformation {
 AnmManager& animations;Rng& visual;ScreenGridViewport& viewport;i32 text_bank;
 bool upload();
public:
 ScreenGridObject mesh;std::string error;bool enabled=true;
 StageDeformation(AnmManager& a,Rng& v,ScreenGridViewport& view,i32 bank):animations(a),visual(v),viewport(view),text_bank(bank){}
 ~StageDeformation(){mesh.retire(animations);}
 bool begin(i32 mode);bool update(StageScriptState&,bool spell_active=false);
 bool prepare_draw(){return !enabled||!mesh.grid.column_count()||upload();}
};
}
