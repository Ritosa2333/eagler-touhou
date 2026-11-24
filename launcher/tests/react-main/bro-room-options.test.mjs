import {test,beforeAll} from 'vitest';
import assert from 'node:assert/strict';
import {build} from 'esbuild';
import {fileURLToPath} from 'node:url';
let api;
beforeAll(async()=>{const result=await build({stdin:{resolveDir:fileURLToPath(new URL('../../',import.meta.url)),contents:`export * from './src/launcher/bro-room-options.mts';export * from './src/launcher/multiplayer-runtime-options.mts';export * from './src/launcher/multiplayer-room-session.mts';export * from './src/launcher/multiplayer-relay-url.mts';export * from './src/contracts/product-catalog.mts';`},bundle:true,write:false,format:'esm',platform:'node'});api=await import('data:text/javascript;base64,'+Buffer.from(result.outputFiles[0].text).toString('base64'));});
const storage=()=>{const map=new Map();return {getItem:k=>map.get(k)??null,setItem:(k,v)=>map.set(k,v),map};};

test('BRO difficulty exposes Vanilla/Legacy plus AEX/BadApple chapter entries',()=>{
  const expected=['Vanilla','Legacy','天空璋AEX','Bad Apple!'];
  assert.deepEqual(api.PRODUCT_GAMES.bro.multiplayer.difficulties,expected);
  assert.deepEqual(api.PRODUCT_GAMES.bromp?.multiplayer?.difficulties ?? api.PRODUCT_GAMES.bro.multiplayer.difficulties,expected);
  const config=api.PRODUCT_GAMES.bro.multiplayer;
  const base={url:'ws://relay.test/?room=bromp-1234&run=1&player=0',player:0,playerCount:2,seed:19,spectator:false,spectatorId:'',spectatorCount:0,iceServers:[],loadouts:[{character:0,shot:0},{character:1,shot:0}]};
  for(let difficulty=0;difficulty<expected.length;difficulty++){
    assert.equal(api.buildMultiplayerRuntimeOptions({...base,difficulty},config).netplayDifficulty,difficulty);
  }
  assert.throws(()=>api.buildMultiplayerRuntimeOptions({...base,difficulty:4},config),/0–3/);
});

test('BRO systems and stage queue validators enforce their domains',()=>{
  assert(api.validBroSystems([1,3,11]));
  assert(api.validBroSystems([]));
  assert.equal(api.validBroSystems([0]),false);
  assert.equal(api.validBroSystems([12]),false);
  assert.equal(api.validBroSystems([2,2]),false,'systems de-duplicated');
  assert(api.validBroStageQueue([1,5,19]));
  assert(api.validBroStageQueue([2,2]),'queue allows repeats');
  assert(api.validBroStageQueue([]));
  assert.equal(api.validBroStageQueue([0]),false);
  assert.equal(api.validBroStageQueue([20]),false);
  assert.deepEqual(api.BRO_DEFAULT_SYSTEMS,[1,2,3,4,5,6,7,8,9,10,11]);
  assert.deepEqual(api.BRO_DEFAULT_STAGE_QUEUE,[]);
  assert.equal(api.BRO_SYSTEMS.length,11);
  assert.equal(api.BRO_STAGES.length,19);
});

test('BRO systems and queue reach runtime without leaking into other titles',()=>{
  const systems=[1,2,11],queue=[1,5,19],config=api.PRODUCT_GAMES.bro.multiplayer;
  const base={url:'ws://relay.test/?room=bromp-1234&run=1&player=0',player:0,playerCount:2,seed:19,difficulty:0,spectator:false,spectatorId:'',spectatorCount:0,iceServers:[],loadouts:[{character:0,shot:0},{character:1,shot:0}]};
  const result=api.buildMultiplayerRuntimeOptions({...base,broSystems:systems,broStageQueue:queue},config);
  assert.deepEqual(result.netplayBroSystems,systems);
  assert.deepEqual(result.netplayBroStageQueue,queue);
  assert.equal(result.netplayDifficulty,0);
  for(const game of ['th06','th19','th15']){
    const input={...base,url:base.url.replace('bromp',game+'mp'),broSystems:systems,broStageQueue:queue};
    assert.throws(()=>api.buildMultiplayerRuntimeOptions(input,api.PRODUCT_GAMES[game].multiplayer),/BRO/);
    delete input.broSystems; delete input.broStageQueue;
    assert.equal('netplayBroSystems' in api.buildMultiplayerRuntimeOptions(input,api.PRODUCT_GAMES[game].multiplayer),false);
  }
});

test('BRO creation URLs and saved sessions carry only their own room options',()=>{
  const systems=[1,2,11],queue=[3,1,7];
  const url=api.buildMultiplayerLobbyRelayUrl('ws://relay.test/',{product:'bromp',roomCode:'1234',clientId:'test_host_1234',intent:'create',broSystems:systems,broStageQueue:queue});
  assert.deepEqual(JSON.parse(new URL(url.url).searchParams.get('broSystems')),systems);
  assert.deepEqual(JSON.parse(new URL(url.url).searchParams.get('broStageQueue')),queue);
  const other=api.buildMultiplayerLobbyRelayUrl('ws://relay.test/?broSystems=bad',{product:'th06mp',roomCode:'1235',clientId:'test_host_1235',intent:'create',broSystems:systems,broStageQueue:queue});
  assert.equal(new URL(other.url).searchParams.has('broSystems'),false);
  const saved=storage(),store=api.createMultiplayerRoomSessionStore({storage:saved});
  store.save('bromp',{room:{code:'1234',playerCount:2,difficulty:1,broSystems:systems,broStageQueue:queue},seat:0});
  const loaded=store.load({product:'bromp',roomCode:'1234',playerCounts:[2,3],difficulties:['Vanilla','Legacy','天空璋AEX','Bad Apple!']}).room;
  assert.deepEqual(loaded.broSystems,systems);
  assert.deepEqual(loaded.broStageQueue,queue);
});
