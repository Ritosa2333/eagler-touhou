import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {setTimeout as sleep} from 'node:timers/promises';

const protocol='eagler-touhou/1',code='a'.repeat(64),layout=`sha256-${'58cd45fe7cac9af281d9de993a527ccfbbe6f27b89bcd0ca98a7031333fce200'}`;
const entry=`runtime/th128/multiplayer/${code}/th128.html`,normal=`runtime/th128/${'b'.repeat(64)}/th128.html`,language='lang_zh-hans';
const profile='th128-multiplayer-local-preview';
const sha=data=>createHash('sha256').update(data).digest('hex');

function packageFiles(){
  const files={},objects={},payload=new Map(),ogg=[];
  const add=(id,target,number)=>{
    const data=Uint8Array.of(79,number),digest=sha(data),revision=digest.slice(0,16);
    files[id]={source:`fixture/${id}`,target,bytes:data.length,revision,sha256:digest};
    objects[id]={objectId:`owned-${id}`,revision};payload.set(id,data);
  };
  add('game-data','/th128.dat',0);add('language','/__eagler/language/lang_zh-hans.zip',1);
  for(const [index,track] of ['00','01','02','05','06','07','08','09','10','11'].entries()){
    const id=`ogg${track}`;add(id,`/music/th128_${track}.ogg`,index+2);ogg.push(id);
  }
  return {payload,generation:{id:'installed-th128-mp',game:'th128',files:objects,descriptor:{
    schema:'eagler-touhou/package/1',game:'th128',revision:'fixture-1',files,
    runtimeRequirement:{protocol,target:'th128',dataFile:'game-data',dataLayout:layout},base:{files:['game-data']},
    components:{ogg:{type:'ogg',files:ogg},language:{type:'language',entries:[{id:language,file:'language'}]}},
  }}};
}
function hostManifest(generation){
  const {bytes,sha256}=generation.descriptor.files['game-data'];
  return {schema:'eagler-touhou/site-manifest/1',protocol,profile,shared:{resourceMode:'import',testBuild:true,runtimeManifest:'runtime-manifest.json'},games:{
    th128:{runtime:normal,multiplayerRuntime:entry,gameData:{path:'th128.dat',bytes,sha256,version:`sha256-${sha256}`,layout},
      music:{midi:{files:[],supported:false},ogg:null},features:{thprac:true},languages:[],languageOptions:[{id:language,title:'简体中文',pack:null}],
      offlineCompatibility:{schema:'eagler-touhou/offline-game-pack/1',runtimeCompatibility:{protocol,dataLayout:layout,versionSource:'offline-pack'},
        requiredShared:[],languages:{source:'offline-pack',baseline:[language]}},
    },
  }};
}

function makeFixture(api,{origin='http://127.0.0.1:49182',replacement=null}={}){
  const baseUrl=`${origin}/deploy/`,{generation,payload}=packageFiles(),checks=[],messages=new Set(),navigations=[],outbound=[],programs=[],reads=[];
  const host={location:{origin,href:baseUrl},addEventListener(name,fn){assert.equal(name,'message');messages.add(fn);},
    removeEventListener(_name,fn){messages.delete(fn);}};
  const child={document:{},location:{href:'about:blank',replace(url){this.href=url;child.document={};navigations.push(url);}},
    postMessage(data,target){assert.equal(target,origin);outbound.push(data);},addEventListener(){},removeEventListener(){}};
  const frame={contentWindow:child,isConnected:true,addEventListener(){},removeEventListener(){}};
  function file(selected,id){
    assert.equal(selected.id,generation.id);reads.push(id);const data=payload.get(id);
    return {fileId:id,path:generation.descriptor.files[id].target,bytes:data.length,buffer:data.slice().buffer};
  }
  const service=api.createRuntimeService({hostWindow:host,frame,baseUrl,worker:null,translate:key=>key,
    fetchImpl:async()=>assert.fail('no network in the owned parent service fixture'),
    dependencies:{async prepareCode(url,options){programs.push({url,expected:options.expectedGeneration,requireCurrent:options.requireCurrent});
      return replacement??{url,generation:code};},async readData(selected){return file(selected,'game-data');},async readResource(selected,id){return file(selected,id);},
      async retainGeneration(){},async releaseGeneration(){},
    }});
  const plan={game:'th128',runtimeVariant:'multiplayer',entry,publishedRuntime:true,generation,
    runtimeHost:hostManifest(generation),configure:{music:'none',language:'zh',options:{thpracEnabled:false}}};
  function emit(data){const current=service.getSnapshot();for(const fn of messages)fn({source:child,origin,data:{protocol,game:'th128',epoch:current.epoch,...data}});}
  async function until(predicate){const deadline=performance.now()+1600;do{const value=predicate();if(value)return value;await sleep(3);}while(performance.now()<deadline);throw Error('Launcher fixture did not reach its expected boundary');}
  function ack(message,extra={}){emit({request:message.request,ok:true,...extra});}
  const command=(name,after=0)=>until(()=>outbound.slice(after).find(row=>row.command===name));
  function dispose(){if(navigations.length&&!service.getSnapshot().launched)service.cancel();service.dispose();}
  return {baseUrl,service,child,host,plan,checks,emit,ack,until,command,navigations,outbound,programs,reads,dispose};
}

