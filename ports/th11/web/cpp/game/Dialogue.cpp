#include "Dialogue.hpp"
#include <cstdlib>
namespace th11 {
namespace {
template<class T>T read(const u8* p){T v;std::memcpy(&v,p,sizeof v);return v;}
bool decode(const u8* p,u32 n,std::string& out){u8 key=0x77,step=7;out.clear();for(u32 i=0;i<n;++i){const u8 c=p[i]^key;key=u8(key+step);step=u8(step+0x10);if(!c)return true;out.push_back(char(c));}return false;}
}
void Dialogue::clear(){for(auto& id:state.animations){families.erase(id);id=0;}active=false;}
bool Dialogue::create(u32 slot,AnmResource& resource,i32 script,u16 file){auto* vm=manager.create(resource,script,file,22,false,false);if(!vm)return fail("dialogue animation creation");if(slot<8)state.animations[slot]=vm->id;return true;}
bool Dialogue::begin(const std::vector<u8>& bytes,i32 id){
    if(bytes.size()<4||id<0||u32(id)>=read<u32>(bytes.data())||u64(4)+u64(id)*8+8>bytes.size())return fail("invalid MSG entry");
    const u32 offset=read<u32>(bytes.data()+4+u32(id)*8);if(u64(offset)+4>bytes.size())return fail("invalid MSG offset");
    clear();error.clear();state={};state.id=id;state.instruction=bytes.data()+offset;end=bytes.data()+bytes.size();
    state.elapsed.set(0,&manager.rate);state.time.set(0,&manager.rate);state.wait.set(0,&manager.rate);
    for(auto& p:state.text_positions)p={8,0,0};state.colors[0]=0xf8f08f;state.colors[1]=0x8088ff;state.colors[2]=0xd8d8d8;
    for(u32 i=0;i<4;++i){if(!create(i+3,text,i,0))return false;auto* vm=find(state.animations[i+3]);vm->rectangle_columns=vm->rectangle_rows=16;}
    active=true;return true;
}
bool Dialogue::write(u32 slot,const std::string& value,i32 offset,i32 font,i32 style){
    if(!find(state.animations[slot]))return fail("missing dialogue text animation");
    if(!effects.dialogue_text({state.animations[slot],state.colors[state.speaker],offset,font,style<0?i32((state.flags>>1)&1):style,value}))return fail("dialogue glyph output");return true;
}
bool Dialogue::speaker(i32 value){
    if(value==0){families.interrupt(state.animations[1],3);families.interrupt(state.animations[0],2);families.interrupt(state.animations[2],2);}
    else if(value==1){families.interrupt(state.animations[0],3);families.interrupt(state.animations[1],2);families.interrupt(state.animations[2],3);}
    else for(u32 i=0;i<3;++i)families.interrupt(state.animations[i],8);
    state.speaker=value;for(u32 i=3;i<7;++i){families.position(state.animations[i],state.text_positions[value],false);auto* vm=find(state.animations[i]);if(!vm)return fail("missing dialogue line");vm->child_position.y=0;}
    state.flags=value==2?state.flags|2:state.flags&~2u;state.next_line=state.lines_initialized=0;return true;
}
bool Dialogue::expression(bool enemy_side,i32 value){
    auto* root=find(state.animations[enemy_side?1:0]);if(!root)return fail("missing dialogue portrait");
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
    case 1:return create(0,player,51+20*character,7);
    case 2:{i32 script=52+20*character;if(stage==5||(stage==6&&state.id==2))++script;else if(stage==6)script+=2;return create(1,player,script,7);}
    case 3:return create(2,front,45,5);
    case 4:families.interrupt(state.animations[0],1);state.animations[0]=0;break;
    case 5:families.interrupt(state.animations[1],1);state.animations[1]=0;families.interrupt(state.animations[7],1);break;
    case 6:for(u32 i=2;i<7;++i)families.interrupt(state.animations[i],1);break;
    case 7:case 8:case 9:return speaker(op-7);
    case 10:state.flags=(state.flags&~1u)|(p[0]&1);break;
    case 12:state.release_signal=1;break;
    case 13:case 14:return expression(op==14,read<i32>(p));
    case 15:case 16:case 17:{
        if(op==17&&state.next_line==0&&!state.lines_initialized){for(u32 i=3;i<7;++i)if(!write(i," ",0,i>=5?1:0))return false;state.lines_initialized=1;for(u32 i=3;i<7;++i)families.interrupt(state.animations[i],3);}
        std::string value;if(!decode(p,n,value))return fail("unterminated MSG text");u32 slot=op==15?3:op==16?4:state.next_line?4:3;i32 offset=0,font=0;
        if(op==17&&!value.empty()&&value[0]=='|'){const auto a=value.find(',',1),b=a==std::string::npos?a:value.find(',',a+1);if(b==std::string::npos)return fail("invalid MSG ruby text");offset=std::atoi(value.c_str()+1);font=std::atoi(value.c_str()+a+1);value=value.substr(b+1);slot=state.next_line?6:5;}
        else if(op==17){if(state.next_line)state.next_line=state.lines_initialized=0;else ++state.next_line;}
        if(!write(slot,value,offset,font,slot>=5?2:-1))return false;families.interrupt(state.animations[slot],2);break;
    }
    case 18:for(u32 i=3;i<7;++i)families.interrupt(state.animations[i],3);break;
    case 19:if(!effects.dialogue_music())return fail("dialogue boss music");return create(8,logo,2,27);
    case 20:{constexpr i32 scripts[]={9,9,10,14,18,19,9};if(stage<1||stage>7)return fail("invalid dialogue stage");return create(7,enemy,scripts[stage-1],9);}
    case 21:if(!effects.dialogue_stage_complete())return fail("stage completion unavailable");break;
    case 22:if(!effects.dialogue_fade(stage==6?8.f:2.f))return fail("dialogue music fade");break;
    case 23:case 24:families.interrupt(state.animations[op-23],7);break;
    case 25:for(u32 i=3;i<7;++i){auto* vm=find(state.animations[i]);if(!vm)return fail("missing dialogue line");vm->child_position.y=float(read<i32>(p));}break;
    case 26:state.flags|=2;break;
    default:return fail("unsupported MSG opcode");
    }return true;
}
i32 Dialogue::update(u32 held,u32 pressed){
    if(!active)return 1;if(!error.empty())return -2;
    if(state.release_signal>0)--state.release_signal;
    if(state.instruction+4>end){fail("truncated MSG instruction");return -2;}
    if((state.flags&1)&&(held&0x200))state.time.set(read<u16>(state.instruction),&manager.rate);
    while(state.instruction+4<=end&&read<u16>(state.instruction)<=state.time.current){
        const auto* p=state.instruction;last_opcode=p[2];const u32 n=p[3];if(p+4+n>end){fail("truncated MSG payload");return -2;}
        if(last_opcode==0){clear();return 1;}
        if(last_opcode==11){
            if(n<4){fail("truncated MSG wait");return -2;}if(state.wait.current<1)state.wait.set(read<i32>(p+4),&manager.rate);state.wait.advance(-1);
            if((pressed&0x80001)||state.wait.current<=0){if(!effects.dialogue_sound(0)){fail("dialogue confirmation sound");return -2;}}
            else if(!(state.flags&1)||!(held&0x200))return 0;
            state.wait.set(0,&manager.rate);state.next_line=state.lines_initialized=0;
        }else if(!command(u8(last_opcode),p+4,n))return -2;
        state.instruction+=4+n;
    }
    if(state.instruction+4>end){fail("missing MSG end instruction");return -2;}state.time.tick();return 0;
}
}
