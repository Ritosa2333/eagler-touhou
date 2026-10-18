import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtempSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import {execFileSync} from 'node:child_process';
import {compilerArguments} from './compiler-arguments.mjs';
test('long Windows paths round-trip through the parser Emscripten uses',()=>{
 const output=mkdtempSync(join(tmpdir(),'eagler-compiler-args-'));
 const args=['C:\\游戏 目录\\font.ttf','-DNAME="quoted"','',...Array.from({length:1500},(_,i)=>`C:\\long source path\\${i}\\object.o`)];
 const encoded=compilerArguments(args,{compiler:'emcc.py',output,platform:'win32'});
 assert.equal(encoded.length,1);
 const decoded=JSON.parse(execFileSync('python',['-c','import json,shlex,sys;print(json.dumps(shlex.split(open(sys.argv[1],encoding="utf-8").read())))',encoded[0].slice(1)],{encoding:'utf8'}));
 assert.deepEqual(decoded,args);
 assert.deepEqual(compilerArguments(args,{compiler:'emcc.py',output,platform:'linux'}),args);
});
