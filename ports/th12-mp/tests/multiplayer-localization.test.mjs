import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {runInNewContext} from 'node:vm';
import {prepareLocalization,isLocalizationResourcePath} from '../src/localization/loader.mjs';
import {managedCommandHandler} from '../src/managed-shell.mjs';
import {RuntimeAssets} from '../src/managed-data.mjs';
import {messageTextPoses} from '../src/renderer.mjs';
import {spellTitleDescriptors} from '../src/spell-presentation.mjs';
import {multiplayerSetupWords} from '../src/multiplayer.mjs';
import {th12MessagePatch} from '../../../launcher/server/th12-thcrap-compiler.mjs';
import {createStaticThcrapPack} from '../../../launcher/server/thcrap-static-pack.mjs';

const command=(time,op,payload=Buffer.alloc(0))=>{
  const header=Buffer.from([time&255,time>>8,op,payload.length]);return Buffer.concat([header,payload]);
};
const encrypt=text=>{
  const bytes=Buffer.from(text+'\0');let key=0x77,step=7;
  for(let i=0;i<bytes.length;i++){bytes[i]^=key;key=(key+step)&255;step=(step+16)&255;}return bytes;
};
function fixture(){
  const header=Buffer.alloc(12);header.writeUInt32LE(1);header.writeUInt32LE(12,4);
  return Buffer.concat([header,command(0,7),command(0,17,encrypt('one')),command(0,17,encrypt('two')),
    command(0,11,Buffer.from([60,0,0,0])),command(10,8),command(10,17,encrypt('again')),
    command(10,11,Buffer.from([60,0,0,0])),command(11,0)]);
}
function pack(original){
  const result=th12MessagePatch(original,{'0':{'0_0':{lines:['|前缀,名字,注音','第一句','第二句']},'10_0':{lines:['后来一句']}}});
  const overlay={schema:'th12-localization-overlay/2',game:'th12',language:'lang_zh-hans',sourceKind:'thcrap',
    sourceExeSha256:'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417',
    dialogue:{'st01_00a.msg':result.lookup},dialoguePatches:{'st01_00a.msg':result.patch},endings:{},endingPatches:{},
    spells:{0:{sourceHex:'7370656c6c',text:'中文符卡',byteLength:8}},textures:{},musicComments:{}};
  const bytes=Buffer.from(JSON.stringify(overlay));
  const built=createStaticThcrapPack({pack:{game:'th12',language:'lang_zh-hans',assets:[]},
    resources:[{targetPath:'/thcrap/th12/overlay.json',bytes}]});
  return {language:overlay.language,manifest:built.manifest,files:[{path:'/thcrap/th12/overlay.json',bytes}],result};
}
const atlases={text:{sprites:Array.from({length:4},()=>({w:354,h:17}))}};
const pose=slot=>({a:{name:'text',sprite:slot,visible:true,alpha:1},x:56,y:373+22*(slot%2)});

