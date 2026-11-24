import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
test('runtime publication rejects compiler-time source changes before replacing the last good manifest',()=>{
  const work=path.resolve(root,'../../work');
  const temporary=fs.mkdtempSync(path.join(work,'runtime-source-race-'));
  const fixture=path.join(temporary,'outputs/th128-web');
  fs.mkdirSync(path.join(fixture,'tools'),{recursive:true});
  fs.mkdirSync(path.join(fixture,'site/runtime'),{recursive:true});
  fs.cpSync(path.join(root,'src'),path.join(fixture,'src'),{recursive:true});
  fs.copyFileSync(path.join(root,'tools/build-runtime.mjs'),path.join(fixture,'tools/build-runtime.mjs'));
  const previous=JSON.stringify({generation:'last-good-runtime',gameplayImplemented:false});
  const manifest=path.join(fixture,'site/runtime/build.json');
  fs.writeFileSync(manifest,previous);
  const compiler=path.join(temporary,'fake-sdk/upstream/emscripten/emcc.py');
  fs.mkdirSync(path.dirname(compiler),{recursive:true});
  fs.writeFileSync(compiler,[
    'import pathlib, sys',
    'if "--version" in sys.argv:',
    '    print("Emscripten 4.0.22 test compiler")',
    '    raise SystemExit(0)',
    'output=pathlib.Path(sys.argv[sys.argv.index("-o")+1])',
    'output.parent.mkdir(parents=True,exist_ok=True)',
    'output.write_text("export default()=>({});",encoding="utf8")',
    'output.with_suffix(".wasm").write_bytes(bytes.fromhex("0061736d01000000"))',
    'source=pathlib.Path("src/game/StageWorld.hpp")',
    'source.write_bytes(source.read_bytes()+b"\\n")',
  ].join('\n')+'\n');
  const result=spawnSync(process.execPath,[path.join(fixture,'tools/build-runtime.mjs')],
    {cwd:fixture,encoding:'utf8',timeout:30000,env:{...process.env,TH128_EMCC:compiler,TH128_PYTHON:process.env.TH128_PYTHON??'python'}});
  assert.notEqual(result.status,0,'source-changing compiler must not publish a generation');
  assert.match(result.stdout+result.stderr,/source.*changed.*compil/i);
  assert.equal(fs.readFileSync(manifest,'utf8'),previous);
  assert.equal(fs.existsSync(path.join(fixture,'site/runtime/builds')),false);
});
