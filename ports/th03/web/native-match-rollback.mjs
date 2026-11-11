import {NativeRollback} from './native-rollback.mjs';
import {FireControl} from './fire-control.js';
import {unpackTouch} from './touch-input.js';
import {createVersusPause} from './native-versus-pause.mjs';

export function createRollbackMatch({seat,channel,engine,sample,options={},onFrame=()=>{},onConfirmedInput=()=>{},onError=()=>{},now=()=>performance.now()}){
  const fire=[new FireControl(),new FireControl()], pause=createVersusPause(), outputs=new Map(),proofs=new Map(),remoteProofs=new Map();let running=true,lastProgress=now(),confirmedInput=[0,0];
  const metadata=()=>({scene:engine._th03_classic_scene(),round:engine._th03_semantic_round?.()??-1,serial:engine._th03_classic_match_serial?.()??0,winner:engine._th03_classic_match_winner?.()??-1});
  const compare=frame=>{if(!proofs.has(frame)||!remoteProofs.has(frame))return;if(proofs.get(frame)!==remoteProofs.get(frame))throw Error(`双方游戏状态不同步（第 ${frame} 帧）`);proofs.delete(frame);remoteProofs.delete(frame);};
  const freeSlots=[7,6,5,4,3,2,1,0];let captures=0,restores=0;
  engine._th03_classic_defer_audio();
  const snapshot=()=>{const slot=freeSlots.pop();if(slot===undefined)throw Error('原生回滚历史已满');if(!engine._th03_classic_capture(slot)){freeSlots.push(slot);return null;}captures++;return {slot,fire:fire.map(item=>({...item.state})),pause:pause.capture()};};
  const release=s=>{freeSlots.push(s.slot);};
  const restore=s=>{if(!engine._th03_classic_restore(s.slot))throw Error('原生回滚状态恢复失败');restores++;fire.forEach((item,i)=>item.state={...s.fire[i]});pause.restore(s.pause);};
  const sync=new NativeRollback({seat,send:text=>channel.send(text),capture:snapshot,restore,release,onError,step(inputs,frame,speculative){
    const recordedInputs=[...inputs];
    if(speculative&&(pause.state.owner!==null||(engine._th03_classic_scene()===9&&((inputs[0]|inputs[1])&128))))return false;
    const menu=pause.advance(inputs,engine._th03_classic_scene(),engine._th03_classic_surrender_allowed());
    if(menu.paused){fire.forEach(f=>f.reset());outputs.set(frame,{paused:true,lanes:[0,0],inputs:recordedInputs,replaySamples:engine._th03_semantic_replay_samples(),...metadata()});lastProgress=now();return;}
    if(menu.resumed){fire.forEach(f=>f.reset());if(menu.result>1)engine._th03_classic_surrender(menu.seat,menu.result===3);inputs=[0,0];}
    inputs.forEach((value,seat)=>{const t=unpackTouch(value);engine._th03_classic_touch(seat,t.active,t.begin||false,t.unlimited,t.x,t.y);});
    const selecting=engine._th03_classic_scene()===3;
    const lanes=inputs.map((bits,index)=>(bits&31)|((selecting?fire[index].selection(!!(bits&512),!!(bits&32)):fire[index].sample(!!(bits&512),!!(bits&32),frame*1000/60,!(bits&1024)))?32:0));
    engine._th03_classic_assist_pair(options.focus!==false&&!!(inputs[0]&64),options.focus!==false&&!!(inputs[1]&64));
    const system=lanes[0]|((engine._th03_classic_scene()!==9&&(inputs[0]&128))?0x1000:0)|((inputs[0]&256)?0x2000:0);
    const charge=((inputs[0]&32)?1:0)|((inputs[1]&32)?2:0);
    engine._th03_classic_speculative(speculative?1:0);
    let result;try{result=engine._th03_classic_step_pair(lanes[0],lanes[1],system,charge);}finally{engine._th03_classic_speculative(0);}
    if(speculative&&(result!==0||engine._th03_classic_boundary()||engine._th03_classic_scene()!==9||engine._th03_classic_paused()))return false;
    if(result!==0)throw Error('游戏结束或同步失败：'+result);
    const ptr=engine._th03_classic_audio_commands(),count=engine._th03_classic_audio_command_count();
    let proof;
    if((frame+1)%120===0)proof=engine._th03_classic_scene()===9?engine._th03_semantic_state_hash()>>>0:0;
    outputs.set(frame,{commands:engine.HEAPU8.slice(ptr,ptr+count*4),count,proof,lanes,inputs:recordedInputs,replaySamples:engine._th03_semantic_replay_samples(),...metadata()});
    lastProgress=now();
  },confirm(frame){const output=outputs.get(frame);outputs.delete(frame);if(!output)throw Error('确认帧音频缺失');confirmedInput=output.lanes;let sound=new Int32Array(0);if(!output.paused){engine.HEAPU8.set(output.commands,engine._th03_classic_audio_commands());if(engine._th03_classic_audio_commit(output.count)!==0)throw Error('原生音频提交失败');const ptr=engine._th03_classic_audio(),count=engine._th03_classic_audio_frames();sound=engine.HEAP32.slice(ptr>>>2,(ptr>>>2)+count*2);}if(output.proof!==undefined){proofs.set(frame,output.proof);channel.send(JSON.stringify({type:'native-rollback-proof',frame,proof:output.proof}));compare(frame);}onConfirmedInput(frame,output.inputs,{replaySamples:output.replaySamples,scene:output.scene,round:output.round,serial:output.serial,winner:output.winner});onFrame(frame,sound,output.commands);}});
  const originalRestore=sync.restore;
  sync.restore=(s,at)=>{originalRestore(s);for(const frame of outputs.keys())if(frame>=at)outputs.delete(frame);};
  channel.onmessage=event=>{try{const msg=JSON.parse(event.data);if(msg.type==='native-rollback-proof'){
    if(!Number.isSafeInteger(msg.frame)||msg.frame<0||(msg.frame+1)%120!==0||msg.frame>sync.frame+120||!Number.isInteger(msg.proof)||msg.proof<0||msg.proof>0xffffffff||remoteProofs.has(msg.frame))throw Error('回滚校验消息无效');
    remoteProofs.set(msg.frame,msg.proof);compare(msg.frame);
  }else {
    sync.receive(event.data);
    // A confirmed frame may be waiting for this input while prediction is
    // disabled (selection, pause, or a lifecycle boundary). Run that already
    // sampled frame immediately instead of waiting for the next clock tick.
    if(!sync.closed&&sync.local.has(sync.frame)&&sync.remote.has(sync.frame))sync.advance(false);
  }}catch(error){running=false;sync.close();onError(error);}};
  channel.onclose=()=>{if(running){running=false;onError(Error('另一位玩家已断开'));}};
  return {tick(){if(!running)return;if(now()-lastProgress>10000){running=false;sync.close();onError(Error('另一位玩家连接超时'));return;}
    const scene=engine._th03_classic_scene(),canPredict=scene===9&&!!engine._th03_classic_surrender_allowed()&&pause.state.owner===null&&!engine._th03_classic_paused();
    if(sync.frame-sync.confirmed>=sync.maxPrediction)return;
    if(sync.local.has(sync.frame))sync.advance(canPredict);else sync.submit(sample()+(options.rapid===false?1024:0),canPredict);
  },stop(){running=false;sync.close();outputs.clear();proofs.clear();remoteProofs.clear();},get snapshotStats(){return {captures,restores};},get pause(){return pause;},get frame(){return sync.frame;},get confirmed(){return sync.confirmed;},get confirmedInput(){return confirmedInput;},get rollbacks(){return sync.rollbacks;},get running(){return running;}};
}
