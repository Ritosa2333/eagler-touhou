import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {createRequire} from 'node:module';
import {mkdtemp,mkdir,readFile,rm,writeFile} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {resolve} from 'node:path';
import test from 'node:test';
import {runInNewContext} from 'node:vm';
import {PACKAGE_DESCRIPTOR_SCHEMA,canonicalPackagePayload} from '../../launcher/package/package-descriptor.mjs';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';
import {assessPackageInstallationAuthority,classifyPackageAuthorityPrompt,snapshotPackageRuntime,createPublishedPackageFixture,INSTALLATION_AUTHORITY_JOURNAL_SCHEMA} from './offline-installation-authority.mjs';

const {strToU8,zipSync}=createRequire(new URL('../../launcher/package.json',import.meta.url))('fflate');
const sha256=bytes=>createHash('sha256').update(bytes).digest('hex');
const game='th08',other='th07';

async function withTemp(callback){
  const directory=await mkdtemp(resolve(tmpdir(),'eagler-package-authority-'));
  try{return await callback(directory);}finally{await rm(directory,{recursive:true,force:true});}
}

function packageZip({revisionSuffix='next',filesExtra={}}={}){
  const data=Buffer.from('same private game data'),shared=Buffer.from('shared font'),ogg=Buffer.from('updated soundtrack');
  const files={
    'game-data':{source:`games/${game}/th08.dat`,target:PRODUCT_GAMES[game].package.dataTarget,revision:'data-r1',bytes:data.length,sha256:sha256(data)},
    'shared-msgothic':{source:'shared/msgothic.ttc',target:'/msgothic.ttc',revision:'font-r1',bytes:shared.length,sha256:sha256(shared)},
    'ogg:th08':{source:`games/${game}/music/ogg/th08.ogg`,target:'/bgm/th08.ogg',revision:`ogg-${revisionSuffix}`,bytes:ogg.length,sha256:sha256(ogg)},
    ...filesExtra,
  };
  const descriptor={schema:PACKAGE_DESCRIPTOR_SCHEMA,game,revision:'pending',runtimeRequirement:{protocol:'eagler-touhou/1',target:game,
      dataFile:PRODUCT_GAMES[game].package.dataFileId,dataLayout:'fixture-layout'},files,base:{files:['game-data','shared-msgothic']},
    components:{ogg:{type:'ogg',files:['ogg:th08']}}};
  descriptor.revision=createHash('sha256').update(canonicalPackagePayload(descriptor)).digest('hex').slice(0,16);
  const payloads={
    [files['game-data'].source]:data,[files['shared-msgothic'].source]:shared,[files['ogg:th08'].source]:ogg,
  };
  return{descriptor,bytes:zipSync({'package.json':strToU8(JSON.stringify(descriptor)),
    ...Object.fromEntries(Object.entries(payloads).map(([path,bytes])=>[path,Uint8Array.from(bytes)]))},{level:0})};
}

function read({phase,store,key,result,sequence=1,mode='readonly'}){return{sequence,phase,store,key,mode,result,error:null};}
function validJournal({gameId=game,base='base-1',baseRevision='base-r1',updated='next-2',updatedRevision='next-r2',nonSelected=other}={}){
  return{schema:INSTALLATION_AUTHORITY_JOURNAL_SCHEMA,overflow:false,pending:0,reads:[
    read({phase:'post-import-card-reopen',store:'installations',key:gameId,result:{game:gameId,currentGeneration:base}}),
    read({phase:'post-import-card-reopen',store:'generations',key:[gameId,base],result:{id:base,game:gameId,revision:baseRevision}}),
    read({phase:'post-import-card-reopen',store:'installations',key:nonSelected,result:null}),
    read({phase:'resource-preparation-and-update-prompt',store:'installations',key:gameId,result:{game:gameId,currentGeneration:base}}),
    read({phase:'resource-preparation-and-update-prompt',store:'generations',key:[gameId,base],result:{id:base,game:gameId,revision:baseRevision}}),
    read({phase:'update-ready-checkpoint',store:'installations',key:gameId,result:{game:gameId,currentGeneration:updated,pendingGeneration:null}}),
    read({phase:'update-ready-checkpoint',store:'generations',key:[gameId,updated],result:{id:updated,game:gameId,revision:updatedRevision}}),
    read({phase:'published-update-commit',store:'installations',key:gameId,result:{game:gameId,currentGeneration:updated}}),
    read({phase:'published-update-commit',store:'generations',key:[gameId,updated],result:{id:updated,game:gameId,revision:updatedRevision}}),
    read({phase:'offline-reopen',store:'installations',key:gameId,result:{game:gameId,currentGeneration:updated}}),
    read({phase:'offline-reopen',store:'generations',key:[gameId,updated],result:{id:updated,game:gameId,revision:updatedRevision}}),
    read({phase:'offline-resource-preparation',store:'installations',key:gameId,result:{game:gameId,currentGeneration:updated}}),
    read({phase:'offline-resource-preparation',store:'generations',key:[gameId,updated],result:{id:updated,game:gameId,revision:updatedRevision}}),
  ]};
}
const prompt={observed:true,confirmed:true,publishedRevision:'next-r2',currentGenerationAtPrompt:'next-2',
  currentRevisionAtPrompt:'next-r2',pinnedRuntimeGenerationAtPrompt:'base-1',pinnedRuntimeEpoch:1,pendingGenerationAtPrompt:null};
