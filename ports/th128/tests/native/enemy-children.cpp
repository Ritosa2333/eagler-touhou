#include "game/EnemyVisuals.hpp"
#include "animation/AnmScene.hpp"
#include "engine/Random.hpp"
#include "../ecl-fixture.hpp"
#include <fstream>
#include <iostream>
#include <iterator>
#include <set>
using namespace ecl_fixture;

Bytes read(const char* path){std::ifstream file(path,std::ios::binary);require(bool(file),"actual ANM file is missing");return {std::istreambuf_iterator<char>(file),{}};}
th128::enemy::EnemyVisuals::Pose pose(const th128::anm::Scene& scene,const th128::anm::Node& node){
    const auto& state=node.vm.state();th128::enemy::EnemyVisuals::Pose result{
        state.x+state.engineX+state.offsetX,state.y+state.engineY+state.offsetY,state.z+state.engineZ+state.offsetZ,
        state.sx,state.sy,state.rotation};
    if(node.parent){const auto parent=scene.nodes().find(node.parent);require(parent!=scene.nodes().end(),"reference parent missing");
        const auto inherited=pose(scene,*parent->second);result.x+=inherited.x;result.y+=inherited.y;result.z+=inherited.z;
        result.sx*=inherited.sx;result.sy*=inherited.sy;if(state.mode==1||state.mode==16)result.rotation+=inherited.rotation;}
    return result;
}