test('preloaded Chinese text and spell resources survive pack-free room binding, return and replay configuration',async()=>{
  const original=fixture(),runtimePack=pack(original),assets=new RuntimeAssets({location:{href:'https://local.invalid/?managedData=1'}});
  assets.fonts.set('/unifont.otf',new Uint8Array([1]));assets.content={fetch:async()=>new Response(original)};
  const source=readFileSync(new URL('../site/game.mjs',import.meta.url),'utf8');
  const start=source.indexOf('configure:async settings=>{')+'configure:'.length,end=source.indexOf('\n    },',start)+6;
  const context={localization:null,assets,prepareLocalization,isLocalizationResourcePath,installRuntimeFonts:async()=>{},
    touch:{configure(){}},runtimeOptions:{},runtimeMusic:null};
  const configure=runInNewContext('('+source.slice(start,end)+')',context);
  const handler=managedCommandHandler({running:()=>false,resources:async()=>{},configure});
  try{
    await handler({command:'configure',music:'none',runtimePack,options:{}});
    const provider=context.localization;
    assert.equal(provider.language,'lang_zh-hans');
    const bytes=new Uint8Array(await (await assets.fetch('/assets/st01_00a.msg')).arrayBuffer());
    assert.deepEqual(bytes,new Uint8Array(original),'the cooperative VM receives original MSG bytes');
    provider.setContext({stage:1,character:0,shot:0});
    const pc=runtimePack.result.patch.entries[0].anchors.find(a=>a.texts.length).offset;
    const snapshot={entry:0,active:1,pc,textRecords:[]},before=structuredClone(snapshot);
    for(const options of [{},{replayViewer:true},{thpracEnabled:false}]){
      await handler({command:'configure',music:'none',options,resources:[],sharedResources:[]});
      assert.equal(context.localization,provider,'room binding retains the installed provider and its MSG index');
      const text=messageTextPoses(snapshot,[pose(0),pose(1)],atlases);
      assert.deepEqual(text.map(p=>p.text),['第一句','第二句','注音']);
      const single=messageTextPoses(snapshot,[pose(0)],atlases);
      assert.deepEqual(single.map(p=>p.text),['第一句','第二句','注音'],'extra translated lines reuse the visible source pose');
      assert.deepEqual(single.map(p=>p.y),[373,395,367]);
      assert.deepEqual(snapshot,before,'local layout leaves the native snapshot untouched');
      const title=spellTitleDescriptors({cardId:0,titleHex:'7370656c6c'},[{a:{name:'text',sprite:7,visible:true,alpha:1}}]);
      assert.equal(title[0].text,'中文符卡');
    }
    await assert.rejects(handler({command:'configure',music:'none',language:'lang_en',options:{}}),/语言包清单无效/);
    assert.equal(context.localization,provider,'a failed language replacement preserves the current resources');
    await handler({command:'configure',music:'none',language:'ja',options:{}});
    assert.equal(context.localization.language,'ja');
    assert.deepEqual(new Uint8Array(await (await assets.fetch('/assets/st01_00a.msg')).arrayBuffer()),bytes);
  }finally{context.localization?.deactivate();}
});

test('local dialogue surfaces follow original counters across later boxes, rollback and route changes',async()=>{
  const original=fixture(),runtimePack=pack(original);
  const provider=await prepareLocalization({language:runtimePack.language,manifest:runtimePack.manifest,
    readFile:()=>runtimePack.files[0].bytes});
  await provider.prepareMessage('/assets/st01_00a.msg',original);provider.setContext({stage:1,character:0,shot:0});
  const anchors=runtimePack.result.patch.entries[0].anchors.filter(a=>a.texts.length),before=original.slice();
  provider.activate();
  try{
    for(const index of [0,1,0,1]){
      const snapshot={active:1,entry:0,pc:anchors[index].offset,textRecords:[]};
      const result=messageTextPoses(snapshot,[pose(0),pose(1)],atlases);
      assert.deepEqual(result.map(p=>p.text),index?['后来一句']:['第一句','第二句','注音']);
    }
    provider.setContext({stage:1,character:1,shot:1});
    const snapshot={active:1,entry:0,pc:anchors[0].offset,textRecords:[{textHex:'6f726967696e616c'}]};
    assert.equal(messageTextPoses(snapshot,[pose(0)],atlases)[0].text,'original','an uninstalled route keeps its own source text');
    assert.deepEqual(original,before);
  }finally{provider.deactivate();}
});

test('language, fonts, music and display preferences do not enter the room agreement',async()=>{
  const room={netplayMode:'lan',netplayUrl:'ws://localhost/?room=independent&run=1',netplayPlayerCount:2,netplayPlayer:0,
    netplaySeed:1234,netplayDifficulty:1,netplayLoadouts:[{character:0,shot:0},{character:2,shot:1}],
    netplayAdonisMode:1,netplayPredictionLimit:0};
  const a=await multiplayerSetupWords({...room,language:'lang_zh-hans',music:'ogg',multiplayerLocalPlayerVisibility:true},[1,2,3,4]);
  const b=await multiplayerSetupWords({...room,language:'ja',music:'none',multiplayerLocalPlayerVisibility:false},[1,2,3,4]);
  assert.deepEqual(a.words,b.words);
});
