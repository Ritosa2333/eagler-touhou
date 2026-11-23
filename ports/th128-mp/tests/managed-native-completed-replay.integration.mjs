import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {createMultiplayerRules} from '../../th128/site/runtime/multiplayer-rules.mjs';
import {parseStd,StageBackground} from '../../th128/site/runtime/renderer.mjs';
import {createConfirmedHistory,readMultiplayerReplay} from '../runtime/confirmed-history.mjs';
import {openTh128ReplayPlayback} from '../runtime/replay-playback.mjs';
import {managedSceneStep} from '../runtime/managed-view.mjs';
import {verifyManagedStage} from './managed-package.integration.mjs';

const sha=bytes=>createHash('sha256').update(bytes).digest('hex');
const frameTime=1000/60;

/** Reconstitute the exact 1611 completed archive, then verify playback using a
 * real Core127 pair and background on a controlled clock. This is not a UI test. */
export function probeManagedCompletedReplay({expectedArchiveSHA256,expectedStateSHA256,...fixture}){
  return verifyManagedStage({...fixture,async probeStage({preview,identity,signal}){
    const rules=createMultiplayerRules({schema:'th128-multiplayer-rules/1',matchId:'mp-long-native-boundary',epoch:1,mode:'parallel-stage',
      route:'A1-3',seed:34772,inputDelay:0,rollbackWindow:0,coreSHA256:identity.coreSHA256,resourceSHA256:identity.resourceSHA256});
    const completed=6342,history=await createConfirmedHistory({rules,allowedMask:763});
    const input=Object.freeze([0,1].map(player=>Object.freeze({player,held:0,pressed:0,released:0,predicted:false})));
    for(let frame=0;frame<completed;frame++)history.append({frame,inputs:input});
    history.seal('completed');const bytes=history.replayBytes();assert.equal(sha(bytes),expectedArchiveSHA256);
    const tape=await readMultiplayerReplay(bytes,identity);
    const renders=[],boundaries=[];let backgroundTicks=0,worldsOpened=0;
    const replay=await openTh128ReplayPlayback({tape,signal,
      async openWorld(wire,owner){
        const pair=await preview.openPair(createMultiplayerRules(wire),owner);worldsOpened++;
        try{
          const {module,std}=await preview.openBackground();owner.throwIfAborted();
          const background=new StageBackground(parseStd(std),module),step=managedSceneStep(pair,background,{confirmedOnly:true});
          return {pair,background,engine:{step(row){step(row);backgroundTicks++;},close:pair.close}};
        }catch(error){pair.close();throw error;}
      },render({pair,background},seat,cursor){
        assert.equal(background.frame,cursor);
        if(cursor<6||cursor===completed)renders.push([seat,cursor]);
        if(cursor===completed){
          assert.deepEqual([pair.state(0).frame,pair.state(1).frame],[6341,6341]);
          for(const nativeSeat of [0,1]){
            assert.deepEqual(pair.terminal(nativeSeat),{reason:'game-over',frame:6341});
            assert.equal(sha(pair.stateText(nativeSeat)),expectedStateSHA256);
          }
          boundaries.push({seat,cursor,nativeFrame:pair.state(seat).frame});
        }
      }});
    try{
      assert.equal(replay.play(),true);
      for(let frame=0;frame<=completed;frame++){
        if(frame%120===0)signal.throwIfAborted();
        replay.advance(frame*frameTime);
      }
      assert.deepEqual(replay.status(),{phase:'ended',seat:0,frame:completed,total:completed,end:'completed'});
      assert.equal(replay.advance(1_000_000).advanced,0);
      assert.equal(replay.selectSeat(1),true);
      assert.equal(await replay.seek(5),true);
      replay.advance(1_000_001);assert.equal(replay.status().phase,'seeking');
      replay.advance(1_000_002);assert.deepEqual(replay.status(),{phase:'paused',seat:1,frame:5,total:completed,end:'completed'});
      assert.deepEqual(boundaries,[{seat:0,cursor:completed,nativeFrame:6341},{seat:1,cursor:completed,nativeFrame:6341}]);
      assert.deepEqual(renders.at(-1),[1,5]);assert.equal(worldsOpened,2);assert.equal(backgroundTicks,completed+5);
      return {verifiedArchiveSHA256:sha(bytes),frames:completed,backgroundTicks,boundaries,backwardRebuiltFrame:5,worldsOpened};
    }finally{replay.close();}
  }});
}
