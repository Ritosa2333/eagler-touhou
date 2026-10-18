#pragma once
#include "AnmManager.hpp"
#include "EnemyState.hpp"
namespace th11 {
struct DeformationControl {
    virtual ~DeformationControl()=default;
    virtual bool create_deformation(EnemyState&)=0;
    virtual bool release_deformation(EnemyState&)=0;
};
// 40e530/40e6c0/40e890 and the deformation suffix of 411750.
// Each adjacent column is a native ANM triangle strip sampling text.anm @R.
class ScreenDeformation {
public:
    static constexpr u32 side=17;
    AnmManager& manager;
    u32 columns=side,rows=side;
    std::vector<AnmVertex> vertices;
    std::vector<Vec3> positions;
    std::vector<u32> animations;
    explicit ScreenDeformation(AnmManager& m):manager(m){}
    ~ScreenDeformation();
    bool initialize(AnmResource& text,u32 width=side,u32 height=side,bool second_target=false);
    void rectangle(float x,float y,float width,float height);
    void update(EnemyState&,i32 spell_id);
    void copy_strips();
    static void draw_callback(AnmVm&);
};
}
