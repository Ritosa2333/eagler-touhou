import {test} from 'vitest';
import assert from 'node:assert/strict';
import {th12MessagePatch,th12TextRecord} from '../../server/th12-thcrap-compiler.mjs';
import {prepareLocalization} from '../../../ports/th12/src/localization/loader.mjs';
import {thcrapLayout} from '../../../ports/th12/src/localization/layout.mjs';
import {createStaticThcrapPack} from '../../server/thcrap-static-pack.mjs';
import {managedCommandHandler} from '../../../ports/th12/src/managed-shell.mjs';

test('TH12 managed configure accepts installed thcrap languages and rejects mismatched packs',async()=>{
  const seen=[];
  const handler=managedCommandHandler({running:()=>false,resources:async()=>{},configure:async value=>seen.push(value.language)});
  for(const language of ['ja','lang_zh-hans','lang_en'])await handler({command:'configure',music:'none',language,runtimePack:{language}});
  assert.deepEqual(seen,['ja','lang_zh-hans','lang_en']);
  await assert.rejects(handler({command:'configure',music:'none',language:'lang_en',runtimePack:{language:'lang_zh-hans'}}),/不匹配/);
});

const encrypt=text=>{const raw=Buffer.from(text+'\0'),out=Buffer.alloc((raw.length+3)&~3);let key=0x77,step=7;for(let i=0;i<out.length;i++){out[i]=(raw[i]??0)^key;key=(key+step)&255;step=(step+16)&255;}return out;};
const command=(time,op,payload=Buffer.alloc(0))=>{const header=Buffer.alloc(4);header.writeUInt16LE(time);header[2]=op;header[3]=payload.length;return Buffer.concat([header,payload]);};
const message=commands=>{const header=Buffer.alloc(12);header.writeUInt32LE(1);header.writeUInt32LE(12,4);return Buffer.concat([header,...commands]);};
function overlay(result){return {schema:'th12-localization-overlay/2',game:'th12',language:'lang_zh-hans',sourceKind:'thcrap',sourceExeSha256:'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417',dialogue:{'st01_00a.msg':result.lookup},dialoguePatches:{'st01_00a.msg':result.patch},endings:{},endingPatches:{},spells:{},textures:{},musicComments:{}};}
async function provider(value){const pack=createStaticThcrapPack({pack:{game:'th12',language:'lang_zh-hans',assets:[]},resources:[{targetPath:'/thcrap/th12/overlay.json',bytes:Buffer.from(JSON.stringify(value))}]});return prepareLocalization({language:'lang_zh-hans',manifest:pack.manifest,readFile:()=>Buffer.from(JSON.stringify(value))});}

test('TH12 text-only patches keep automatic dialogue timestamps and complete translated lines',async()=>{
  const original=message([command(0,10,Buffer.from([1])),command(2,17,encrypt('old1')),command(8,17,encrypt('old2')),command(8,11,Buffer.alloc(4)),command(9,0)]);
  const result=th12MessagePatch(original,{'0':{'2_0':{lines:['第一句','第二句','第三句']}}});
  const texts=result.patch.entries[0].anchors.flatMap(a=>a.texts);
  assert.deepEqual(texts.map(t=>[t.time,t.text]),[[2,'第一句'],[8,'第二句'],[8,'第三句']]);
  const p=await provider(overlay(result)),patched=await p.patchMessage('/assets/st01_00a.msg',original);
  assert.ok(patched.length>original.length);p.setContext({stage:1,character:0,shot:0});
  for(const record of texts)assert.equal(p.textForHex('dialogue',record.sourceHex,{entry:0}).text,record.text);
  const corrupt=Buffer.from(original);corrupt[14]^=1;await assert.rejects(p.patchMessage('/assets/st01_00a.msg',corrupt),/源文件不匹配/);
});
test('missing TH12 translation boxes retain Japanese bytes and OP_DELETE removes only opcode25',async()=>{
  const original=message([command(0,17,encrypt('untouched')),command(0,25,Buffer.alloc(4)),command(1,0)]);
  const result=th12MessagePatch(original,{}),p=await provider(overlay(result));
  const bytes=Buffer.from(await p.patchMessage('/assets/st01_00a.msg',original));
  assert.ok(bytes.includes(encrypt('untouched')));assert.equal(bytes.length,original.length-8);
  result.patch.entries[0].anchors[1].skip=true;const bad=await provider(overlay(result));
  await assert.rejects(bad.patchMessage('/assets/st01_00a.msg',original),/只能移除/);
});
test('thcrap ruby preserves both ordinary text slots and carries annotation layout',()=>{
  const r=th12TextRecord('|\t宝船前面的\t,\t那个\t,• •',{formatted:true});
  assert.equal(r.text,'• •');assert.deepEqual(r.ruby,{base:'宝船前面的',span:'那个'});
  assert.equal(Buffer.from(r.utf8Hex,'hex').toString(),'|0,1,• •');
});

test('repeated ruby text keeps each box placement instead of reusing another annotation',async()=>{
  const original=message([command(0,17,encrypt('one')),command(0,11,Buffer.alloc(4)),command(10,17,encrypt('two')),command(10,11,Buffer.alloc(4)),command(11,0)]);
  const result=th12MessagePatch(original,{'0':{'0_0':{lines:['|before,word,• •','first line']},'10_0':{lines:['|longer before,word,• •','second line']}}});
  const p=await provider(overlay(result));p.setContext({stage:1,character:0,shot:0});
  await p.patchMessage('/assets/st01_00a.msg',original);
  const rubies=result.patch.entries[0].anchors.flatMap(a=>a.texts).filter(r=>r.ruby);
  assert.equal(rubies.length,2);assert.notEqual(rubies[0].offset,rubies[1].offset);
  for(const r of rubies)assert.deepEqual(p.textForHex('dialogue',r.sourceHex,{entry:0,offset:r.offset,spacing:r.spacing}).ruby,r.ruby);
});
test('thcrap layout uses measured font widths, persistent tabs and styled runs',()=>{
  const tabs=[],measure=text=>[...text].length*16;
  const first=thcrapLayout('<ts$灵梦  $魔理沙  >你好',{measure,tabs});
  assert.equal(first.runs[0].text,'你好');assert.equal(first.width,32);assert.equal(tabs[0],80);
  const second=thcrapLayout('<l$灵梦  >后续',{measure,tabs});assert.equal(second.runs[1].x,80);
  const right=thcrapLayout('<r$符卡名$>',{measure,width:640});assert.equal(right.runs[0].x,592);
});