export async function verifyTh128MultiplayerParent(api){
  const result=[];
  const f=makeFixture(api);
  try{
    const plan=f.plan,accepted=api.validateSiteManifest(plan.runtimeHost);
    assert.equal(accepted.games.th128.multiplayerRuntime,entry);
    assert.equal(api.productEnabledForBuild('th128mp',false),false,'the candidate must not enter the public catalog');
    assert.equal(api.productEnabledForBuild('th128mp',true),false,'the isolated child is not registered as a finished product');
    if (api.multiplayerConfigForProduct) {
      const policy=api.multiplayerConfigForProduct('th128mp');
      assert.equal(api.PRODUCT_IDS.includes('th128mp'),false);
      assert.equal(api.gameIdForProduct('th128mp'),'th128');assert.equal(policy.gameplay,'parallel');
      assert.deepEqual(policy.difficulties,['Normal']);assert.deepEqual(policy.playerCounts,[2]);assert.equal(policy.spectator,true);
      const buildRoom=overrides=>api.buildMultiplayerRuntimeOptions({url:'ws://127.0.0.1:49920/?room=th128mp-34772',
        player:0,playerCount:2,seed:34772,difficulty:0,spectator:false,spectatorId:'',spectatorCount:0,iceServers:[],
        loadouts:[{character:0,shot:0},{character:0,shot:0}],...overrides},policy);
      assert.equal(buildRoom().netplayDifficulty,1,'the native TH128 rank index is distinct from the room Normal option');
      assert.throws(()=>buildRoom({difficulty:1}),/0–0/);
    }
    result.push('explicit local preview Host admits a pinned child without registering a public product');
    let initial;
    const preparing=api.prepareGame({productId:'th128mp',baseUrl:f.baseUrl,translate:key=>key,runtime:f.service,
      multiplayer:{kind:'room-preload',runtimeGeneration:code},touchLayout:null,
      settings:{gameId:'th128',gameVersion:'original',music:'none',musicPreference:'none',language,
        context:{productId:'th128mp',musicAvailability:{audio:false}},options:{thpracEnabled:true}},
      acquisition:{getMetadata:()=>({siteManifest:accepted,releaseCatalog:null}),async acquire(request){
        assert.equal(request.productId,'th128mp');assert.equal(request.updateBeforePrepare,true);
        return {kind:'managed',generation:plan.generation,deferredUpdate:null};
      }},prepareMidi:async()=>assert.fail('TH128 does not start MIDI'),onRuntimePlan(value){initial=value;},
    });void preparing.catch(()=>{});
    await Promise.race([preparing, f.until(()=>f.navigations[0])]);
    assert.deepEqual(initial.runtimeHost,accepted,'the installed-package path must carry the validated Host');
    assert.deepEqual(initial.multiplayerProgram,{generation:code});
    assert.deepEqual(f.programs,[{url:new URL(entry,f.baseUrl).href,expected:code,requireCurrent:true}]);
    const before=f.service.getSnapshot(),navigation=new URL(f.navigations[0]);
    assert.equal(navigation.searchParams.get('managedPackage'),'1');assert.equal(navigation.searchParams.get('runtimeVariant'),'multiplayer');
    assert.equal(navigation.searchParams.get('runtimeEpoch'),String(before.epoch));
    const provide=f.host.__eaglerPrepareManagedRuntimeDataV1;
    const request={game:'th128',generation:plan.generation.id,epoch:before.epoch,packageVersion:'th128-dat-ogg/1'};
    await assert.rejects(provide({...request,epoch:before.epoch+1}),/SUPERSEDED/);
    const receipt=await provide(request);assert.equal(receipt.managedPackage.generation,plan.generation.id);
    assert.equal(receipt.managedPackage.epoch,before.epoch);assert.equal(receipt.fileId,'game-data');
    for(const id of Object.keys(plan.generation.descriptor.files).filter(id=>id!=='game-data'))assert.equal((await receipt.managedPackage.readFile(id)).fileId,id);
    assert.equal(new Set(f.reads).size,12);assert.equal(f.reads.length,12);
    result.push('actual Launcher service delivers the complete managed Package only to the owned navigation epoch');
    f.emit({event:'ready',roomPreview:true,roomReturn:true});const configure=await f.command('configure');
    assert.notEqual(configure.options?.thpracEnabled,true,'the single-player preference cannot enable training in multiplayer');
    f.ack(configure);const prepared=await preparing;assert.deepEqual(prepared.plan.runtimeHost,accepted);
    assert.equal(f.service.getSnapshot().codeGeneration,code);assert.equal(f.service.getSnapshot().phase,'prepared');
    const preview=f.service.prepareRoomPreview(before.epoch);f.ack(await f.command('prepare-room'),{presented:true});await preview;
    assert.equal(f.service.getSnapshot().roomPreview,true);assert.equal(f.service.getSnapshot().firstFrame,false);
    const offset=f.outbound.length;
    const room={netplayMode:'lan',netplaySpectator:false,netplayPlayer:0,netplayPlayerCount:2,netplaySpectatorId:'',
      netplaySeed:34772,netplayDifficulty:1,netplayLoadouts:[{character:0,shot:0},{character:0,shot:0}],
      netplayUrl:`ws://127.0.0.1:49920/?room=th128mp-34772&run=1&member=owned-member&player=0&runtimeGeneration=${code}`};
    const mismatched={...room,netplayUrl:room.netplayUrl.replace(code,'b'.repeat(64))};
    await assert.rejects(f.service.configurePreparedRoom(before.epoch,mismatched),/generation/);
    const ambiguous={...room,netplayUrl:`${room.netplayUrl}&runtimeGeneration=${code}`};
    await assert.rejects(f.service.configurePreparedRoom(before.epoch,ambiguous),/generation/);
    await assert.rejects(f.service.configurePreparedRoom(before.epoch,{...room,
      netplayUrl:room.netplayUrl.replace('ws://127.0.0.1:49920','wss://public.invalid')}),/generation/);
    assert.equal(f.outbound.length,offset);assert.equal(f.service.getSnapshot().phase,'prepared');
    const configured=f.service.configurePreparedRoom(before.epoch,room);const routed=await f.command('configure',offset);f.ack(routed);await configured;
    const launch=f.service.launch({awaitFirstFrame:true});const ready=await f.command('launch');
    assert.equal(ready.launchSerial,1);f.ack(ready);f.emit({event:'first-frame',launchSerial:2,th128RoomPath:'rtc'});
    assert.equal(f.service.getSnapshot().firstFrame,false);
    f.emit({event:'first-frame',launchSerial:1});assert.equal(f.service.getSnapshot().firstFrame,false);
    f.emit({event:'first-frame',launchSerial:1,th128RoomPath:'spectator-relay'});
    assert.equal(f.service.getSnapshot().firstFrame,false);
    f.emit({event:'first-frame',launchSerial:1,th128RoomPath:'rtc'});await launch;
    assert.equal(f.service.getSnapshot().firstFrame,true);
    assert.equal(f.service.getSnapshot().th128RoomPath,'rtc');
    f.emit({event:'first-frame',launchSerial:1,th128RoomPath:'relay'});
    assert.equal(f.service.getSnapshot().th128RoomPath,'rtc');
    result.push('actual author parent keeps prepare, room configuration, launch serial and first-frame boundaries');
    const document=f.child.document, beforeReturn=f.outbound.length;
    const returning=f.service.returnToRoom();
    f.ack(await f.command('sync',beforeReturn));
    f.ack(await f.command('return-room',beforeReturn),{presented:true});
    const restored=await f.command('configure',beforeReturn);
    assert.equal(restored.options.netplayUrl,undefined);f.ack(restored);await returning;
    assert.equal(f.service.getSnapshot().epoch,before.epoch);
    const profile=f.service.getSnapshot();
    assert.equal(profile.roomPreview,true);assert.equal(profile.launched,false);assert.equal(profile.th128RoomPath,undefined);
    assert.equal(f.child.document,document);
    assert.equal(f.navigations.length,1);
    const nextOffset=f.outbound.length, spectatorUrl=new URL(room.netplayUrl);
    spectatorUrl.searchParams.delete('player');spectatorUrl.searchParams.set('spectator','owned-spectator');
    const spectator={...room,netplaySpectator:true,netplaySpectatorId:'owned-spectator',netplayUrl:spectatorUrl.href};
    const rejoin=f.service.configurePreparedRoom(before.epoch,spectator);
    const nextConfig=await f.command('configure',nextOffset);assert.equal(nextConfig.options.netplaySpectator,true);f.ack(nextConfig);await rejoin;
    const relaunch=f.service.launch({awaitFirstFrame:true}), nextLaunch=await f.command('launch',nextOffset);
    assert.equal(nextLaunch.launchSerial,2);f.ack(nextLaunch);f.emit({event:'first-frame',launchSerial:1,th128RoomPath:'spectator-relay'});
    assert.equal(f.service.getSnapshot().firstFrame,false);
    f.emit({event:'first-frame',launchSerial:2,th128RoomPath:'rtc'});
    assert.equal(f.service.getSnapshot().firstFrame,false);
    f.emit({event:'first-frame',launchSerial:2,th128RoomPath:'spectator-relay'});await relaunch;
    assert.equal(f.service.getSnapshot().th128RoomPath,'spectator-relay');
    assert.equal(f.service.getSnapshot().spectator,true);assert.equal(f.child.document,document);
    assert.equal(f.navigations.length,1);assert.equal(f.reads.length,12);
    result.push('same child returns to the room and relaunches as observer without a new Package read');
    assert.equal(await f.service.close({discardUnsaved:true}),true);
    await assert.rejects(receipt.managedPackage.readFile('language'),/SUPERSEDED/);
    assert.equal(f.navigations.at(-1),'about:blank');
    result.push('retiring the child revokes the previous Package reader');
  }finally{f.dispose();}

  for(const change of ['profile','test-build','public-origin','no-host','changed-entry','changed-program','redirected-program','foreign-room','file-only']){
    const f=makeFixture(api,{...(change==='public-origin'?{origin:'https://www.example.invalid'}:{}),
      ...(change==='changed-program'?{replacement:{url:'http://127.0.0.1:49182/deploy/'+entry,generation:'b'.repeat(64)}}:{}),
      ...(change==='redirected-program'?{replacement:{url:'http://127.0.0.1:49182/deploy/'+normal,generation:code}}:{})});
    try{
      if(change==='profile')f.plan.runtimeHost.profile='production';
      if(change==='test-build')f.plan.runtimeHost.shared.testBuild=false;
      if(change==='no-host')delete f.plan.runtimeHost;
      if(change==='changed-entry')f.plan.entry+='?other=1';
      if(change==='foreign-room')f.plan.configure.options={netplayMode:'lan',netplayUrl:`ws://127.0.0.1:49920/?runtimeGeneration=${'b'.repeat(64)}`};
      if(change==='file-only')f.plan.fileOnly=true;
      await assert.rejects(f.service.prepare(f.plan),/TH128.*(?:multiplayer|generation)/);
      assert.ok(f.navigations.every(url=>url==='about:blank'),change);assert.equal(f.reads.length,0);
    }finally{f.dispose();}
  }
  for(const change of ['profile','test-build','unversioned-entry','no-runtime-manifest']){
    const manifest=hostManifest(packageFiles().generation);
    if(change==='profile')manifest.profile='production';
    if(change==='test-build')manifest.shared.testBuild=false;
    if(change==='unversioned-entry')manifest.games.th128.multiplayerRuntime='runtime/th128/multiplayer/th128.html';
    if(change==='no-runtime-manifest')delete manifest.shared.runtimeManifest;
    assert.throws(()=>api.validateSiteManifest(manifest),/invalid Site Manifest games|immutable generation/,change);
  }
  result.push('ordinary, foreign and mismatched entries receive no navigation or Package data');
  return result;
}
