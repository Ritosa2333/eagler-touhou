import assert from 'node:assert/strict';
import {join} from 'node:path';
import {writeFile} from 'node:fs/promises';
import {chooseCirnoPalettes,cirnoPalettes,cirnoPaletteSet} from '../runtime/coop-cirno-palettes.mjs';

const panel='#roomWorkspaceTab-game',selector='#mpRoomRandomCirnoPalette';
export async function configurePalettes({pages,enabled,until,work}){
  for(const page of pages){await page.bringToFront();await page.click(panel);await page.waitForSelector(selector,{visible:true,timeout:10000});}
  const values=()=>Promise.all(pages.map(page=>page.evaluate(id=>{
    const control=document.querySelector(id);if(!control||!control.getClientRects().length)return null;
    return {on:control.getAttribute('aria-checked')==='true'||control.checked===true,locked:control.disabled===true||control.getAttribute('aria-disabled')==='true'};
  },selector)));
  const want=value=>until(`every lobby mirrors random Cirno colors ${value}`,async()=>{
    const actual=await values();return actual.every((item,i)=>item&&item.on===value&&item.locked===(i!==0))?actual:false;
  });
  const defaultOff=await want(false);
  if(enabled){
    const toggle=()=>pages[0].evaluate(id=>document.querySelector(id).click(),selector);
    await toggle();await want(true);await toggle();await want(false);await toggle();await want(true);
  }
  await pages[0].screenshot({path:join(work,'cirno-room-rule.png')});
  await writeFile(join(work,'cirno-room-rule.json'),JSON.stringify({defaultOff,enabled,visibleTo:pages.length},null,2));
}

export async function configureFreezeMotivation({pages,until,work,multiplier}){
  const select='#mpRoomFreezeMotivationMultiplier',trigger=select+'-trigger',host=pages[0];
  for(const page of pages){await page.bringToFront();await page.click(panel);}
  const states=()=>Promise.all(pages.map(p=>p.$eval(select,el=>({value:Number(el.value),disabled:el.disabled,options:[...el.options].map(o=>[Number(o.value),o.textContent])}))));
  const check=value=>until(`shared freeze motivation ${value}`,async()=>{
    const rows=await states().catch(()=>[]);
    return rows.length===pages.length&&rows.every((s,i)=>s.value===value&&s.disabled===(i!==0))?rows:null;
  });
  const baseline=await check(1);for(const row of baseline){
    assert.deepEqual(row.options.map(([value])=>value),[2.5,2,1,0.75,0.5]);
    assert.deepEqual(row.options.map(([,label])=>label.match(/^×[\d.]+/)?.[0]),['×2.5','×2.0','×1.0','×0.75','×0.5']);
  }
  await host.bringToFront();await host.click(trigger);
  await host.waitForSelector(`[role="option"][data-value="${multiplier}"]`,{visible:true});
  await host.click(`[role="option"][data-value="${multiplier}"]`);
  const mirrored=await check(multiplier);
  await host.screenshot({path:join(work,'freeze-motivation-rule.png')});
  await writeFile(join(work,'freeze-motivation-room.json'),JSON.stringify({multiplier,baseline,mirrored},null,2));
}

async function observe(frame){
  await frame.evaluate(async()=>{
    if(window.__cirnoColors)return;
    const url=new URL('ports/th128/site/runtime/renderer-owned-ending-v2.mjs',location.href),{WebGlRenderer}=await import(url.href);
    const render=WebGlRenderer.prototype.render,state=window.__cirnoColors={sample:null};
    WebGlRenderer.prototype.render=function(records,options){
      const output=render.call(this,records,options);
      if(options?.world&&this.canvas.id==='game')state.sample=records.filter(r=>r.drawPass==='player'&&r.entry===0&&r.id>=4000000&&r.id<=4000020&&(r.id-4000000)%10===0)
        .map(r=>({seat:(r.id-4000000)/10,bank:r.bank,source:this.textures.get(`${r.bank}:${r.entry}`)?.sourceKey})).sort((a,b)=>a.seat-b.seat);
      return output;
    };
  });
}
export async function readPalettePaint(frame,seats,until,scene){
  await observe(frame);
  return until(`all ${seats} Cirno body records painted in ${scene}`,async()=>{
    const evidence=await frame.evaluate(()=>({scene:document.querySelector('#game')?.dataset.scene,records:window.__cirnoColors.sample}));
    return evidence.scene===scene&&evidence.records?.length===seats?evidence.records:null;
  });
}
export function assertPalettePaint(records,expected){
  assert.deepEqual(records.map(row=>row.seat),[...expected.keys()]);
  for(const [i,id] of expected.entries()){
    const asset=id?cirnoPalettes[id-1]:null;
    assert.equal(records[i].bank,asset?.bank??7);
    assert.ok(records[i].source.includes(asset?`/palettes/${asset.file}`:'/textures/pl00-000.png'),'the actual alternate (or original) atlas must be loaded by WebGL');
  }
}
export async function observeCoopPalettes({players,watcher,child,until,work}){
  const clients=await Promise.all([...players,...(watcher?[watcher]:[])].map(async p=>{
    const {f}=await child(p);return {f,records:await readPalettePaint(f,players.length,until,'match')};
  }));
  const result=clients.map(c=>c.records),enabled=process.env.TH128_TEST_PALETTE==='on';
  const colors=result[0].map(row=>row.bank===7?0:row.bank-79);
  assert.equal(colors[0],0);assert.equal(new Set(colors).size,enabled?players.length:1);
  if(enabled)assert.ok(colors.slice(1).every(id=>id>=1&&id<=cirnoPalettes.length));
  for(const rows of result)assertPalettePaint(rows,colors);
  await (await clients[0].f.$('#game')).screenshot({path:join(work,'cirno-palettes-battle.png')});
  await writeFile(join(work,'cirno-palettes-live.json'),JSON.stringify({colors,clients:result},null,2));
  return colors;
}
export async function verifyRecordedPalettes(record,live,frame,until){
  const rule=record.header.rules;
  assert.equal(rule.randomCirnoPalette===true,process.env.TH128_TEST_PALETTE==='on');
  assert.equal(rule.cirnoPaletteSet,rule.randomCirnoPalette?cirnoPaletteSet:undefined);
  assert.deepEqual(chooseCirnoPalettes(rule),live,'the tape reproduces the colors everybody just watched');
  assertPalettePaint(await readPalettePaint(frame,rule.players,until,'replay'),live);
}
