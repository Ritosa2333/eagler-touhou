// Read only caller-owned original dialogue/ending assets from the local author package.
import assert from 'node:assert/strict';import {readFile} from 'node:fs/promises';
import {readZipDirectory,readEntryData} from '../../th128/site/runtime/zip-reader.mjs';
import {readCoopEndingAssets} from '../runtime/coop-ending.mjs';
import {openTh128Dat} from '../../th128/site/runtime/th128-dat.mjs';
import {OriginalEndingPixels} from '../../th128/site/runtime/native-ending-pixels.mjs';
import {stageTextRequests} from '../runtime/managed-stage-text.mjs';
export async function openOriginalEndingFixture(path){
 const zip=bytes=>new Map(readZipDirectory(bytes).entries.map(row=>[row.name,row]));
 const json=async entry=>JSON.parse(new TextDecoder().decode(await readEntryData(entry)));
 const outer=zip(await readFile(path)),p=await json(outer.get('package.json')),lang=zip(await readEntryData(outer.get(p.files['language:lang_zh-hans'].source)));
 const key=[...lang.keys()].find(name=>name.endsWith('/zh/manifest.json')),manifest=await json(lang.get(key)),base='http://127.0.0.1/'+key.replace(/manifest\.json$/,'');
 const dat=openTh128Dat(await readEntryData(outer.get(p.files['game-data'].source)));
 const resources={base,endingManifest:manifest.nativeEndingAssets,async fetch(url){const name=new URL(url).pathname.slice(1),e=lang.get(name);if(e)return new Response(await readEntryData(e));
   const raw=/\/runtime\/([a-z0-9_.-]+)$/.exec(name)?.[1];if(raw){const entry=lang.get('raw/'+raw);return new Response(entry?await readEntryData(entry):dat.read(raw));}return new Response(null,{status:404});}};
 const text=await json(lang.get(key.replace('manifest.json','animations/original-stage-text-full42.json'))),rasterKeys=new Set(text.rows.map(r=>r.key));let dialogues=0,spellTitles=0;
 return {
  async raw(name,fallback){const entry=lang.get('raw/'+name);return entry?readEntryData(entry):fallback(name);},
  inspect(pose){for(const req of stageTextRequests(pose)){assert.ok(rasterKeys.has(req.kind+':'+req.rawHex),'original package glyph for active '+req.kind+': '+req.rawHex);if(req.kind==='message')dialogues++;if(req.kind==='spell')spellTitles++;}},
  async end(g,stage){
    assert.ok(dialogues>0,'a real campaign produced visible dialogue glyphs');assert.ok(spellTitles>0);assert.equal(g.outcome(),'cleared');
    if(stage===16){console.log(JSON.stringify({route:'EX',dialogues,spellTitles,extraOriginalClear:true}));return;}
    const signal=new AbortController().signal,assets=await readCoopEndingAssets(resources,stage,signal);
    const native=g.startEnding(assets.inputs),pixels=new OriginalEndingPixels(assets.inputs[2],assets.inputs[1]);
    let held=0,tick=0,last=-1,glyph=null,visible=0,previous=null;const media=[];for(const event of native.snapshot().media.filter(row=>!row.acknowledged)){media.push([event.operation,event.path]);native.acknowledge(event);}
    for(;tick<14000;tick++){
      const next=tick%4===0?1:0,state=native.step({holding:next,pressed:next&~held,released:held&~next});held=next;
      for(const event of state.media.filter(row=>!row.acknowledged)){media.push([event.operation,event.path]);native.acknowledge(event);}
      if(state.executedOp3-1!==last){last=state.executedOp3-1;glyph=await assets.glyph(last,state.textColor);}
      if(tick%90===0||state.originalReturnRequested){const image=pixels.frame(state,glyph);assert.equal(image.length,640*480*4);if(image.some((byte,i)=>i%4!==3&&byte))visible++;}
      previous=state;if(state.originalReturnRequested)break;
    }
    assert.ok(visible>2&&last>=3,'several original ending panels must really render');
    assert.deepEqual(media,[[8,'bgm/th128_09.wav'],[5,'bgm/th128_09.wav']]);assert.equal(previous.returnScene,16);
    console.log(JSON.stringify({stage,realEndingTicks:previous.endingTicks,dialogues,spellTitles,originalTextPages:last+1,visible,media,originalReturnScene:previous.returnScene}));
  }
 };
}
