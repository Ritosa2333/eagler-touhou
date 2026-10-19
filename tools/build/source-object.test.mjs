import assert from 'node:assert/strict';
import {test} from 'node:test';
import {dirname,resolve} from 'node:path';
import {sourceObjectPath} from './source-object.mjs';

test('absolute and parent-directory shared inputs stay inside the object cache',()=>{
  const workspace=resolve('ports/th08'),objects=resolve(workspace,'web/artifacts/sdl3/objects');
  const inputs=[resolve('common/third_party/imgui/imgui.cpp'),resolve(workspace,'web/cpp/game/Player.cpp'),
    resolve(workspace,'../../common/third_party/imgui/imgui_draw.cpp')];
  const outputs=inputs.map(source=>sourceObjectPath(objects,workspace,source));
  for(const output of outputs)assert.equal(dirname(output),objects);
  assert.equal(new Set(outputs).size,inputs.length);
  assert.notEqual(sourceObjectPath(objects,workspace,resolve('common/a.cpp')),
    sourceObjectPath(objects,workspace,resolve('common/nested/a.cpp')));
});
