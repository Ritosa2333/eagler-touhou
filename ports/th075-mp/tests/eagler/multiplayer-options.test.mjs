import assert from 'node:assert/strict';
import {Multiplayer,validateMultiplayerOptions} from '../../web/sdl-runtime/multiplayer.mjs';
globalThis.location={href:'https://example.test/runtime/th075.html'};
const base={netplayUrl:'wss://example.test/netplay?room=th075mp-1234&run=1&player=0',
  netplayMode:'lan',netplayPlayerCount:2,netplayPlayer:0,netplaySeed:1947,netplayDifficulty:1,
  netplayAdonisMode:1,netplayPredictionLimit:0,netplayInputDelay:2,netplayInputDelayAuto:false,
  netplayLoadouts:[{character:0,shot:0},{character:10,shot:8}]};
assert.equal(validateMultiplayerOptions(base).spectator,false);
for(const patch of [{netplayPlayerCount:3},{netplayAdonisMode:0},{netplayAdonisMode:2},
  {netplayPredictionLimit:1},{netplayInputDelay:10},{netplaySeed:65536},
  {netplayDifficulty:4},{netplayLoadouts:[{character:11,shot:0},{character:0,shot:0}]},
  {netplayUrl:base.netplayUrl.replace('&player=0','')}])assert.throws(()=>validateMultiplayerOptions({...base,...patch}));
assert.equal(validateMultiplayerOptions({...base,netplayAdonisMode:0,netplayPredictionLimit:8,netplayInputDelay:0}).spectator,false);
for(const patch of [{netplayPredictionLimit:0},{netplayPredictionLimit:9},{netplayInputDelay:1},{netplayInputDelayAuto:true}])assert.throws(()=>validateMultiplayerOptions({...base,netplayAdonisMode:0,netplayPredictionLimit:8,netplayInputDelay:0,...patch}));
const viewer={...base,netplayPlayer:null,netplaySpectator:true,netplaySpectatorId:'viewer_1234',
  netplayUrl:base.netplayUrl.replace('player=0','spectator=viewer_1234')};
assert.equal(validateMultiplayerOptions(viewer).spectator,true);
assert.throws(()=>validateMultiplayerOptions({...viewer,netplayUrl:viewer.netplayUrl+'&player=0'}));
assert.throws(()=>validateMultiplayerOptions({...viewer,netplaySpectatorId:'another_1234'}));

const transport={relay:{readyState:1},peers:new Map()};
const playback=new Uint32Array(22);
const events=[],calls=[];
const core={
  th075ContentIdentity:'split-data',
  HEAPU32:playback,
  ccall(name,_type,_types,args){
    calls.push([name,args]);
    if(name==='th075_mp_begin')globalThis.__eaglerPeerTransport=transport;
    return 1;
  },
  _th075_mp_playback_state:()=>4,
  _th075_mp_playback_marks:()=>40,
  _th075_mp_end(){delete globalThis.__eaglerPeerTransport;},
};
const session=new Multiplayer({core,options:{...viewer,netplayInGameSelection:true},build:{version:'test'},
  emit:(event,fields)=>events.push({event,...fields}),onError:error=>{throw error;},isBusy:()=>true});
try{
  await session.connect();
  assert.deepEqual(calls[0],['th075_mp_prepare_selection',[base.netplaySeed,base.netplayDifficulty]]);
  assert.equal(globalThis.__th075PeerTransport,transport);
  session.reportPlayback();
  assert.deepEqual(events,[]);
  playback.set([0,1,4,4,0,0,1,1,1],1);
  playback.set([1,0,1],10);
  session.reportPlayback();
  assert.equal(events[0].spectatorPlayback.position,1);
  assert.deepEqual(events[0].spectatorPlayback.nodes,[{id:1,kind:'match',frame:1,stage:1,bossSlots:0}]);

  playback.set([1,0,1, 3,1,1, 10,0,2, 15,1,2],10);
  playback.set([0,6,20,6,1,0,0,4,15],1);
  session.reportPlayback();
  let report=events.at(-1).spectatorPlayback;
  assert.deepEqual([report.min,report.edge,report.received],[1,9,20]);
  assert.deepEqual(report.nodes.map(node=>node.frame),[1,3],'the first match cannot seek into the next match');

  playback.set([0,16,20,20,0,0,1,4,15],1);
  session.reportPlayback();
  report=events.at(-1).spectatorPlayback;
  assert.deepEqual([report.min,report.position,report.edge],[10,16,20]);
  assert.deepEqual(report.nodes.map(node=>node.frame),[10,15]);

  playback.set([0,2,20,10,1,1,0,4,15],1);
  session.reportPlayback();
  report=events.at(-1).spectatorPlayback;
  assert.deepEqual([report.min,report.position,report.target],[10,10,10],'reconstructing the second match keeps its time origin');
}finally{
  session.close();
}
assert.equal(globalThis.__th075PeerTransport,undefined);
