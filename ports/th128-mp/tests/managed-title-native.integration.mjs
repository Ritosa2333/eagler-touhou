// Offline: node tests/managed-title-native.integration.mjs <original core.mjs/core.wasm directory> <original raw ANM directory>
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {join,resolve} from 'node:path';
import {test} from 'node:test';
import {pathToFileURL} from 'node:url';
import {openManagedRoomTitle} from '../runtime/managed-stage-resources.mjs';
import {createManagedTitlePresenter} from '../runtime/managed-title-presenter.mjs';

const tick=1000/60;
const [coreDir,anmDir]=process.argv.slice(2);
test('actual TH128 Main introduction, animated ASCII children and version art survive readiness and a room handoff',async t=>{
  assert.ok(coreDir&&anmDir,'supply the original web core directory and raw ANM directory');
  const core=(await import(pathToFileURL(resolve(coreDir,'core.mjs')))).default;
  const factories=[],reads=[],assets=[],life=new AbortController();let nativeTicks=0;
  // The shipping Emscripten glue targets web. Supply all bytes directly; no browser, server or mounts.
  globalThis.window={};t.after(()=>{delete globalThis.window;});
  const title=await openManagedRoomTitle({signal:life.signal,factory:core,onFactory:name=>factories.push(name),onTitleTick:()=>nativeTicks++,
    resources:{read(key){reads.push(key);assert.equal(key,'wasm');return readFile(join(coreDir,'core.wasm'));},
      raw(name){assets.push(name);assert.ok(['ascii.anm','title.anm','title_v.anm'].includes(name));return readFile(join(anmDir,name));}}});
  t.after(()=>title.close());
  assert.deepEqual(title.banks,[[11,'ascii'],[24,'title'],[25,'title_v']]);
  const banks=new Set(title.banks.map(([bank])=>bank));
  const samples=new Map(),wanted=new Set([0,60,61,71,72,73,1872,1873]);
  let raf=null,paints=0;
  const presenter=createManagedTitlePresenter({title,visible:()=>true,
    requestFrame(fn){assert.equal(raf,null);raf=fn;return 0;},cancelFrame(){raf=null;},onError:e=>{throw e;},
    draw(){
      const rows=title.draws(),{page,ready,transitioning,mainAction}=title.status(),frame=title.ticks();
      assert.equal(page,0);assert.equal(mainAction,0);assert.ok(rows.length>0&&rows.every(row=>banks.has(row.bank)));
      if(wanted.has(frame))samples.set(frame,{ready,transitioning,rows});
      paints++;return true;
    }});
  t.after(()=>presenter.close());
  function frame(now){assert.ok(raf,`expected a live title RAF after native tick ${title.ticks()}`);const fn=raf;raf=null;fn(now);}
  const first=presenter.present(life.signal);
  for(let n=0;n<=72;n++)frame(n*tick);
  assert.equal(await first,true);
  for(const n of [0,60,61,71])assert.deepEqual([samples.get(n).ready,samples.get(n).transitioning],[0,1]);
  const has=(n,bank,script)=>samples.get(n).rows.some(row=>row.bank===bank&&row.script===script);
  for(const script of [98,99,100])assert.ok(has(0,24,script),'the opening title art comes from the original Main roots');
  assert.ok(!has(60,25,0)&&has(61,25,0),'the version root is bound on the update entered at native frame 60');
  for(let script=3;script<=16;script++)assert.ok(has(61,24,script),`missing native Main row ${script}`);
  assert.deepEqual([samples.get(72).ready,samples.get(72).transitioning],[1,0]);
  // The ordinary page's separate 1,800-idle-tick JS clock launches an RPY. The room only owns native Main.
  for(let n=73;n<=1873;n++)frame(n*tick);
  function visibleFlakes(n){return samples.get(n).rows.filter(r=>r.bank===11&&r.script===23&&r.sprite===316&&(r.color>>>24)>0&&r.x>0&&r.x<640&&r.y>0&&r.y<480);}
  for(const [a,b] of [[72,73],[1872,1873]]){
    const after=new Map(visibleFlakes(b).map(r=>[r.id,r]));
    assert.ok(visibleFlakes(a).some(r=>{const s=after.get(r.id);return s&&s.y!==r.y&&s.rotation!==r.rotation;}),
      `an already visible native particle must move and rotate from tick ${a} to ${b}, not just change a frame counter`);
    assert.ok(has(b,25,0));assert.deepEqual([samples.get(b).ready,samples.get(b).transitioning],[1,0]);
  }
  const oldFrame=raf,before=paints,retained=samples.get(1873).rows;
  presenter.stop();oldFrame(99999);assert.equal(raf,null);assert.equal(paints,before);assert.equal(nativeTicks,1873);
  const back=presenter.present(life.signal);frame(100000);assert.equal(await back,true);
  assert.equal(paints,before+1);assert.deepEqual(samples.get(1873).rows,retained,'a return redraws the retained native ANM scene');
  frame(100000+tick);assert.equal(title.ticks(),1874);assert.equal(nativeTicks,1874);
  const stale=raf;life.abort();stale(100000+2*tick);assert.equal(raf,null);assert.equal(paints,before+2);assert.equal(nativeTicks,1874);
  assert.deepEqual(factories,['title']);assert.deepEqual(reads,['wasm']);assert.deepEqual(assets,['ascii.anm','title.anm','title_v.anm']);
});
