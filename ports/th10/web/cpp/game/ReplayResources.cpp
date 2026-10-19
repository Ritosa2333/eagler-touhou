#include "ReplayResources.hpp"
namespace th10 {
// 0x428f60. Recording owns separate header/info/stage objects. Playback info
// points into the decompressed file, and its stage pointers live in readers.
i32 ReplayResources::start(i32 mode,const char* name){
    auto& env=environment;auto& services=*env.gameplay;auto& game=*services.game;replay.mode=mode;
    if(mode==0){
        *env.current=&replay;replay.clear_buffers(game.stage,services);replay.active_buffer=replay.add_buffer(game.stage,services);
        replay.header=static_cast<ReplayHeader*>(services.allocate(sizeof(ReplayHeader)));if(!replay.header)return -1;replay.header->initialize();
        replay.info=static_cast<ReplayInfo*>(services.allocate(sizeof(ReplayInfo)));if(!replay.info)return -1;replay.info->initialize();
        auto* snapshot=static_cast<ReplayStage*>(services.allocate(sizeof(ReplayStage)));replay.stages[game.stage]=snapshot;if(!snapshot)return -1;snapshot->initialize();
        replay.info->character=game.character;replay.info->shot_type=game.shot_type;replay.info->difficulty=game.difficulty;if(env.configuration)std::memcpy(replay.info->configuration,env.configuration,52);
        snapshot->stage=static_cast<std::int16_t>(game.stage);snapshot->seed=services.random->seed;services.random->calls=0;snapshot->flags=(snapshot->flags&~1u)|(*services.recording_mode&1);if(*services.recording_mode)snapshot->position={0,0};snapshot->capture_game(game);replay.info->score_units=game.score_units;
    }else if(mode==1){
        *env.current=&replay;if(env.load(replay,name))return -1;
        std::memcpy(env.configuration,replay.info->configuration,52);auto& reader=replay.readers[game.stage];reader.rewind();game.character=replay.info->character;game.shot_type=replay.info->shot_type;game.difficulty=replay.info->difficulty;reader.stage->restore_game(game,*services.random,services.rate);
    }else return mode==2&&env.load(replay,name)?-1:0;
    replay.update_entry=(*env.chain)->add(env.input_callback,&replay,11,false,false,*env.callbacks);
    replay.end_frame_entry=(*env.chain)->add(env.end_frame_callback,&replay,27,false,false,*env.callbacks);
    replay.draw_entry=(*env.chain)->add(env.draw_callback,&replay,5,true,false,*env.callbacks);
    replay.active_stage=mode==0?game.stage:-1;return 0;
}
// 0x4294a0. Member nodes are unlinked in reverse order after releasing the
// callback entries. Header/unpacked fields intentionally retain their values.
void ReplayResources::shutdown(){auto& env=environment;auto& services=*env.gameplay;services.release(replay.header);for(i32 i=0;i<8;i++)replay.clear_buffers(i,services);services.release(replay.info);replay.info=nullptr;for(auto*& stage:replay.stages){services.release(stage);stage=nullptr;}(*env.chain)->remove_locked(replay.update_entry,*env.callbacks);(*env.chain)->remove_locked(replay.end_frame_entry,*env.callbacks);(*env.chain)->remove_locked(replay.draw_entry,*env.callbacks);if(*env.current==&replay)*env.current=nullptr;for(i32 i=7;i>=0;i--){auto& node=replay.readers[i].node;if(node.next)node.next->previous=node.previous;if(node.previous)node.previous->next=node.next;node.next=node.previous=nullptr;}}
Replay* ReplayResources::create(i32 mode,const char* name,ReplayResourceEnvironment& env){auto* replay=static_cast<Replay*>(env.gameplay->allocate(sizeof(Replay)));if(!replay)return nullptr;replay->initialize();ReplayResources resources{*replay,env};if(resources.start(mode,name)){resources.shutdown();env.gameplay->release(replay);return nullptr;}return replay;}
}
