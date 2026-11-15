import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath,pathToFileURL} from 'node:url';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');

test('WASM ECL core loads all actual resources and exposes the first unimplemented game callback',
  {skip:!fs.existsSync(path.join(root,'local/catalog.json'))||!fs.existsSync(path.join(root,'site/runtime/core.mjs'))},async()=>{
    const published=JSON.parse(fs.readFileSync(path.join(root,'site/runtime/build.json'),'utf8'));
    const {default:createRuntime}=await import(pathToFileURL(path.join(root,'site',published.entry)).href);
    const module=await createRuntime({print:()=>{},printErr:()=>{}});
    assert.doesNotThrow(()=>assert.equal(module._th128_load_ecl(63,module.HEAPU8.byteLength-4,64),0));
    assert.doesNotThrow(()=>assert.equal(module._th128_load_ecl(63,module.HEAPU8.byteLength-1,64),0));
    const catalog=JSON.parse(fs.readFileSync(path.join(root,'local/catalog.json'),'utf8'));
    const manifestPath=path.join(root,'local',catalog.variants.ja.manifest);
    const manifest=JSON.parse(fs.readFileSync(manifestPath,'utf8'));
    let rootSlot=-1;
    for(const [slot,record] of manifest.ecl.entries()){
      const data=fs.readFileSync(path.join(path.dirname(manifestPath),'raw',record.name));
      const pointer=module._malloc(data.length);module.HEAPU8.set(data,pointer);
      assert.equal(module._th128_load_ecl(slot,pointer,data.length),1,module.UTF8ToString(module._th128_ecl_error()));
      module._free(pointer);
      if(record.name==='st_a1_1.ecl')rootSlot=slot;
    }
    assert.ok(rootSlot>=0);
    assert.equal(module._th128_ecl_start(rootSlot,1),1,module.UTF8ToString(module._th128_ecl_error()));
    assert.equal(module._th128_ecl_tick(),0,'Unimplemented game opcode must stop rather than silently skip');
    const reason=module.UTF8ToString(module._th128_ecl_error());
    assert.match(reason,/402/);assert.match(reason,/main/);
    assert.equal(module._th128_ecl_status(),3);
    const loadedRoot=fs.readFileSync(path.join(path.dirname(manifestPath),'raw','st_a1_1.ecl'));
    const activePointer=module._malloc(loadedRoot.length);
    module.HEAPU8.set(loadedRoot,activePointer);
    assert.equal(module._th128_ecl_start(rootSlot,1),1);
    assert.equal(module._th128_load_ecl(rootSlot,activePointer,loadedRoot.length),0,
      'Even a running VM cannot retain an unaccounted superseded slot generation');
    assert.equal(module._th128_ecl_status(),1);
    module._free(activePointer);
    assert.equal(module._th128_ecl_tick(),0);
    assert.equal(module._th128_load_ecl(100,0,0),0);
    assert.equal(module.UTF8ToString(module._th128_ecl_error()),reason,'Load errors must not erase the VM fault');
    const replacement=fs.readFileSync(path.join(path.dirname(manifestPath),'raw','st_a1_1.ecl'));
    const replacementPointer=module._malloc(replacement.length);
    module.HEAPU8.set(replacement,replacementPointer);
    assert.equal(module._th128_load_ecl(63,replacementPointer,replacement.length),1);
    assert.equal(module.UTF8ToString(module._th128_ecl_error()),reason,'Successful other-slot load preserves VM fault');
    assert.equal(module._th128_load_ecl(rootSlot,replacementPointer,replacement.length),0,
      'Replacing the VM-pinned slot must require explicit VM reset');
    module._free(replacementPointer);
    assert.equal(module.UTF8ToString(module._th128_ecl_error()),reason);
    assert.equal(module._th128_ecl_start(-1,1),0);
  });
