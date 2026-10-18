#include "EclVm.hpp"
#include "GuiState.hpp"
namespace th08 {
void EclTimelineExecutor::record(EclTimelineVm& vm,const EclTimelineInstruction& instruction)noexcept{
    u32 args[6]{};const u32 count=(instruction.size-8)/4;
    std::memcpy(args,reinterpret_cast<const u8*>(&instruction)+8,(count<6?count:6)*4);
    globals.record(i16(0x100+instruction.opcode),args,count,vm.timer.current);
}
bool EclTimelineExecutor::start(EclTimelineVm& vm,EclProgram& program,i32 timeline,u32 difficulty){
    vm=EclTimelineVm{};vm.program=&program;vm.timeline=timeline;vm.difficulty=difficulty;
    vm.instruction=program.timeline(timeline);vm.timer.set(0);
    if(!vm.instruction){vm.invalid=true;return false;}return true;
}
// Original EnemyManager timeline interpreter 0x42a8a0. Enemy creation,
// dialogue setup and power checks are explicit subsystem action boundaries.
bool EclTimelineExecutor::step(EclTimelineVm& vm){
    if(vm.invalid||!vm.instruction||!vm.program)return false;
    const auto begin=reinterpret_cast<std::uintptr_t>(vm.program->timeline(vm.timeline));
    const auto end=begin+vm.program->timeline_size(vm.timeline);
    u32 budget=0;
    while(++budget<4096){
        const auto address=reinterpret_cast<std::uintptr_t>(vm.instruction);
        if(address<begin||address+8>end){vm.invalid=true;return false;}
        const auto& ins=*vm.instruction;
        if(ins.time<0){vm.finished=true;break;}
        if(ins.time>vm.timer.current)break;
        if(ins.size<8||address+ins.size>end){vm.invalid=true;return false;}
        if(ins.time==vm.timer.current&&(ins.difficulties&globals.difficulty_mask)){
            const auto* args=reinterpret_cast<const u8*>(&ins)+8;
            const auto n=[&](u32 index){i32 value=0;if(12+index*4<=ins.size)std::memcpy(&value,args+index*4,4);else vm.invalid=true;return value;};
            const auto f=[&](u32 index){const i32 bits=n(index);float value;std::memcpy(&value,&bits,4);return value;};
            const auto boss=[&](i32 index){return index>=0&&index<8?globals.boss_slots[index]:nullptr;};
            bool wait=false;
            const u32 op=ins.opcode;
            switch(op){
            case 0:case 1:case 2:case 3:case 4:case 5:case 11:case 12:case 15:
                if(op==15||(!globals.gui_blocks_spawn&&!globals.stop_spawn)){
                    TimelineSpawn request;request.subroutine=i16(n(0));request.mirror=op==1||op==4||op==5||op==12;
                    if(op==2||op==4){const float low=f(1),high=f(2);request.position.x=(rng.range(Scalar::sub(high,low))+number(low)).to_float();request.position.y=f(3);request.life=n(4);request.item=i8(n(5));request.score=n(6);}
                    else if(op==3||op==5){request.position.x=rng.range(384).to_float();request.position.y=f(1);request.life=n(2);request.item=i8(n(3));request.score=n(4);}
                    else{request.position={f(1),f(2),0};request.life=n(3);request.item=i8(n(4));request.score=n(5);
                        if(op==11||op==12){request.multiple_items=true;request.item=-1;request.point_items=n(4);request.power_items=n(5);request.score=n(6);}}
                    if(!vm.invalid&&globals.timeline_actions)globals.timeline_actions->spawn(request);
                }
                break;
            case 6:{const i32 entry=n(0);if(globals.timeline_actions)globals.timeline_actions->message(entry);break;}
            case 7:
                // 0043587e permits ECL to resume for a message-script pulse
                // even while a portrait/textbox remains on screen.
                if(globals.gui&&globals.gui->implementation){const auto& d=globals.gui->implementation->dialogue;wait=d.message>=0&&!d.ignore_wait;}
                else wait=globals.dialogue_active;
                break;
            case 8:{const i32 index=n(0);const i16 interrupt=i16(n(1));if(auto* target=boss(index))target->pending_interrupt=interrupt;break;}
            case 9:{const i32 power=n(0);if(globals.timeline_actions)globals.timeline_actions->set_power(power);break;}
            case 10:{const auto* target=boss(n(0));wait=target&&(target->flags&1);break;}
            case 13:{const i32 signal=n(0);wait=true;for(auto& value:globals.timeline_signals)if(value==signal){value=-1;wait=false;}break;}
            case 14:{const i32 signal=n(0);for(auto& value:globals.timeline_signals)if(value<0)value=signal;break;}
            case 16:globals.stage_completion=1;break;
            default:break;
            }
            if(vm.invalid)return false;
            record(vm,ins);
            if(wait){vm.timer.decrement(1,timing);break;}
        }
        vm.instruction=reinterpret_cast<EclTimelineInstruction*>(address+ins.size);
    }
    if(budget>=4096){vm.invalid=true;return false;}
    vm.timer.tick(timing);return !vm.finished;
}
}
