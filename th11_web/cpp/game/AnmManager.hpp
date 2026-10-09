#pragma once
#include "AnmGeometry.hpp"
#include <array>
#include <memory>
#include <unordered_map>
#include <vector>
namespace th11 {
class AnmManager final:public AnmEnvironment {
public:
    ~AnmManager() override;
    AnmVm* create(AnmResource& resource,i32 script,u16 file_id,u32 layer,bool overlay=false,bool prepend=false,const Vec3* initial_position=nullptr);
    AnmVm* spawn(AnmVm& parent,i32 script,u32 opcode) override;
    AnmVm* find(u32 id) noexcept;
    void destroy(AnmVm& vm);
    void clear();
    void retire_resource(const AnmResource&);
    bool references(const AnmResource&)const noexcept;
    bool update(bool overlay);
    bool change_draw_mode(AnmVm& vm) override;
    bool allocate_geometry(AnmVm& vm,u32 bytes) override;
    bool update_geometry(AnmVm& vm) override;
    void release_geometry(AnmVm& vm);
    AnmVm* layer_first(u32 layer)const noexcept {return layer<layers.size()?layers[layer]:nullptr;}
    u32 active_count()const noexcept {return active.size();}
    u32 last_error=0;
#ifdef TH11_MULTIPLAYER
    unsigned multiplayer_owner=0;
    std::unordered_map<const AnmVm*,unsigned> multiplayer_tags;
    unsigned multiplayer_tag(const AnmVm& vm)const noexcept{auto i=multiplayer_tags.find(&vm);return i==multiplayer_tags.end()?0:i->second;}
#endif
private:
    std::vector<std::unique_ptr<AnmVm>> storage;
    std::vector<AnmVm*> available;
    std::unordered_map<u32,AnmVm*> active;
    std::unordered_map<AnmVm*,u32> geometry_sizes;
    AnmVm::Node* heads[2]{},*tails[2]{};
    std::array<AnmVm*,31> layers{};
    u32 next_id=0,spawn_depth=0;
};
}