int main(int argc,char** argv){try{
    require(argc==4,"pass actual bullet.anm, enemy.anm and ascii.anm");const auto bullet=read(argv[1]),enemy=read(argv[2]),font=read(argv[3]);
    th128::enemy::EnemyVisuals actual;std::string error;
    require(actual.load(0,bullet.data(),bullet.size(),error)&&actual.load(1,enemy.data(),enemy.size(),error)
        &&actual.load(11,font.data(),font.size(),error),error);
    th128::anm::Scene reference;require(reference.registry.loadBank(0,bullet.data(),bullet.size(),error),error);
    require(reference.registry.loadBank(1,enemy.data(),enemy.size(),error)&&reference.registry.loadBank(11,font.data(),font.size(),error),error);
    th128::Random actualRandom{34772},referenceRandom{34772};
    actual.setRandomSources([&]{return actualRandom.next32();},{});actual.setUpdateControl({true,0});
    reference.runtime.gameRandom32=[&]{return referenceRandom.next32();};reference.runtime.updateControl=th128::anm::UpdateControl{true,0};
    reference.runtime.cameraPosition.fill(std::numeric_limits<float>::quiet_NaN());
    reference.runtime.cameraEyeOffset.fill(std::numeric_limits<float>::quiet_NaN());
    std::vector<std::string> faults;reference.runtime.unsupported=[&](int,int,int,const char* reason){faults.emplace_back(reason);};
    for(const auto& request:std::array<std::array<int,4>,9>{{{41,0,1,0},{42,1,0,99},{42,2,0,189},{43,3,0,182},{44,4,0,123},{45,5,0,209},{46,6,0,133},{47,7,0,160},{48,8,0,161}}}){
        require(actual.create(request[0],request[1],request[2],request[3],7,error),error);
        require(reference.bind((std::uint64_t(request[0])<<8)|request[1],request[0],request[2],request[3],
            th128::anm::Membership::Primary,0,0,{},true,nullptr,224,16,7)!=nullptr,"reference birth failed");
    }
    unsigned observed176=0,observed177=0,observed178=0,observed179=0,observed180=0,observed181=0;
    unsigned observed186=0,observed187=0,observed188=0,observed121=0,observed122=0,whiteGlyph122=0,customMode15=0,observed206=0,observed207=0,observed208=0,observed132=0,observed159=0;std::set<std::uint32_t> identities;
    const auto compare=[&]{
        const auto snapshots=actual.snapshots();const auto expected=reference.chain(th128::anm::Membership::Primary);
        require(faults.empty()&&snapshots.size()==expected.size(),"real global primary chain node count differs");
        require(actualRandom.seed==referenceRandom.seed&&actualRandom.calls==referenceRandom.calls,"actual child births must retain shared RNG order/count");
        std::set<std::uint32_t> live;
        for(std::size_t index=0;index<snapshots.size();++index){const auto& current=snapshots[index];const auto& state=expected[index]->vm.state();
            require(current.node&&live.insert(current.node).second,"every root and child needs a distinct stable identity");identities.insert(current.node);
            require(current.state.bank==state.bank&&current.state.script==state.script&&current.state.clock==state.clock
                &&current.state.ticks==state.ticks&&current.state.sprite==state.sprite&&current.state.primary==state.primary
                &&current.state.flags2==state.flags2,"actual child/root script order, lifetime or state differs");
            const auto world=pose(reference,*expected[index]);
            float nativeRadius=float(state.geometryRadius*state.sx);
            if(expected[index]->parent)nativeRadius=float(nativeRadius*reference.nodes().at(expected[index]->parent)->vm.state().sx);
            require(current.world.radius==nativeRadius,"circle radius must multiply own then parent X scale before polar vertex generation");
            require(current.world.x==world.x&&current.world.y==world.y&&current.world.z==world.z&&current.world.sx==world.sx
                &&current.world.sy==world.sy&&current.world.rotation==world.rotation,"child geometry must compose real ancestry exactly once");
            if(current.parent){require(((current.entity==42&&current.slot==2)||(current.entity==43&&current.slot==3)
                ||(current.entity==44&&current.slot==4)||(current.entity==45&&current.slot==5)||(current.entity==46&&current.slot==6)||(current.entity==47&&current.slot==7)||(current.entity==48&&current.slot==8))&&current.state.engineX==0&&current.state.engineY==0,
                "attached children retain local engine pose, not a second owner translation");
                observed176+=state.script==176;observed177+=state.script==177;observed178+=state.script==178;
                observed179+=state.script==179;observed180+=state.script==180;observed181+=state.script==181;
                observed186+=state.script==186;observed187+=state.script==187;observed188+=state.script==188;}
            observed121+=current.parent&&state.script==121;observed122+=current.parent&&state.script==122;
            whiteGlyph122+=state.script==122&&state.spriteBank==11&&state.sprite==313;
            customMode15+=state.script==122&&state.mode==16&&state.geometryCount==36;
            observed206+=current.parent&&state.script==206;observed207+=current.parent&&state.script==207;observed208+=current.parent&&state.script==208;
            observed132+=current.parent&&state.script==132&&state.spriteBank==11&&state.sprite==313&&state.mode==16;
            observed159+=current.parent&&state.script==159;
        }
    };
    compare();
    for(int tick=0;tick<100;++tick){
        const unsigned flags=tick>=10&&tick<20?2:0;
        actual.setUpdateControl({true,flags});reference.runtime.updateControl=th128::anm::UpdateControl{true,flags};
        if(tick%9==0){require(actual.position(42,float(tick-30),float(tick+80),3),"actual owner pose");
            for(const int slot:{1,2}){auto* node=reference.find((std::uint64_t(42)<<8)|slot);require(node,"reference root missing");
                node->vm.control().engineX=float(tick-30);node->vm.control().engineY=float(tick+80);node->vm.control().engineZ=3;}}
        require(actual.tickAll(error),error);reference.tickPrimary();compare();
    }
    require(observed176&&observed177&&observed178&&observed179&&observed180&&observed181,
        "actual boss aura script182 must schedule its authored script176..181 children in the same global Scene chain");
    require(observed186&&observed187&&observed188&&identities.size()>20,"all actual authored children must birth, expire and be replaced through multiple loops");
    require(observed121&&observed122,"actual Boss123 ANM owner must schedule its121/122 children through shared Scene order");
    require(whiteGlyph122&&customMode15,"actual122 negative sprite resolves through original ascii313 while retaining mode16/36 geometry state");
    require(observed206&&observed207&&observed208,"actual209 Boss entrance must schedule all three original206..208 children and shared RNG");
    require(observed132,"actual133 firing effect must schedule its132 original diffuse fan and121 particles");
    require(observed159,"original160/161 Boss defeat effects must create159 cubic moving particles");
    const auto key=(std::uint64_t(42)<<8)|2;const auto oldPose=reference.find(key)->vm.state();
    const auto beforeReplace=actual.snapshots();
    const auto oldGeneration=std::find_if(beforeReplace.begin(),beforeReplace.end(),[](const auto& node){return node.entity==42&&node.slot==2;});
    require(oldGeneration!=beforeReplace.end(),"replacement owner must have a live original generation");
    const auto oldRootId=oldGeneration->node;
    require(actual.create(42,2,0,99,7,error),error);reference.remove(key);reference.collect();
    auto* replaced=reference.bind(key,42,0,99,th128::anm::Membership::Primary,0,0,{},true,nullptr,224,16,7);
    require(replaced,"reference replacement failed");replaced->vm.control().engineX=oldPose.engineX;
    replaced->vm.control().engineY=oldPose.engineY;replaced->vm.control().engineZ=oldPose.engineZ;
    compare();const auto afterReplace=actual.snapshots();
    require(std::none_of(afterReplace.begin(),afterReplace.end(),[oldRootId](const auto& node){return node.parent==oldRootId;})
        &&std::any_of(afterReplace.begin(),afterReplace.end(),[](const auto& node){return node.entity==42&&node.slot==1;})
        &&std::any_of(afterReplace.begin(),afterReplace.end(),[](const auto& node){return node.entity==42&&node.slot==2&&node.parent==0&&node.state.script==99;}),
        "successful root replacement retires only its attached generation and preserves sibling slots");
    require(actual.removeOwner(42),"owner retirement");reference.removeOwner(42);reference.collect();compare();
    const auto afterOwnerRetire=actual.snapshots();
    require(std::all_of(afterOwnerRetire.begin(),afterOwnerRetire.end(),[](const auto& node){return node.entity==41||node.entity==43||node.entity==44||node.entity==45||node.entity==46||node.entity==47||node.entity==48;}),
        "owner removal must collect that owner's descendants and retain unrelated root/aura owners");
    th128::enemy::EnemyVisuals rejected;require(rejected.load(0,bullet.data(),bullet.size(),error),error);rejected.setUpdateControl({true,0});
    require(rejected.create(7,2,0,99,7,error),error);const auto retained=rejected.snapshots()[0];
    require(!rejected.create(7,2,0,189,7,error)&&error.find("RNG")!=std::string::npos,"missing real child RNG must reject startup");
    require(rejected.snapshots().size()==1&&rejected.snapshots()[0].node==retained.node&&rejected.snapshots()[0].state.script==99,
        "failed parent replacement must preserve old root and leak no staged child");
    unsigned throws=0;rejected.setRandomSources([&]()->std::uint32_t{++throws;throw std::runtime_error("child RNG failure");},{});
    require(!rejected.create(7,2,0,189,7,error)&&throws==1&&rejected.snapshots().size()==1,"child callback failure must stay sticky during synchronous birth");
    require(rejected.snapshots()[0].node==retained.node,"callback failure must retain previous slot identity");
    th128::enemy::EnemyVisuals reentry;require(reentry.load(0,bullet.data(),bullet.size(),error),error);reentry.setUpdateControl({true,0});
    unsigned reentryCalls=0;
    reentry.setRandomSources([&]{++reentryCalls;std::string nested;require(!reentry.tickAll(nested),"child RNG cannot reenter its host");return 7u;},{});
    require(!reentry.create(7,2,0,189,7,error)&&error.find("reentry")!=std::string::npos&&reentry.snapshots().empty(),
        "ignored child callback reentry must remain sticky and publish no root or child");
    require(reentryCalls==1,"a rejected callback must not consume later child RNG requests");
    reentry.setRandomSources([]{return 7u;},{});
    require(reentry.create(7,2,0,189,7,error),"rejected startup releases its guard and allows a new explicit attempt");
    th128::enemy::EnemyVisuals budget;require(budget.load(0,bullet.data(),bullet.size(),error),error);
    th128::Random budgetRandom{17};budget.setRandomSources([&]{return budgetRandom.next32();},{});budget.setUpdateControl({true,0});
    require(budget.create(42,2,0,189,7,error),error);
    for(unsigned index=0;index<4093;++index)require(budget.create(100+index,0,0,99,7,error),error);
    require(budget.snapshots().size()==4095,"node budget includes the actual initial child");
    require(budget.tickAll(error)&&budget.tickAll(error)&&budget.tickAll(error),"a registered parent may append the final4096th node");
    require(budget.snapshots().size()==4096,"global primary pool may reach its exact declared bound");
    require(!budget.tickAll(error)&&error.find("budget")!=std::string::npos,"4097th child fails at its genuine birth boundary");
    th128::enemy::EnemyVisuals overflow;Bytes bytes=enemy;
    const auto script=get32(bytes,64+4*(unsigned(bytes[4])|(unsigned(bytes[5])<<8))+4);
    std::fill(bytes.begin()+script,bytes.begin()+script+48,0);
    put16(bytes,script,3);put16(bytes,script+2,12);put16(bytes,script+12,48);put16(bytes,script+14,20);
    put32(bytes,script+20,floatBits(std::numeric_limits<float>::max()));put16(bytes,script+32,63);put16(bytes,script+34,8);put16(bytes,script+40,65535);
    require(overflow.load(1,bytes.data(),bytes.size(),error)&&overflow.create(1,0,1,0,7,error),error);
    require(overflow.position(1,std::numeric_limits<float>::max(),0,0),"finite engine pose is accepted");
    const auto overflowing=overflow.snapshots();
    require(overflowing.size()==1&&std::isinf(overflowing[0].world.x),"derived overflow must reach the explicit presentation guard, not silently disappear");
    std::cout<<"actual189 children match global Scene order, RNG, geometry, control and100ticks; startup failures retain old roots\n";
    return 0;
}catch(const std::exception& error){std::cerr<<"FAIL enemy children: "<<error.what()<<'\n';return 1;}}
