import assert from 'node:assert/strict';import fs from 'node:fs/promises';import {resolve} from 'node:path';import {createRequire} from 'node:module';
import {prepareLocalization} from '../src/localization/loader.mjs';
import {th12MessageEntries} from '../../../launcher/server/th12-thcrap-compiler.mjs';
import createCore from '../artifacts/message-tests/core.mjs';
const {unzipSync}=createRequire(new URL('../../../launcher/package.json',import.meta.url))('fflate');
const options=new Map(process.argv.slice(2).map(arg=>{const at=arg.indexOf('=');return [arg.slice(0,at),arg.slice(at+1)];}));
if(!options.get('--retail')||!options.get('--packs'))throw Error('Supply --retail=JAPANESE_EXTRACTED_DIRECTORY --packs=LANGUAGE_CONTENT_ROOT [--output=REPORT_DIRECTORY]');
const task=resolve(options.get('--output')??'ports/th12-mp/artifacts/thcrap-check'),retail=resolve(options.get('--retail')),packs=resolve(options.get('--packs'));await fs.mkdir(task,{recursive:true});
const c=await createCore(),originalVM=await createCore(),translatedVM=await createCore();
function state(){return [...new Int32Array(c.HEAPU8.buffer,c._test_state(),10)];}
function actions(core=c){let p=core._test_text(),end=p;while(core.HEAPU8[end])end++;return JSON.parse(new TextDecoder().decode(core.HEAPU8.subarray(p,end)));}
function checkPresentation(provider,original,patched,entry,skip,label){
  for(const [core,data]of [[originalVM,original],[translatedVM,patched]]){
    const ptr=core._malloc(data.length);try{core.HEAPU8.set(data,ptr);assert.equal(core._test_load(ptr,data.length),1);}finally{core._free(ptr);}
    assert.equal(core._test_start(entry),1);
  }
  const start=original.readUInt32LE(4+8*entry),visible=records=>records.map(r=>!r.textHex||r.textHex==='20'?null:r);
  const records=Array.from({length:4},()=>({textHex:''}));
  for(let ticks=0;ticks<20000;ticks++){
    originalVM._test_tick(skip?512:0,skip?0:1);translatedVM._test_tick(skip?512:0,skip?0:1);
    const a=Array.from(new Int32Array(originalVM.HEAPU8.buffer,originalVM._test_state(),10)),
      b=Array.from(new Int32Array(translatedVM.HEAPU8.buffer,translatedVM._test_state(),10));
    assert.equal(a[0],b[0],label+' completion');assert.equal(a[1],b[1],label+' clock');assert.equal(a[6]|b[6],0);
    for(const {slot,interrupt,...record}of actions(translatedVM))records[slot]=record;
    const projected=provider.presentMessage({entry,pc:start+a[3]});
    assert.equal(projected.localizedPresentation,true,label+' original program counter');
    assert.deepEqual(visible(projected.textRecords),visible(records),label+' text surfaces at tick '+ticks);
    if(a[0]<0)return ticks+1;
  }
  throw Error(label+' MSG did not terminate');
}
function run(data,entry,skip){const ptr=c._malloc(data.length);c.HEAPU8.set(data,ptr);assert.equal(c._test_load(ptr,data.length),1);c._free(ptr);assert.equal(c._test_start(entry),1);const texts=[];let ticks=0;for(;ticks<20000;ticks++){c._test_tick(skip?512:0,skip?0:1);const s=state();assert.equal(s[6],0);texts.push(...actions().filter(a=>a.interrupt===2));if(s[0]<0)break;}assert.ok(ticks<20000,'MSG must terminate');return {ticks,texts};}
const report=[];
for(const language of ['lang_zh-hans','lang_en']){
  const zip=unzipSync(await fs.readFile(resolve(packs,'language/'+language+'.zip'))),manifest=JSON.parse(new TextDecoder().decode(zip['manifest.json']));
  const p=await prepareLocalization({language,manifest,readFile:path=>zip[path.slice(1)]});
  const o=JSON.parse(new TextDecoder().decode(zip['thcrap/th12/overlay.json']));
  const base=JSON.parse(await fs.readFile(resolve(options.get('--assets')??resolve(retail,'../assets'),'menu-resources.json'))),menu=p.applyMenuResources(base);
  let files=0,entries=0,lines=0,timed=0,skipped=0,presentationTicks=0;const previews=[];
  for(const name of Object.keys(o.dialogue)){
    const original=await fs.readFile(resolve(retail,name)),patched=Buffer.from(await p.patchMessage('/assets/'+name,original));
    await p.prepareMessage('/assets/'+name,original);
    const route=/st(\d+)_0(\d)([ab])/.exec(name);p.setContext({stage:+route[1],character:+route[2],shot:+(route[3]==='b')});
    const old=th12MessageEntries(original),next=th12MessageEntries(patched);
    assert.equal(old.length,next.length);
    for(const [entry,patch]of o.dialoguePatches[name].entries.entries())for(const anchor of patch.anchors)for(const record of anchor.texts){
      if(record.originalOffset!==undefined)continue;
      const display=p.textForHex('dialogue',record.sourceHex,{resource:name,entry,offset:record.offset,spacing:record.spacing});
      assert.equal(display?.text,record.text,name+' renderer text lookup');
      assert.deepEqual(display?.ruby,record.ruby,name+' annotation placement');
    }
    for(let entry=0;entry<old.length;entry++){
      const control=commands=>commands.filter(c=>![15,16,17,25].includes(c.op)).map(c=>[c.time,c.op,c.payload.toString('hex')]);
      assert.deepEqual(control(next[entry]),control(old[entry]),name+' controls');
      const times=commands=>[...new Set(commands.filter(c=>[15,16,17].includes(c.op)).map(c=>c.time))];
      assert.deepEqual(times(next[entry]),times(old[entry]),name+' text times');
      const expected=next[entry].filter(c=>[15,16,17].includes(c.op)).map(c=>{
        const b=c.raw;if(b[0]!==124)return b.toString('hex');let a=b.indexOf(44),z=b.indexOf(44,a+1);return b.subarray(z+1).toString('hex');
      });
      for(const skip of [false,true]){
        const originalRun=run(original,entry,skip),actual=run(patched,entry,skip);
        assert.equal(actual.ticks,originalRun.ticks,name+' timing '+skip);
        assert.deepEqual(actual.texts.map(a=>a.textHex),expected,name+' every translated line '+skip);
        presentationTicks+=checkPresentation(p,original,patched,entry,skip,name+' entry '+entry+' skip '+skip);
        if(skip)skipped++;
      }
      if(!old[entry].some(c=>c.op===11)&&times(old[entry]).length>1)timed++;
      if(name==='st01_00a.msg'&&entry===0){
        previews.push(...next[entry].filter(c=>c.op===17&&c.time===130).map(c=>({textHex:c.raw.toString('hex'),offset:0,spacing:0,style:0,color:0xf8f08f})));
      }
      lines+=expected.length;entries++;
    }
    await fs.writeFile(resolve(task,name),patched);files++;
  }
  assert.equal(files,42);assert.equal(Object.keys(o.spells).length,113);assert.equal(menu.musicComments.filter(c=>c.localized).length,17);
  for(let n=0;n<12;n++){
    const controls=e=>e.commands.filter(c=>c.op!==3).map(c=>[c.time,c.op,c.payloadHex]);
    assert.deepEqual(controls(menu.endings[n]),controls(base.endings[n]),'ending controls');
  }
  await fs.writeFile(resolve(task,language+'-preview.json'),JSON.stringify({menu,previews}));
  report.push({language,files,entries,lines,timed,skipCases:skipped,presentationTicks,spells:Object.keys(o.spells).length,textures:Object.keys(o.textures).length,music:17,endings:12});
}
await fs.writeFile(resolve(task,'verification.json'),JSON.stringify(report,null,2));console.log(report);
