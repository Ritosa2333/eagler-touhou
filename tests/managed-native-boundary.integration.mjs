import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {createMultiplayerRules} from '../../th128/site/runtime/multiplayer-rules.mjs';
import {verifyManagedStage} from './managed-package.integration.mjs';

const sha=text=>createHash('sha256').update(text).digest('hex');

/** Offline diagnostic only. Does not turn a native menu/fault into a played frame. */
export async function probeManagedNativeBoundary({maxFrames,holdTail=0,...fixture}){
  assert.ok(Number.isSafeInteger(maxFrames)&&maxFrames>=300&&maxFrames<=7200);
  assert.ok(Number.isSafeInteger(holdTail)&&holdTail>=0&&holdTail<=120);
  const native=[];
  return verifyManagedStage({...fixture,observeNativeFactory(module){
    assert.equal(typeof module._th128_world_pause_active,'function');
    const original=module._th128_world_tick,observed={module,ticks:0,stopped:null};native.push(observed);
    module._th128_world_tick=()=>{
      const result=original();observed.ticks++;
      if(result!==1){
        assert.equal(observed.stopped,null,'diagnostic must stop on its first non-playing tick');
        observed.stopped={result,pauseActive:module._th128_world_pause_active()===1,
          detail:module.UTF8ToString(module._th128_world_error())};
      }
      return result;
    };
  },async probeStage({preview,identity,signal}){
    const rules=createMultiplayerRules({schema:'th128-multiplayer-rules/1',matchId:'mp-long-native-boundary',epoch:1,mode:'parallel-stage',
      route:'A1-3',seed:34772,inputDelay:0,rollbackWindow:0,coreSHA256:identity.coreSHA256,resourceSHA256:identity.resourceSHA256});
    const pair=await preview.openPair(rules,signal,{holdGameOver:holdTail>0}),samples=[];
    assert.equal(native.length,2);
    function inspect(frame){
      const states=[pair.state(0),pair.state(1)];
      const [a,b]=[pair.stateText(0),pair.stateText(1)];
      assert.equal(a,b,`two neutral Worlds diverged at frame ${frame}`);
      const state=JSON.parse(a);assert.equal(states[0].frame,frame);assert.equal(states[1].frame,frame);
      assert.equal(states[0].normalRecord.bound,false);
      return {frame,sha256:sha(a),player:{x:states[0].playerX,y:states[0].playerY,state:states[0].playerState,
        deaths:states[0].deathsCommitted,motivation:states[0].motivationRaw},soundJournal:state.soundRequests,
        stageControl:states[0].stageControl.current,messageActive:!!states[0].message?.active};
    }
    let advanced=0,terminal=null,ticksAtHold=null;
    try{
      samples.push(inspect(0));
      for(;advanced<maxFrames;advanced++){
        if(advanced%120===0)signal.throwIfAborted();
        const row={frame:advanced,inputs:[0,1].map(player=>({player,held:0,pressed:0,released:0,predicted:false}))};
        try{pair.step(row);}catch(error){
          const stopped=native.find(row=>row.stopped);
          if(!stopped)throw error;
          terminal={attemptedFrame:advanced,native:stopped.stopped,adapter:error.message,
            seatFrames:[pair.state(0).frame,pair.state(1).frame],worldStateSHA256:[0,1].map(seat=>sha(pair.stateText(seat)))};
          break;
        }
        if(holdTail&&native.some(row=>row.stopped)){
          const labels=[0,1].map(seat=>pair.terminal(seat)),states=[0,1].map(seat=>pair.stateText(seat));
          if(!terminal){
            assert.deepEqual(labels,[0,1].map(()=>({reason:'game-over',frame:advanced})));
            assert.deepEqual(native.map(row=>row.stopped),[0,1].map(()=>({result:2,pauseActive:true,detail:''})));
            assert.equal(pair.state(0).frame,advanced);assert.equal(pair.state(1).frame,advanced);
            assert.equal(states[0],states[1]);ticksAtHold=native.map(row=>row.ticks);
            terminal={attemptedFrame:advanced,native:native.map(row=>row.stopped),markers:labels,stateSHA256:sha(states[0])};
          }else{
            assert.deepEqual(labels,terminal.markers);assert.deepEqual(native.map(row=>row.ticks),ticksAtHold);
            assert.ok(states.every(text=>sha(text)===terminal.stateSHA256),'frozen real Worlds changed during shared tail');
          }
          if(advanced-terminal.attemptedFrame===holdTail){advanced++;break;}
        }else if((advanced+1)%600===0)samples.push(inspect(advanced+1));
      }
    }finally{pair.close();}
    return {pattern:'neutral input on both seats',maxFrames,advanced,terminal,samples,ticksPerSeat:native.map(row=>row.ticks),
      ...(holdTail?{logicalRowsAfterGameOver:terminal?advanced-terminal.attemptedFrame-1:0}: {})};
  }});
}
