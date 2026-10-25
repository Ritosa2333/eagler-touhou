import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import test from 'node:test';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';
import {deriveReplayStorageLocations,parseReplayStoragePolicy,readRuntimeReplayApis,readRuntimeStorageMount,readRuntimeUserFileBoundary,runtimeReplayFilePath,validateReplayInspection} from './runtime-storage-contract.mjs';

const root=resolve(import.meta.dirname,'../..');
const policySource=await readFile(resolve(root,'launcher/src/launcher/replay-storage-policy.mts'),'utf8');
const policy=parseReplayStoragePolicy(policySource);
test('native user-file reader copies bytes and canonical mount identity without invoking game logic',t=>{
  const previous=globalThis.location;t.after(()=>{delete globalThis.__storageBoundaryFixture;if(previous===undefined)delete globalThis.location;else globalThis.location=previous;});
  globalThis.location={href:'https://fixture/runtime?runtimeEpoch=3'};
  const node={mode:1,mount:{mountpoint:'/save'}},files=new Map([['/save/config',node]]);
  globalThis.__storageBoundaryFixture={app:12,Module:{FS:{getPath:()=>'/save/config',lookupPath:path=>{if(!files.has(path))throw{errno:44};return{node:files.get(path)};},isFile:mode=>mode===1,readFile:()=>Uint8Array.of(1,2)}}};
  const args={runtime:{global:'__storageBoundaryFixture',module:'Module',app:'app'},root:'/save',paths:['config']};
  const result=readRuntimeUserFileBoundary(args);assert.deepEqual(result.files,[{path:'config',absolutePath:'/save/config',resolvedPath:'/save/config',mountpoint:'/save',bytes:[1,2]}]);
  assert.equal(result.document.epoch,3);assert.equal(result.outside.exists,false);
  files.set('/save/../outside-offline-acceptance.dat',node);assert.equal(readRuntimeUserFileBoundary(args).outside.exists,true);
  node.mode=2;assert.throws(()=>readRuntimeUserFileBoundary(args),/regular file/);
  assert.throws(()=>readRuntimeUserFileBoundary({...args,paths:['../config']}),/relative/);
});

test('normal and multiplayer database paths are derived from the production policy and product root',()=>{
  const th08=deriveReplayStorageLocations({game:'th08',product:PRODUCT_GAMES.th08,policy});
  assert.deepEqual(th08.normal,{game:'th08',variant:'normal',database:'/savesth08',root:'/savesth08',language:'ja'});
  assert.deepEqual(th08.multiplayer,{game:'th08',variant:'multiplayer',database:'/savesth08-multiplayer',root:'/savesth08-multiplayer',language:'ja'});
  assert.notEqual(th08.normal.database,th08.multiplayer.database);
  const th09=deriveReplayStorageLocations({game:'th09',product:PRODUCT_GAMES.th09,policy});
  assert.equal(th09.multiplayer.database,'/savesth09mp');
  const th10=deriveReplayStorageLocations({game:'th10',product:PRODUCT_GAMES.th10,policy});
  assert.equal(th10.normal.root,'/savesth10/jp');
  assert.equal(th10.multiplayer.root,'/savesth10-multiplayer/jp');
  const th20=deriveReplayStorageLocations({game:'th20',product:PRODUCT_GAMES.th20,policy});
  assert.equal(th20.multiplayer,null,'Products without an MP Runtime do not receive a fabricated MP storage root');
  const changedPolicy=parseReplayStoragePolicy(policySource.replace("'-multiplayer'", "'-shared'"));
  assert.equal(deriveReplayStorageLocations({game:'th08',product:PRODUCT_GAMES.th08,policy:changedPolicy}).multiplayer.database,
    '/savesth08-shared','Derived evidence follows a changed production declaration rather than a test-owned title list');
  for(const [game,product] of Object.entries(PRODUCT_GAMES)){
    const locations=deriveReplayStorageLocations({game,product,policy});
    assert.equal(locations.normal.database,product.storage.saveRoot,`${game}: normal namespace follows the product owner`);
    assert.equal(Boolean(locations.multiplayer),Boolean(product.multiplayerRuntime),
      `${game}: only products with a declared multiplayer Runtime receive an MP namespace`);
    if(locations.multiplayer)assert.notEqual(locations.normal.database,locations.multiplayer.database,
      `${game}: selected Runtime variants keep separate databases`);
  }
});