const passArgs={journal:validJournal(),game,nonSelectedGame:other,baseGenerationId:'base-1',baseRevision:'base-r1',
  updatedGenerationId:'next-2',updatedRevision:'next-r2',finalRuntimeGenerationId:'next-2',offlineRuntimeGenerationId:'next-2',updatePrompt:prompt};

test('PACKAGE-06 distinguishes the real MIDI launch consent from the later published update decision',()=>{
  assert.equal(classifyPackageAuthorityPrompt({message:'将会使用 MIDI 播放音乐。\n如果想听正常的音乐，请在音乐中选择 OGG。',confirm:'仍要启动'}),'midi-launch-consent');
  assert.equal(classifyPackageAuthorityPrompt({message:'The game will continue with MIDI music.\nIf you want normal music playback, choose OGG under Music.',confirm:'Start anyway'}),'midi-launch-consent');
  assert.equal(classifyPackageAuthorityPrompt({message:'新版已下载并校验完成。立即重启会先保存并结束当前游戏，再使用新版启动。也可以继续游戏，下次启动时使用新版。',confirm:'立即重启'}),'published-package-update-ready');
  assert.equal(classifyPackageAuthorityPrompt({message:'The update has been downloaded and verified. Restart now to save and end the current game, then launch the new version. You can also keep playing and use it on the next launch.',confirm:'Restart now'}),'published-package-update-ready');
  assert.equal(classifyPackageAuthorityPrompt({message:'将会使用 MIDI 播放音乐。',confirm:'立即更新'}),null);
  assert.equal(classifyPackageAuthorityPrompt({message:'服务器有新版游戏资源。',confirm:'仍要启动'}),null);
  assert.equal(classifyPackageAuthorityPrompt({message:'Unknown decision',confirm:'确认'}),null);
});

test('published fixture uses the real second ZIP descriptor and publishes its declared files on the site origin',async()=>withTemp(async temporary=>{
  const site=resolve(temporary,'site');await mkdir(site);await writeFile(resolve(site,'release-catalog.json'),JSON.stringify({schema:'eagler-touhou/release-catalog/1',games:{}}));
  const archive=packageZip();const archivePath=resolve(temporary,'update.zip');await writeFile(archivePath,archive.bytes);
  const fixture=await createPublishedPackageFixture({siteDirectory:site,game,archivePath,expectedRevision:archive.descriptor.revision});
  assert.equal(fixture.revision,archive.descriptor.revision);
  assert.equal(fixture.files.length,Object.keys(archive.descriptor.files).length);
  const catalog=JSON.parse(await readFile(resolve(site,'release-catalog.json'),'utf8'));
  assert.deepEqual(catalog.games[game],{revision:archive.descriptor.revision,descriptor:fixture.descriptor.path});
  assert.deepEqual(await readFile(resolve(site,'games/th08/th08.dat')),Buffer.from('same private game data'));
  assert.ok((await readFile(resolve(site,fixture.descriptor.path),'utf8')).includes(archive.descriptor.revision));
  for(const file of Object.values(archive.descriptor.files)){
    const resolvedUrl=new URL(file.source,new URL(fixture.descriptor.path,'https://fixture.invalid/'));
    const bytes=await readFile(resolve(site,resolvedUrl.pathname.slice(1)));
    assert.equal(sha256(bytes),file.sha256,'Actual installer relative URL must reach the declared file');
  }
}));

test('interruption fixture serves real files without publishing a background update',async()=>withTemp(async temporary=>{
  const site=resolve(temporary,'site');await mkdir(site);const catalog=JSON.stringify({schema:'eagler-touhou/release-catalog/1',games:{}});
  await writeFile(resolve(site,'release-catalog.json'),catalog);const archive=packageZip(),archivePath=resolve(temporary,'update.zip');await writeFile(archivePath,archive.bytes);
  const fixture=await createPublishedPackageFixture({siteDirectory:site,game,archivePath,expectedRevision:archive.descriptor.revision,publishCatalog:false});
  assert.equal(fixture.catalog,null);assert.equal(await readFile(resolve(site,'release-catalog.json'),'utf8'),catalog);
  assert.ok((await readFile(resolve(site,fixture.descriptor.path))).length>0);
}));

