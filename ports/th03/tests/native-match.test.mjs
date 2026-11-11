import test from 'node:test';
import assert from 'node:assert/strict';
import {createNativeMatch} from '../web/native-match.mjs';
import {createVersusPause} from '../web/native-versus-pause.mjs';
import {createRollbackMatch} from '../web/native-match-rollback.mjs';

test('Japanese pause glyph keeps the strokes in 背 separate',()=>{
 const menu=createVersusPause(),rows=[];menu.advance([128,0],9,true);
 menu.draw({createImageData:(width,height)=>({data:new Uint8ClampedArray(width*height*4)}),putImageData:image=>rows.push(image)},'ja');
 const red=y=>rows[2].data[(y*288+98)*4];
 assert.deepEqual([red(20),red(22),red(24)],[255,85,255]);
});

test('paired frames preserve both inputs, focus and charge',()=>{
 const channels=[{},{}], pending=[[],[]], calls=[[],[]], errors=[];
 channels.forEach((channel,i)=>channel.send=value=>pending[1-i].push(value));
 const peers=channels.map((channel,seat)=>createNativeMatch({seat,channel,sample:()=>seat?32|64|8:512|4,engine:{
  _th03_classic_assist_pair:(...args)=>calls[seat].push(['focus',...args]),
  _th03_classic_step_pair:(...args)=>{calls[seat].push(['step',...args]);return 0;},
 },onError:e=>errors.push(e)}));
 for(let i=0;i<20;i++){
  peers[0].tick();peers[1].tick();
  while(pending[0].length)channels[0].onmessage({data:pending[0].shift()});
  while(pending[1].length)channels[1].onmessage({data:pending[1].shift()});
 }
 assert.deepEqual(calls[0],calls[1]);assert.deepEqual(calls[0][0],['focus',false,true]);
 assert.deepEqual(calls[0][1],['step',36,40,36,2]);assert.equal(errors.length,0);
});
test('different rendered pixels do not disconnect matching gameplay state',()=>{
 const channels=[{},{}],pending=[[],[]],errors=[];
 channels.forEach((channel,i)=>channel.send=value=>pending[1-i].push(value));
 const peers=channels.map((channel,seat)=>createNativeMatch({seat,channel,sample:()=>0,onError:error=>errors.push(error),engine:{
  HEAPU8:new Uint8Array([seat]),_th03_classic_pixels:()=>0,_th03_classic_scene:()=>9,
  _th03_semantic_state_hash:()=>123,_th03_classic_assist_pair(){},_th03_classic_step_pair:()=>0,
 }}));
 for(let frame=0;frame<125;frame++){peers.forEach(peer=>peer.tick());for(let seat=0;seat<2;seat++)while(pending[seat].length)channels[seat].onmessage({data:pending[seat].shift()});}
 assert.deepEqual(errors,[]);assert.ok(peers.every(peer=>peer.running));
});
test('wait for remote input without consuming new local key pulses',()=>{
 let sampled=0,time=0,error;
 const channel={send(){}};
 const match=createNativeMatch({seat:0,channel,sample:()=>{sampled++;return 0;},now:()=>time,
  engine:{_th03_classic_assist_pair(){},_th03_classic_step_pair(){return 0;}},onError:e=>error=e});
 match.tick();match.tick();assert.equal(sampled,1);
 time=10001;match.tick();assert.match(error.message,/超时/);assert.equal(match.running,false);
});
test('only the pausing player controls the menu and can surrender',()=>{
 const pending=[[],[]],channels=[{},{}], calls=[[],[]], engines=[0,1].map(seat=>({
  _th03_classic_assist_pair(){},_th03_classic_scene(){return 9;},_th03_classic_surrender_allowed(){return 1;},
  _th03_classic_surrender(...args){calls[seat].push(['surrender',...args]);},
  _th03_classic_step_pair(...args){calls[seat].push(['step',...args]);return 0;}}));
 channels.forEach((channel,i)=>channel.send=value=>pending[1-i].push(value));
 let input=[0,128];const peers=channels.map((channel,seat)=>createNativeMatch({seat,channel,engine:engines[seat],sample:()=>input[seat]}));
 function frame(){peers.forEach(p=>p.tick());for(let i=0;i<2;i++)while(pending[i].length)channels[i].onmessage({data:pending[i].shift()});}
 frame();assert.equal(peers[0].pause.state.owner,1);input=[0,0];frame();input=[2,0];frame();assert.equal(peers[0].pause.state.choice,0);
 input=[0,2];frame();assert.equal(peers[0].pause.state.choice,1);input=[0,0];frame();input=[0,512];frame();
 assert.equal(peers[0].pause.state.owner,null);assert.deepEqual(calls[0],calls[1]);assert.deepEqual(calls[0][0],['surrender',1,false]);
});
test('holding Escape after resuming cannot open the native menu',()=>{
 const channels=[{},{}],pending=[[],[]],systems=[[],[]];let input=[128,0];
 channels.forEach((ch,i)=>ch.send=value=>pending[1-i].push(value));
 const peers=channels.map((channel,seat)=>createNativeMatch({seat,channel,sample:()=>input[seat],engine:{_th03_classic_scene:()=>9,_th03_classic_surrender_allowed:()=>1,_th03_classic_assist_pair(){},_th03_classic_step_pair(_a,_b,system){systems[seat].push(system);return 0;}}}));
 function frame(){peers.forEach(p=>p.tick());for(let i=0;i<2;i++)while(pending[i].length)channels[i].onmessage({data:pending[i].shift()});}
 frame();input=[0,0];frame();input=[128,0];frame();frame();frame();
 assert.equal(peers[0].pause.state.owner,null);assert.ok(systems[0].length>=2);assert.ok(systems.flat().every(system=>!(system&0x1000)));
});
test('selection advances as soon as a delayed remote input arrives',()=>{
 const queues=[[],[]],channels=[{},{}],engines=[0,1].map(()=>({HEAPU8:new Uint8Array(16),HEAP32:new Int32Array(4),FS:{readdir:()=>['.','..']},
  _th03_classic_scene:()=>3,_th03_classic_surrender_allowed:()=>0,_th03_classic_paused:()=>0,_th03_classic_touch(){},_th03_classic_assist_pair(){},_th03_classic_step_pair:()=>0,_th03_classic_audio:()=>0,_th03_classic_audio_frames:()=>0,_th03_classic_defer_audio(){},_th03_classic_speculative(){},_th03_semantic_replay_samples:()=>0,_th03_classic_audio_commands:()=>0,_th03_classic_audio_command_count:()=>0,_th03_classic_audio_commit:()=>0}));
 channels.forEach((ch,i)=>ch.send=value=>queues[1-i].push(value));
 const peers=channels.map((channel,seat)=>createRollbackMatch({seat,channel,engine:engines[seat],sample:()=>0}));
 peers.forEach(peer=>peer.tick());assert.deepEqual(peers.map(peer=>peer.frame),[0,0]);
 channels.forEach((channel,i)=>channel.onmessage({data:queues[i].shift()}));
 assert.deepEqual(peers.map(peer=>peer.frame),[1,1]);
});
