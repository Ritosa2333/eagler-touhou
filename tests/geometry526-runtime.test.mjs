import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath,pathToFileURL} from 'node:url';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
test('WASM owns actual text@R/Main82 state, rejects bad geometry configuration and keeps draw snapshots pure',async()=>{
  const build=JSON.parse(fs.readFileSync(path.join(root,'site/runtime/build.json')));
  const {default:createRuntime}=await import(pathToFileURL(path.join(root,'site',build.entry)).href);
  const module=await createRuntime({print:()=>{},printErr:()=>{}});
  assert.equal(typeof module._th128_world_enable_geometry526,'function','owning geometry526 WASM export is missing');
  const catalog=JSON.parse(fs.readFileSync(path.join(root,'local/catalog.json')));
  const directory=path.join(root,'local',path.dirname(catalog.variants.ja.manifest));
  const allocate=bytes=>{const pointer=module._malloc(bytes.length);module.HEAPU8.set(bytes,pointer);return pointer;};
  for(const name of ['st_a1_1.ecl','default.ecl','st_a1_1b.ecl']){
    const label=Buffer.from(name),bytes=fs.readFileSync(path.join(directory,'raw',name)),namePointer=allocate(label),dataPointer=allocate(bytes);
    assert.equal(module._th128_world_load(namePointer,label.length,dataPointer,bytes.length),1);
    module._free(namePointer);module._free(dataPointer);
  }
  const label=Buffer.from('st_a1_1.ecl'),namePointer=allocate(label);
  assert.equal(module._th128_world_start(namePointer,label.length,1,34772),1);module._free(namePointer);
  assert.equal(module._th128_world_enable_geometry526(module.HEAPU8.byteLength-1,100),0);
  const bytes=fs.readFileSync(path.join(directory,'raw/text.anm')),pointer=allocate(bytes);
  assert.equal(module._th128_world_enable_geometry526(pointer,8),0);
  assert.equal(module._th128_world_enable_geometry526(pointer,bytes.length),1,module.UTF8ToString(module._th128_world_error()));
  assert.equal(module._th128_world_enable_geometry526(pointer,bytes.length),0,'fresh host cannot be replaced after registration');
  module._free(pointer);
  const snapshot=()=>module.UTF8ToString(module._th128_world_state());
  const before=snapshot(),records=module.UTF8ToString(module._th128_world_bullet_draws());
  assert.equal(module.UTF8ToString(module._th128_world_bullet_draw_error()),'');
  assert.equal(module.UTF8ToString(module._th128_world_bullet_draws()),records);assert.equal(snapshot(),before);
  const state=JSON.parse(before);assert.equal(state.geometry526Configured,true);assert.equal(state.geometry526Rows,0);
  const main=JSON.parse(records).find(record=>record.stageSurface==='main');
  assert.ok(main);assert.equal(main.id,10000000);assert.equal(main.bank,6);assert.equal(main.entry,2);
  assert.equal(main.script,82);assert.equal(main.sprite,83);assert.equal(main.layer,29);assert.equal(main.blend,3);assert.equal(main.mode,0);
  assert.equal(main.x,128);assert.equal(main.y,16);assert.equal(main.width,384);assert.equal(main.height,448);
  module._th128_world_reset();assert.equal(JSON.parse(snapshot()).geometry526Configured,false);
  assert.equal(JSON.parse(module.UTF8ToString(module._th128_world_bullet_draws())).length,0);
});
