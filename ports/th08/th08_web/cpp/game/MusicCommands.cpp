// Command stages and queue compaction recovered from TH08 SoundPlayer.
#include "MusicCommands.hpp"
#include "Arithmetic.hpp"
namespace th08 {
bool MusicCommands::enqueue(i32 opcode,i32 arg,const char* text){
    if(!text||std::strlen(text)>=256)return false;
    for(u32 i=0;i<31;++i)if(!commands[i].opcode){auto& command=commands[i];command.opcode=opcode;command.arg=arg;command.step=0;std::memcpy(command.text,text,std::strlen(text)+1);return true;}return false;
}
i32 MusicCommands::process(){
    if(!context.initialized)return 0;
    u32 cursor=0;
    for(;;){auto& command=commands[cursor];bool remove=false,repeat=false;
        switch(command.opcode){
        case 8:if(context.has_music)actions.set_volume(context.master_volume);remove=true;break;
        case 1:
            if(!context.preload||!command.step){if(context.preload)actions.stop_all();actions.preload(command.arg,command.text);remove=repeat=true;}
            else command.step=wrapping_add(command.step,1);
            break;
        case 2:
            if(context.preload&&command.arg>=0){
                if(!command.step){if(actions.load(command.arg)!=0)remove=true;}
                else if(command.step==2){if(context.has_music&&actions.reset()<0)remove=true;}
                else if(command.step==5){command.arg=context.total_length!=0;if(actions.fill(command.arg)<0)remove=true;}
                else if(command.step==7)actions.play();
                else if(command.step>=20)remove=true;
            }else if(!context.has_music)remove=true;
            else if(!command.step)actions.stop();
            else if(command.step==1){if(context.locked)return commands[0].opcode;actions.recreate_buffers();}
            else if(command.step==2){const char* path=command.arg>=0&&command.arg<16?filenames[command.arg]:command.text;actions.reopen(path);}
            else if(command.step==3){actions.reset();command.arg=context.total_length!=0;if(actions.fill(command.arg)<0)remove=true;}
            else if(command.step==4)actions.play();
            else if(command.step>=7)remove=true;
            if(!remove)command.step=wrapping_add(command.step,1);
            break;
        case 4:
            if(!context.has_music)remove=true;
            else if(!command.step)actions.stop();
            else if(command.step==1){if(!context.has_thread)remove=true;else actions.request_thread_stop();}
            else if(command.step==2){if(!actions.thread_stopped()){actions.request_thread_stop();command.step=wrapping_sub(command.step,1);}else context.has_thread=false;}
            else if(command.step==3){actions.close_music();context.has_thread=context.has_music=false;}
            else if(command.step==10)remove=true;
            if(!remove)command.step=wrapping_add(command.step,1);
            break;
        case 3:
            if(!context.has_music||command.step==1)remove=true;
            else{if(!command.step)actions.stop();command.step=wrapping_add(command.step,1);}
            break;
        case 5:actions.fade_out(Extended::from_int(command.arg).to_float());remove=true;break;
        case 6:case 7:
            if(context.wav){if(context.locked)return commands[0].opcode;if(context.has_music){if(command.opcode==6)actions.pause();else actions.unpause();}}
            remove=true;break;
        default:break;
        }
        if(!remove)break;
        // The original retains the cursor after shifting. A preload re-enters
        // the switch at the terminating slot, not at the new queue head.
        for(u32 i=0;i<31&&cursor<31;++i,++cursor){if(!commands[cursor].opcode)break;commands[cursor]=commands[cursor+1];}
        if(!repeat||cursor>=31)break;
    }
    return commands[0].opcode;
}
}
