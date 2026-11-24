import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath,pathToFileURL} from 'node:url';
import {spawnSync} from 'node:child_process';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const compiler=process.env.TH128_EMCC ?? path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
const catalogPath=path.join(root,'local/catalog.json');

test('actual TH128 title banks execute in the standalone WASM animation core',
  {skip:!fs.existsSync(compiler)||!fs.existsSync(catalogPath),timeout:1200000},async()=>{
    const built=spawnSync(process.execPath,['tools/build-runtime.mjs'],{cwd:root,encoding:'utf8',env:{...process.env,TH128_EMCC:compiler},timeout:1100000});
    assert.equal(built.status,0,built.stdout+built.stderr);
    const published=JSON.parse(fs.readFileSync(path.join(root,'site/runtime/build.json'),'utf8'));
    const {default:createRuntime}=await import(pathToFileURL(path.join(root,'site',published.entry)).href+`?test=${Date.now()}`);
    const module=await createRuntime({print:()=>{},printErr:()=>{}});
    assert.equal(module._th128_boot_title(0),0);
    assert.doesNotThrow(()=>assert.equal(module._th128_load_anm(99,module.HEAPU8.byteLength-4,64),0));
    assert.doesNotThrow(()=>assert.equal(module._th128_load_anm(99,module.HEAPU8.byteLength-1,64),0));
    const catalog=JSON.parse(fs.readFileSync(catalogPath,'utf8'));
    const original=path.dirname(path.join(root,'local',catalog.variants.ja.manifest));
    for(const [bank,name] of [[24,'title.anm'],[25,'title_v.anm'],[11,'ascii.anm']]){
      const data=fs.readFileSync(path.join(original,'raw',name));
      const pointer=module._malloc(data.length);
      module.HEAPU8.set(data,pointer);
      assert.equal(module._th128_load_anm(bank,pointer,data.length),1,module.UTF8ToString(module._th128_error()));
      module._free(pointer);
    }
    assert.equal(module._th128_boot_title(0x1234),1,module.UTF8ToString(module._th128_error()));
    for(let frame=0;frame<120;frame++) assert.equal(module._th128_tick(),1,module.UTF8ToString(module._th128_error()));
    assert.equal(module._th128_frame(),120);
    assert.ok(module._th128_nodes()>10);
    assert.ok(module._th128_draw_count()>10);
    assert.equal(module._th128_draw_stride(),160);
    const visible=module._th128_draw_count();
    module._th128_draw_enabled(1,0);
    assert.ok(module._th128_draw_count()<visible,'Scene drawEnabled must suppress its root and descendants');
    module._th128_draw_enabled(1,1);
    assert.equal(module._th128_draw_count(),visible);
    assert.equal(module._th128_freeze_can_release(2999,45,0),0);
    assert.equal(module._th128_freeze_can_release(3000,45,0),1);
    assert.equal(module._th128_freeze_can_release(10000,44,0),0);
    assert.equal(module._th128_freeze_duration(3000),12);
    assert.equal(module._th128_freeze_duration(10000),100);
    assert.equal(module._th128_freeze_add_power(9999,5),10000);
    assert.equal(module.UTF8ToString(module._th128_error()),'');
    module._th128_select(1);
    assert.equal(module._th128_tick(),1,module.UTF8ToString(module._th128_error()));
    assert.equal(module._th128_selection(),1);
  });
