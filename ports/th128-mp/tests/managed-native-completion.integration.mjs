import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {createMultiplayerRules} from '../../th128/site/runtime/multiplayer-rules.mjs';
import {readMultiplayerReplay} from '../runtime/confirmed-history.mjs';
import {createPlayerMatch} from '../runtime/player-match.mjs';
import {createMultiplayerStartGate} from '../runtime/startup.mjs';
import {createTh128SpectatorDecoder,createTh128SpectatorEncoder} from '../runtime/spectator-wire.mjs';
import {verifyManagedStage} from './managed-package.integration.mjs';

/** Exact native pair, emulated opposite wire peer; no Socket, browser or timings claimed. */
export function probeManagedNativeCompletion(fixture){
  return verifyManagedStage({...fixture,async probeStage({preview,identity,signal}){
    const rules=createMultiplayerRules({schema:'th128-multiplayer-rules/1',matchId:'mp-long-native-boundary',epoch:1,mode:'parallel-stage',
      route:'A1-3',seed:34772,inputDelay:0,rollbackWindow:0,coreSHA256:identity.coreSHA256,resourceSHA256:identity.resourceSHA256});
    const mask=763,outgoing=[];let world=null,confirmed=0;
    const match=await createPlayerMatch({rules,localPlayer:0,allowedMask:mask,maxFrames:6400,signal,
      async openEngine(selection,owner){world=await preview.openPair(selection,owner);return world;},
      send(packet){outgoing.push(packet);},onConfirmed(ownedRules,row){assert.equal(ownedRules,rules);assert.equal(row.frame,confirmed++);}});
    try{
      const remote=await createMultiplayerStartGate({rules,localPlayer:1,allowedMask:mask});
      match.announceStart();const remoteAck=await remote.receive(outgoing.shift(),0);
      await match.receive(remote.offer(),1);await remote.receive(outgoing.shift(),0);await match.receive(remoteAck,1);
      assert.equal(remote.status().canStart,true);assert.equal(match.status().startup.canStart,true);
      for(let frame=0;frame<6400&&!match.status().history.end;frame++){
        if(frame%120===0)signal.throwIfAborted();
        await match.receive({matchId:rules.matchId,epoch:1,player:1,frame,held:0},1);
        assert.equal(match.tick(0,{allowPrediction:false}).advanced,true);
        assert.deepEqual(outgoing.shift(),{matchId:rules.matchId,epoch:1,player:0,frame,held:0});
      }
      assert.deepEqual(match.status().history,{nextFrame:6342,end:'completed'});assert.equal(confirmed,6342);
      const markers=[0,1].map(seat=>world.terminal(seat)),nativeFrames=[0,1].map(seat=>world.state(seat).frame);
      assert.deepEqual(markers,[0,1].map(()=>({reason:'game-over',frame:6341})));assert.deepEqual(nativeFrames,[6341,6341]);
      assert.deepEqual(match.replayReceipt(),{epoch:1,end:'completed'});
      const stateSHA256=[0,1].map(seat=>createHash('sha256').update(world.stateText(seat)).digest('hex'));
      assert.equal(stateSHA256[0],stateSHA256[1]);
      assert.deepEqual(match.tick(()=>assert.fail('completed native owner sampled input')),{advanced:false,reason:'round-completed'});
      assert.equal(outgoing.length,0);
      const source=match.spectatorSource(),bytes=match.previousReplay(),replay=await readMultiplayerReplay(bytes,identity);
      assert.deepEqual(replay.status(),source.status());assert.equal(replay.read(6342),null);
      const sender=await createTh128SpectatorEncoder(source.header,identity),receiver=createTh128SpectatorDecoder({build:identity,matchId:rules.matchId});
      receiver.announce(sender.announce());await receiver.accept(sender.open().subarray(1));
      for(let frame=0;frame<confirmed;frame++){
        if(frame%120===0)signal.throwIfAborted();
        await receiver.accept(sender.frame(source.read(frame)).subarray(1));
        assert.deepEqual(receiver.read(frame),replay.read(frame));
      }
      assert.equal(await receiver.accept(sender.finish('completed').subarray(1)),'end');assert.deepEqual(receiver.retain().status(),replay.status());
      return {nativeFrames,markers,stateSHA256,confirmed,archiveEnd:replay.status().end,archiveSHA256:createHash('sha256').update(bytes).digest('hex'),
        spectatorEnd:receiver.status().end,emulatedWirePeer:true,noFurtherSampling:true};
    }finally{match.close();}
  }});
}
