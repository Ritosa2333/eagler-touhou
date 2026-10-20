#include "../../src/game/ItemPresentation.hpp"
#include <sstream>
#include <iomanip>
namespace th12 {
anm_logic::Scene animationScene;
item_system::Manager itemManager;
ufo_system::Manager ufoManager;
int frame=0;
}
using namespace th12;
std::string result;
extern "C" {
int load_bank(int bank,const uint8_t* bytes,int size){std::string error;return animationScene.registry.loadBank(bank,bytes,size,error);}
void reset(){animationScene.reset();animationScene.runtime={};itemManager.reset();ufoManager.reset();frame=0;resetItemPresentation();}
int spawn_item(int type,float x,float y){auto* item=itemManager.spawn(type,{x,y},0,0);return item?int(item->id):0;}
void sync(int logicalFrame){frame=logicalFrame;syncItemPresentation();animationScene.collect();}
void tick_items(float x,float y){
    item_system::FrameView view;view.player=item_system::PlayerView::native(0,{x,y});
    view.inventoryColors=&ufoManager.inventory.colors;view.inventoryCount=&ufoManager.inventory.count;
    view.animate=itemPresentationTick;
    itemManager.tick(view,[](const auto& event){itemPresentationEvent(event);});
}
void tick_primary(){animationScene.tickPrimary();animationScene.collect();}
void hint_create(int id,int irq){if(auto* item=itemManager.find(id)){item->hintActive=true;++item->hintGeneration;item->hintInterrupt=irq;itemPresentationEvent({item_system::EventKind::HintCreated,*item,irq});}}
void hint_collect(int id){if(auto* item=itemManager.find(id)){itemPresentationEvent({item_system::EventKind::HintCollected,*item,1});item->hintActive=false;item->state=item_system::State::Inactive;}}
void rebind_item(int id,int type){if(auto* item=itemManager.find(id)){item->type=type;item->visualScript=type+165;item->visualAge=0;itemPresentationEvent({item_system::EventKind::AnimationRebind,*item,item->visualScript});}}
void stock_color(int color){auto resources=item_system::ResourceState::initial(1);const auto plan=ufoManager.collectToken(color,resources);itemPresentationPlan(plan);consumeUfoPresentation();}
void summon(){ufo_system::EnemyHooks hooks;hooks.spawn=[](const auto&){return 42u;};ufoManager.tick({},hooks);consumeUfoPresentation();}
void fill(){for(int i=0;i<40;i++)ufoManager.absorb(2,1);consumeUfoPresentation();}
void pose(int age,float x,float y,int life,int maxLife){ufoManager.age=age;ufoManager.elapsed=float(age);ufo_system::EnemyHooks hooks;hooks.query=[=](uint32_t){return ufo_system::EnemyView{true,42,{x,y},life,maxLife};};ufoManager.tick({},hooks);consumeUfoPresentation();}
void clear_ufo(){ufo_system::EnemyHooks hooks;hooks.query=[](uint32_t){return ufo_system::EnemyView{};};ufoManager.tick({},hooks);consumeUfoPresentation();}
const char* snapshot(){
    std::ostringstream out;out<<std::setprecision(9)<<"[";bool comma=false;
    std::vector<uint32_t> ids;for(const auto& [id,node]:animationScene.nodes())if(node->alive)ids.push_back(id);std::sort(ids.begin(),ids.end());
    for(auto id:ids){if(comma)out<<',';comma=true;const auto& node=*animationScene.nodes().at(id);const auto& s=node.vm.state();out<<"{\"id\":"<<node.id<<",\"owner\":"<<node.owner<<",\"parent\":"<<node.parent<<",\"membership\":"<<int(node.membership)<<",\"script\":"<<s.script<<",\"bank\":"<<s.bank<<",\"spriteBank\":"<<s.spriteBank<<",\"sprite\":"<<s.sprite<<",\"clock\":"<<s.clock<<",\"ticks\":"<<s.ticks<<",\"pending\":"<<node.vm.diagnostics().pendingInterrupt<<",\"x\":"<<s.x<<",\"y\":"<<s.y<<",\"engine\":["<<s.engineX<<','<<s.engineY<<"],\"scale\":["<<s.sx<<','<<s.sy<<"],\"rotationX\":"<<s.rx<<",\"alpha\":"<<(s.primary>>24)<<",\"flags\":"<<s.flags<<",\"viewport\":["<<node.viewportX<<','<<node.viewportY<<"]}";}
    out<<"]";result=out.str();return result.c_str();
}
}
