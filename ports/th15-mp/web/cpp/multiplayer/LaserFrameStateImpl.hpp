// Included once, after LaserScene's concrete types have been defined.
#include "LaserFrameState.hpp"
#include "AnimationState.hpp"
#include <tuple>
#include <variant>
namespace th15::multiplayer {
namespace laser_frame_detail {
template<class... T>auto values(const std::tuple<T&...>& fields){return std::apply([](const auto&... v){return std::make_tuple(v...);},fields);}
template<class T>auto fields(T& laser){return std::tie(laser.request,laser.motion,laser.contact,laser.contacts,laser.program,laser.cancellation.error);}
template<class T>auto state(T& laser){return values(fields(laser));}
inline auto base(LaserObject& laser){return std::tie(laser.flags,laser.id,laser.state,laser.kind,laser.age,laser.outside_delay,laser.outside_count,laser.error);}
struct CounterGuard {u64 original=AnmVm::presentation_counter;~CounterGuard(){AnmVm::presentation_counter=original;}};
}
struct LaserFrameState::Data {
    using Moving=decltype(laser_frame_detail::state(std::declval<LaserScene::MovingLaser&>()));
    using Stationary=decltype(laser_frame_detail::state(std::declval<LaserScene::StationaryLaser&>()));
    using Curved=decltype(laser_frame_detail::state(std::declval<LaserScene::CurveLaser&>()));
    using Segmented=std::tuple<SegmentedLaserRequest,MovingLaserMotion,Vec3,float,std::array<float,512>>;
    struct Visual {
        struct Vms {AnmVm body,head,tip;};
        std::unique_ptr<Vms> vms;
        i32 resource=0,type=0,color=0;
        bool curve_source=false,retain=false,released=false;
        std::vector<AnmGeometryVertex> curve_vertices;
        std::string error;
        void capture(const LaserVisual& v,bool own_visuals){if(own_visuals){if(!vms)vms=std::make_unique<Vms>();vms->body=v.body;vms->head=v.head;vms->tip=v.tip;}else vms.reset();resource=v.resource_id;type=v.type;color=v.color;curve_source=v.curve_source;curve_vertices=v.curve_vertices;retain=v.retain_children_on_retire;released=v.released_;error=v.error;}
        void restore(LaserVisual& v)const{if(vms){v.body=vms->body;v.head=vms->head;v.tip=vms->tip;}v.resource_id=resource;v.type=type;v.color=color;v.curve_source=curve_source;v.curve_vertices=curve_vertices;v.retain_children_on_retire=retain;v.released_=released;v.error=error;}
    };
    struct Entry {
        decltype(laser_frame_detail::values(laser_frame_detail::base(std::declval<LaserObject&>()))) base;
        std::variant<std::monostate,Moving,Stationary,Curved,Segmented> state;
        // Constructed only for a visual owner; segmented lasers have no VMs.
        std::unique_ptr<Visual> visual;
    };
    LaserScene* owner=nullptr;
    std::vector<std::shared_ptr<LaserObject>> owners;
    std::vector<Entry> entries;
    Rng game_random,visual_random;
    u32 generation=0;
    Vec3 query_position{},query_size{};
    i32 query_count=0;
    float rate=1;
    std::string error,scene_error;
    bool ready=false;
};
LaserFrameState::LaserFrameState()=default;
LaserFrameState::~LaserFrameState()=default;
LaserFrameState::LaserFrameState(LaserFrameState&&)noexcept=default;
LaserFrameState& LaserFrameState::operator=(LaserFrameState&&)noexcept=default;
void LaserFrameState::discard()noexcept{if(saved_){saved_->owners.clear();saved_->ready=false;}}
size_t LaserFrameState::size()const noexcept{return saved_&&saved_->ready?saved_->owners.size():0;}
void LaserFrameState::append_roots(LaserScene& scene,const AnmRegistry&,std::vector<AnmVm*>& out){
    for(auto& owner:scene.manager.objects){LaserVisual* visual=nullptr;
        switch(owner->kind){case LaserKind::moving:visual=&static_cast<LaserScene::MovingLaser&>(*owner).visual;break;case LaserKind::stationary:visual=&static_cast<LaserScene::StationaryLaser&>(*owner).visual;break;case LaserKind::curved:visual=&static_cast<LaserScene::CurveLaser&>(*owner).visual;break;default:break;}
        if(visual){out.push_back(&visual->body);out.push_back(&visual->head);out.push_back(&visual->tip);}
    }
}
bool LaserFrameState::capture(LaserScene& scene,bool own_visuals){
    laser_frame_detail::CounterGuard guard;error.clear();if(!saved_)saved_=std::make_unique<Data>();auto& s=*saved_;s.ready=false;
    s.owner=&scene;auto& manager=scene.manager;s.owners.assign(manager.objects.begin(),manager.objects.end());
    if(s.entries.size()<s.owners.size())s.entries.resize(s.owners.size());
    for(size_t i=0;i<s.owners.size();++i){auto& object=*s.owners[i];auto& entry=s.entries[i];entry.base=laser_frame_detail::values(laser_frame_detail::base(object));
        if(object.kind!=LaserKind::segmented&&!entry.visual)entry.visual=std::make_unique<Data::Visual>();
        const auto capture=[&](auto& laser){entry.state=laser_frame_detail::state(laser);entry.visual->capture(laser.visual,own_visuals);};
        switch(object.kind){
        case LaserKind::moving:capture(static_cast<LaserScene::MovingLaser&>(object));break;
        case LaserKind::stationary:capture(static_cast<LaserScene::StationaryLaser&>(object));break;
        case LaserKind::curved:capture(static_cast<LaserScene::CurveLaser&>(object));break;
        case LaserKind::segmented:{auto& laser=static_cast<LaserScene::SegmentedLaser&>(object);entry.state=std::make_tuple(laser.request,laser.motion,laser.script_vector,laser.script_width,laser.history);break;}
        default:error="Unknown laser rollback kind";return false;
        }
    }
    s.game_random=scene.game_random;s.visual_random=scene.visual_random;s.rate=scene.rate;s.scene_error=scene.error;
    s.generation=manager.generation;s.query_position=manager.query_position;s.query_size=manager.query_size;s.query_count=manager.query_count;s.error=manager.error;
    s.ready=true;return true;
}
bool LaserFrameState::restore(LaserScene& scene)const{
    laser_frame_detail::CounterGuard guard;error.clear();if(!saved_||!saved_->ready||saved_->owner!=&scene){error="Laser rollback crossed its scene owner";return false;}
    const auto& s=*saved_;auto& manager=scene.manager;
    // Dropping a future owner is an undo, not a logical retirement. Its
    // registry graph is restored by AnimationState after the owners are back.
    for(const auto& owner:manager.objects)switch(owner->kind){
    case LaserKind::moving:static_cast<LaserScene::MovingLaser&>(*owner).visual.released_=true;break;
    case LaserKind::stationary:static_cast<LaserScene::StationaryLaser&>(*owner).visual.released_=true;break;
    case LaserKind::curved:static_cast<LaserScene::CurveLaser&>(*owner).visual.released_=true;break;
    case LaserKind::segmented:break;
    }
    manager.objects.assign(s.owners.begin(),s.owners.end());
    for(size_t i=0;i<s.owners.size();++i){auto& object=*s.owners[i];const auto& entry=s.entries[i];laser_frame_detail::base(object)=entry.base;
        const auto restore=[&](auto& laser){using State=decltype(laser_frame_detail::state(laser));laser_frame_detail::fields(laser)=std::get<State>(entry.state);entry.visual->restore(laser.visual);};
        switch(object.kind){
        case LaserKind::moving:restore(static_cast<LaserScene::MovingLaser&>(object));break;
        case LaserKind::stationary:restore(static_cast<LaserScene::StationaryLaser&>(object));break;
        case LaserKind::curved:restore(static_cast<LaserScene::CurveLaser&>(object));break;
        case LaserKind::segmented:{auto& laser=static_cast<LaserScene::SegmentedLaser&>(object);std::tie(laser.request,laser.motion,laser.script_vector,laser.script_width,laser.history)=std::get<Data::Segmented>(entry.state);break;}
        default:error="Unknown laser rollback kind";return false;
        }
    }
    scene.game_random=s.game_random;scene.visual_random=s.visual_random;scene.rate=s.rate;scene.error=s.scene_error;
    manager.generation=s.generation;manager.query_position=s.query_position;manager.query_size=s.query_size;manager.query_count=s.query_count;manager.error=s.error;
    return true;
}
}
