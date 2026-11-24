#include "LaserLineHost.hpp"
#include <algorithm>
#include <cmath>
#include <exception>
#include <limits>
#include <utility>

namespace th128::laser {
namespace {
class ExecutionGuard {
public:
    explicit ExecutionGuard(bool& executing):executing_(executing){executing_=true;}
    ~ExecutionGuard(){executing_=false;}
    ExecutionGuard(const ExecutionGuard&)=delete;
    ExecutionGuard& operator=(const ExecutionGuard&)=delete;
private:
    bool& executing_;
};
bool requiresChildOwner(int opcode){
    return opcode==88||opcode==90||opcode==91||opcode==92||opcode==95||opcode==96||opcode==97;
}
bool bounded(float value){return std::isfinite(value)&&std::abs(value)<=65536;}
bool readLaserAcceleration(const bullet::Emitter& emitter,std::optional<bullet::ExtensionRaw>& acceleration,std::string& error){
    acceleration.reset();
    for(int index=0;index<18;++index){
        const auto record=bullet::extensionRecord(emitter,index);
        if(!record||record->control!=0){error="unsupported live laser extension control";return false;}
        if(record->type==0)continue;
        if(index!=0||record->type!=4||record->callback||record->parameterA<0||record->parameterA>65535
            ||!bounded(record->parameterR)||!std::isfinite(record->parameterS)||record->parameterS> -990.f){
            error="unsupported live laser extension record";return false;
        }
        acceleration=*record;
    }
    return true;
}
std::optional<bullet::Extension4Step> stepLaserAcceleration(const bullet::Extension4& state,float speed,float angle,bullet::Position velocity){
    if(!state.active||state.time<0||state.period<0||state.fraction!=float(state.time)||!bounded(speed)||!bounded(angle))return std::nullopt;
    bullet::Extension4Step result{state,speed,angle,velocity};
    if(state.time>=state.period){result.next.active=false;result.finished=true;return result;}
    result.speed=float(double(speed)+state.magnitude);
    result.velocity={float(double(velocity.x)+state.vector.x),float(double(velocity.y)+state.vector.y),float(double(velocity.z)+state.vector.z)};
    if(std::abs(result.velocity.x)>0.00009999999747378752||std::abs(result.velocity.y)>0.00009999999747378752)result.angle=float(std::atan2(double(result.velocity.y),double(result.velocity.x)));
    result.next.previous=state.time;++result.next.time;result.next.fraction+=1;
    return result;
}
float normalized(float value){
    constexpr double pi=3.1415927410125732421875,tau=6.283185482025146484375;
    while(value>pi)value=float(double(value)-tau);
    while(value< -pi)value=float(double(value)+tau);
    return value;
}
}
std::optional<CollisionKind> classifyOverlap(const LineSnapshot& line,const CollisionContext& context){
    const std::array scalars{line.source.x,line.source.y,line.angle,line.length,line.width,
        context.player.x,context.player.y,context.halfWidth,context.halfHeight};
    if(!std::all_of(scalars.begin(),scalars.end(),bounded)||line.length<0||line.width<0
        ||context.halfWidth<0||context.halfHeight<0||std::abs(line.angle)>2048)return std::nullopt;
    const float originDistance=float(double(line.length)/10.0);
    const float helperX=float(double(line.source.x)+float(std::cos(double(line.angle))*originDistance));
    const float helperY=float(double(line.source.y)+float(std::sin(double(line.angle))*originDistance));
    const float helperLength=float(double(line.length)*4.0/5.0);
    const float helperWidth=line.width<32?float(double(line.width)*.5):float(double(line.width)*.5-8.0);
    const float deltaX=float(double(context.player.x)-helperX),deltaY=float(double(context.player.y)-helperY);
    const float sine=float(std::sin(-double(line.angle))),cosine=float(std::cos(-double(line.angle)));
    const float horizontal=float(double(cosine)*deltaX-double(sine)*deltaY);
    const float vertical=float(double(sine)*deltaX+double(cosine)*deltaY);
    const float grazeHalfX=float(double(context.halfWidth)*16.0),grazeHalfY=float(double(context.halfHeight)*16.0);
    if(float(double(horizontal)-grazeHalfX)>=helperLength||float(double(vertical)-grazeHalfY)>double(helperWidth)*.5
        ||float(double(horizontal)+grazeHalfX)<0||float(double(vertical)+grazeHalfY)<= -double(helperWidth)*.5)return CollisionKind::Miss;
    if(float(double(horizontal)-context.halfWidth)>helperLength||float(double(vertical)-context.halfHeight)>double(helperWidth)*.5
        ||float(double(horizontal)+context.halfWidth)<0||float(double(vertical)+context.halfHeight)<= -double(helperWidth)*.5)return CollisionKind::Graze;
    if((context.control.present&&context.control.counter5a1c!=0)||context.gate.stateF78==2||context.gate.stateF78==3
        ||context.gate.stateF78==4||(context.gate.flags438ec&2u)||context.gate.counter438dc>0)return CollisionKind::Miss;
    return CollisionKind::HitCallRequired;
}
bool LineHost::validConfiguration(const LineConfiguration& value){
    return bounded(value.initialLength)&&bounded(value.maximumLength)&&bounded(value.travelLimit)&&bounded(value.width)
        &&value.initialLength>=0&&value.maximumLength>0&&value.initialLength<=value.maximumLength
        &&value.travelLimit>=value.maximumLength&&value.width>0;
}
bool LineHost::configure(const void* bytes,std::size_t size,CollisionOwner collision,SoundOwner sound,
                         RandomOwner gameRandom,std::string& error){
    error.clear();
    if(executing_||registry_||!lines_.empty()||!bytes||size==0||size>8u*1024u*1024u||!collision||!sound||!gameRandom){
        error="laser configure requires fresh actual ANM/collision/sound/random owners";return false;
    }
    auto candidate=std::make_unique<anm::Registry>();
    if(!candidate->loadBank(0,static_cast<const std::uint8_t*>(bytes),size,error))return false;
    const auto bank=candidate->bank(0);
    if(!bank||bank->scripts.size()<=227||bank->sprites.empty()){
        error="laser ANM lacks original type20/circle/tip scripts";return false;
    }
    auto effects=std::make_unique<cancel::LaserWholeLineEffectsOwner>();
    if(!effects->configure(bytes,size,gameRandom,error))return false;
    std::vector<Line> ownedSlots;ownedSlots.reserve(Capacity);
    registry_=std::move(candidate);collision_=std::move(collision);sound_=std::move(sound);random_=std::move(gameRandom);
    lines_=std::move(ownedSlots);
    wholeEffects_=std::move(effects);
    failure_.clear();return true;
}
void LineHost::reset(){
    if(executing_)return;
    lines_.clear();registry_.reset();wholeEffects_.reset();collision_={};sound_={};random_={};regionEffect_={};failure_.clear();serial_=0x10000;
    selector3Begin_={};selector3End_={};selector3Input_={};selector3Contact_={};
    if(++epoch_==0)++epoch_;
}
anm::Runtime LineHost::runtime(Line& line,bool remapped){
    anm::Runtime services;
    services.gameRandom32=random_;
    services.unsupported=[&line](int,int,int,const char* reason){if(line.failure.empty())line.failure=reason;};
    services.remapSprite=[this,&line,remapped](int identifier){
        const auto sprite=remapped?line.profile.remap(line.state.color,identifier):std::optional<int>(identifier);
        const auto bank=registry_->bank(0);
        if(!sprite||*sprite<0||!bank||std::size_t(*sprite)>=bank->sprites.size()){
            line.failure="laser actual profile/sprite callback is not owned";return -1;
        }
        return *sprite;
    };
    services.spawnChild=[&line](const anm::ChildRequest&){line.failure="laser ANM requested an unproved child owner";};
    return services;
}
bool LineHost::check(const Line& line,std::string& error) const{
    if(!registry_||line.state.epoch!=epoch_){error="laser resource epoch is stale";return false;}
    if(!line.failure.empty()){error=line.failure;return false;}
    for(const auto& visual:line.visuals)if(visual.faulted()){
        const auto diagnostic=visual.diagnostics();error="laser ANM: "+diagnostic.reason;return false;
    }
    return true;
}
bool LineHost::tickVisual(Line& line,std::size_t visual,std::string& error){
    auto services=runtime(line,visual==0);
    line.visuals[visual].tick(services);
    return check(line,error);
}
bool LineHost::recordFailure(std::string& error,const char* fallback){
    if(failure_.empty())failure_=error.empty()?fallback:error;
    error=failure_;return false;
}
bool LineHost::bindVisuals(Line& line,std::string& error){
    const std::array<int,3> scripts{line.profile.script,0x38+line.state.color,0xdd+line.state.color};
    for(std::size_t index=0;index<scripts.size();++index){
        auto services=runtime(line,index==0);
        const bool bound=line.visuals[index].bind(*registry_,0,scripts[index],services);
        if(!check(line,error)||!bound)return recordFailure(error,"laser ANM birth binding failed");
        if(index!=2){
            line.visuals[index].interrupt(2);
            if(!tickVisual(line,index,error))return recordFailure(error,"laser ANM birth interrupt failed");
        }
        auto& pose=line.visuals[index].control();
        pose.blend=1;pose.flags=(pose.flags&~0xe0u)|0x20u;pose.mode=1;
        pose.flags=(pose.flags&~0x1f000000u)|0x1000000u;
        if(index==0){pose.anchorX=0;pose.anchorY=2;pose.flags=(pose.flags&~0xf00000u)|0x800000u;}
    }
    return true;
}
bool LineHost::appendRemainder(const Line& parent,const cancel::CircleRemainderPlan& plan,std::string& error){
    if(lines_.size()>=Capacity||lines_.capacity()<Capacity||serial_>=0x0fffffffu||parent.state.epoch!=epoch_
        ||parent.state.ownerEntity==0||!bounded(plan.x)||!bounded(plan.y)||plan.z!=0
        ||!std::isfinite(plan.length)||plan.length<16||plan.length>160){
        error="native circle remainder allocation identity/capacity/domain rejected";return recordFailure(error,"circle remainder allocation rejected");
    }
    Line candidate;
    const auto nextSerial=++serial_;
    candidate.descriptor=parent.descriptor;
    candidate.descriptor.source={plan.x,plan.y,plan.z};
    if(!parent.acceleration)candidate.descriptor.angle=parent.state.angle;
    candidate.descriptor.configuration.initialLength=plan.length;
    candidate.descriptor.configuration.maximumLength=plan.length;
    const auto& descriptor=candidate.descriptor;
    const float childAngle=parent.acceleration?normalized(descriptor.angle):parent.state.angle;
    std::optional<bullet::ExtensionRaw> acceleration;
    if(!readLaserAcceleration(descriptor.emitter,acceleration,error))return recordFailure(error,"remainder acceleration descriptor rejected");
    if(bool(acceleration)!=bool(parent.acceleration)){error="remainder acceleration owner does not match the copied descriptor";return recordFailure(error,"remainder acceleration identity rejected");}
    if(acceleration){
        candidate.acceleration=bullet::initializeExtension4(*acceleration,childAngle,descriptor.source,descriptor.source);
        if(!candidate.acceleration){error="original remainder acceleration parameters rejected";return recordFailure(error,"remainder acceleration initialization failed");}
    }
    candidate.state={nextSerial,epoch_,parent.state.ownerEntity,0,descriptor.source,
        {float(std::cos(double(childAngle))*descriptor.speed),
         float(std::sin(double(childAngle))*descriptor.speed),0},
        childAngle,plan.length,descriptor.configuration.width,descriptor.speed,0,plan.length,
        descriptor.configuration.travelLimit,descriptor.emitter.type,descriptor.emitter.color,
        descriptor.emitter.shotSound,descriptor.emitter.transformSound};
    candidate.profile=parent.profile;
    const auto geometryReady=candidate.state;
    candidate.state.source={0,0,0};
    candidate.state.velocity={0,0,0};
    candidate.state.angle=0;candidate.state.length=0;candidate.state.width=0;candidate.state.speed=0;
    lines_.push_back(std::move(candidate));
    auto& owned=lines_.back();
    if(!bindVisuals(owned,error))return false;
    if(owned.state.shotSound>=0){
        if(!sound_(owned.state.shotSound,0.f,error)||!error.empty()||!failure_.empty())
            return recordFailure(error,"native remainder sound owner refused after committed membership/VMs");
    }
    owned.state=geometryReady;
    return true;
}
bool LineHost::birth(std::uint32_t ownerEntity,const LineDescriptor& descriptor,const bullet::Profile& profile,std::string& error){
    error.clear();
    if(!failure_.empty()){error=failure_;return false;}
    if(executing_||!registry_||!collision_||!sound_||!random_||ownerEntity==0){
        error="laser birth requires the current typed manager and actual service owners";return false;
    }
    const auto& emitter=descriptor.emitter;
    if(!validConfiguration(descriptor.configuration)||!bounded(descriptor.source.x)||!bounded(descriptor.source.y)
        ||descriptor.source.z!=0||!bounded(descriptor.angle)||std::abs(descriptor.angle)>2048
        ||!bounded(descriptor.speed)||descriptor.speed<=0||profile.type!=20||profile.script!=74||profile.secondaryScript!=0
        ||profile.tail.hitboxSize!=7||profile.tail.auxiliarySize!=22||profile.tail.value!=2||profile.tail.freezePolicy!=0
        ||profile.remap(6,0)!=390||profile.cancellationScript(6)!=381
        ||emitter.type!=20||emitter.color!=6||descriptor.configuration.width!=16
        ||descriptor.configuration.initialLength!=0
        ||(descriptor.configuration.maximumLength!=128&&descriptor.configuration.maximumLength!=160)
        ||descriptor.configuration.travelLimit!=640){
        error="laser type0 requires original A1 type20/74 geometry and finite zero-depth domain";return false;
    }
    std::optional<bullet::ExtensionRaw> acceleration;
    if(!readLaserAcceleration(emitter,acceleration,error))return false;
    for(const int script:{profile.script,0x38+emitter.color,0xdd+emitter.color}){
        const auto bank=registry_->bank(0);
        if(script<0||!bank||std::size_t(script)>=bank->scripts.size()){
            error="laser script binding is outside original resource";return false;
        }
        for(const auto& command:bank->scripts[std::size_t(script)].commands){
            if(requiresChildOwner(command.op)){error="laser ANM child owner is unproved";return false;}
        }
    }
    if(lines_.size()>=Capacity){error="original laser manager256 object capacity reached";return false;}
    if(serial_>=0x0fffffffu){error="laser serial/draw identity budget";return false;}
    ExecutionGuard guard(executing_);
    const char* operation="laser ANM birth owner";
    try{
        Line candidate;
        candidate.state={serial_+1,epoch_,ownerEntity,0,descriptor.source,
            {float(std::cos(double(normalized(descriptor.angle)))*descriptor.speed),
             float(std::sin(double(normalized(descriptor.angle)))*descriptor.speed),0},
            normalized(descriptor.angle),descriptor.configuration.initialLength,descriptor.configuration.width,
            descriptor.speed,0,descriptor.configuration.maximumLength,descriptor.configuration.travelLimit,
            emitter.type,emitter.color,emitter.shotSound,emitter.transformSound};
        candidate.profile=profile;
        candidate.descriptor=descriptor;
        if(acceleration){
            candidate.acceleration=bullet::initializeExtension4(*acceleration,candidate.state.angle,candidate.state.source,candidate.state.source);
            if(!candidate.acceleration){error="original laser acceleration parameters rejected";return false;}
        }
        if(!bindVisuals(candidate,error))return false;
        operation="laser shot sound owner";
        if(candidate.state.shotSound>=0&&!sound_(candidate.state.shotSound,candidate.state.source.x,error))
            return recordFailure(error,"laser shot sound owner refused without a diagnostic");
        operation="laser manager birth commit";
        const auto committedSerial=candidate.state.serial;
        lines_.push_back(std::move(candidate));serial_=committedSerial;return true;
    }catch(const std::exception& problem){
        error=std::string(operation)+" threw: "+problem.what()+"; external owner mutations are retained";
        return recordFailure(error,"laser birth owner exception");
    }catch(...){
        error=std::string(operation)+" threw a nonstandard exception; external owner mutations are retained";
        return recordFailure(error,"laser birth owner exception");
    }
}
bool LineHost::advancePhysical(LineSnapshot& state){
    if(state.maximumLength>state.length)state.length=std::min(float(double(state.length)+double(state.speed)),state.maximumLength);
    else{
        state.travel=float(double(state.travel)+double(state.speed));
        state.source.x=float(double(state.source.x)+state.velocity.x);
        state.source.y=float(double(state.source.y)+state.velocity.y);
        state.source.z=float(double(state.source.z)+state.velocity.z);
        if(double(state.length)+state.travel>=state.travelLimit){
            state.length=float(double(state.travelLimit)-state.travel);state.maximumLength=state.length;
            if(state.length<=0)return false;
        }
    }
    return true;
}
bool LineHost::tick(std::string& error){
    error.clear();if(!registry_||executing_||!failure_.empty()){error=failure_.empty()?"laser tick has no healthy owner":failure_;return false;}
    ExecutionGuard guard(executing_);
    const char* operation="laser physical update owner";
    try{
        if(selector3Begin_&&!selector3Begin_(error))return recordFailure(error,"selector3 actual manager contact prefix rejected");
        for(auto current=lines_.begin();current!=lines_.end();){
            const bool hadNext=current+1!=lines_.end();
            if(current->selector3State){
                auto& state=*current->selector3State;
                if(state.retirement){if(++state.retirement>=2){current=lines_.erase(current);continue;}}
                if(!selector3Input_||!selector3Contact_){error="selector3 actual input/contact owners are unavailable";return recordFailure(error,"selector3 service owner lost");}
                const auto input=selector3Input_(error);selector3::StepResult step;
                if(!input||!error.empty()||!selector3::advance(state,*input,step,error))return recordFailure(error,"selector3 actual column update rejected");
                if(step.playerColumns&&!selector3Contact_(step.playerColumns,error))return recordFailure(error,"selector3 actual player contact tail rejected");
                current->state.age=std::uint32_t(state.age);current->state.retirement7c=std::uint8_t(state.retirement);
                if(!hadNext)break;++current;continue;
            }
            if(!check(*current,error))return recordFailure(error,"laser update resource check failed");
            if(current->state.nativeState==1){
                if(!validateDeferred(std::size_t(current-lines_.begin()),error))return recordFailure(error,"laser deferred membership is invalid");
                current=lines_.erase(current);continue;
            }
            if(current->state.retirement7c!=0){
                if(++current->state.retirement7c>=2){current=lines_.erase(current);continue;}
            }
            auto next=current->state;
            if(current->acceleration&&current->acceleration->active){
                const auto step=stepLaserAcceleration(*current->acceleration,next.speed,next.angle,next.velocity);
                if(!step){error="original laser acceleration step rejected";return recordFailure(error,"laser extension4 failed");}
                current->acceleration=step->next;next.speed=step->speed;next.angle=step->angle;next.velocity=step->velocity;
            }
            if(!advancePhysical(next)){current=lines_.erase(current);continue;}
            if(next.boundsGrace>0)--next.boundsGrace;
            else{
                const float tipX=float(double(next.source.x)+float(std::cos(double(next.angle))*next.length));
                const float tipY=float(double(next.source.y)+float(std::sin(double(next.angle))*next.length));
                const auto outside=[&](float horizontal,float vertical){
                    return double(horizontal)+next.width<=-192.0||double(horizontal)-next.width>=192.0
                        ||double(vertical)+next.width<=0.0||double(vertical)-next.width>=448.0;
                };
                const bool outsideSource=outside(next.source.x,next.source.y),outsideTip=outside(tipX,tipY);
                if(outsideSource&&outsideTip){current=lines_.erase(current);continue;}
            }
            operation="native laser collision 0x43c660 owner";
            if(next.length>=16&&next.width>=3&&!collision_(next,error))
                return recordFailure(error,"native laser collision 0x43c660 owner refused without a diagnostic");
            current->state=next;
            auto& pose=current->visuals[0].control();
            const auto bank=registry_->bank(0);
            if(pose.sprite<0||std::size_t(pose.sprite)>=bank->sprites.size()){
                error="laser primary sprite resource unavailable";return recordFailure(error,"laser sprite owner failed");
            }
            const auto sprite=bank->sprites[std::size_t(pose.sprite)];
            if(sprite.width<=0||sprite.height<=0){
                error="laser primary sprite dimensions are invalid";return recordFailure(error,"laser sprite owner failed");
            }
            pose.sx=float(double(next.width)/sprite.width);pose.sy=float(double(next.length)/sprite.height);pose.flags|=8;
            operation="laser ANM update owner";
            if(!tickVisual(*current,0,error)||(next.travel!=0&&!tickVisual(*current,1,error))||!tickVisual(*current,2,error))
                return recordFailure(error,"laser ANM update failed");
            ++current->state.age;
            if(!hadNext)break;
            ++current;
        }
        if(selector3End_&&!selector3End_(error))return recordFailure(error,"selector3 actual manager contact effect rejected");
        return true;
    }catch(const std::exception& problem){
        error=std::string(operation)+" threw: "+problem.what()+"; external owner mutations are retained";
        return recordFailure(error,"laser update owner exception");
    }catch(...){
        error=std::string(operation)+" threw a nonstandard exception; external owner mutations are retained";
        return recordFailure(error,"laser update owner exception");
    }
}
std::optional<std::vector<LineDraw>> LineHost::draws(std::string& error) const{
    error.clear();std::vector<LineDraw> result;
    if(!registry_){error="laser draw registry is unavailable";return std::nullopt;}
    const auto bank=registry_->bank(0);
    for(const auto& line:lines_){
        if(line.selector3State)continue;
        if(!check(line,error))return std::nullopt;
        if(line.state.nativeState==1)continue;
        for(std::size_t index=0;index<line.visuals.size();++index){
            if(index==1&&line.state.travel==0)continue;
            auto pose=line.visuals[index].state();
            if(!pose.visible||!(pose.flags&2u)||pose.deleteRequested)continue;
            if(pose.sprite<0||std::size_t(pose.sprite)>=bank->sprites.size()){
                error="laser draw sprite identity is stale";return std::nullopt;
            }
            pose.engineX=line.state.source.x;pose.engineY=line.state.source.y;pose.engineZ=line.state.source.z;
            if(index==0)pose.rotation=normalized(float(double(line.state.angle)+1.5707963705062866));
            if(index==2){
                pose.engineX=float(double(pose.engineX)+float(std::cos(double(line.state.angle))*line.state.length));
                pose.engineY=float(double(pose.engineY)+float(std::sin(double(line.state.angle))*line.state.length));
            }
            result.push_back({0x40000000u+line.state.serial*4u+std::uint32_t(index),pose,bank->sprites[std::size_t(pose.sprite)]});
        }
    }
    return result;
}
std::vector<LineSnapshot> LineHost::snapshots() const{
    std::vector<LineSnapshot> result;for(const auto& line:lines_)result.push_back(line.state);return result;
}
std::uint32_t LineHost::actualCount() const{return std::uint32_t(lines_.size());}
bool LineHost::healthy() const{return registry_&&!executing_&&failure_.empty();}
bool LineHost::validateDeferred(std::size_t index,std::string& error) const{
    if(index>=lines_.size()||epoch_>=0x10000u){error="laser deferred membership epoch/index domain";return false;}
    const auto& line=lines_[index].state;
    const auto previous=index==0?epoch_:lines_[index-1].state.serial;
    const auto next=index+1==lines_.size()?0u:lines_[index+1].state.serial;
    const cancel::LineMembership membership{line.serial,lines_.front().state.serial,lines_.back().state.serial,
        epoch_,previous,next,line.serial,next?line.serial:0u,std::uint32_t(lines_.size())};
    const auto plan=cancel::prepareDeferredLineUnlink(0x4a2c1c,line.nativeState,membership);
    if(!plan||plan->count!=lines_.size()-1){error="laser deferred original state1 unlink plan rejected actual membership";return false;}
    return true;
}
bool LineHost::setRegionEffectOwner(RegionEffectOwner owner,std::string& error){
    error.clear();
    if(!healthy()||!lines_.empty()||regionEffect_||!owner){error="laser region effects require fresh actual service ownership";return false;}
    regionEffect_=std::move(owner);return true;
}
bool LineHost::cancelRegion(LineSnapshot& line,bullet::Position center,float width,float height,float angle,
                          bool honorGuard,std::string& error){
    error.clear();
    const auto live=std::find_if(lines_.begin(),lines_.end(),[&](const Line& owned){return owned.state.serial==line.serial&&owned.state.epoch==line.epoch;});
    if(!executing_||!failure_.empty()||!regionEffect_||live==lines_.end()||line.nativeState!=2
        ||line.type!=20||line.color!=6||line.width!=16||line.length<16||line.length>160
        ||!bounded(line.length)||!bounded(line.angle)||!bounded(center.x)||!bounded(center.y)
        ||center.z!=0||line.source.z!=0||!bounded(width)||!bounded(height)||width<=0||height<=0||angle!=0){
        error="native laser full-region 0x42c180 requires current owned type20/color6 axis-aligned geometry";return false;
    }
    if(honorGuard&&line.guard5b4)return true;
    const float directionX=float(std::cos(double(line.angle))*8.),directionY=float(std::sin(double(line.angle))*8.);
    const float stepX=float(double(directionX)*2.),stepY=float(double(directionY)*2.);
    const float halfWidth=float(double(width)*.5),halfHeight=float(double(height)*.5);
    float localX=float(double(float(double(line.source.x)-center.x))+directionX);
    float localY=float(double(float(double(line.source.y)-center.y))+directionY);
    bullet::Position position{float(double(line.source.x)+directionX),float(double(line.source.y)+directionY),0};
    std::vector<bullet::Position> requests;
    std::array<bool,10> coveredSamples{};
    std::uint32_t sampleCount=0;
    float distance=8;
    do{
        if(sampleCount>=coveredSamples.size()){error="native laser region sample budget";return false;}
        const bool covered=localX> -halfWidth&&localX<halfWidth&&localY> -halfHeight&&localY<halfHeight;
        coveredSamples[sampleCount++]=covered;
        if(covered)requests.push_back(position);
        position.x=float(double(position.x)+stepX);position.y=float(double(position.y)+stepY);
        localX=float(double(localX)+stepX);localY=float(double(localY)+stepY);
        distance=float(double(distance)+16.);
    }while(double(line.length)>=double(distance)+8.);
    if(requests.empty())return true;
    std::uint32_t removedPrefix=0,remainingPrefix=0;
    while(removedPrefix<sampleCount&&coveredSamples[removedPrefix])++removedPrefix;
    while(remainingPrefix<sampleCount&&!coveredSamples[remainingPrefix])++remainingPrefix;
    bool requiresRemainder=false;
    if(removedPrefix&&removedPrefix<sampleCount){
        for(auto sample=removedPrefix;sample<sampleCount;++sample)if(coveredSamples[sample])requiresRemainder=true;
    }else if(!removedPrefix){
        for(auto sample=remainingPrefix;sample<sampleCount;++sample)if(!coveredSamples[sample])requiresRemainder=true;
    }
    std::optional<cancel::CircleRemainderPlan> remainder;
    if(requiresRemainder){
        if(removedPrefix||!remainingPrefix||!check(*live,error)){
            if(error.empty())error="native rectangle remainder requires contiguous original sample ownership";
            return false;
        }
        auto firstRemaining=remainingPrefix;
        while(firstRemaining<sampleCount&&coveredSamples[firstRemaining])++firstRemaining;
        for(auto sample=firstRemaining;sample<sampleCount;++sample)if(coveredSamples[sample]){
            error="native rectangle has an unsupported noncontiguous remainder";return false;
        }
        const auto remainingSamples=sampleCount-firstRemaining;
        const float remainderLength=float(remainingSamples*16u);
        if(remainderLength>24){
            if(lines_.size()>=Capacity||lines_.capacity()<Capacity||serial_>=0x0fffffffu
                ||live->state.ownerEntity==0||live->profile.type!=20||live->profile.script!=74
                ||live->profile.remap(6,0)!=390||live->profile.cancellationScript(6)!=381){
                error="native rectangle fresh child requires actual manager/profile/resource capacity";return false;
            }
            remainder=cancel::CircleRemainderPlan{
                float(double(line.source.x)+float(double(stepX)*firstRemaining)),
                float(double(line.source.y)+float(double(stepY)*firstRemaining)),0,remainderLength,
                firstRemaining,remainingSamples};
        }
    }
    for(const auto request:requests)if(!regionEffect_(request,error))return recordFailure(error,"native laser full-region effect381 owner rejected");
    if(requests.size()==sampleCount)line.retirement7c=1;
    else if(removedPrefix){
        const float removed=float(removedPrefix*16u);
        line.source.x=float(double(line.source.x)+float(double(stepX)*removedPrefix));
        line.source.y=float(double(line.source.y)+float(double(stepY)*removedPrefix));
        line.length=float(double(line.length)-removed);line.maximumLength=line.length;line.travel=removed;
        if(line.length<=24)line.retirement7c=1;
    }else{
        const float remaining=float(remainingPrefix*16u);
        line.maximumLength=float(double(line.maximumLength)-(double(line.length)-remaining));line.length=remaining;
        if(line.length<24)line.retirement7c=1;
    }
    if(remainder&&!appendRemainder(*live,*remainder,error))return recordFailure(error,"native rectangle remainder construction failed");
    return true;
}
bool LineHost::cancelLive(std::string& error,bool honorGuard,bool clearGuard,bool skipRetired){
    error.clear();
    if(!healthy()||!wholeEffects_){error=failure_.empty()?"laser whole cancellation requires current actual FX owner":failure_;return false;}
    ExecutionGuard guard(executing_);
    try{
        for(auto& line:lines_){
            if(line.selector3State){if(!honorGuard){line.selector3State->retirement=1;line.state.retirement7c=1;}continue;}
            if(!check(line,error))return recordFailure(error,"laser whole cancellation resource epoch is stale");
            if(line.state.ownerEntity==0||line.state.serial<=0x10000u||line.profile.type!=20||line.profile.script!=74
                ||line.profile.secondaryScript!=0||line.profile.tail.hitboxSize!=7||line.profile.tail.auxiliarySize!=22
                ||line.profile.tail.value!=2||line.profile.tail.freezePolicy!=0
                ||line.profile.remap(6,0)!=390||line.profile.cancellationScript(6)!=381){
                error="laser whole cancellation requires actual current owner and typed288B profile20 provenance";
                return recordFailure(error,"laser whole cancellation typed owner rejected");
            }
            if(skipRetired&&line.state.nativeState==1)continue;
            if(line.state.nativeState==1){
                error="repeated whole cancellation before deferred unlink requires a separately proven original state1 caller";
                return recordFailure(error,"laser repeated whole cancellation is unsupported");
            }
            cancel::OwnedWholeLine owned{0x4a2c1c,line.state.nativeState,line.state.guard5b4,line.state.type,line.state.color,
                line.state.source.x,line.state.source.y,line.state.source.z,line.state.angle,line.state.length};
            if(clearGuard){owned.guard5b4=0;line.state.guard5b4=0;line.state.boundsGrace=0;}
            const auto completed=wholeEffects_->cancel(owned,honorGuard,error);
            if(!completed)return recordFailure(error,"native whole laser color6 FX owner rejected");
            line.state.nativeState=owned.state;
        }
        return true;
    }catch(const std::exception& problem){
        error=std::string("laser whole cancellation owner threw: ")+problem.what()+"; external owner mutations are retained";
        return recordFailure(error,"laser whole cancellation exception");
    }catch(...){
        error="laser whole cancellation owner threw; external owner mutations are retained";
        return recordFailure(error,"laser whole cancellation exception");
    }
}
bool LineHost::cancelEntryRadius(bullet::Position center,float radius,bool honorGuard,std::string& error){
    error.clear();
    if(!healthy()||!wholeEffects_){error=failure_.empty()?"entry circle cancellation requires healthy actual manager/FX owner":failure_;return false;}
    if(!bounded(center.x)||!bounded(center.y)||center.z!=0||!std::isfinite(radius)||radius<0||radius>1024){
        error="entry circle cancellation has invalid original center/radius domain";return false;
    }
    ExecutionGuard guard(executing_);
    try{
        for(std::size_t index=0,next=0;index<lines_.size();index=next){
            next=index+1<lines_.size()?index+1:Capacity;
            auto& line=lines_[index];
            if(line.selector3State)continue;
            if(!check(line,error))return recordFailure(error,"entry circle cancellation resource epoch rejected");
            if(line.state.nativeState==1)continue;
            if(line.state.ownerEntity==0||line.profile.type!=20||line.profile.script!=74||line.profile.remap(6,0)!=390
                ||line.profile.cancellationScript(6)!=381){
                error="entry circle cancellation requires original typed profile20 current owner";return recordFailure(error,"circle owner rejected");
            }
            const cancel::OwnedWholeLine owned{0x4a2c1c,line.state.nativeState,line.state.guard5b4,line.state.type,line.state.color,
                line.state.source.x,line.state.source.y,line.state.source.z,line.state.angle,line.state.length};
            const auto plan=cancel::planCircleCancellation(owned,center.x,center.y,center.z,radius,honorGuard,error);
            if(!plan)return recordFailure(error,"native42cd90 circle domain rejected");
            if(plan->requiresRemainder&&((!plan->remainder&&!plan->remainderBelowThreshold)
                ||(plan->remainder&&(lines_.size()>=Capacity||lines_.capacity()<Capacity||serial_>=0x0fffffffu))
                ||line.state.maximumLength!=line.state.length)){
                error="entry partial circle requires original42cd90 clipping/remainder owner0x42d28f ->0x47296c; not whole cancellation";
                return recordFailure(error,"entry partial circle owner unavailable");
            }
            if(!wholeEffects_->emitOwnedSamples(plan->effects,error))return recordFailure(error,"native42cd90 colored16 FX failed");
            if(plan->full)line.state.retirement7c=1;
            else if(plan->removedPrefix){
                const float removed=float(plan->removedPrefix*16u);
                line.state.source.x=float(double(line.state.source.x)+float(double(plan->stepX)*plan->removedPrefix));
                line.state.source.y=float(double(line.state.source.y)+float(double(plan->stepY)*plan->removedPrefix));
                line.state.length=float(double(line.state.length)-removed);
                line.state.maximumLength=line.state.length;line.state.travel=removed;
                if(line.state.length<=24)line.state.retirement7c=1;
            }else if(plan->remainingPrefix){
                const float remaining=float(plan->remainingPrefix*16u);
                line.state.maximumLength=float(double(line.state.maximumLength)-(double(line.state.length)-remaining));
                line.state.length=remaining;
                if(line.state.length<24)line.state.retirement7c=1;
            }
            if(plan->requiresRemainder&&plan->remainder&&!appendRemainder(line,*plan->remainder,error))
                return recordFailure(error,"native circle remainder construction failed");
        }
        return true;
    }catch(const std::exception& problem){
        error=std::string("entry circle cancellation owner threw: ")+problem.what()+"; external mutations retained";
        return recordFailure(error,"entry circle exception");
    }catch(...){error="entry circle owner threw; external mutations retained";return recordFailure(error,"entry circle exception");}
}
bool LineHost::tickEffects(std::string& error){
    error.clear();if(!healthy()||!wholeEffects_){error=failure_.empty()?"laser actual FX phase owner unavailable":failure_;return false;}
    ExecutionGuard guard(executing_);
    if(!wholeEffects_->tickPrimary(error))return recordFailure(error,"laser actual FX phase rejected");
    return true;
}
std::vector<EffectSnapshot> LineHost::effectSnapshots() const{
    std::vector<EffectSnapshot> result;if(!wholeEffects_)return result;
    for(const auto& [identifier,node]:wholeEffects_->scene().nodes())if(node->alive)
        result.push_back({identifier,node->owner,node->parent,node->vm.state()});
    std::sort(result.begin(),result.end(),[](const EffectSnapshot& first,const EffectSnapshot& second){return first.id<second.id;});
    return result;
}
std::optional<std::vector<LineDraw>> LineHost::effectDraws(std::string& error) const{
    error.clear();std::vector<LineDraw> result;if(!wholeEffects_)return result;
    const auto& scene=wholeEffects_->scene();
    const auto compose=[&](auto&& self,const anm::Node& node,anm::State& pose,std::size_t depth)->bool{
        if(depth>=64||node.vm.faulted())return false;
        pose=node.vm.state();pose.x+=pose.engineX+pose.offsetX;pose.y+=pose.engineY+pose.offsetY;pose.z+=pose.engineZ+pose.offsetZ;
        if(node.parent){
            const auto parent=scene.nodes().find(node.parent);anm::State inherited;
            if(parent==scene.nodes().end()||!parent->second->alive||parent->second->owner!=node.owner||!self(self,*parent->second,inherited,depth+1))return false;
            pose.x+=inherited.x;pose.y+=inherited.y;pose.z+=inherited.z;
            pose.sx*=inherited.sx;pose.sy*=inherited.sy;if(pose.mode==1)pose.rotation+=inherited.rotation;
        }
        return bounded(pose.x)&&bounded(pose.y)&&bounded(pose.z)&&bounded(pose.sx)&&bounded(pose.sy)&&bounded(pose.rotation);
    };
    for(const auto* node:scene.chain(anm::Membership::Primary)){
        auto pose=node->vm.state();if(!node->alive||!pose.visible||!(pose.flags&2u)||pose.deleteRequested)continue;
        const auto bank=scene.registry.bank(pose.spriteBank);
        if(!bank||pose.sprite<0||std::size_t(pose.sprite)>=bank->sprites.size()||!compose(compose,*node,pose,0)){
            error="laser actual color FX draw resource/parent geometry is stale";return std::nullopt;
        }
        result.push_back({0x60000000u+node->id,pose,bank->sprites[std::size_t(pose.sprite)]});
    }
    return result;
}

bool LineHost::setSelector3Services(Selector3Begin begin,Selector3Input input,Selector3Contact contact,Selector3Begin end,std::string& error){
    error.clear();
    if(!healthy()||!lines_.empty()||selector3Begin_||!begin||!input||!contact||!end){error="selector3 services require fresh actual manager owners";return false;}
    selector3Begin_=std::move(begin);selector3Input_=std::move(input);selector3Contact_=std::move(contact);selector3End_=std::move(end);return true;
}
bool LineHost::birthSelector3(std::uint32_t owner,const selector3::Descriptor& descriptor,std::string& error){
    error.clear();
    if(!healthy()||!selector3Begin_||!selector3Input_||!selector3Contact_||!selector3End_||!owner){error="selector3 birth requires current real column manager and services";return false;}
    if(lines_.size()>=Capacity||serial_>=0x0fffffffu){error="original shared laser256 capacity or monotonic identity exhausted";return false;}
    selector3::State state;if(!selector3::initialize(descriptor,state,error))return false;
    Line candidate;candidate.selector3State=state;candidate.state.serial=serial_+1;candidate.state.epoch=epoch_;candidate.state.ownerEntity=owner;candidate.state.nativeState=3;
    candidate.state.source={state.source.x,state.source.y,state.source.z};candidate.state.angle=state.angle;candidate.state.length=state.length;candidate.state.width=state.width;
    lines_.push_back(std::move(candidate));++serial_;return true;
}
bool LineHost::setSourceFromIdentifier(int identifier,PositionReader reader,std::string& error){
    error.clear();
    if(!healthy()||!reader){error="original604 requires current actual manager and both float resolvers";return false;}
    ExecutionGuard guard(executing_);
    auto found=lines_.end();
    if(identifier!=0)found=std::find_if(lines_.begin(),lines_.end(),[identifier](const Line& line){
        const auto actual=line.selector3State?std::uint32_t(line.selector3State->identifier):line.state.serial;
        return actual==std::uint32_t(identifier);
    });
    float horizontal=0,vertical=0;
    try{
        if(!reader(horizontal,vertical)||!error.empty()||!failure_.empty())return recordFailure(error,"original604 actual float resolvers rejected");
    }catch(const std::exception& problem){error=std::string("original604 float resolver threw: ")+problem.what();return recordFailure(error,"original604 resolver exception");}
    catch(...){error="original604 float resolver threw";return recordFailure(error,"original604 resolver exception");}
    if(!bounded(horizontal)||!bounded(vertical)){error="original604 source is outside the verified finite owner domain";return recordFailure(error,"original604 numeric domain");}
    if(found==lines_.end())return true;
    found->state.source={horizontal,vertical,0};
    if(found->selector3State)found->selector3State->source={horizontal,vertical,0};
    return true;
}
bool LineHost::setAngleFromIdentifier(int identifier,AngleReader reader,std::string& error){
    error.clear();
    if(!healthy()||!reader){error="original608 requires current actual manager and float resolver";return false;}
    ExecutionGuard guard(executing_);
    if(identifier==0)return true;
    const auto found=std::find_if(lines_.begin(),lines_.end(),[identifier](const Line& line){
        const auto actual=line.selector3State?std::uint32_t(line.selector3State->identifier):line.state.serial;
        return actual==std::uint32_t(identifier);
    });
    if(found==lines_.end())return true;
    float angle=0;
    try{
        if(!reader(angle)||!error.empty()||!failure_.empty())return recordFailure(error,"original608 actual float resolver rejected");
    }catch(const std::exception& problem){error=std::string("original608 float resolver threw: ")+problem.what();return recordFailure(error,"original608 resolver exception");}
    catch(...){error="original608 float resolver threw";return recordFailure(error,"original608 resolver exception");}
    if(!bounded(angle)){error="original608 angle is outside the verified finite owner domain";return recordFailure(error,"original608 numeric domain");}
    found->state.angle=angle;
    if(found->selector3State)found->selector3State->angle=angle;
    return true;
}
bool LineHost::setSelector3AngularVelocity(int identifier,float value,std::string& error){
    error.clear();if(!healthy()||!selector3::finite(value)){error="selector3 angular mutation requires actual finite live manager";return false;}
    for(auto& line:lines_)if(line.selector3State&&line.selector3State->identifier==identifier){line.selector3State->angularVelocity=value;break;}return true;
}
bool LineHost::setSelector3FollowBoss(int identifier,int value,std::string& error){
    error.clear();if(!healthy()){error="selector3 follow mutation requires actual current manager";return false;}
    for(auto& line:lines_)if(line.selector3State&&line.selector3State->identifier==identifier){line.selector3State->followBoss=(value&1)!=0;break;}return true;
}
std::vector<selector3::State> LineHost::selector3Snapshots() const{
    std::vector<selector3::State> result;for(const auto& line:lines_)if(line.selector3State)result.push_back(*line.selector3State);return result;
}
std::optional<std::vector<Selector3Draw>> LineHost::selector3Draws(std::string& error) const{
    error.clear();if(!healthy()){error="selector3 draw requires current healthy manager";return std::nullopt;}
    std::vector<Selector3Draw> result;
    for(const auto& line:lines_)if(line.selector3State){const auto quads=selector3::columnQuads(*line.selector3State,error);if(!quads)return std::nullopt;for(const auto& quad:*quads)result.push_back({line.state.serial,line.state.ownerEntity,quad});}
    return result;
}
}
