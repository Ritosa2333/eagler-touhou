#include "AnimationState.hpp"
#include "../game/AnmOverlay.hpp"
#include <bitset>
#include <algorithm>
#include <limits>
#include <unordered_set>

namespace th15::multiplayer {
namespace {
constexpr u32 none=std::numeric_limits<u32>::max();
template<class F> void paired_vms(const AnmVm& from,AnmVm& to,F&& visit){
    visit(from,to);
    if(from.geometry.overlay&&to.geometry.overlay)
        for(size_t i=0;i<from.geometry.overlay->panels.size();++i)
            if(from.geometry.overlay->panels[i]&&to.geometry.overlay->panels[i])
                paired_vms(*from.geometry.overlay->panels[i],*to.geometry.overlay->panels[i],visit);
}
template<class F> void each_vm(AnmVm& vm,F&& visit){
    visit(vm);
    if(vm.geometry.overlay)for(auto& panel:vm.geometry.overlay->panels)if(panel)each_vm(*panel,visit);
}
template<class F> void each_vm(const AnmVm& vm,F&& visit){
    visit(vm);
    if(vm.geometry.overlay)for(const auto& panel:vm.geometry.overlay->panels)if(panel)each_vm(static_cast<const AnmVm&>(*panel),visit);
}
size_t dynamic_bytes(const AnmVm& vm){
    const auto& g=vm.geometry;
    size_t bytes=vm.error.capacity()+g.screen_vertices.capacity()*sizeof(AnmGeometryVertex)+g.world_vertices.capacity()*sizeof(AnmWorldVertex);
    if(g.distortion)bytes+=sizeof(AnmDistortion);
    if(g.trail)bytes+=sizeof(AnmTrail);
    if(g.gather)bytes+=sizeof(AnmGatherEffect);
    if(g.overlay){bytes+=sizeof(AnmOverlay);for(const auto& panel:g.overlay->panels)if(panel)bytes+=sizeof(AnmVm)+dynamic_bytes(*panel);}
    return bytes;
}
}
struct AnimationState::Data {
    struct Image {
        AnmVm vm;
        AnmVm* borrowed=nullptr;
        const AnmVm* original=nullptr;
        u32 slot=none,handle=0,parent=none;
        bool active=false,registered=false,alternate=false,frozen=false,external=false,nested=false;
        std::vector<u32> children;
    };
    // Retain high-water storage across captures; inactive pool payloads are not
    // cloned. Their allocation presence and exact free-list order are retained.
    std::vector<Image> nodes;
    size_t count=0;
    struct RootImage {AnmVm vm;AnmVm* original=nullptr;};
    std::vector<RootImage> roots;
    size_t root_count=0;
    std::bitset<0x1fff> present;
    std::vector<u32> free_slots,pending;
    std::array<std::vector<u32>,2> groups;
    std::array<std::vector<const AnmVm*>,42> layers;
    std::vector<std::pair<const AnmVm*,const AnmVm*>> originals;
    // Capture scratch survives ring reuse; stable registry nodes keep their
    // index entries rather than reallocating hash nodes on every frame.
    struct Index {u32 position=0;u64 epoch=0;};
    std::vector<const AnmRegistry::Node*> selected;
    std::unordered_map<const AnmRegistry::Node*,Index> indices;
    std::vector<AnmVm*> unregistered;
    u64 capture_epoch=0;
    u32 generation=0,active_count=0;
    u64 presentation_counter=0;
    std::string error;
    bool captured=false;
};
AnimationState::AnimationState()=default;
AnimationState::~AnimationState()=default;
AnimationState::AnimationState(AnimationState&&)noexcept=default;
AnimationState& AnimationState::operator=(AnimationState&&)noexcept=default;
bool AnimationState::ready()const noexcept{return saved_&&saved_->captured;}
size_t AnimationState::instances()const noexcept{return ready()?saved_->count+saved_->root_count:0;}
bool AnimationState::registered_tree(const AnmRegistry& registry,const AnmVm& vm){bool found=false;each_vm(vm,[&](const AnmVm& value){found|=registry.node(const_cast<AnmVm&>(value))!=nullptr;});return found;}
bool AnimationState::capture(const AnmRegistry& registry,const std::vector<AnmVm*>& owned_roots){
    error.clear();
    // Copying overlay panels invokes AnmVm constructors. Capture must not
    // consume presentation identities or change any live allocator state.
    struct IdentityGuard{u64 value=AnmVm::presentation_counter;~IdentityGuard(){AnmVm::presentation_counter=value;}} guard;
    if(!saved_)saved_=std::make_unique<Data>();
    auto& s=*saved_;s.captured=false;s.count=s.root_count=0;s.present.reset();s.originals.clear();
    auto& selected=s.selected;selected.clear();auto& indices=s.indices;
    if(!++s.capture_epoch){indices.clear();++s.capture_epoch;}
    auto add=[&](const AnmRegistry::Node* n){if(n){auto& index=indices[n];if(index.epoch!=s.capture_epoch){index={u32(selected.size()),s.capture_epoch};selected.push_back(n);}}};
    for(size_t i=0;i<registry.pool.size();++i)if(const auto* n=registry.pool[i].get()){s.present.set(i);bool nested_live=false;if(n->vm.geometry.overlay)each_vm(n->vm,[&](const AnmVm& v){if(&v!=&n->vm&&registry.node(const_cast<AnmVm&>(v)))nested_live=true;});if(n->active||nested_live)add(n);}
    for(const auto& entry:registry.overflow)add(entry.first);
    for(const auto& entry:registry.external)add(entry.second.get());
    for(const auto& layer:registry.layers)for(auto* vm:layer)add(registry.node(*vm));
    for(auto* n:registry.pending_deletion)add(n);
    auto& unregistered=s.unregistered;unregistered.assign(owned_roots.begin(),owned_roots.end());
    std::sort(unregistered.begin(),unregistered.end(),std::less<AnmVm*>{});
    unregistered.erase(std::unique(unregistered.begin(),unregistered.end()),unregistered.end());
    unregistered.erase(std::remove_if(unregistered.begin(),unregistered.end(),[&](AnmVm* vm){if(!vm)return true;if(auto* n=registry.node(*vm)){add(n);return true;}return false;}),unregistered.end());
    for(size_t i=0;i<selected.size();++i){
        add(selected[i]->parent);
        for(auto* child:selected[i]->children)add(child);
    }
    // A registered borrowed VM can itself be owned by another VM's overlay.
    // Restore that C++ ownership tree before rebinding its registry node.
    std::unordered_set<const AnmVm*> nested;
    const auto ownership=[&](const AnmVm& vm){each_vm(vm,[&](const AnmVm& v){if(&v!=&vm)nested.insert(&v);});};
    for(const auto* n:selected)ownership(n->borrowed?*n->borrowed:n->vm);
    for(auto* vm:unregistered)ownership(*vm);
    unregistered.erase(std::remove_if(unregistered.begin(),unregistered.end(),[&](const AnmVm* vm){return nested.count(vm)!=0;}),unregistered.end());
    s.count=selected.size();if(s.nodes.size()<s.count)s.nodes.resize(s.count);
    for(size_t i=0;i<s.count;++i){
        const auto& n=*selected[i];auto& image=s.nodes[i];
        image.borrowed=n.borrowed;image.external=n.borrowed!=nullptr;image.frozen=n.frozen_root;
        image.original=n.borrowed?n.borrowed:&n.vm;image.nested=nested.count(image.original)!=0;
        image.vm=n.borrowed?*n.borrowed:n.vm;
        image.slot=n.slot;image.handle=n.handle;image.active=n.active;image.registered=n.registered;image.alternate=n.alternate;
        image.parent=n.parent?indices.at(n.parent).position:none;
        image.children.clear();for(auto* child:n.children)image.children.push_back(indices.at(child).position);
    }
    s.root_count=unregistered.size();if(s.roots.size()<s.root_count)s.roots.resize(s.root_count);
    for(size_t i=0;i<s.root_count;++i){s.roots[i].original=unregistered[i];s.roots[i].vm=*unregistered[i];}
    for(size_t i=0;i<s.count;++i){if(s.nodes[i].nested)continue;const auto& n=*selected[i];
        paired_vms(n.borrowed?*n.borrowed:n.vm,s.nodes[i].vm,[&](const AnmVm& original,AnmVm& copy){s.originals.emplace_back(&original,&copy);});
    }
    for(size_t i=0;i<s.root_count;++i)paired_vms(*s.roots[i].original,s.roots[i].vm,[&](const AnmVm& original,AnmVm& copy){s.originals.emplace_back(&original,&copy);});
    const auto by_original=[](const auto& a,const auto& b){return std::less<const AnmVm*>{}(a.first,b.first);};
    std::sort(s.originals.begin(),s.originals.end(),by_original);
    for(size_t i=0;i<s.count;++i)if(const auto& image=s.nodes[i];image.nested&&image.original){
        const auto at=std::lower_bound(s.originals.begin(),s.originals.end(),std::make_pair(image.original,static_cast<const AnmVm*>(nullptr)),by_original);
        if(at==s.originals.end()||at->first!=image.original){error="Rollback is missing an embedded VM's owning root";return false;}
    }
    for(auto at=indices.begin();at!=indices.end();)if(at->second.epoch!=s.capture_epoch)at=indices.erase(at);else ++at;
    for(size_t group=0;group<s.groups.size();++group){s.groups[group].clear();for(auto* n:registry.groups[group])s.groups[group].push_back(indices.at(n).position);}
    for(size_t layer=0;layer<s.layers.size();++layer)s.layers[layer].assign(registry.layers[layer].begin(),registry.layers[layer].end());
    s.pending.clear();for(auto* n:registry.pending_deletion)s.pending.push_back(indices.at(n).position);
    s.free_slots=registry.free_slots;s.generation=registry.generation;s.active_count=registry.active_count;s.presentation_counter=guard.value;s.error=registry.error;
    s.captured=true;return true;
}
bool AnimationState::restore(AnmRegistry& registry,const RootResolver& roots,const SpriteResolver& sprites)const{
    if(!ready())return false;const auto& s=*saved_;
    registry.groups[0].clear();registry.groups[1].clear();for(auto& layer:registry.layers)layer.clear();
    registry.pending_deletion.clear();registry.nodes.clear();registry.overflow.clear();registry.external.clear();
    for(size_t i=0;i<registry.pool.size();++i){auto& owned=registry.pool[i];if(!s.present[i]){owned.reset();continue;}
        if(!owned)owned=std::make_unique<AnmRegistry::Node>();auto& node=*owned;node.slot=u32(i);node.handle=0;
        node.active=node.registered=false;node.parent=nullptr;node.children.clear();registry.nodes.emplace(&node.vm,&node);
    }
    std::vector<AnmRegistry::Node*> nodes(s.count);
    std::vector<std::unique_ptr<AnmRegistry::Node>> external(s.count);
    std::unordered_map<const AnmVm*,AnmVm*> copies,originals;
    // Allocate node shells without dereferencing borrowed VMs. An overlay may
    // have destroyed/recreated those embedded addresses since capture.
    for(size_t i=0;i<s.count;++i){const auto& image=s.nodes[i];AnmRegistry::Node* node=nullptr;
        if(image.external){external[i]=std::make_unique<AnmRegistry::Node>();node=external[i].get();node->frozen_root=image.frozen;if(image.frozen)node->borrowed=&node->vm;}
        else if(image.slot<registry.pool.size())node=registry.pool[image.slot].get();
        else{auto owned=std::make_unique<AnmRegistry::Node>();node=owned.get();registry.overflow.emplace(node,std::move(owned));registry.nodes.emplace(&node->vm,node);}
        node->slot=image.slot;node->handle=image.handle;node->active=image.active;node->registered=image.registered;node->alternate=image.alternate;nodes[i]=node;
    }
    const auto pair=[&](const AnmVm& image,AnmVm& target){paired_vms(image,target,[&](const AnmVm& copy,AnmVm& value){copies.emplace(&copy,&value);});};
    // Copy only the topmost owning roots; their deep copy recreates all nested
    // overlay panels. Copying a panel again would invalidate its children.
    for(size_t i=0;i<s.count;++i){const auto& image=s.nodes[i];if(image.nested)continue;auto& node=*nodes[i];
        if(image.external&&!image.frozen){node.borrowed=roots?roots(image.borrowed):image.borrowed;if(!node.borrowed){registry.error="Rollback animation owner was not rebound";return false;}}
        auto& vm=node.borrowed?*node.borrowed:node.vm;vm=image.vm;pair(image.vm,vm);
    }
    std::vector<AnmVm*> targets(s.root_count);
    for(size_t i=0;i<s.root_count;++i){const auto& image=s.roots[i];auto* target=roots?roots(image.original):image.original;
        if(!target){registry.error="Rollback embedded VM owner was not rebound";return false;}*target=image.vm;targets[i]=target;pair(image.vm,*target);
    }
    for(const auto& link:s.originals){auto found=copies.find(link.second);if(found==copies.end()){registry.error="Rollback animation graph is incomplete";return false;}originals.emplace(link.first,found->second);}
    for(size_t i=0;i<s.count;++i){const auto& image=s.nodes[i];auto& node=*nodes[i];
        if(image.nested){auto found=originals.find(image.original);if(found==originals.end()||!image.external||image.frozen){registry.error="Rollback embedded VM ownership is invalid";return false;}node.borrowed=found->second;}
        if(image.external){auto* borrowed=node.borrowed;if(!registry.external.emplace(borrowed,std::move(external[i])).second){registry.error="Duplicate rollback VM owner";return false;}}
    }
    auto parent=[&](AnmVm* old){auto found=originals.find(old);return found!=originals.end()?found->second:old&&roots?roots(old):old;};
    bool missing_sprite=false;
    const auto remap=[&](AnmVm& vm){each_vm(vm,[&](AnmVm& value){value.rotation_parent=parent(value.rotation_parent);value.creation_parent=parent(value.creation_parent);if(value.sprite_source&&sprites){value.sprite_source=sprites(value.sprite_source);missing_sprite|=value.sprite_source==nullptr;}});};
    for(size_t i=0;i<s.count;++i){auto& node=*nodes[i];const auto& image=s.nodes[i];node.parent=image.parent==none?nullptr:nodes[image.parent];
        node.children.clear();for(auto child:image.children)node.children.push_back(nodes[child]);
        if(!image.nested)remap(node.borrowed?*node.borrowed:node.vm);
    }
    for(auto* target:targets)remap(*target);
    if(missing_sprite){registry.error="Rollback sprite owner was not rebound";return false;}
    for(size_t group=0;group<s.groups.size();++group)for(auto index:s.groups[group]){auto* node=nodes[index];node->position=registry.groups[group].insert(registry.groups[group].end(),node);}
    for(size_t layer=0;layer<s.layers.size();++layer)for(auto* old:s.layers[layer]){auto found=originals.find(old);if(found==originals.end()){registry.error="Rollback animation draw order is incomplete";return false;}registry.layers[layer].push_back(found->second);}
    for(auto index:s.pending)registry.pending_deletion.push_back(nodes[index]);
    registry.free_slots=s.free_slots;registry.generation=s.generation;registry.active_count=s.active_count;registry.error=s.error;AnmVm::presentation_counter=s.presentation_counter;return true;
}
size_t AnimationState::retained_bytes()const noexcept{
    if(!saved_)return 0;const auto& s=*saved_;size_t bytes=sizeof(Data)+s.nodes.capacity()*sizeof(Data::Image)+s.roots.capacity()*sizeof(Data::RootImage)+s.free_slots.capacity()*sizeof(u32)+s.pending.capacity()*sizeof(u32);
    for(const auto& image:s.nodes)bytes+=dynamic_bytes(image.vm)+image.children.capacity()*sizeof(u32);
    for(const auto& image:s.roots)bytes+=dynamic_bytes(image.vm);
    for(const auto& group:s.groups)bytes+=group.capacity()*sizeof(u32);
    for(const auto& layer:s.layers)bytes+=layer.capacity()*sizeof(AnmVm*);
    return bytes+s.originals.capacity()*sizeof(decltype(s.originals)::value_type)+s.selected.capacity()*sizeof(void*)+s.unregistered.capacity()*sizeof(void*)+s.indices.size()*(sizeof(void*)*2+sizeof(Data::Index));
}
}