test('Replay filenames retain the fixture format and product-owned prefix',()=>{
  assert.equal(runtimeReplayFilePath({product:PRODUCT_GAMES.th08,fixturePath:'C:/fixture/accepted.rpy',suffix:'a1b2'}),
    'replay/th8_uda1b2.rpy');
  assert.equal(runtimeReplayFilePath({product:{...PRODUCT_GAMES.th08,replay:{...PRODUCT_GAMES.th08.replay,extensions:['rpyx']}},
    fixturePath:'C:/fixture/accepted.rpyx',suffix:'00a9'}),'replay/th8_ud00a9.rpyx');
  assert.equal(runtimeReplayFilePath({product:PRODUCT_GAMES.th075,fixturePath:'C:/fixture/accepted.rep',suffix:'00a9'}),
    'replay/th075_ud00a9.rep','Product-owned extension and prefix are retained for filename-style Replay products');
  assert.throws(()=>runtimeReplayFilePath({product:PRODUCT_GAMES.th08,fixturePath:'C:/fixture/fake.dat',suffix:'a1b2'}),/extension/);
  assert.throws(()=>runtimeReplayFilePath({product:PRODUCT_GAMES.th08,fixturePath:'C:/fixture/accepted.rpy',suffix:'bad!'}),/four lowercase/);
});

test('Runtime storage reader distinguishes an observed IDBFS mount from absent or wrong mounts',()=>{
  const previous=globalThis.window;
  const IDBFS={name:'IDBFS'};
  const mount={mountpoint:'/savesth08',type:IDBFS};
  const fs={mounts:[mount],lookupPath:path=>({node:{mount:path==='/savesth08'?mount:null}})};
  const runtime={Module:{FS:fs,IDBFS},core:{eagler_replay_inspect(){},eagler_replay_open(){}}};
  const mapping={global:'__th08Runtime',module:'Module',core:'core'};
  globalThis.window={__th08Runtime:runtime,Module:{FS:{lookupPath:()=>({node:{mount:{mountpoint:'/savesth08',type:{name:'IDBFS'}}}})},
    IDBFS:{name:'IDBFS'},eagler_replay_inspect(){},eagler_replay_open(){}}};
  try {
    assert.deepEqual(readRuntimeStorageMount({runtime:mapping,expectedDatabase:'/savesth08'}),
      {available:true,expectedDatabase:'/savesth08',runtimeGlobal:'__th08Runtime',moduleField:'Module',mountpoint:'/savesth08',typeName:'IDBFS',idbfs:true});
    assert.deepEqual(readRuntimeStorageMount({runtime:mapping,expectedDatabase:'/savesth08-multiplayer'}),
      {available:false,expectedDatabase:'/savesth08-multiplayer',runtimeGlobal:'__th08Runtime',moduleField:'Module',
        reason:'Declared persistence mount is absent from the Runtime FS'});
    assert.deepEqual(readRuntimeReplayApis({runtime:mapping}),
      {available:true,runtimeGlobal:'__th08Runtime',coreField:'core',moduleField:'Module',inspect:true,open:true,heap:false,fileSystem:true});
    assert.equal(readRuntimeStorageMount({runtime:{...mapping,global:'__missingRuntime'},expectedDatabase:'/savesth08'}).available,false,
      'A global Module belonging to another instance cannot satisfy this Runtime mapping');
    assert.equal(readRuntimeReplayApis({runtime:{...mapping,global:'__missingRuntime'}}).available,false,
      'A global Replay API belonging to another instance cannot satisfy this Runtime mapping');
    assert.equal(readRuntimeStorageMount({expectedDatabase:'/savesth08'}).available,false,
      'Unregistered legacy readers do not guess a Runtime global');
    mount.type={name:'MEMFS'};
    assert.equal(readRuntimeStorageMount({runtime:mapping,expectedDatabase:'/savesth08'}).idbfs,false);
  } finally {
    if(previous===undefined)delete globalThis.window;else globalThis.window=previous;
  }
});

test('official Replay inspection proves a recognized file without claiming its natural endpoint',()=>{
  assert.deepEqual(validateReplayInspection({description:{stages:[{id:0}]},inspectedPath:'replay/th8_ud1234.rpy',expectedPath:'replay/th8_ud1234.rpy'}),
    {status:'passed',method:'official-runtime-inspection',path:'replay/th8_ud1234.rpy',stageCount:1,naturalEndpointClaimed:false});
  assert.throws(()=>validateReplayInspection({description:{stages:[]},inspectedPath:'replay/th8_ud1234.rpy',expectedPath:'replay/th8_ud1234.rpy'}),/did not recognize/);
  assert.throws(()=>validateReplayInspection({description:{stages:[{id:0}]},inspectedPath:'replay/other.rpy',expectedPath:'replay/th8_ud1234.rpy'}),/file written by this scenario/);
});
