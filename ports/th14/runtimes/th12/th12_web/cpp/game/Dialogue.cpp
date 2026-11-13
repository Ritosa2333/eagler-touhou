#include "ShotFiles.hpp"
#include "Dialogue.hpp"
#include <cstdlib>
#include <string>
namespace th11 {
namespace {
template<class T>T read(const u8* p){T v;std::memcpy(&v,p,sizeof v);return v;}
bool decode(const u8* p,u32 n,std::string& out){u8 key=0x77,step=7;out.clear();for(u32 i=0;i<n;++i){const u8 c=p[i]^key;key=u8(key+step);step=u8(step+0x10);if(!c)return true;out.push_back(char(c));}return false;}
// How the dialogue animation failure names its position and the commands that led there.
std::string anm_hex4(u32 value){char hex[4]={};for(u32 n=0;n<4;++n){const u32 digit=(value>>((3-n)*4))&15;hex[n]=char(digit<10?'0'+digit:'a'+digit-10);}return std::string(hex,4);}
std::string anm_path(const AnmManager& manager){const u32 first=manager.last_path_count>8?manager.last_path_count-8:0;if(!manager.last_path_count)return std::string();std::string out=" path";for(u32 i=first;i<manager.last_path_count;++i){out+=i>first?",":"";out+=anm_hex4(manager.last_path[i].offset);out+=":";out+=std::to_string(manager.last_path[i].opcode);out+="(len ";out+=std::to_string(manager.last_path[i].length);out+=")";}return out;}
}
void Dialogue::clear(){for(auto& id:state.animations){
#if PORT_TITLE == 14
    if(auto* vm=manager.find(id);vm&&vm->dialogue_owner==this){vm->dialogue_owner=nullptr;vm->draw_callback=nullptr;}
#endif
    families.erase(id);id=0;}if constexpr(kPortTitle==14){for(auto& id:th14_enemy_portraits){families.erase(id);id=0;}families.erase(th14_text_background);th14_text_background=0;}active=false;}
bool Dialogue::th14_asset(i32 stage,i32 selection,u32 opcode,i32& file,i32& script)noexcept{
    // Stage records at 4d5988 + stage*cc. MSG 20 selects +4c/+50,
    // MSG 2 selects +54/+58, both with a 36-byte speaker stride;
    // MSG 31 uses the fixed +78/+7c pair.
    constexpr i32 assets[7][3][4]={
        {{3,16,3,10},{-1,-1,-1,-1},{0,0,0,0}},
        {{3,28,3,22},{-1,-1,-1,-1},{0,0,0,0}},
        {{3,18,3,13},{3,18,3,13},{0,0,0,0}},
        {{3,13,3,8},{4,9,4,8},{0,0,0,0}},
        {{3,13,3,8},{-1,-1,-1,-1},{0,0,0,0}},
        {{3,19,3,14},{-1,-1,-1,-1},{0,0,0,0}},
        {{3,16,3,12},{-1,-1,4,8},{-1,-1,5,8}}
    };
    if(stage<1||stage>7)return false;
    if(opcode==31)selection=1;
    if(u32(selection)>=3||(opcode!=2&&opcode!=20&&opcode!=31))return false;
    const u32 offset=opcode==20?0:2;file=assets[stage-1][selection][offset];script=assets[stage-1][selection][offset+1];return true;
}
bool Dialogue::create_th14(u32 slot,i32 selection,u32 opcode){
    i32 file,script;if(!th14_animations||!th14_asset(stage,selection,opcode,file,script)||u32(file)>=16)return fail("invalid TH14 dialogue asset");
    const auto& resource=th14_animations->resources[file];if(!resource.file)return fail("missing TH14 dialogue asset");
    if(opcode==2||opcode==31){auto* vm=manager.create(*resource.file,script,resource.id,u32(-1),false,false);if(!vm)return fail("TH14 enemy portrait animation");th14_enemy_portraits[opcode==31?1:selection]=vm->id;return true;}
    if(!create(slot,*resource.file,script,resource.id))return false;
    // MSG20 calls4350b0 immediately after publishing the entrance animation
    // at433606; the ordinary HUD callback may still be blocked by dialogue.
    if(opcode==20&&!effects.dialogue_boss_name())return fail("TH14 boss name animation");
    return true;
}
// TH14 MSG factories (432bd7/432cbb/432d98/433589) copy the initialized
// ANM prefix and retain its layer; they do not override it with layer22.
bool Dialogue::create(u32 slot,AnmResource& resource,i32 script,u16 file){auto* vm=manager.create(resource,script,file,kPortTitle==14?u32(-1):22,false,false);if(!vm){const std::string message="dialogue animation creation: slot "+std::to_string(slot)+" , script "+std::to_string(script)+" of "+std::to_string(resource.scripts.size())+" (file "+std::to_string(file)+", depth "+std::to_string(manager.depth())+" , ANM opcode "+std::to_string(manager.last_error)+" at +0x"+anm_hex4(manager.last_error_offset)+anm_path(manager)+")";return fail(message.c_str());}if(slot<8)state.animations[slot]=vm->id;return true;}
bool Dialogue::locate(const std::vector<u8>& bytes,i32 id,u32& offset,u32& flags){
    // The original performs no bounds check here at all. Its reader loads the message bytes and then
    // indexes the entry table straight away (th12.exe 0x41fc4e: `mov edx,dword ptr [ecx+ebx*8+4]`,
    // with ebx the id from the script) - no comparison against the entry count precedes it. The
    // scripts do pass negative ids (six records in TH12's resources, nine in TH13's, and TH13 also
    // has -2 and -3), and for one the original reads before the table; the port keeps the id
    // unsigned-safe by counting back from the end, which is the reading that keeps the ids'
    // descending order meaningful (the id is stored on the new dialogue and the section after it is
    // `id + 1`, so a negative id means an earlier section).
    const u32 count=bytes.size()<4?0:read<u32>(bytes.data());
    if(bytes.size()<4)return false;
    const i64 index=id<0?i64(count)+id:i64(id);
    if(index<0||u64(index)>=count||u64(4)+u64(index)*8+8>bytes.size())return false;
    offset=read<u32>(bytes.data()+4+u32(index)*8);flags=read<u32>(bytes.data()+4+u32(index)*8+4);
    return u64(offset)+4<=bytes.size();
}
bool Dialogue::begin(const std::vector<u8>& bytes,i32 id){
    u32 offset=0,flags=0;if(!locate(bytes,id,offset,flags)){
        const std::string message="invalid MSG entry id="+std::to_string(id)+" count="+std::to_string(bytes.size()<4?0:read<u32>(bytes.data()))+" bytes="+std::to_string(bytes.size());return fail(message.c_str());
    }
    clear();error.clear();state={};th14_text_width=0;th14_text_layout=0;state.id=id;state.instruction=bytes.data()+offset;end=bytes.data()+bytes.size();
    state.elapsed.set(0,&manager.rate);state.time.set(0,&manager.rate);state.wait.set(0,&manager.rate);
    for(auto& p:state.text_positions)p={8,0,0};state.colors[0]=0xf8f08f;state.colors[1]=0x8088ff;state.colors[2]=0xd8d8d8;
    if constexpr(kPortTitle==14){for(auto& p:state.text_positions)p={16,0,0};for(auto& c:state.colors)c=0;text_pair_194={384,640};th14_text_width=320;}
    for(u32 i=0;i<4;++i){if(!create(i+3,text,kPortTitle==14?i/2:i,0))return false;auto* vm=find(state.animations[i+3]);vm->rectangle_columns=vm->rectangle_rows=kPortTitle==14?21:16;
#if PORT_TITLE == 14
        vm->flags2|=0x800;if(i&1)vm->interrupt(7);vm->dialogue_owner=this;vm->draw_callback=th14_text_position;
#endif
    }
    active=true;return true;
}
bool Dialogue::write(u32 slot,const std::string& value,i32 offset,i32 font,i32 style){
    auto* vm=find(state.animations[slot]);if(!vm)return fail("missing dialogue text animation");
    u32 color=state.colors[state.speaker];
    if constexpr(kPortTitle==14){if(slot>=5&&style==2){vm->flags2|=0x800;color=0x408080;}}
    if(!effects.dialogue_text({state.animations[slot],color,offset,font,style<0?i32((state.flags>>1)&1):style,value}))return fail("dialogue glyph output");return true;
}
bool Dialogue::th14_background(i32 layout){
    // 434020/4341e0 use front.anm, and write the children's float variable
    // +4bc (10004), which their ANM scripts consume on the following update.
    families.erase(th14_text_background);
    const Vec3 p{text_pair_194.x,text_pair_194.y,0};
    auto* vm=manager.create(front,216+layout,5,22,false,false,&p);
    if(!vm)return fail("TH14 dialogue background animation");
    th14_text_background=vm->id;th14_text_layout=layout;
    for(auto* node=&vm->child;node;node=node->next){auto* child=node->value;if(child!=vm&&(child->script_index==168+layout||child->script_index==200+layout))child->floats[0]=float(th14_text_width+16.f);}
    return true;
}
void Dialogue::th14_text_position(AnmVm& line){
#if PORT_TITLE == 14
    if(!line.dialogue_owner)return;auto& d=*static_cast<Dialogue*>(line.dialogue_owner);auto* panel=d.find(d.th14_text_background);if(!panel)return;
    for(auto* node=panel->child.next;node;node=node->next){const auto* child=node->value;if(child->script_index!=168+d.th14_text_layout)continue;
        // Retail 433b80 -> 4339c0 runs at draw time (478f6e), following
        // the animated panel child, then converting back to text ANM units.
        auto p=child->render_position(d.manager.render_space);const float scale=float(2.f/d.manager.render_space.scale);p.x=float(p.x*scale);p.y=float(p.y*scale);
        if(d.state.speaker<1)p.x=float(p.x-36.f);
        else if(child->scale.x<1.f)p.x=float(p.x+float(float(child->scale.x*32.f)-6.f));
        else p.x=float(p.x+26.f);
        line.position=p;return;
    }
#else
    (void)line;
#endif
}
bool Dialogue::speaker(i32 value,i32 selection){
    if constexpr(kPortTitle==14){
        // Retail 432f77/433043/4331b2 own four enemy portraits separately.
        // 47f1b0 updates each portrait family immediately;47f140 only
        // queues the label for the reserved family at native+54.
        if(value==1){if(!families.interrupt_now(th14_enemy_portraits[selection],2))return fail("TH14 portrait interrupt");}
        else for(auto id:th14_enemy_portraits)if(!families.interrupt_now(id,3))return fail("TH14 portrait interrupt");
        if(!families.interrupt_now(state.animations[0],value==0?2:3))return fail("TH14 player interrupt");
        families.interrupt(state.animations[2],value==0?2:3);
        state.speaker=value==1?1:0;
        for(u32 i=3;i<7;++i){auto* vm=find(state.animations[i]);if(!vm)return fail("missing dialogue line");if(value==2)vm->position=state.text_positions[0];vm->child_position.y=0;}
        state.flags&=~2u;state.next_line=state.lines_initialized=0;return true;
    }
    if(value==0){families.interrupt(state.animations[1],3);families.interrupt(state.animations[0],2);families.interrupt(state.animations[2],2);}
    else if(value==1){families.interrupt(state.animations[0],3);families.interrupt(state.animations[1],2);families.interrupt(state.animations[2],3);}
    else for(u32 i=0;i<3;++i)families.interrupt(state.animations[i],8);
    state.speaker=value;for(u32 i=3;i<7;++i){families.position(state.animations[i],state.text_positions[value],false);auto* vm=find(state.animations[i]);if(!vm)return fail("missing dialogue line");vm->child_position.y=0;}
    state.flags=value==2?state.flags|2:state.flags&~2u;state.next_line=state.lines_initialized=0;return true;
}
bool Dialogue::expression(bool enemy_side,i32 value){
    // The original never requires the portrait to exist. Its opcode 13 and 14 handlers walk three
    // child entries, look each one up, and skip it when the lookup returns null:
    //   0x4206ad call 0x461920 (find) ; 0x4206b2 test eax,eax ; 0x4206b4 je 0x4206c5 (skip)
    // Failing here instead made a dialogue whose portrait had already been interrupted abort the
    // whole battle, which is not something this binary can do.
    auto* root=find(state.animations[enemy_side?1:0]);
    if(!root)return true;
    i32 children[3]={39+20*character,43+20*character,47+20*character},offsets[3]={64+13*character,72+13*character,80+13*character};
    if(enemy_side){i32 delta=1;if(stage==5||(stage==6&&state.id==2))delta=2;else if(stage==6)delta=3;for(auto& c:children)c+=delta;
        constexpr i32 table[7][3]={{16,24,33},{8,16,25},{8,17,26},{18,27,36},{16,25,34},{29,38,47},{13,25,37}};
        if(stage<1||stage>7)return fail("invalid dialogue stage");for(u32 j=0;j<3;++j)offsets[j]=table[stage-1][j];
        if(stage==6&&state.id==2){offsets[0]=60;offsets[1]=68;offsets[2]=76;}
        if(stage==7){if(state.id==2){offsets[0]=53;offsets[1]=61;offsets[2]=69;}else if(value>11){offsets[0]=41;offsets[1]=49;offsets[2]=57;}}
    }
    for(u32 j=0;j<3;++j)for(auto* node=&root->child;node;node=node->next)if(node->value->script_index==children[j]){auto* vm=node->value;if(enemy_side)vm->resource=&enemy;if(!vm->bind_sprite(wrapping_add(offsets[j],value)))return fail("invalid dialogue expression sprite");break;}
    return true;
}
bool Dialogue::command(u8 op,const u8* p,u32 n){
    if((op==10&&n<1)||((op==13||op==14||op==25)&&n<4))return fail("truncated MSG argument");
    switch(op){
    // MSG opcode 1: the portrait script index is not computed from the character. The original reads
    // [0x4b0c90] (the character) and indexes the table at [0x4b2f84]:
    //   0x42013f mov eax,[0x4b0c90] ; 0x420144 mov ecx,[eax*4+0x4b2f84] ; 0x420163 call 0x4615a0
    // Its first three entries are 31/30/28, which are exactly the last script index of
    // pl00/pl01/pl02 (32/31/29 scripts). An earlier revision used 51+20*character, copied from the
    // previous title's port, which is out of range for every TH12 player file.
    case 1:{
        if(character<0||u32(character)>player_character_max)return fail("invalid dialogue character");
        // The original reads the character global and indexes a per-character table of portrait script
        // indices. Both tables were read from the images, and the TH12 read is the check: it
        // returns exactly the three constants this case already carried.
        //   th12.exe 0x42013f  mov eax,[0x4b0c90] ; mov ecx,[eax*4+0x4b2f84]   (31, 30, 28)
        //   th13.exe 0x429298  mov edx,[0x4be7b8] ; mov eax,[edx*4+0x4c0aa0]   (29, 34, 30, 28)
        // TH13's is four entries because it has four characters, one file and one shot type each
        // (ShotFiles.hpp: player_file_count 4, player_subtype_max 0).
        constexpr i32 portrait_th12[3]={31,30,28};
        constexpr i32 portrait_th13[4]={29,34,30,28};
        // The generation's own tables, from the same shape of site in each title's image
        // (tools/th-portrait-table.py, whose controls are the two above). The scan asks the image
        // for a `mov r32,[selection]` followed by a `table[register*4]` read and then checks the
        // answer against that title's own player files:
        //   th17.exe 0x42e153 mov eax,[0x4b59f4] ; mov eax,[eax*4+0x4a1760]  -> 40 46 33
        //   th18.exe 0x43f7b2 mov eax,[0x4cccf4] ; mov eax,[eax*4+0x4b6620]  -> 40 46 29 28
        // Both are exactly the last script index of their title's player files in order
        // (41 47 34 and 41 47 30 29 by the exported sizes), which is the rule TH12 and TH13
        // satisfied too (32 31 29 and 30 35 31 29 -> 31 30 28 and 29 34 30 28).
        //   th16.exe 0x4443a1 mov eax,[0x4a57a4] ; mov eax,[eax*4+0x492c00]  -> 8 7 11 11
        // TH16's numbers are not the last index of its files (27 17 23 36), so they are landed as
        // what the site reads - every one in range for the file order pl00 pl02 pl03 pl01 that
        // ShotFiles.hpp measured - and the site is the same shape as TH17's verified one: the read
        // feeds the second argument of the created object's own setter (`push value; push object;
        // call 0x407b20` where TH17 has 0x407230 and TH12's verified read is a create argument).
        constexpr i32 portrait_th16[4]={8,7,11,11};
        constexpr i32 portrait_th17[3]={40,46,33};
        constexpr i32 portrait_th18[4]={40,46,29,28};
        // TH14's is the same shape of site (0x432ba0: mov eax,[0x4f5828] ; mov eax,
        // [eax*4+0x4d9c7c]) with 27, 37, 25; its
        // entries are read from retail; the full portrait-family check executes these scripts.
        constexpr i32 portrait_th14[3]={27,37,25};
        i32 portrait=0;
        switch(kPortTitle){
        case 13:portrait=portrait_th13[character];break;
        case 14:portrait=portrait_th14[character];break;
        case 16:portrait=portrait_th16[character];break;
        case 17:portrait=portrait_th17[character];break;
        case 18:portrait=portrait_th18[character];break;
        default:portrait=portrait_th12[character];break;
        }
        return create(0,player,portrait,kPortTitle==14?9:7);
    }
    // MSG opcode 2: the original indexes a stage table. The handler at 0x420174 branches
    //   0x420179 cmp eax,5 / jne  ->  stage 5 with dialogue id >= 2 uses the literal 0xa
    //   0x4201af cmp eax,7 / jne  ->  stage 7 with dialogue id >= 2 uses the literal 0xd
    //   0x4201e5 mov edx,[eax*4 + 0x4b2f90]   <- the general case, keyed by stage
    // The table's entries for stages 1..7 are 13, 16, 10, 19, 10, 12, 11. The earlier 52+20*character
    // was the same inherited formula as opcode 1 and never matched this binary.
    case 2:{
        if constexpr(kPortTitle==14){if(n<4)return fail("truncated MSG argument");const i32 selection=read<i32>(p);if(u32(selection)>=3)return fail("invalid dialogue speaker");return create_th14(u32(selection)+1,selection,2);}
        constexpr i32 op2_by_stage[8]={0,13,16,10,19,10,12,11};
        if(stage<1||stage>7)return fail("invalid dialogue stage");
        i32 script=op2_by_stage[stage];
        if((stage==5&&state.id>=2)||(stage==7&&state.id>=2))script=stage==5?10:13;
        return create(1,player,script,7);
    }
    // MSG opcode 3: the original pushes the literal 0x34 = 52 as the script index
    // (0x420112 mov eax,[0x4b43e4] ; 0x420117 mov ecx,[eax+0x6d44] ; 0x42011f push 0x34).
    // The earlier value 45 was another inherited constant with no basis in this binary.
    case 3:if constexpr(kPortTitle==14)break;return create(2,front,52,5);
    case 4:families.interrupt(state.animations[0],1);state.animations[0]=0;break;
    case 5:if constexpr(kPortTitle==14){if(n<4)return fail("truncated MSG portrait close");const i32 selection=read<i32>(p);if(u32(selection)>=4)return fail("invalid dialogue speaker");families.interrupt(th14_enemy_portraits[selection],1);th14_enemy_portraits[selection]=0;families.interrupt(state.animations[7],1);break;}families.interrupt(state.animations[1],1);state.animations[1]=0;families.interrupt(state.animations[7],1);break;
    case 6:if constexpr(kPortTitle==14){for(u32 i=3;i<7;++i)families.interrupt(state.animations[i],1);families.erase(th14_text_background);th14_text_background=0;break;}for(u32 i=2;i<7;++i)families.interrupt(state.animations[i],1);break;
    case 7:case 8:case 9:{i32 selection=0;if constexpr(kPortTitle==14){if(op==8){if(n<4)return fail("truncated MSG speaker");selection=read<i32>(p);if(u32(selection)>=4)return fail("invalid dialogue speaker");}}return speaker(op-7,selection);}
    case 10:state.flags=(state.flags&~1u)|(p[0]&1);break;
    case 12:state.release_signal=1;break;
    case 13:case 14:if constexpr(kPortTitle==14){u32 handle=state.animations[0];if(op==14){if(n<8)return fail("truncated MSG expression");const i32 selection=read<i32>(p+4);if(u32(selection)>=4)return fail("invalid dialogue expression speaker");handle=th14_enemy_portraits[selection];}if(!families.interrupt_now(handle,wrapping_add(read<i32>(p),17)))return fail("TH14 expression interrupt");break;}return expression(op==14,read<i32>(p));
    case 15:case 16:case 17:{
        if(op==17&&state.next_line==0&&!state.lines_initialized){if constexpr(kPortTitle==14)th14_text_width=0;for(u32 i=3;i<7;++i)if(!write(i,kPortTitle==14?"  ":" ",0,i>=5?1:0))return false;state.lines_initialized=1;for(u32 i=3;i<7;++i)families.interrupt(state.animations[i],3);}
        std::string value;if(!decode(p,n,value))return fail("unterminated MSG text");u32 slot=op==15?3:op==16?4:state.next_line?4:3;i32 offset=0,font=0;
        if(op==17&&!value.empty()&&value[0]=='|'){const auto a=value.find(',',1),b=a==std::string::npos?a:value.find(',',a+1);if(b==std::string::npos)return fail("invalid MSG ruby text");offset=std::atoi(value.c_str()+1);font=std::atoi(value.c_str()+a+1);value=value.substr(b+1);slot=state.next_line?6:5;}
        else if(op==17){
            if constexpr(kPortTitle==14){
                // Retail deliberately converts the wrapped unsigned byte width.
                const float width=float(u32((u32(value.size())*8u)&~15u)-28u)*2.f;
                if(width>th14_text_width)th14_text_width=width;
                const i32 layout=state.speaker+2*i32((state.flags>>2)&15)+(state.next_line?8:0);
                if(!th14_background(layout))return false;
            }
            if(state.next_line)state.next_line=state.lines_initialized=0;else ++state.next_line;
        }
        i32 style=slot>=5?2:-1;if constexpr(kPortTitle==14){if(op!=17)style=i32((state.flags>>1)&1)+4;}
        if(!write(slot,value,offset,font,style))return false;
        if constexpr(kPortTitle==14){if(op==17){const Vec3 position{text_pair_194.x,text_pair_194.y,0};if(slot>=5||state.speaker<1){find(state.animations[slot])->position=position;}else for(u32 i=3;i<7;++i)find(state.animations[i])->position=position;}}
        if constexpr(kPortTitle==14){
            // Retail queues the second ruby line (4329dd -> 432553 ->
            // 47f140); the first ruby and both body lines use 47f1b0.
            if(op==17&&slot!=6){if(!families.interrupt_now(state.animations[slot],2))return fail("TH14 text interrupt");}
            else families.interrupt(state.animations[slot],2);
        }else families.interrupt(state.animations[slot],2);break;
    }
    case 18:if constexpr(kPortTitle==14){families.erase(th14_text_background);th14_text_background=0;}for(u32 i=3;i<7;++i)families.interrupt(state.animations[i],3);break;
    case 19:if(!effects.dialogue_music())return fail("dialogue boss music");return create(8,logo,2,kPortTitle==14?6:27);
    case 20:{if constexpr(kPortTitle==14){if(n<4)return fail("truncated MSG argument");return create_th14(7,read<i32>(p),20);}constexpr i32 scripts[]={9,9,10,14,18,19,9};if(stage<1||stage>7)return fail("invalid dialogue stage");return create(7,enemy,scripts[stage-1],9);}
    case 31:if constexpr(kPortTitle==14)return create_th14(2,1,31);return fail("unsupported MSG opcode");
    case 32:{
        if constexpr(kPortTitle!=14)return fail("unsupported MSG opcode");
        if(n<4)return fail("truncated MSG argument");
        const i32 value=read<i32>(p);if(u32(value)>=3)return fail("invalid dialogue speaker");
        // Retail 433107 queues the reserved native+54 family, and resets
        // line displacement (+4f0), leaving each line's position untouched.
        families.interrupt(state.animations[2],3);state.speaker=value;
        for(u32 i=3;i<7;++i){auto* vm=find(state.animations[i]);if(!vm)return fail("missing dialogue line");vm->child_position.y=0;}
        state.flags&=~2u;state.next_line=state.lines_initialized=0;break;
    }
    case 21:if(!effects.dialogue_stage_complete())return fail("stage completion unavailable");break;
    // The fade a title uses for its stage-six dialogue is measured, not assumed. Both titles
    // compare the stage against 6 and load a float, and the two binaries load opposite values:
    //   th12.exe 0x420cc7 cmp [0x4b0cb0],6 ; [0x4a3e54] (8.0, six) / [0x4a4014] (2.0)
    //   th13.exe 0x429bf5 cmp [0x4be81c],6 ; [0x4aed90] (2.0, six) / [0x4aea74] (8.0)
    case 22:{const bool later=kPortTitle==13||kPortTitle==14;const float fade=(stage==6)?(later?2.f:8.f):(later?8.f:2.f);if(!effects.dialogue_fade(fade))return fail("dialogue music fade");break;}
    case 23:case 24:if constexpr(kPortTitle==14){if(op==23)families.interrupt(state.animations[0],7);else for(u32 i=0;i<2;++i)families.interrupt(th14_enemy_portraits[i],7);break;}families.interrupt(state.animations[op-23],7);break;
    case 25:for(u32 i=3;i<7;++i){auto* vm=find(state.animations[i]);if(!vm)return fail("missing dialogue line");vm->child_position.y=float(read<i32>(p));}break;
    case 26:state.flags|=2;break;
    case 27:if constexpr(kPortTitle==14){if(n<4)return fail("truncated MSG argument");if(!effects.dialogue_fade(read<float>(p)))return fail("dialogue music fade");break;}return fail("unsupported MSG opcode");
    // TH13's case 30 is the shared epilogue (its table entry 30 is 0x429c43, where the
    // interpreter jumps to advance to the next instruction), so the opcode carries no work.
    // TH12's table is bounded by 27, so the number is TH13-only.
    case 30:break;
    // TH13's MSG opcode 28 (th13.exe 0x428c8e) reads the payload's first two floats and
    // writes them to the dialogue state at +0x194 and +0x198. TH12 has no case here at all - and
    // neither has it 28 anywhere in its own messages - while every title from 13 on uses it
    // heavily, always with an eight-byte payload whose two dwords decode as two small finite
    // floats - every payload in every one of them (823 in 13, 1116 in 14, 1022 in 16, 2125 in
    // 17, 1605 in 18, 6691 in all), pairs like (112,288) and (224,144), the numbers a message
    // gives a drawn character. TH14 4325c0 multiplies both by the literal 2.
    case 28:if(kPortTitle!=12){if(n<8)return fail("truncated MSG position");const float scale=kPortTitle==14?2.f:1.f;text_pair_194=Vec2{read<float>(p)*scale,read<float>(p+4)*scale};break;}return fail("unsupported MSG opcode");
    // TH13's MSG opcode 29 (th13.exe 0x429c2d) is
    //   mov edx,[eax+4] ; add edx,edx ; add edx,edx    edx = payload << 2
    //   xor edx,[edi+0x178] ; and edx,0x3c ; xor [edi+0x178],edx
    // which is exactly "flag bits 2..5 := the payload's low four bits" - the dialogue style
    // bits. TH12 has no style argument; the generation does (4 bytes there), and its own payload
    // values are 0..7 in 17 and 0..3 in 14, 16 and 18, all inside the four bits the mask keeps,
    // so the same reading is the one the four corpora support.
    case 29:if(kPortTitle!=12){const u32 style=(read<u32>(p)<<2)&0x3cu;state.flags=(state.flags&~0x3cu)|style;break;}return fail("unsupported MSG opcode");
    default:return fail("unsupported MSG opcode");
    }return true;
}
i32 Dialogue::update(u32 held,u32 pressed,u32 skip_frames,u32 fire_frames){
    if(!active)return 1;if(!error.empty())return -2;
    if(state.release_signal>0)--state.release_signal;
    if(state.instruction+4>end){fail("truncated MSG instruction");return -2;}
    // Retail 432426 latches a new Ctrl/Z press; 432448/432651 require
    // twenty continuous input frames, including holds begun before dialogue.
    if constexpr(kPortTitle==14){if(pressed&0x201)state.flags|=0x40;}
    const bool long_hold=kPortTitle==14?
        ((state.flags&0x40)&&(((held&0x200)&&skip_frames>=20)||((held&1)&&fire_frames>=20))):bool(held&0x200);
    if((state.flags&1)&&long_hold)state.time.set(read<u16>(state.instruction),&manager.rate);
    while(state.instruction+4<=end&&read<u16>(state.instruction)<=state.time.current){
        const auto* p=state.instruction;last_opcode=p[2];const u32 n=p[3];if(p+4+n>end){fail("truncated MSG payload");return -2;}
        if(last_opcode==0){clear();return 1;}
        if(last_opcode==11){
            if(n<4){fail("truncated MSG wait");return -2;}if(state.wait.current<1)state.wait.set(read<i32>(p+4),&manager.rate);state.wait.advance(-1);
            if((pressed&0x80001)||state.wait.current<=0){if(!effects.dialogue_sound(0)){fail("dialogue confirmation sound");return -2;}}
            else if(!(state.flags&1)||!long_hold)return 0;
            state.wait.set(0,&manager.rate);state.next_line=state.lines_initialized=0;
        }else if(!command(u8(last_opcode),p+4,n))return -2;
        state.instruction+=4+n;
    }
    if(state.instruction+4>end){fail("missing MSG end instruction");return -2;}state.time.tick();return 0;
}
}
