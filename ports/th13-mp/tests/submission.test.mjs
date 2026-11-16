import test from 'node:test';
import assert from 'node:assert/strict';
import {spawnSync} from 'node:child_process';
import {mkdtempSync,readFileSync,readdirSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {resolve,dirname,join} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';

const port=resolve(dirname(fileURLToPath(import.meta.url)),'..');
const source=rel=>readFileSync(resolve(port,rel),'utf8');
test('actual GraphicsDevice native submission controls (no SDL/GLES dependency)',{timeout:120000},()=>{
  const dir=mkdtempSync(join(tmpdir(),'th13-submission-'));
  const compile=spawnSync('g++',['-std=c++17','-O2','-Wall','-Wextra','-Werror','-Wno-attributes',
    '-Wno-missing-field-initializers','-fno-exceptions','-fno-rtti','-ffp-contract=off',
    '-I'+resolve(port,'tests/submission/include'),resolve(port,'tests/submission/submission_test.cpp'),
    resolve(port,'web/cpp/sdl/GraphicsDevice.cpp'),resolve(port,'web/cpp/game/TextureImage.cpp'),
    '-o',join(dir,'test')],{encoding:'utf8'});
  assert.equal(compile.status,0,compile.stdout+compile.stderr);
  const run=spawnSync(join(dir,'test'),[dir],{encoding:'utf8'});
  assert.equal(run.status,0,run.stdout+run.stderr);
  const records=readFileSync(join(dir,'base.jsonl'),'utf8').trimEnd().split('\n').map(JSON.parse);
  assert.equal(records[0].schema,'th13-submission-v2');assert.equal(records[0].content_hash,'sha256');
  let frame=0,seq=0;
  for(const r of records.slice(1,-1)) {assert.equal(r.frame,frame);assert.equal(r.seq,seq++);if(r.kind==='present')++frame;}
  assert.deepEqual(records.at(-1),{schema:'th13-submission-v2',kind:'complete',frames:frame,events:seq,complete:true,error:''});
  const draw=records.find(r=>r.kind==='draw');assert.equal(draw.vertices.length,6*28*2);
  const texture=draw.resources[0];
  assert.equal(texture.content,createHash('sha256').update(Buffer.alloc(16)).digest('hex'));
  const paths=[['vertices'],['vertices'],['state','pipeline'],['state','bindings'],['state','layout'],['state','viewport'],['state','matrices']];
  for(let m=1;m<=8;++m) {
    const variant=readFileSync(join(dir,`mutation${m}.jsonl`),'utf8').trimEnd().split('\n').map(JSON.parse);
    if(m<=7){let a=draw,b=variant.find(r=>r.kind==='draw');for(const key of paths[m-1]){a=a[key];b=b[key];}assert.notDeepEqual(a,b,`mutation ${m}`);}
    else {const update=variant.find(r=>r.kind==='update');assert.equal(update.previous,0);assert.equal(update.resources[0].revision,1);assert.notEqual(update.resources[0].content,texture.content);}
  }
  assert.ok(run.stdout.includes('real write and close failures'));
  for(const line of run.stdout.split('\n').filter(line=>line.startsWith('hash '))) {
    const [,n,hash]=line.split(' ');const bytes=Buffer.alloc(65);bytes[64]=1;
    assert.equal(hash,createHash('sha256').update(bytes.subarray(0,Number(n))).digest('hex'));
  }
});

test('production submission bypass guard and legacy-only title supplements',()=>{
  const walk=dir=>readdirSync(dir,{withFileTypes:true}).flatMap(e=>e.isDirectory()?walk(join(dir,e.name)):[join(dir,e.name)]);
  for(const path of walk(resolve(port,'web/cpp')).filter(p=>/\.(cpp|hpp)$/.test(p))) {
    const text=readFileSync(path,'utf8');
    assert.doesNotMatch(text,/\bbackend\.(?:draw|draw_batch|clear|copy|resample|changed|prepare|release|present|flush)\s*\(/,path);
    assert.doesNotMatch(text,/capture_shadow|capture_handles_/,path);
  }
  const device=source('web/cpp/sdl/GraphicsDevice.cpp');
  assert.match(device,/c\.dispatch\(backend, submission_trace, trace_resource, this\)/);
  assert.doesNotMatch(device,/draw_capture\s*=/);
  assert.match(source('web/cpp/game/TitleFrame.cpp'),/!draw_capture\s*\|\|\s*!draw_capture->legacy_harness\(\)/);
  assert.match(source('web/cpp/game/DrawCapture.hpp'),/#ifdef __EMSCRIPTEN__\s+return false;/);
});

test('framebuffer sampling restores read/draw bindings and pack destination',()=>{
  const runtime=source('web/cpp/sdl/BrowserRuntime.cpp');
  for(const name of ['READ_FRAMEBUFFER_BINDING','DRAW_FRAMEBUFFER_BINDING','PACK_ALIGNMENT','PACK_ROW_LENGTH','PACK_SKIP_ROWS','PACK_SKIP_PIXELS','PIXEL_PACK_BUFFER_BINDING'])
    assert.ok(runtime.includes(`glGetIntegerv(GL_${name}`),name);
  assert.match(runtime,/glBindFramebuffer\(GL_READ_FRAMEBUFFER, GLuint\(read\)\)/);
  assert.match(runtime,/glBindFramebuffer\(GL_DRAW_FRAMEBUFFER, GLuint\(draw\)\)/);
  assert.match(runtime,/glBindBuffer\(GL_PIXEL_PACK_BUFFER, GLuint\(pbo\)\)/);
  assert.match(runtime,/capture_finish\(\)[\s\S]*?return ~0u;/);
});
