import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {pathToFileURL,fileURLToPath} from 'node:url';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
test('WASM world advances real ECL beyond root opcode 402 and retains its first real enemy',async()=>{
  const build=JSON.parse(fs.readFileSync(path.join(root,'site/runtime/build.json'),'utf8'));
  const {default:createRuntime}=await import(pathToFileURL(path.join(root,'site',build.entry)).href);
  const module=await createRuntime({print:()=>{},printErr:()=>{}});
  module._th128_player_reset(0,51200);
  assert.equal(module._th128_player_move(16,16,0,1,1,4,0),1);
  assert.equal(module._th128_player_x(),0);assert.equal(module._th128_player_y(),50624);
  assert.equal(module._th128_player_move(16|8,0,0,1,1,4,0),1);
  assert.equal(module._th128_player_y(),50368);
  assert.doesNotThrow(()=>assert.equal(module._th128_world_load(module.HEAPU8.byteLength-1,10,0,0),0));
  module._th128_world_reset();
  const local=path.join(root,'local');const catalog=JSON.parse(fs.readFileSync(path.join(local,'catalog.json'),'utf8'));
  const directory=path.dirname(path.join(local,catalog.variants.ja.manifest));
  const nameBuffer=name=>{const bytes=Buffer.from(name);const pointer=module._malloc(bytes.length);module.HEAPU8.set(bytes,pointer);return [pointer,bytes.length];};
  for(const name of ['st_a1_1.ecl','default.ecl','st_a1_1b.ecl']){
    const bytes=fs.readFileSync(path.join(directory,'raw',name));const pointer=module._malloc(bytes.length);module.HEAPU8.set(bytes,pointer);
    const [namePointer,nameLength]=nameBuffer(name);
    assert.equal(module._th128_world_load(namePointer,nameLength,pointer,bytes.length),1,module.UTF8ToString(module._th128_world_error()));
    module._free(namePointer);module._free(pointer);
  }
  const [stagePointer,stageLength]=nameBuffer('st_a1_1.ecl');
  assert.equal(module._th128_world_start(stagePointer,stageLength,1,34772),1,module.UTF8ToString(module._th128_world_error()));
  assert.equal(module._th128_world_tick(),1);
  const retained=module.UTF8ToString(module._th128_world_state());
  assert.equal(module._th128_world_start(stagePointer,stageLength,5,34772),0);
  assert.equal(module.UTF8ToString(module._th128_world_state()),retained,'rejected start keeps active world state');
  assert.equal(module._th128_world_tick(),1,'old world continues after rejected start');
  module._free(stagePointer);
  for(let frame=0;frame<200;frame++)if(!module._th128_world_tick())break;
  const state=JSON.parse(module.UTF8ToString(module._th128_world_state()));
  assert.ok(state.spawns>=1);assert.equal(state.linkedSources,3);
  const first=state.entities.find(entity=>!entity.root);
  assert.equal(first.x,-128);assert.equal(first.y,-32);assert.equal(first.life,20);
  assert.equal(first.animation0,0);assert.equal(first.bank0,1);
  assert.equal(first.emitter0.extensionType,2);
  assert.equal(state.positionIntegrated,false);
  assert.match(module.UTF8ToString(module._th128_world_error()),/opcode=501/);
  assert.notEqual(module.UTF8ToString(module._th128_world_error()),'','Unimplemented later game callback still stops');
});
