import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {readFile} from 'node:fs/promises';
import {pathToFileURL} from 'node:url';
import {setTimeout as later} from 'node:timers/promises';
import {provideTh128ManagedPackage} from '../../../launcher/src/launcher/th128-managed-package.mts';
import {readZipDirectory,readEntryData} from '../../th128/site/runtime/zip-reader.mjs';
import {buildMultiplayerGameplayRelayUrl} from '../../../launcher/src/launcher/multiplayer-relay-url.mts';
import {openManagedTh128StageResources,openManagedRoomTitle} from '../runtime/managed-stage-resources.mjs';
import {prepareA13Preview} from '../runtime/preview-world.mjs';
import {mountManagedTh128Room} from '../runtime/managed-room.mjs';
import {createMultiplayerRules} from '../../th128/site/runtime/multiplayer-rules.mjs';
import {createConfirmedHistory} from '../runtime/confirmed-history.mjs';
import {makeTh128ReplayFile} from '../runtime/replay-file.mjs';
import {createManagedReplayMenu} from '../runtime/managed-replay-menu.mjs';
import {managedSceneStep} from '../runtime/managed-view.mjs';
import {parseStd,StageBackground} from '../../th128/site/runtime/renderer.mjs';

const sha=bytes=>createHash('sha256').update(bytes).digest('hex'),text=new TextDecoder(),name='game-data';
export async function verifyManagedStage({archivePath,program,programPaths,comparison,onFactory,onTitleTick,onWorldStart,onTick,signal,
  exerciseReplay=false,replayFrames=4,onReplayFixture=async()=>{},probeStage=null,probeCoop=null,observeNativeFactory=null}){
  assert.ok(Number.isSafeInteger(replayFrames)&&replayFrames>=4&&replayFrames<=240);
  const total=exerciseReplay?replayFrames:4,checkpoints=new Set([1,2,4,...(total>4?[60,120,total].filter(frame=>frame<=total):[])]);
  const archive=readZipDirectory(new Uint8Array(await readFile(archivePath))),entries=new Map(archive.entries.map(row=>[row.name,row]));
  const descriptor=JSON.parse(text.decode(await readEntryData(entries.get('package.json'),{signal})));
  const generation={id:'owned-package-stage-1582',game:'th128',descriptor,files:{}};
  for(const [id,row] of Object.entries(descriptor.files))generation.files[id]={objectId:`input:${id}`,revision:row.revision};
  const handed=[],mounts=[],titleFrames=[],routes=[];
  const origin='http://127.0.0.1:46182',epoch=51;
  const address=new URL(`/mp/th128.html?managedData=1&managedPackage=1&gameGeneration=${generation.id}&runtimeEpoch=${epoch}`,origin);
  let live=true,dataCalls=0;
  const checkCurrent=()=>{signal.throwIfAborted();assert.equal(live,true);};
  const file=async(_generation,id)=>{
    checkCurrent();handed.push(id);
    const row=descriptor.files[id],bytes=await readEntryData(entries.get(row.source),{signal});
    assert.equal(sha(bytes),row.sha256);return {fileId:id,path:row.target,buffer:bytes.slice().buffer,bytes:bytes.length};
  };
  const listeners=new Map(),messages=[];
  const parent={postMessage(row,to){assert.equal(to,origin);messages.push(row);},async __eaglerPrepareManagedRuntimeDataV1(value){
    dataCalls++;assert.deepEqual(value,{game:'th128',generation:generation.id,epoch,packageVersion:'th128-dat-ogg/1'});
    return provideTh128ManagedPackage({generation,epoch,checkCurrent,readData:(g,options)=>{
      assert.equal(options.preferBlob,false);return file(g,name);
    },readResource:file});
  }};
  const document={referrer:`${origin}/launcher.html`},win={parent,document,location:address,
    addEventListener(event,callback){if(!listeners.has(event))listeners.set(event,new Set());listeners.get(event).add(callback);},
    removeEventListener(event,callback){listeners.get(event)?.delete(callback);}};
  const command=async(id,verb,fields={})=>{
    const data={protocol:'eagler-touhou/1',game:'th128',epoch,request:id,command:verb,...fields};
    for(const callback of listeners.get('message')??[])callback({data,source:parent,origin});
    const deadline=performance.now()+3500;
    while(performance.now()<deadline){
      checkCurrent();const reply=messages.find(row=>row.request===id);if(reply){assert.equal(reply.ok,true,JSON.stringify(reply));return reply;}
      await later(4);
    }
    throw Error(`Managed room command ${verb} did not reply`);
  };
  let owner,bridge,preview,title,play=null,replayMenu=null,replayFile=null,replayProof=null;
  const reference=new Map();
  function nativeReceipt(world){return [0,1].map(seat=>({state:sha(world.stateText(seat)),draws:sha(JSON.stringify(world.draws(seat)))}));}
  try{
    owner=await openManagedTh128StageResources({window:win,document,location:address,signal,program,
      loadProgram:async path=>new Uint8Array(await readFile(programPaths[path]))});
    assert.equal(handed.length,12);assert.equal(new Set(handed).size,12);assert.equal(dataCalls,1);
    assert.equal(owner.dataReceipt.sha256,descriptor.files[name].sha256);
    assert.equal(owner.identity.coreSHA256,(program.coopWasm??program.wasm).sha256);assert.equal(owner.revision,descriptor.revision);
    const tracks=await Promise.all(owner.musicTracks.map(async track=>{
      const bytes=await owner.music(track.name);assert.equal(text.decode(bytes.subarray(0,4)),'OggS');
      assert.equal(sha(bytes),track.sha256);assert.equal(bytes.length,track.bytes);
      return track.name;
    }));
    assert.equal(new Set(tracks).size,10);
    const profileKeys=['baseBullet','iceMasks','bullet4','bullet6','bullet9','bullet10','bullet13','bullet16','bullet18','bullet20',
      'bullet21','bullet23','bullet24','bullet25','bullet29','bullet30','bullet32'];
    for(const key of profileKeys)assert.equal(sha(await owner.read(key)),comparison[key],`${key} does not match established native input bytes`);
    await assert.rejects(owner.raw('../th128.dat'),/raw/);
    if(probeCoop){assert.equal(owner.identity.gameplay,'same-stage-coop');return {files:handed.length,tracks:tracks.length,profiles:profileKeys.length,packageRevision:descriptor.revision,onlineFetches:0,results:await probeCoop(owner)};}
    const {default:nativeFactory}=await import(pathToFileURL(programPaths[program.core.url]));
    const factory=observeNativeFactory?async options=>{const module=await nativeFactory(options);observeNativeFactory(module);return module;}:nativeFactory;
    preview=await prepareA13Preview({resources:owner,factory,onFactory,onWorldStart,onTick,signal});
    const loadedBanks=await Promise.all(preview.banks.map(async([bank,asset])=>{
      const metadata=JSON.parse(text.decode(await owner.read(`anim_${asset}`)));
      assert.ok(Array.isArray(metadata.entries)&&metadata.entries.length>0);return bank;
    }));
    if(probeStage){
      const boundary=await probeStage({preview,identity:owner.identity,signal});
      assert.equal(dataCalls,1);checkCurrent();
      return {revision:descriptor.revision,files:handed.length,profiles:profileKeys.length,codeGeneration:owner.identity.generation,
        coreSHA256:owner.identity.coreSHA256,resourceSHA256:owner.identity.resourceSHA256,loadedBanks,boundary,
        simulatedOuterWindow:true,webgl:false,audioPlayback:false,network:false};
    }
    title=await openManagedRoomTitle({resources:owner,factory,onFactory,onTitleTick,signal});
    bridge=mountManagedTh128Room({window:win,document,location:address,identity:owner.identity,
      async verifyMounts(rows){await owner.verifyMounts(rows);mounts.push(rows.length);},
      async presentTitle(){
        for(let tick=0;tick<160&&!title.status().ready;tick++)title.step();
        const menu=title.status(),draws=title.draws();assert.equal(menu.page,0);assert.equal(menu.ready,1);assert.ok(draws.length>0);
        titleFrames.push({draws:draws.length,titleTicks:title.ticks()});return true;
      },
      async launchRoom({room,signal,present}){
        if(room.role==='player'){
          const history=exerciseReplay?await createConfirmedHistory({rules:room.rules,allowedMask:763}):null;
          play=await preview.openPair(room.rules,signal);
          for(const seat of [0,1])assert.equal(play.state(seat).frame,0);
          const heldAt=(frame,seat)=>frame<0?0:frame<4||frame>=20&&frame%60<20?[64,128][seat]:frame%60>=30&&frame%60<44?[128,64][seat]:0;
          for(let frame=0;frame<total;frame++){
            const row={frame,inputs:[0,1].map(player=>{const held=heldAt(frame,player),last=heldAt(frame-1,player);
              return {player,held,pressed:held&~last,released:last&~held,predicted:false};})};
            play.step(row);history?.append(row);if(history&&checkpoints.has(frame+1))reference.set(frame+1,nativeReceipt(play));
            if(frame===3){const left=play.state(0),right=play.state(1);
              assert.deepEqual([left.frame,right.frame,left.playerX,right.playerX],[4,4,-18,18]);}
          }
          if(history){history.seal('restart');replayFile=await makeTh128ReplayFile(history.replayBytes(),owner.build);}
          const left=play.state(0),right=play.state(1);
          assert.deepEqual([left.frame,right.frame],[total,total]);
          assert.equal(left.normalRecord.bound,false);assert.equal(right.normalRecord.bound,false);
          assert.ok(play.draws(0).length>0);routes.push({role:room.role,frames:[left.frame,right.frame],positions:[left.playerX,right.playerX]});
          assert.equal(present(true,'relay'),true); // Test bridge injects a route; no real transport is opened.
        }else {routes.push({role:room.role,frames:[]});assert.equal(present(false),false);}
        return {close(){play?.close();play=null;}};
      },
      disposeResources(){title.close();owner.close();},
    });
    assert.equal(bridge.markReady(),true);
    await command('menu','configure',{music:'ogg',language:'zh'});await command('title','prepare-room');
    const configure=role=>({music:'ogg',language:'zh',options:{netplayMode:'lan',netplayPlayer:role.player??0,netplayPlayerCount:2,
      netplaySeed:34772,netplayDifficulty:1,netplaySpectator:'spectator' in role,netplaySpectatorId:role.spectator??'',
      netplaySpectatorCount:1,netplayIceServers:[],netplayInputDelay:0,netplayAdonisMode:0,netplayPredictionLimit:2,
      netplayLoadouts:[{character:0,shot:0},{character:0,shot:0}],netplayUrl:buildMultiplayerGameplayRelayUrl('ws://127.0.0.1:41182/',{
        product:'th128mp',roomCode:'34772',runId:2,role,memberId:'owned-member',runtimeGeneration:owner.identity.generation})}});
    await command('player','configure',configure({player:0}));await command('start-player','launch',{launchSerial:51});
    assert.equal((await command('return-player','return-room')).presented,true);assert.equal(play,null);
    if(exerciseReplay){
      let views=0,restores=0,backgroundTicks=0;const matched=new Set();
      replayMenu=createManagedReplayMenu({build:owner.build,signal,
        async openWorld(wire,childSignal){
          const pair=await preview.openPair(createMultiplayerRules(wire),childSignal);
          try{
            const {module,std}=await preview.openBackground();childSignal.throwIfAborted();
            const background=new StageBackground(parseStd(std),module),step=managedSceneStep(pair,background,{confirmedOnly:true});
            return {background,engine:{...pair,step(row){step(row);backgroundTicks++;}}};
          }catch(error){pair.close();throw error;}
        },
        render({engine,background},seat,frame){views++;assert.ok([0,1].includes(seat));assert.equal(background.frame,frame);
          if(checkpoints.has(frame)){assert.deepEqual(nativeReceipt(engine),reference.get(frame));matched.add(frame);}},
        async restoreTitle(){restores++;assert.equal(title.status().ready,1);assert.ok(title.draws().length>0);return true;}});
      assert.equal(await replayMenu.open(replayFile),false);replayMenu.enableTitle();assert.equal(await replayMenu.open(replayFile),true);
      await replayMenu.act('play');replayMenu.advance(0);replayMenu.advance(1000/30);
      assert.equal(replayMenu.status().playback.frame,2);await replayMenu.act('seat',1);
      let timestamp=1000/30;
      if(total===4){await replayMenu.act('seek',4);replayMenu.advance(timestamp+=1000/60);}
      else for(let frame=3;frame<=total;frame++)replayMenu.advance(timestamp+=1000/60);
      assert.equal(replayMenu.status().playback.phase,'ended');
      await replayMenu.act('seek',1);replayMenu.advance(timestamp+=1000/60);assert.equal(replayMenu.status().playback.frame,1);
      assert.equal(await replayMenu.returnToTitle(),true);assert.equal(replayMenu.status().playback,null);assert.equal(restores,1);
      replayMenu.suspend();assert.equal(await replayMenu.open(replayFile),false);
      assert.deepEqual([...matched].sort((a,b)=>a-b),[...checkpoints]);
      await onReplayFixture(replayFile,owner.build);
      replayProof={file:replayFile.name,sha256:sha(new Uint8Array(await replayFile.arrayBuffer())),frames:total,views,restores,backgroundTicks,
        matchedPositions:[...checkpoints],generatedManagedFixture:true,deviceFileChooser:false};
    }
    await command('menu2','configure',{music:'none',language:'zh'});await command('watch','configure',configure({spectator:'local-watcher'}));
    await command('start-watch','launch',{launchSerial:52});await command('return-watch','return-room');
    assert.deepEqual(messages.filter(row=>row.event==='first-frame').map(row=>row.launchSerial),[51]);
    assert.deepEqual(messages.filter(row=>row.event==='room-menu').map(row=>row.launchSerial),[51,52]);
    assert.equal(dataCalls,1);assert.equal(titleFrames.length,3);assert.deepEqual(titleFrames[0],titleFrames[2]);
    await command('exit','exit');assert.equal(bridge.phase,'exited');expectDisposed(owner);
    return {revision:descriptor.revision,files:handed.length,tracks,profiles:profileKeys.length,codeGeneration:owner.identity.generation,
      resourceSHA256:owner.identity.resourceSHA256,readyEvents:messages.filter(row=>row.event==='ready').length,loadedBanks,titleFrames,routes,
      simulatedOuterWindow:true,webgl:false,audioPlayback:false,network:false,archiveRebuilt:!!replayProof,...(replayProof?{replayProof}:{})};
  }finally{
    replayMenu?.close();play?.close();await bridge?.dispose();title?.close();owner?.close();live=false;
  }
}

function expectDisposed(owner){assert.throws(()=>owner.raw('ascii.anm'),error=>error.name==='AbortError');}
