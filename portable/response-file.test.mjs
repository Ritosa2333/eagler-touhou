import test from 'node:test';
import assert from 'node:assert/strict';
import {spawnSync} from 'node:child_process';
import {responseFileContents} from './response-file.mjs';

test('Emscripten response file roundtrips long Windows and quoted arguments exactly',()=>{
 const args=['-O2','-sENVIRONMENT=web,worker','D:\\source with spaces\\对象.o',
  `single'quote.o`,'embedded"quote','trailing\\','', 'tab\tvalue', 'line\nvalue',
  ...Array.from({length:300},(_,i)=>`D:\\long path\\${'nested-directory\\'.repeat(12)}unit_${i}.o`)];
 const result=spawnSync(process.env.TH_PYTHON||'python',[
  '-c','import sys,shlex,json;print(json.dumps(shlex.split(sys.stdin.read()),ensure_ascii=True))'
 ],{input:responseFileContents(args),encoding:'utf8',env:{...process.env,PYTHONUTF8:'1'},windowsHide:true});
 assert.equal(result.status,0,result.stderr);assert.deepEqual(JSON.parse(result.stdout),args);
});
test('invalid argument values reject before launching the compiler',()=>{
 for(const args of [null,[1],['a\0b']])assert.throws(()=>responseFileContents(args),TypeError);
});
