import test from 'node:test';
import assert from 'node:assert/strict';
import {assertMusicGameplayRestart} from './music-gameplay-restart.mjs';
function fixture(mode='ogg'){
  const contract={pauseKey:'Escape',restartKey:'KeyR',stage:0,minimumFrame:180,source:'Independent native stage restart rule',mode,expectedSource:'stage-track'};
  const sample=(frame,paused,cursor,opens=2,noteOns=4)=>({identity:{game:'th08',epoch:2,app:18,timeOrigin:42,url:'https://fixture/runtime'},
    protocol:{exit:0,first:1},ready:!paused,paused,frame,stage:0,audio:{source:'stage-track',cursor,opens,noteOns,playing:true}});
  const input=code=>({code,source:'Host keyboard protocol',down:{protocol:'eagler-touhou/1',game:'th08',epoch:2,ok:true,request:code+'-down'},
    up:{protocol:'eagler-touhou/1',game:'th08',epoch:2,ok:true,request:code+'-up'}});
  return{schema:'eagler-touhou/music-gameplay-restart/1',contract,before:sample(300,false,15000),paused:sample(310,true,16000),settled:sample(310,true,16000),
    after:sample(8,false,4000,3,5),outputState:sample(40,false,6000,3,7),inputs:[input('Escape'),input('KeyR')],heldAfter:{pause:false,restart:false}};
}
test('native music restart verifies actual frame reconstruction, same app, protocol continuity and the title audio rule',()=>{
  for(const mode of ['ogg','midi']){const e=fixture(mode);assertMusicGameplayRestart(e,e.contract);}
});
for(const [name,mutate] of Object.entries({
  'only unpaused':e=>e.after.frame=320,
  'pause still advances':e=>e.settled.frame++,
  'different app':e=>e.after.identity.app++,
  'different document':e=>e.after.identity.timeOrigin++,
  'different epoch':e=>e.after.identity.epoch++,
  'Runtime stop instead':e=>e.after.protocol.exit++,
  'Runtime launch instead':e=>e.after.protocol.first++,
  'wrong stage':e=>e.outputState.stage=1,
  'wrong track':e=>e.outputState.audio.source='other',
  'held key leaked':e=>e.heldAfter.restart=true,
  'wrong input epoch':e=>e.inputs[1].down.epoch=1,
  'input rejected':e=>e.inputs[0].up.ok=false,
  'old OGG cursor':e=>e.after.audio.cursor=20000,
  'world did not continue':e=>e.outputState.frame=e.after.frame,
}))test(`native music restart rejects ${name}`,()=>{const e=fixture();mutate(e);assert.throws(()=>assertMusicGameplayRestart(e,e.contract));});
test('MIDI game restart rejects an unchanged SMF open counter or old notes',()=>{
  for(const field of ['opens','noteOns']){const e=fixture('midi');e.outputState.audio[field]=e.before.audio[field];assert.throws(()=>assertMusicGameplayRestart(e,e.contract));}
});
