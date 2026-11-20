import {NativeRollback,NATIVE_INPUT_AHEAD} from './native-rollback.mjs';
import {FireControl} from './fire-control.js';
import {unpackTouch} from './touch-input.js';
import {createVersusPause} from './native-versus-pause.mjs';
import {NativeChargeHistory,validChargeSample} from './native-charge-history.mjs';

export function createRollbackMatch({seat,channel,engine,sample,options={},onFrame=()=>{},onConfirmedInput=()=>{},onError=()=>{},now=()=>performance.now()}){
  const fire=[new FireControl(),new FireControl()], pause=createVersusPause(), outputs=new Map(),proofs=new Map(),remoteProofs=new Map();let running=true,lastProgress=now(),confirmedInput=[0,0];
  const metadata=()=>({scene:engine._th03_classic_scene(),round:engine._th03_semantic_round?.()??-1,serial:engine._th03_classic_match_serial?.()??0,winner:engine._th03_classic_match_winner?.()??-1});
  const compare=frame=>{if(!proofs.has(frame)||!remoteProofs.has(frame))return;if(proofs.get(frame)!==remoteProofs.get(frame))throw Error(`双方游戏状态不同步（第 ${frame} 帧）`);proofs.delete(frame);remoteProofs.delete(frame);};
  const freeSlots=Array.from({length:16},(_,i)=>15-i);let captures=0,restores=0,correction=Infinity;
  const remoteCharge=new NativeChargeHistory(NATIVE_INPUT_AHEAD-1),localCharge=new Map(),chargeBoundaries=new Set();
  let localShadow;
  const supportsCharge=typeof engine._th03_classic_charge_enable==='function';
  const activeCharge=()=>supportsCharge&&engine._th03_classic_scene()===9&&!!engine._th03_classic_surrender_allowed()&&pause.state.owner===null&&!engine._th03_classic_paused();
  const needsCharge=frame=>{
    if(chargeBoundaries.has(frame))return false;
    if(outputs.has(frame))return !!outputs.get(frame).needsCharge;
    if(!activeCharge())return false;
    const a=sync.local.get(frame),b=sync.remote.get(frame);
    return !((a!==undefined&&b!==undefined)&&((a|b)&128));
  };
  engine._th03_classic_defer_audio();
  const snapshot=()=>{const slot=freeSlots.pop();if(slot===undefined)throw Error('原生回滚历史已满');if(!engine._th03_classic_capture(slot)){freeSlots.push(slot);return null;}captures++;return {slot,fire:fire.map(item=>({...item.state})),pause:pause.capture()};};
  const release=s=>{freeSlots.push(s.slot);};
  const restore=s=>{if(!engine._th03_classic_restore(s.slot))throw Error('原生回滚状态恢复失败');restores++;fire.forEach((item,i)=>item.state={...s.fire[i]});pause.restore(s.pause);};
  const sync=new NativeRollback({seat,send:text=>channel.send(text),capture:snapshot,restore,release,onError,maxPrediction:12,canConfirm:frame=>!needsCharge(frame)||(localCharge.has(frame)&&!!remoteCharge.get(frame)),onCaptureUnavailable(frame){if(!supportsCharge||!needsCharge(frame))return false;chargeBoundaries.add(frame);channel.send(JSON.stringify({type:'native-charge-boundary',frame}));return true;},step(inputs,frame,speculative){
    const recordedInputs=[...inputs];
    const chargeFrame=needsCharge(frame);
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
    if(supportsCharge){
      engine._th03_classic_charge_enable(chargeFrame?1:0);
      if(chargeFrame)for(let p=0;p<2;p++){
        const sample=p===seat?localCharge.get(frame):remoteCharge.get(frame);
        const previous=p===seat&&!sample&&localShadow?.frame===frame-1&&(localShadow.sample[3]&4)?localShadow.sample:undefined;
        engine._th03_classic_charge_set(p,sample?1:previous?2:0,...(sample??previous??[0,0,-1,0]));
      }
    }
    engine._th03_classic_speculative(speculative?1:0);
    let result;try{result=engine._th03_classic_step_pair(lanes[0],lanes[1],system,charge);}finally{engine._th03_classic_speculative(0);}
    let chargePair;
    if(chargeFrame){
      const observed=[0,1].map(p=>[0,1,2,3].map(field=>engine._th03_classic_charge_get(p,field)));
      if(observed.some(sample=>!validChargeSample(sample)))throw Error('原生蓄力数据无效');
      if(!localCharge.has(frame)){localCharge.set(frame,observed[seat]);localShadow={frame,sample:observed[seat]};channel.send(JSON.stringify({type:'native-charge',frame,sample:observed[seat]}));}
      const remoteSample=remoteCharge.get(frame)??observed[1-seat];
      remoteCharge.markUsed(frame,remoteSample);
      chargePair=seat===0?[localCharge.get(frame),remoteSample]:[remoteSample,localCharge.get(frame)];
    }else if(localShadow?.frame===frame-1)localShadow=undefined;
    if(speculative&&(result!==0||engine._th03_classic_boundary()||engine._th03_classic_scene()!==9||engine._th03_classic_paused()))return false;
    if(result!==0)throw Error('游戏结束或同步失败：'+result);
    const ptr=engine._th03_classic_audio_commands(),count=engine._th03_classic_audio_command_count();
    let proof;
    if((frame+1)%120===0)proof=engine._th03_classic_scene()===9?engine._th03_semantic_state_hash()>>>0:0;
    outputs.set(frame,{commands:engine.HEAPU8.slice(ptr,ptr+count*4),count,proof,lanes,inputs:recordedInputs,needsCharge:chargeFrame,chargePair,replaySamples:engine._th03_semantic_replay_samples(),...metadata()});
    lastProgress=now();
  },confirm(frame){const output=outputs.get(frame);outputs.delete(frame);if(!output)throw Error('确认帧音频缺失');if(output.needsCharge)remoteCharge.confirm(frame);else remoteCharge.skip(frame);localCharge.delete(frame);chargeBoundaries.delete(frame);confirmedInput=output.lanes;let sound=new Int32Array(0);if(!output.paused){engine.HEAPU8.set(output.commands,engine._th03_classic_audio_commands());if(engine._th03_classic_audio_commit(output.count)!==0)throw Error('原生音频提交失败');const ptr=engine._th03_classic_audio(),count=engine._th03_classic_audio_frames();sound=engine.HEAP32.slice(ptr>>>2,(ptr>>>2)+count*2);}if(output.proof!==undefined){proofs.set(frame,output.proof);channel.send(JSON.stringify({type:'native-rollback-proof',frame,proof:output.proof}));compare(frame);}onConfirmedInput(frame,output.inputs,{charge:output.chargePair,replaySamples:output.replaySamples,scene:output.scene,round:output.round,serial:output.serial,winner:output.winner});onFrame(frame,sound,output.commands);}});
  const originalRestore=sync.restore;
  sync.restore=(s,at)=>{originalRestore(s);remoteCharge.rewind(at);for(const frame of outputs.keys())if(frame>=at)outputs.delete(frame);};
  const canPredict=()=>engine._th03_classic_scene()===9&&!!engine._th03_classic_surrender_allowed()&&pause.state.owner===null&&!engine._th03_classic_paused();
  function reconcile(){
    if(correction!==Infinity){const frame=correction;correction=Infinity;sync.rewind(frame);}
    sync.flush();
  }
  function receive(messages){
    if(!running)return;
    try{
      let receivedInput=false,boundary=false;
      for(const text of messages){
        const msg=JSON.parse(text);
        if(msg.type==='native-rollback-proof'){
          if(!Number.isSafeInteger(msg.frame)||msg.frame<0||(msg.frame+1)%120!==0||msg.frame>sync.frame+240||!Number.isInteger(msg.proof)||msg.proof<0||msg.proof>0xffffffff||remoteProofs.has(msg.frame))throw Error('回滚校验消息无效');
          remoteProofs.set(msg.frame,msg.proof);compare(msg.frame);continue;
        }
        receivedInput=true;
        if(msg.type==='native-charge-boundary'){
          if(!supportsCharge||!sync.canReceive(msg.frame))throw Error('蓄力边界数据无效');
          if(msg.frame<sync.confirmed)continue;
          boundary=true;
          chargeBoundaries.add(msg.frame);
          if(msg.frame<sync.frame&&outputs.get(msg.frame)?.needsCharge)correction=Math.min(correction,msg.frame);
        }else if(msg.type==='native-charge'){
          if(!supportsCharge)throw Error('联机版本不同，请刷新双方页面');
          if(remoteCharge.accept(msg.frame,msg.sample,sync.nextLocal)&&msg.frame<sync.frame)correction=Math.min(correction,msg.frame);
        }else {
          const frame=sync.acceptInput(msg);
          if(frame!==undefined){correction=Math.min(correction,frame);boundary||=!!(msg.input&128);}
        }
      }
      if(!receivedInput)return;
      // Coalesce gameplay corrections until the next simulation tick. A
      // waiting frame or a lifecycle boundary still needs immediate recovery.
      if(correction!==Infinity&&!boundary&&!sync.blocked&&sync.frame-sync.confirmed<sync.maxPrediction&&canPredict())return;
      reconcile();
      while(!sync.closed&&sync.local.has(sync.frame)&&sync.remote.has(sync.frame)&&sync.advance(false)){}
    }catch(error){running=false;sync.close();onError(error);}
  }
  channel.onmessage=event=>receive([event.data]);
  channel.oninputbatch=receive;
  channel.onclose=()=>{if(running){running=false;onError(Error('另一位玩家已断开'));}};
  return {tick(){if(!running)return;if(now()-lastProgress>10000){running=false;sync.close();onError(Error('另一位玩家连接超时'));return;}
    const predicting=canPredict();
    // In battle, sample the current frontier only. Pre-sampling an extra
    // frame during a stall keeps one stale control frame ahead of fresh input.
    if(predicting ? sync.nextLocal===sync.frame : sync.nextLocal-sync.confirmed<NATIVE_INPUT_AHEAD)sync.enqueue(sample()+(options.rapid===false?1024:0));
    // Send the fresh frontier input first; historical steps use stored inputs.
    const recovering=correction!==Infinity;
    if(recovering)reconcile();
    sync.advance(recovering?canPredict():predicting);
  },stop(){running=false;sync.close();remoteCharge.clear();localCharge.clear();chargeBoundaries.clear();outputs.clear();proofs.clear();remoteProofs.clear();},get snapshotStats(){return {captures,restores};},get pause(){return pause;},get frame(){return sync.frame;},get confirmed(){return sync.confirmed;},get confirmedInput(){return confirmedInput;},get rollbacks(){return sync.rollbacks;},get running(){return running;}};
}
