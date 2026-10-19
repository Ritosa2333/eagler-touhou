#include "AnmVm.hpp"
#include <cmath>
namespace th15 {
namespace {
template<class T>T read(const u8* p){T result;std::memcpy(&result,p,sizeof result);return result;}
bool span(u32 off,u32 n,u32 size){return off<=size&&n<=size-off;}
bool integer_math(i32 a,i32 b,u32 operation,i32& out){
    if(operation==0)out=b;else if(operation==1)out=wrapping_add(a,b);else if(operation==2)out=wrapping_sub(a,b);else if(operation==3)out=signed_bits(u32(a)*u32(b));
    else{if(!b||(a==INT32_MIN&&b==-1))return false;out=operation==4?a/b:a%b;}return true;
}
float float_math(float a,float b,u32 operation){switch(operation){case 0:return b;case 1:return float(a+b);case 2:return float(a-b);case 3:return float(a*b);case 4:return float(a/b);default:return float(std::fmod(double(a),double(b)));}}
}
bool AnmVm::bind(AnmScript& data){presentation_generation=++presentation_counter;presentation_motion=false;const i32 layer=visual.layer;const Vec3 translation=visual.translation;script=&data.bytes;resource=nullptr;source_script=-1;sprite=nullptr;sprite_resource=nullptr;sprite_source=nullptr;rotation_parent=nullptr;creation_parent=nullptr;slowdown=0;variables={};visual={};visual.layer=layer;visual.translation=translation;visual.sprite_matrix.identity();interpolators={};timer={};timer.set(0);age={};age.set(0);saved_timer={0,0,0,0,0};saved_offset=0;geometry.clear();instruction_offset=0;visible=false;pending_interrupt=0;error.clear();return script->size()>=8;}
bool AnmVm::bind(AnmResource& data,u32 index){if(index>=data.scripts.size()||!bind(data.scripts[index]))return false;resource=&data;source_script=i32(index);return true;}
bool AnmVm::select_sprite(i32 index)noexcept{
    AnmResource* source=resource;if(index<0){source=environment?environment->fallback_sprite_resource:nullptr;index=258;}
    if(!source||u32(index)>=source->sprites.size())return false;
    sprite_resource=source;sprite=&source->sprites[index];visual.sprite=index;visual.sprite_size={sprite->width,sprite->height};
    visual.uv[0]={sprite->u0,sprite->v0};visual.uv[1]={sprite->u1,sprite->v0};visual.uv[2]={sprite->u0,sprite->v1};visual.uv[3]={sprite->u1,sprite->v1};
    visual.sprite_matrix.identity();visual.uv_matrix.identity();visual.sprite_matrix.m[0]=float(visual.sprite_size.x*.00390625f);visual.sprite_matrix.m[5]=float(visual.sprite_size.y*.00390625f);visual.transform_matrix=visual.sprite_matrix;
    const auto& texture=source->textures[sprite->texture];visual.uv_matrix.m[0]=float(visual.sprite_size.x/float(texture.width));visual.uv_matrix.m[5]=float(visual.sprite_size.y/float(texture.height));return true;
}
Vec3 AnmVm::total_rotation(u32 depth)noexcept{Vec3 result=variables.rotation;if(rotation_parent&&!(visual.render_flags&0x10000)&&depth<64){const Vec3 parent=rotation_parent->total_rotation(depth+1);result={float(parent.x+result.x),float(parent.y+result.y),float(parent.z+result.z)};variables.rotation={normalize_angle(variables.rotation.x),normalize_angle(variables.rotation.y),normalize_angle(variables.rotation.z)};}return result;}
float AnmVm::effective_slowdown()const noexcept{const AnmVm* source=this;for(u32 depth=0;source->rotation_parent&&!(source->visual.render_flags&0x10000)&&depth<64;depth++)source=source->rotation_parent;return source->slowdown;}
int AnmVm::tick(Rng& rng,float rate){
    const float original_rate=rate,amount=effective_slowdown();
    if(visual.render_flags&0x200)rate=1;
    if(amount>0){rate=float(original_rate-float(amount*original_rate));if(rate<0)rate=0;}
    if(geometry.gather||geometry.overlay){if(!object_host){error="Animation effect update host not bound";return -1;}const i32 result=object_host->update_effect(*this,rate);if(result){if(result<0&&error.empty())error="Animation effect update failed";return result;}}
    if(geometry.trail){const i32 result=geometry.trail->update(rng,rate);if(result){if(result<0)error="Particle trail timer outside verified range";return result;}}
    if(geometry.distortion)geometry.distortion->update(variables.position,visual.translation,visual.color);
    if(!script||instruction_offset<0||(visual.flags&0x100000))return 0;
    age.tick(&rate);
    if(!pending_interrupt&&environment&&environment->paused&&(visual.render_flags&0xc000)==0x4000)return 0;
    auto fail=[&](const char* reason){error=reason;return -1;};
    auto jump=[&](u32 offset,i32 time){if(!span(offset,8,script->size()))return false;instruction_offset=i32(offset);timer.set(time);return true;};
    auto interrupt=[&](){u32 fallback=UINT32_MAX,found=UINT32_MAX;for(u32 off=0;span(off,8,script->size());){const auto* p=script->data()+off;const i16 op=read<i16>(p);const u16 length=read<u16>(p+2);if(op==-1)break;if(length<8||!span(off,length,script->size()))break;if(op==5&&length>=12){const i32 id=read<i32>(p+8);if(id==pending_interrupt){found=off;break;}if(id==-1)fallback=off;}off+=length;}pending_interrupt=0;visual.flags&=~0x4000u;if(found==UINT32_MAX)found=fallback;if(found==UINT32_MAX)return false;saved_timer.set(timer.current);saved_offset=instruction_offset;visible=true;visual.flags|=1;const auto* p=script->data()+found;timer.set(read<i16>(p+4));instruction_offset=i32(found+read<u16>(p+2));return true;};
    if(pending_interrupt&&!interrupt()){timer.decrement(&rate);if(!advance(rate))return -1;timer.tick(&rate);visible=visual.visible();return 0;}
    for(u32 steps=0;steps<100000;steps++){
        if(!span(u32(instruction_offset),8,script->size()))return fail("Animation instruction outside script");
        auto* p=script->data()+instruction_offset;const i16 opcode=read<i16>(p),time=read<i16>(p+4);const u16 length=read<u16>(p+2),mask=read<u16>(p+6);
        if(time>timer.current){if(!advance(rate))return -1;timer.tick(&rate);visible=visual.visible();return 0;}
        if(opcode==-1||opcode==1){visible=false;visual.flags&=~1u;instruction_offset=-1;return 1;}if(opcode==2){instruction_offset=-1;return 0;}
        if(length<8||length%4||!span(u32(instruction_offset),length,script->size()))return fail("Invalid animation instruction length");
        u32 needed=0;if(opcode>=100&&opcode<=111)needed=2;else if(opcode>=112&&opcode<=121)needed=3;else if(opcode==122||opcode==123)needed=2;else if(opcode==200)needed=2;else if(opcode==201)needed=3;else if(opcode>=202&&opcode<=213)needed=4;else if(opcode==5||opcode==6)needed=1;
        else if(opcode>=302&&opcode<=317)needed=(opcode==308||opcode==309||opcode==316||opcode==317)?0:opcode==312?2:1;
        else if(opcode==400||opcode==401||opcode==404||opcode==406||opcode==415)needed=3;
        else if(opcode==402||opcode==416||opcode==429||opcode==434||opcode==436)needed=2;
        else if(opcode==403||opcode==405||opcode==423||opcode==424||opcode==425||opcode==426||opcode==431||opcode==432||opcode==437||opcode==438)needed=1;
        else if(opcode==407||opcode==408||opcode==410||opcode==413)needed=5;
        else if(opcode==409||opcode==411||opcode==414||opcode==427||opcode==428)needed=3;
        else if(opcode==412||opcode==430||opcode==433||opcode==435)needed=4;
        else if(opcode==417)needed=2;
        else if(opcode==420)needed=10;
        else if(opcode>=124&&opcode<=128)needed=2;
        else if(opcode==129)needed=1;
        else if(opcode==130||opcode==131||opcode==433)needed=4;
        else if(opcode==300)needed=1;
        else if(opcode==301)needed=2;
        else if(opcode>=500&&opcode<=508)needed=opcode==505||opcode==506?3:1;
        else if(opcode>=600&&opcode<=611)needed=opcode==611?3:opcode>=603&&opcode<=608?2:1;
        if(8+needed*4>length)return fail("Truncated animation arguments");
        auto arg=[&](u32 n){return p+8+n*4;};
        auto integer=[&](u32 n){const i32 value=read<i32>(arg(n));return mask&(1u<<n)?variables.integer(value,rng):value;};
        bool unresolved=false;auto floating=[&](u32 n){const float value=read<float>(arg(n));if(!(mask&(1u<<n)))return value;const i32 id=truncate_int(value);
            if(id==10026)return total_rotation().z;
            if(id>=10016&&id<=10021){if(!environment){unresolved=true;return 0.f;}const Vec3& a=id<=10018?environment->camera_origin:environment->reference_position;const float values[]={a.x,a.y,a.z};float result=values[(id-10016)%3];if(id<=10018){const float shifts[]={environment->screen_translation.x,environment->screen_translation.y,environment->screen_translation.z};result=float(result+shifts[id-10016]);}return result;}
            if(id>=10030&&id<=10032){if(!environment){unresolved=true;return 0.f;}auto& game_rng=environment->game_rng;return id==10030?float(game_rng.signed_unit()*variables.random_angle):id==10031?float(game_rng.unit()*variables.random_scale):float(game_rng.signed_unit()*variables.random_scale);}
            return variables.floating(value,rng);
        };
        auto int_dest=[&](u32 n){auto* a=reinterpret_cast<i32*>(arg(n));return mask&(1u<<n)?variables.integer_destination(a):a;};
        auto float_dest=[&](u32 n){auto* a=reinterpret_cast<float*>(arg(n));return mask&(1u<<n)?variables.float_destination(a):a;};
        auto bits=[](u32& destination,u32 value,u32 shift,u32 field){destination=(destination&~field)|((value<<shift)&field);};
        auto vec3=[&](){const float z=floating(2),y=floating(1),x=floating(0);return Vec3{x,y,z};};
        auto vec2=[&](){const float y=floating(1),x=floating(0);return Vec2{x,y};};
        auto color=[&](u32& destination){const u32 red=u8(integer(0)),green=u8(integer(1)),blue=u8(integer(2));destination=(destination&0xff000000)|(red<<16)|(green<<8)|blue;};
        auto rgb=[](u32 value){return std::array<i32,3>{u8(value),u8(value>>8),u8(value>>16)};};
        auto interpolate2=[&](Vec2Interpolation& interpolation,Vec2 from){const float y=floating(3),x=floating(2);const i32 frames=integer(0);interpolation.begin(frames,u8(read<u32>(arg(1))),{from.x,from.y},{x,y});};
        auto interpolate_color=[&](ColorInterpolation& interpolation,u32 from){const u8 blue=u8(integer(4)),green=u8(integer(3)),red=u8(integer(2));const i32 frames=integer(0);interpolation.control1={};interpolation.control2={};interpolation.begin(frames,u8(read<u32>(arg(1))),rgb(from),{blue,green,red});};
        auto interpolate_alpha=[&](AlphaInterpolation& interpolation,u32 from){const i32 value=integer(2),frames=integer(0);interpolation.begin(frames,u8(read<u32>(arg(1))),{i32(from>>24)},{i32(u8(value))});};
        if(opcode>=100&&opcode<=121){const bool three=opcode>=112;const u32 operation=three?(opcode-112)/2+1:(opcode-100)/2;
            if(!(opcode&1)){const i32 a=integer(1),b=three?integer(2):0;auto* out=int_dest(0);i32 value;if(!integer_math(three?a:*out,three?b:a,operation,value))return fail("Animation integer divide error");*out=value;}
            else{const float a=three?floating(1):0,b=floating(three?2:1);auto* out=float_dest(0);*out=float_math(three?a:*out,b,operation);}
        }else if(opcode>=202&&opcode<=213){bool take;const u32 compare=(opcode-202)/2;if(opcode&1){const float a=floating(0),b=floating(1);take=compare==0?a==b:compare==1?a!=b:compare==2?a<b:compare==3?a<=b:compare==4?a>b:a>=b;}else{const i32 a=integer(0),b=integer(1);take=compare==0?a==b:compare==1?a!=b:compare==2?a<b:compare==3?a<=b:compare==4?a>b:a>=b;}
            if(take){if(unresolved)return fail("Animation external variable not bound");if(!jump(read<u32>(arg(2)),read<i32>(arg(3))))return fail("Invalid animation jump");continue;}
        }else switch(opcode){
            case 0:case 5:break;
            case 3:case 4:if(opcode==4){visible=false;visual.flags&=~1u;}if(pending_interrupt){if(interrupt())continue;}else{visual.flags|=0x4000;timer.decrement(&rate);if(!advance(rate))return -1;timer.tick(&rate);visible=visual.visible();return 0;}break;
            case 6:{const float frames=float(integer(0));timer.previous=timer.current;timer.fractional=float(timer.fractional-frames);timer.current=truncate_int(timer.fractional);break;}
            case 7:timer.set(saved_timer.current);instruction_offset=i32(saved_offset);continue;
            case 122:{const u32 bound=u32(integer(1));const i32 value=bound?signed_bits(rng.next32()%bound):0;*int_dest(0)=value;break;}
            case 123:{const float maximum=floating(1),value=float(rng.unit()*maximum);*float_dest(0)=value;break;}
            case 124:case 125:case 126:case 127:case 128:{const double angle=double(floating(1));const float value=float(opcode==124?std::sin(angle):opcode==125?std::cos(angle):opcode==126?std::tan(angle):opcode==127?std::acos(angle):std::atan(angle));*float_dest(0)=value;break;}
            case 129:*float_dest(0)=normalize_angle(floating(0));break;
            case 130:{const float radius=floating(3),angle=floating(2);auto* y=float_dest(1);auto* x=float_dest(0);*x=float(std::cos(double(angle))*double(radius));*y=float(std::sin(double(angle))*double(radius));break;}
            case 131:{const float minimum=floating(2),maximum=floating(3),angle=float(rng.signed_unit()*3.1415927410125732421875f),radius=float(float(rng.signed_unit()*float(maximum-minimum))+minimum);const float x=float(std::cos(double(angle))*double(radius)),y=float(std::sin(double(angle))*double(radius));*float_dest(0)=x;*float_dest(1)=y;break;}
            case 200:if(!jump(read<u32>(arg(0)),read<i32>(arg(1))))return fail("Invalid animation jump");continue;
            case 201:{auto* counter=int_dest(0);*counter=wrapping_add(*counter,-1);if(integer(0)>0){if(!jump(read<u32>(arg(1)),read<i32>(arg(2))))return fail("Invalid animation jump");continue;}break;}
            case 300:{visual.flags|=1;visible=true;const i32 parameter=integer(0),index=sprite_source?sprite_source->sprite_index(parameter):parameter;if(!select_sprite(index))return fail("Animation sprite not available");visual.sprite_frame=timer.current;break;}
            case 301:{visual.flags|=1;visible=true;const i32 first=integer(0);const u32 count=u32(integer(1));if(!count)return fail("Animation sprite random range is zero");const i32 parameter=signed_bits(rng.next32()%count+u32(first)),index=sprite_source?sprite_source->sprite_index(parameter):parameter;if(!select_sprite(index))return fail("Animation sprite not available");visual.sprite_frame=timer.current;break;}
            case 302:{const u32 mode=read<u32>(arg(0));bits(visual.flags,mode,25,0x3e000000);if((mode&31)==10){if(!environment)return fail("Animation distortion random source not bound");geometry.clear();geometry.distortion=std::make_unique<AnmDistortion>();geometry.distortion->initialize(variables.position,visual.translation,environment->game_rng);geometry.allocation_bytes=1200;}break;}
            case 303:bits(visual.flags,read<u32>(arg(0)),5,0x1e0);break;
            case 304:visual.set_layer(u8(read<u32>(arg(0))));break;
            case 305:bits(visual.flags,read<u32>(arg(0)),13,0x2000);break;
            case 306:bits(visual.flags,read<u32>(arg(0)),15,0x8000);break;
            case 307:bits(visual.render_flags,u8(read<u32>(arg(0))),10,0x400);break;
            case 308:visual.flags=(visual.flags^0x800)|8;visual.scale.x=float(-visual.scale.x);break;
            case 309:visual.flags=(visual.flags^0x1000)|8;visual.scale.y=float(-visual.scale.y);break;
            case 310:bits(visual.flags,read<u32>(arg(0)),0,1);visible=visual.visible();break;
            case 311:bits(visual.render_flags,read<u32>(arg(0)),11,0x800);break;
            case 312:bits(visual.render_flags,u32(integer(0)),0,3);bits(visual.flags,u32(integer(1)),30,0xc0000000);break;
            case 313:bits(visual.render_flags,u32(integer(0)),20,0x700000);break;
            case 314:bits(visual.render_flags,u32(integer(0)),23,0x800000);break;
            case 315:bits(visual.render_flags,u8(read<u32>(arg(0))),25,0x2000000);break;
            case 316:visual.flags|=2;break;
            case 317:visual.flags&=~2u;break;
            case 400:if(visual.flags&0x400)visual.child_anchor=vec3();else variables.position=vec3();break;
            case 401:variables.rotation.x=floating(0);variables.rotation.y=floating(1);variables.rotation.z=floating(2);visual.flags|=4;break;
            case 402:visual.scale.x=floating(0);visual.scale.y=floating(1);visual.flags|=8;break;
            case 403:visual.color=(visual.color&0xffffff)|(u32(u8(integer(0)))<<24);break;
            case 404:color(visual.color);break;
            case 405:visual.secondary_color=(visual.secondary_color&0xffffff)|(u32(u8(integer(0)))<<24);break;
            case 406:color(visual.secondary_color);break;
            case 407:{const i32 frames=integer(0);const float z=floating(4),y=floating(3),x=floating(2);const Vec3 from=visual.flags&0x400?visual.child_anchor:variables.position;interpolators.position.control1={};interpolators.position.control2={};interpolators.position.begin(frames,read<i32>(arg(1)),{from.x,from.y,from.z},{x,y,z});break;}
            case 408:interpolate_color(interpolators.color,visual.color);break;
            case 409:interpolators.alpha.control1={};interpolators.alpha.control2={};interpolate_alpha(interpolators.alpha,visual.color);break;
            case 410:{const float z=floating(4),y=floating(3),x=floating(2);const i32 frames=integer(0);const Vec3 from=variables.rotation;interpolators.rotation.control1={};interpolators.rotation.control2={};interpolators.rotation.begin(frames,read<i32>(arg(1)),{from.x,from.y,from.z},{x,y,z});visual.flags|=4;break;}
            case 411:{const float to=normalize_angle(floating(2)),from=normalize_angle(variables.rotation.z);const i32 frames=integer(0);interpolators.angle.begin(frames,read<i32>(arg(1)),{from},{to});interpolators.angle.control1={};interpolators.angle.control2={};visual.flags|=4;break;}
            case 412:interpolate2(interpolators.scale,visual.scale);visual.flags|=8;break;
            case 413:interpolate_color(interpolators.secondary_color,visual.secondary_color);visual.flags=(visual.flags&~0x40000u)|0x20000;break;
            case 414:interpolate_alpha(interpolators.secondary_alpha,visual.secondary_color);visual.flags=(visual.flags&~0x40000u)|0x20000;break;
            case 415:visual.angular_velocity=vec3();visual.render_flags|=0x1000000;break;
            case 416:visual.scale_velocity=vec2();visual.render_flags|=0x1000000;break;
            case 417:{const i32 frames=integer(1);interpolators.alpha.control1={};interpolators.alpha.control2={};interpolators.alpha.begin(frames,0,{i32(visual.color>>24)},{i32(u8(read<u32>(arg(0))))});break;}
            case 420:{const Vec3 from=visual.flags&0x400?visual.child_anchor:variables.position;
                const float c1x=floating(1),c1y=floating(2),c1z=floating(3),c2x=floating(7),c2y=floating(8),c2z=floating(9);const i32 frames=integer(0);const float z=floating(6),y=floating(5),x=floating(4);
                interpolators.position.begin(frames,8,{from.x,from.y,from.z},{x,y,z});interpolators.position.control1={c1x,c1y,c1z};interpolators.position.control2={c2x,c2y,c2z};break;}
            case 421:{if(length<12)return fail("Truncated animation arguments");bits(visual.flags,read<u16>(p+8),21,0x600000);bits(visual.flags,read<u16>(p+10),23,0x1800000);break;}
            case 422:variables.position=visual.translation;visual.translation={};break;
            case 423:bits(visual.flags,u8(read<u32>(arg(0))),17,0x60000);break;
            case 424:bits(visual.render_flags,u8(read<u32>(arg(0))),7,0x80);break;
            case 425:visual.uv_velocity.x=floating(0);visual.render_flags|=0x1000000;break;
            case 426:visual.uv_velocity.y=floating(0);visual.render_flags|=0x1000000;break;
            case 427:case 428:{const float target=floating(2);const i32 frames=integer(0);auto& interpolation=opcode==427?interpolators.uv_x:interpolators.uv_y;interpolation.control1={};interpolation.control2={};interpolation.begin(frames,read<i32>(arg(1)),{opcode==427?visual.uv_velocity.x:visual.uv_velocity.y},{target});break;}
            case 429:visual.uv_scale.x=floating(0);visual.uv_scale.y=floating(1);visual.flags|=0x10;break;
            case 430:interpolate2(interpolators.uv_scale,visual.uv_scale);visual.flags|=0x10;break;
            case 431:bits(visual.render_flags,u8(read<u32>(arg(0))),8,0x100);break;
            case 432:bits(visual.render_flags,u32(integer(0)),9,0x200);break;
            case 433:{const i32 frames=integer(0);const Vec3 from=visual.flags&0x400?visual.child_anchor:variables.position;const float radius=floating(3),angle=floating(2),x=float(std::cos(double(angle))*double(radius)),y=float(std::sin(double(angle))*double(radius));interpolators.position.control1={};interpolators.position.control2={};interpolators.position.begin(frames,read<i32>(arg(1)),{from.x,from.y,from.z},{x,y,0});break;}
            case 435:interpolate2(interpolators.secondary_scale,visual.scale);visual.flags|=8;break;
            case 434:visual.secondary_scale.x=floating(0);visual.secondary_scale.y=floating(1);visual.flags|=8;break;
            case 436:visual.size.x=floating(0);visual.size.y=floating(1);break;
            case 437:bits(visual.render_flags,u32(integer(0)),2,0x1c);break;
            case 438:bits(visual.render_flags,u8(read<u32>(arg(0))),18,0xc0000);break;
            case 500:case 501:case 502:case 503:case 504:case 505:case 506:{
                const i32 script_index=integer(0);if(!object_host)return fail("Animation object host not bound");
                AnmVm* child=(opcode==504||opcode==506)?object_host->spawn_detached(*this,script_index):object_host->spawn_child(*this,script_index,opcode==501?4:opcode==502?2:opcode==503?6:0);
                if(!child)return fail("Animation object creation failed");
                if(opcode==505||opcode==506){child->visual.child_anchor.x=floating(1);child->visual.child_anchor.y=floating(2);}
                break;
            }
            case 507:bits(visual.render_flags,u32(integer(0)),16,0x10000);break;
            case 508:{const i32 script_index=integer(0);if(!object_host)return fail("Animation effect host not bound");if(!object_host->spawn_effect(*this,script_index))return fail("Animation effect creation failed");break;}
            case 509:if(creation_parent){std::memcpy(variables.integers,creation_parent->variables.integers,sizeof variables.integers);std::memcpy(variables.floats,creation_parent->variables.floats,sizeof variables.floats);std::memcpy(variables.vector,creation_parent->variables.vector,sizeof variables.vector);std::memcpy(variables.extra_integers,creation_parent->variables.extra_integers,sizeof variables.extra_integers);variables.random_scale=creation_parent->variables.random_scale;variables.random_angle=creation_parent->variables.random_angle;variables.random_bound=creation_parent->variables.random_bound;}break;
            case 600:case 601:case 602:case 609:case 610:{const u32 mode=opcode==600?9:opcode==601?13:opcode==602?14:opcode==609?24:25;visual.flags=(visual.flags&~((mode^31u)<<25))|(mode<<25);if(!geometry.allocate(integer(0),opcode>=609))return fail("Animation geometry count outside range");break;}
            case 603:case 606:case 607:case 608:{const u32 mode=opcode==603?16:opcode==606?20:opcode==607?21:22;visual.flags=(visual.flags&~((mode^31u)<<25))|(mode<<25);visual.sprite_size={floating(0),floating(1)};break;}
            case 604:case 605:{const u32 mode=opcode==604?17:18;visual.flags=(visual.flags&~((mode^31u)<<25))|(mode<<25);visual.sprite_size.x=floating(0);variables.integers[0]=integer(1);break;}
            case 611:visual.flags=(visual.flags&0xe7ffffffu)|0x26000000;visual.sprite_size={floating(0),floating(1)};variables.integers[0]=integer(2);break;
            default:error="Unimplemented ANM opcode "+std::to_string(opcode);return -1;
        }
        if(unresolved)return fail("Animation external variable not bound");instruction_offset+=length;
    }
    return fail("Animation instruction budget exceeded");
}
}
