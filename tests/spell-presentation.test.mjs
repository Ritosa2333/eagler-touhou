import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import {readCoreSpell,decodeSpellTitle,spellAsciiDescriptors,spellTitleDescriptors,spellTextDescriptors} from '../src/spell-presentation.mjs';

const native=JSON.parse(fs.readFileSync(new URL('fixtures/spell-presentation-original.json',import.meta.url)));
const pose=(name,sprite,alpha=1)=>({a:{name,sprite,visible:true,alpha,sx:1,sy:1,anchorX:2,anchorY:1},x:412,y:32,alpha:1,id:123,owner:99,logical:true});

test('spell numeric submissions match original bonus, failure, history, lookup and alpha branches',()=>{
  assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
  assert.equal(native.gpuExecuted,false);assert.equal(native.gdiExecuted,false);
  for(const record of native.text){
    const input=structuredClone(record.input),before=structuredClone(input),poses=input.historyHandlePresent?[pose('ascii',261,input.historyAlpha/255)]:[];
    // A wrong bonus VM/alpha must not override ascii2's native alpha source.
    poses.unshift(pose('ascii',259,0));
    const expected=record.calls.map(call=>({text:call.text,font:call.font,x:call.position[0],y:call.position[1],z:call.position[2],
      scaleX:call.scale[0],scaleY:call.scale[1],colorARGB:call.colorARGB,alignX:call.alignment[0],alignY:call.alignment[1],drawFlag:call.drawFlag}));
    const beforePoses=structuredClone(poses);
    assert.deepEqual(spellAsciiDescriptors(input,poses),expected,record.name);
    assert.deepEqual(input,before,record.name+' must not advance spell');assert.deepEqual(poses,beforePoses,record.name+' must not advance animation');
    assert.equal(record.rngUnchanged,true);
  }
});

test('title CP932 decoding and right-aligned GDI wrapper geometry match original460800 calls',()=>{
  for(const record of native.titles){
    const call=record.calls[0],p=pose('text',7),[descriptor]=spellTitleDescriptors({titleHex:record.titleHex},[p]);
    assert.equal(decodeSpellTitle(record.titleHex),record.title);assert.equal(descriptor.text,record.title);
    assert.equal(descriptor.record.offset,call.offset);assert.equal(descriptor.record.style,call.style);
    assert.equal(descriptor.record.color,call.foregroundColor);assert.equal(descriptor.record.outlineColor,call.outlineColor);
    assert.equal(descriptor.record.spacing,call.spacing);
    assert.deepEqual([descriptor.width,descriptor.height],[call.sourceRect[2]-call.sourceRect[0],call.sourceRect[3]-call.sourceRect[1]]);
    assert.equal(descriptor.metrics.sourceHeight,2*call.fontHeight+2);
    assert.equal(descriptor.raster.sourceX,2*call.offset);assert.equal(descriptor.raster.sourceY,2);
    assert.deepEqual(descriptor.pose,p);assert.notEqual(descriptor.pose,p);assert.notEqual(descriptor.pose.a,p.a);
  }
  assert.equal(decodeSpellTitle('90af98409144004142'),'星蓮船');
  for(const bad of ['x','abcdx','xx',null,'81'])assert.throws(()=>decodeSpellTitle(bad));
});

test('original GUI birth, dim, restore and outro poses survive presentation without a second timeline',()=>{
  const titleHex=native.titles[0].titleHex;
  for(const track of native.animations)for(const frame of track.frames){
    const p={...pose(track.bank,frame.sprite,(frame.primary>>>24)/255),x:frame.position[0],y:frame.position[1],
      a:{...pose(track.bank,frame.sprite,(frame.primary>>>24)/255).a,visible:frame.visible&&!frame.ended,
        sx:frame.scale[0],sy:frame.scale[1],layer:frame.layer}};
    const spell={titleHex,flags:track.interrupt===1?0:3,bonusDisplayed:4000000,records:[12,3,88,77]};
    const before=structuredClone({spell,p}),descriptors=spellTextDescriptors(spell,[p]);
    if(track.bank==='text'){
      const expected=p.a.visible&&p.a.alpha>0?1:0;
      assert.equal(descriptors.length,expected,`text74:${track.interrupt}:${frame.tick}`);
      if(expected)assert.deepEqual(descriptors[0].pose,p);
    }else if(track.script===2){
      const expected=spell.flags&1?2:0;
      assert.equal(descriptors.length,expected);
      if(expected)assert.equal(descriptors[0].colorARGB,frame.primary);
    }else assert.deepEqual(descriptors,[],'static BONUS header is preserved by normal ANM rendering');
    assert.deepEqual({spell,p},before);
  }
  assert.deepEqual(spellTextDescriptors(null,[]),[]);
  assert.deepEqual(spellTextDescriptors({titleHex,flags:3,bonusDisplayed:1,records:[0,0,0,0]},[]),[],'no synthetic fallback animation');
});

function coreRecord(state){
  const bytes=new TextEncoder().encode(JSON.stringify(state)),heap=new Uint8Array(bytes.length+64);heap.set(bytes,16);
  return {HEAPU8:heap,_th12_spell:()=>16,_th12_spell_size:()=>bytes.length};
}
test('compact spell snapshot reader is bounded and does not read or tick the full game',()=>{
  const state={flags:3,titleHex:native.titles[1].titleHex,bonusDisplayed:4000000,records:[1,0,7,3],age:224};
  const core=coreRecord(state);core._th12_state=()=>{throw Error('whole state forbidden');};core._th12_tick=()=>{throw Error('logic tick forbidden');};
  assert.deepEqual(readCoreSpell(core),state);assert.equal(readCoreSpell({}),null);
  for(const[ptr,size]of[[-1,2],[.5,2],[16,65537],[16,1],[Number.MAX_SAFE_INTEGER,2],[16,1.5]]){
    const invalid=coreRecord(state);invalid._th12_spell=()=>ptr;invalid._th12_spell_size=()=>size;assert.throws(()=>readCoreSpell(invalid),/符卡/);
  }
  const outside=coreRecord(state);outside._th12_spell_size=()=>outside.HEAPU8.length;assert.throws(()=>readCoreSpell(outside),/符卡/);
  const utf8=coreRecord(state);utf8.HEAPU8[16]=255;assert.throws(()=>readCoreSpell(utf8));
  for(const value of[[],null,{}, {...state,flags:1.5},{...state,bonusDisplayed:null},{...state,titleHex:'xz'},
    {...state,records:[1,2]},{...state,records:[-1,0,0,0]},{...state,records:[1,null,0,0]}])assert.throws(()=>readCoreSpell(coreRecord(value)),/符卡/);
});