test('published fixture rejects wrong revision, cross-game identity and unsafe resource routes',async()=>withTemp(async temporary=>{
  const site=resolve(temporary,'site');await mkdir(site);await writeFile(resolve(site,'release-catalog.json'),JSON.stringify({schema:'eagler-touhou/release-catalog/1',games:{}}));
  const archive=packageZip(),archivePath=resolve(temporary,'update.zip');await writeFile(archivePath,archive.bytes);
  await assert.rejects(createPublishedPackageFixture({siteDirectory:site,game,archivePath,expectedRevision:'stale-revision'}),/differs from the selected update manifest/);
  const wrong=packageZip({filesExtra:{'outside':{source:'games/../../outside.dat',target:'/outside.dat',revision:'unsafe',bytes:0,sha256:sha256(Buffer.alloc(0))}}});
  const wrongPath=resolve(temporary,'unsafe.zip');await writeFile(wrongPath,wrong.bytes);
  await assert.rejects(createPublishedPackageFixture({siteDirectory:site,game,archivePath:wrongPath,expectedRevision:wrong.descriptor.revision}),/unsafe source path|Unsafe Published Package source|missing declared Package file/);
}));

test('PACKAGE-06 binds Launcher reads, update prompt and both Runtime generations to canonical selected-game rows',()=>{
  const result=assessPackageInstallationAuthority(passArgs);
  assert.equal(result.available,true);assert.equal(result.valid,true);assert.equal(result.updated.generationId,'next-2');
});

test('PACKAGE-06 refuses cached hints, wrong-game reads, absent generation rows and stale Runtime generations',()=>{
  assert.equal(assessPackageInstallationAuthority({...passArgs,journal:null,hint:{generationId:'next-2'}}).available,false,
    'A cached hint cannot substitute for a canonical IndexedDB read journal');
  const wrongGame=validJournal({gameId:'th10'});
  assert.equal(assessPackageInstallationAuthority({...passArgs,journal:wrongGame}).valid,false);
  const missingGeneration=validJournal();missingGeneration.reads=missingGeneration.reads.filter(item=>
    !(item.phase==='published-update-commit'&&item.store==='generations'));
  assert.equal(assessPackageInstallationAuthority({...passArgs,journal:missingGeneration}).valid,false);
  assert.equal(assessPackageInstallationAuthority({...passArgs,updatePrompt:{...prompt,currentGenerationAtPrompt:'base-1'}}).valid,false);
  assert.equal(assessPackageInstallationAuthority({...passArgs,updatePrompt:{...prompt,pinnedRuntimeGenerationAtPrompt:'next-2'}}).valid,false);
  const pending=validJournal();pending.reads.find(r=>r.phase==='update-ready-checkpoint'&&r.store==='installations').result.pendingGeneration='still-pending';
  assert.equal(assessPackageInstallationAuthority({...passArgs,journal:pending}).valid,false);
  assert.equal(assessPackageInstallationAuthority({...passArgs,finalRuntimeGenerationId:'base-1'}).valid,false);
  assert.equal(assessPackageInstallationAuthority({...passArgs,offlineRuntimeGenerationId:'base-1'}).valid,false);
});

test('PACKAGE-06 treats overflow, unfinished reads, nonselected installs and a repeated generation as incomplete',()=>{
  assert.equal(assessPackageInstallationAuthority({...passArgs,journal:{...validJournal(),overflow:true}}).valid,false);
  assert.equal(assessPackageInstallationAuthority({...passArgs,journal:{...validJournal(),pending:1}}).valid,false);
  const contaminated=validJournal();contaminated.reads.find(item=>item.phase==='post-import-card-reopen'&&item.store==='installations'&&item.key===other).result={game:other,currentGeneration:'wrong'};
  assert.equal(assessPackageInstallationAuthority({...passArgs,journal:contaminated}).valid,false);
  assert.equal(assessPackageInstallationAuthority({...passArgs,updatedGenerationId:'base-1'}).valid,false);
});

test('Runtime receipt separates accumulated epochs while retaining duplicate reads in one epoch',()=>{
  const data={game,epoch:2,generation:'next-2',bytes:10,sha256:'a'.repeat(64)};
  const prior={...data,epoch:1,generation:'base-1'};
  const events=[1,2].flatMap(epoch=>['ready','first-frame'].map(event=>({game,epoch,event})));
  const page={URL,document:{getElementById:()=>({src:'https://localhost/runtime/th08/game.html?runtimeEpoch=2'})},
    location:{href:'https://localhost/'},navigator:{onLine:true,serviceWorker:{controller:{}}},
    window:{__offlineRuntimeEvents:events,__offlineManagedDataReads:[prior,data]}};
  const snapshot=runInNewContext(`(${snapshotPackageRuntime.toString()})`,page);
  const value=snapshot({game,generation:'next-2',expectedData:data});
  assert.equal(value.ready.epoch,2);assert.equal(value.first.epoch,2);
  assert.equal(value.dataReads.length,1);assert.equal(value.dataRead,data);
  page.window.__offlineManagedDataReads.push({...data});
  assert.equal(snapshot({game,generation:'next-2',expectedData:data}).dataReads.length,2);
});
