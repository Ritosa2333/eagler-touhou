// Test-only environment: unexpected unexercised dependencies throw.
namespace th20::source::sprite::anm_environment {
float& clock_scale(){return clock_value;}const float* timer_rate(){return &clock_value;}
AnmInstruction* script(Animation& a){return script_start(*controller,a);}bool gameplay_frozen(){return false;}
std::uint32_t random_next(){unexpected("random_next");}std::uint32_t random_bounded(std::uint32_t){unexpected("random_bounded");}
float random_unit(){unexpected("random_unit");}float random_signed_unit(){unexpected("random_signed_unit");}float camera_component(std::int32_t){unexpected("camera_component");}
void assign_sprite(Animation&,std::int32_t){unexpected("assign_sprite");}void set_layer(Animation& a,std::int32_t layer){set_animation_layer(a,layer);}
void add_camera_offset(Vec3&){unexpected("add_camera_offset");}void calculate_corners(Animation&,Vec3(&)[4]){unexpected("calculate_corners");}
float screen_scale(){return 1.f;}std::int32_t screen_offset(unsigned,unsigned){return 0;}
void* allocate_geometry(std::uint32_t){unexpected("allocate_geometry");}
std::uint32_t spawn_child(Animation&,std::int32_t,std::uint32_t){unexpected("spawn_child");}
std::uint32_t spawn_detached(Animation&,std::int32_t,std::uint32_t){unexpected("spawn_detached");}
Animation& lookup_animation(std::uint32_t){unexpected("lookup_animation");}void spawn_effect(Animation&,std::int32_t){unexpected("spawn_effect");}
}
