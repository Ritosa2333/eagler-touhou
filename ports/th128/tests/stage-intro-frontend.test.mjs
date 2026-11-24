import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';

const root=path.resolve(import.meta.dirname,'..');
const source=fs.readFileSync(path.join(root,'site/play.mjs'),'utf8');
function functionBody(name,async=false){
  const marker=`${async?'async ':''}function ${name}(`;
  const start=source.indexOf(marker);
  assert.ok(start>=0,`missing ${name}`);
  const next=source.search(/\n(?:async )?function /g);
  let following=source.length;
  for(const match of source.matchAll(/\n(?:async )?function /g)){
    if(match.index>start){following=match.index;break;}
  }
  const text=source.slice(start,following);
  const open=text.indexOf('{'),close=text.lastIndexOf('}');
  assert.ok(open>=0&&close>open,`missing body ${name}`);
  return text.slice(open+1,close);
}
const logoRecord=new Function('stage',functionBody('stageLogoRecord'));
const trackNumber=new Function('name',functionBody('stageIntroTrackNumber'));
const AsyncFunction=Object.getPrototypeOf(async()=>{}).constructor;
const install=new AsyncFunction('stage','module','renderer','jsonAt','bytesAt','memory','base','signal','stageLogoRecord',functionBody('installStageIntro',true));
const caption=new Function('state','stageIntroTrackNumber','trackName','role',functionBody('stageIntroCaption'));
const drawFailure=new Function('module',functionBody('stageIntroDrawFailure'));
const syncMusic=new Function('state','message','stageIntroCaption',functionBody('syncGameplayMusicState'));

test('actual frontend stage logo loader binds selected logo, variant base, bank30 texture and native load', async()=>{
  const base=path.join(root,'local/generations/8c71c316a4b24e68827f9fc560242050/zh');
  const metadata=JSON.parse(fs.readFileSync(path.join(base,'animations/logo_a1_1.json')));
  const bytes=new Uint8Array(fs.readFileSync(path.join(base,'raw/logo_a1_1.anm')));
  const record=logoRecord({id:1,logo:'logo_a1_1.anm',background:'st_a1_1.std'});
  assert.deepEqual(record,{id:1,logo:'logo_a1_1.anm',metadata:'animations/logo_a1_1.json',bytes:'runtime/logo_a1_1.anm'});
  assert.throws(()=>logoRecord({id:0,logo:'logo_a1_1.anm'}),/canonical stage id/);
  assert.throws(()=>logoRecord({id:1,logo:'st_a1_1.std'}),/canonical stage id/);
  const calls=[];let nextPointer=1000;
  const module={
    _th128_world_load_stage_intro(id,logoPointer,logoLength,dataPointer,size){
      calls.push({id,logoPointer,logoLength,dataPointer,size});return 1;
    },
    _free(pointer){calls.push({freed:pointer});},
    UTF8ToString(){return '';}
  };
  const renderer={async loadBank(bank,meta,assetBase,{signal}){calls.push({bank,meta,assetBase,signal});}};
  const memory=value=>{const pointer=nextPointer;nextPointer+=Math.max(1,value.length);return pointer;};
  const result=await install({id:1,logo:'logo_a1_1.anm'},module,renderer,async name=>{
    assert.equal(name,'animations/logo_a1_1.json');return metadata;
  },async name=>{
    assert.equal(name,'runtime/logo_a1_1.anm');return bytes;
  },memory,'/assets/generations/test/zh/',{aborted:false},logoRecord);
  assert.equal(result.logo,'logo_a1_1.anm');
  const bank=calls.find(call=>call.bank===30);
  assert.ok(bank&&bank.assetBase==='/assets/generations/test/zh/'&&bank.meta===metadata);
  const native=calls.find(call=>call.id===1);
  assert.ok(native&&native.logoLength===13&&native.size===bytes.length);
  assert.equal(calls.filter(call=>Object.hasOwn(call,'freed')).length,2);
});

test('actual caption binding uses first/boss roles once and checksums are not bypassed',()=>{
  assert.equal(trackNumber('th128_02.wav'),2);
  assert.equal(trackNumber('th128_00.wav'),0);
  assert.equal(trackNumber('bad.wav'),-1);
  const calls=[];
  const state={stageIntroLoaded:true,stageIntroCaptionKey:'',module:{_th128_world_track_caption(track,role){
    calls.push([track,role]);return true;
  },UTF8ToString(){return '';}}};
  assert.equal(caption(state,trackNumber,'th128_00.wav',0),true);
  assert.equal(caption(state,trackNumber,'th128_00.wav',0),true);
  assert.deepEqual(calls,[[0,0]]);
  assert.equal(caption(state,trackNumber,'th128_02.wav',1),true);
  assert.deepEqual(calls,[[0,0],[2,1]]);
  assert.equal(drawFailure({_th128_world_stage_intro_draw_error:()=>1,UTF8ToString:()=> 'sticky logo draw failure'}),'sticky logo draw failure');
  assert.equal(drawFailure({}), '');
  const switches=[],captions=[];
  const musicState={stageSelection:{stage:{music:['th128_00.wav','th128_02.wav']}},stageIntroLoaded:true,
    messageMusicSerial:0,stageMusic:{switchTo(track){switches.push(track);}}};
  const syncCaption=(track,role)=>{captions.push([track,role]);};
  syncMusic(musicState,{musicSerial:7},syncCaption);
  syncMusic(musicState,{musicSerial:7},syncCaption);
  assert.deepEqual(switches,['th128_02.wav']);
  assert.deepEqual(captions,[['th128_02.wav',1]]);
  syncMusic(musicState,{musicSerial:8},syncCaption);
  assert.deepEqual(switches,['th128_02.wav','th128_02.wav']);
});
