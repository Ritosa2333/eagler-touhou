import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {th08PracticeParameterCases,assertTh08PracticeParameters} from './th08-practice-parameters.mjs';
import {decodePracticeUiSnapshot} from '../practice-ui.mjs';

const cases=th08PracticeParameterCases({mode:1,stage:0,warp:0,section:0,phase:0,frame:0});
test('parameter samples select their actual native section category rather than silently falling back',()=>{
  const source=readFileSync(new URL('../../../../ports/th08/web/cpp/game/PracticeSectionCatalog.hpp',import.meta.url),'utf8');
  const catalog=new Map([...source.matchAll(/\{(\d+),(\d+),(\d+),(true|false),/g)].map(match=>[Number(match[1]),{stage:Number(match[2]),group:Number(match[3]),spell:match[4]==='true'}]));
  for(const c of cases.filter(c=>c.params.section>0&&c.params.section<10000)){
    const section=catalog.get(c.params.section);assert.ok(section,c.id);assert.equal(section.stage,c.params.stage,c.id);
    assert.ok(c.params.warp===2?section.group===1:c.params.warp===3?section.group===2:c.params.warp===4?!section.spell:c.params.warp===5?section.spell:false,c.id);
  }
});
function fixture(){return cases.map((c,index)=>{
  const bytes=Buffer.alloc(0xd000+(c.id.startsWith('point-')&&c.id!=='point-disabled'?520:0));
  bytes.writeUInt32LE(0x800,0);bytes.writeInt16LE(2,6);bytes.writeUInt32LE(0x100,8);
  for(const patch of c.patches)if(patch.type==='f32')bytes.writeFloatLE(patch.value,patch.offset);else bytes.writeInt32LE(patch.value,patch.offset);
  if(bytes.length>0xd000){bytes.writeInt32LE(0xd000-0x3e14,0x3e24);bytes.writeInt32LE(0x3e30-(0xd000+125*4),bytes.length-4);}
  const words=Array(80).fill(0),offset=c.timeline.offset??0x100;
  Object.assign(words,{0:1,1:1,70:1,71:2,72:c.timeline.frame,73:offset,74:bytes.readUInt16LE(offset+4),75:bytes.length,76:c.timeline.patched?1:0,79:1});
  if(c.timeline.offset===undefined){words[73]=words[74]=-1;words[79]=0;}
  const identity={app:123,epoch:index+1,documentTimeOrigin:100+index,url:'https://fixture.invalid/'+index};
  const uiWords=Array(128).fill(0);for(const at of [0,1,9,39,104,111])uiWords[at]=1;uiWords[2]=uiWords[3]=10;uiWords[110]=0x50554b31;
  const uiRaw={available:true,words:uiWords},nativeRaw={state:{menu:true,accepted:false,active:false,inGame:false}};
  return{parameterId:c.id,status:'passed',enabled:true,locale:'ja-JP',startup:{configuration:{options:{thprac:{params:c.params}}}},
    nativePracticeMenu:{menuInput:{identity,uiRaw,ui:decodePracticeUiSnapshot(uiRaw),nativeRaw,native:nativeRaw}},
    parameterRaw:{available:true,words:[]},parameterConsumption:{initialRaw:{available:true,words},identityBefore:identity},
    programRaw:{available:true,header:[1,1,64,bytes.length],bytes:Array.from(bytes)}};
});}
const assess=(rows,assessConsumption=()=>({status:'observed'}))=>assertTh08PracticeParameters(rows,{cases,decodeSnapshot:raw=>raw,assessConsumption});
test('parameter classes require frozen actual clocks, instruction destinations, point payloads and separate cold owners',()=>assert.equal(assess(fixture()).complete,true));
for(const [name,mutate] of Object.entries({
  'missing consumer class':rows=>rows.pop(),
  'missing native input readiness':rows=>delete rows[0].nativePracticeMenu,
  'unarmed native menu':rows=>{const menu=rows[0].nativePracticeMenu.menuInput;menu.uiRaw.words[104]=0;menu.ui=decodePracticeUiSnapshot(menu.uiRaw);},
  'configured values substituted for ECL consumption':rows=>rows[3].parameterConsumption.initialRaw.words[72]=0,
  'dialogue still uses the no-dialogue destination':rows=>rows[6].parameterConsumption.initialRaw.words[73]=0xac84,
  'phase patch disconnected':rows=>rows[7].programRaw.bytes[0xac6c]=0,
  'boss coordinate patch disconnected':rows=>rows[9].programRaw.bytes.fill(0,0x1ed0,0x1ed4),
  'appended point setup never called':rows=>rows[10].programRaw.bytes.fill(0,0x3e24,0x3e28),
  'point setup returns outside the original program':rows=>rows[10].programRaw.bytes.fill(0,rows[10].programRaw.bytes.length-4),
  'old document reused':rows=>rows[1].parameterConsumption.identityBefore.documentTimeOrigin=100,
  'warped instruction was never selected':rows=>{const w=rows[4].parameterConsumption.initialRaw.words;w[73]=w[74]=-1;w[79]=0;},
  'program byte count changed':rows=>rows[0].programRaw.header[3]++,
  'wrong actual opcode':rows=>rows[0].parameterConsumption.initialRaw.words[74]++,
}))test(`parameter matrix rejects ${name}`,()=>{const rows=fixture();mutate(rows);assert.throws(()=>assess(rows));});
test('resource oracle failures cannot be replaced by valid ECL observations',()=>assert.throws(()=>assess(fixture(),()=>({status:'failed',mismatches:['actual power']}))));
