import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {createHash} from 'node:crypto';
import {fileURLToPath} from 'node:url';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
test('published runtime manifest points to a complete immutable JS/WASM pair',()=>{
  const manifest=JSON.parse(fs.readFileSync(path.join(root,'site/runtime/build.json'),'utf8'));
  assert.match(manifest.entry,/^runtime\/builds\/[a-f0-9]{64}\/core\.mjs$/);
  const directory=path.dirname(path.join(root,'site',manifest.entry));
  for(const [name,record] of Object.entries(manifest.artifacts)){
    const bytes=fs.readFileSync(path.join(directory,name));
    assert.equal(bytes.length,record.bytes);
    assert.equal(createHash('sha256').update(bytes).digest('hex'),record.sha256);
  }
  assert.equal(manifest.game,'th128');
  assert.equal(manifest.gameplayImplemented,false);
});
