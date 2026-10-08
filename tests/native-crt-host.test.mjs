import test from 'node:test';
import assert from 'node:assert/strict';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {readFile} from 'node:fs/promises';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root);

test('original CRT constructors and formatter execute through CPU heap/TLS host imports',async()=>{
  const run=spawnSync(path.join(workspace,'tools/emsdk/python/3.13.3_64bit/python.exe'),['scripts/reverse/sample-crt-format.py'],{cwd:root,encoding:'utf8'});
  assert.equal(run.status,0,run.stdout+run.stderr);
  const native=JSON.parse(await readFile(path.join(root,'artifacts/reverse/crt-format/samples.json'),'utf8'));
  assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
  assert.equal(native.wholeGameOracle,false);assert.equal(native.gameTicksExecuted,0);
  assert.equal(native.gpuExecuted,false);assert.equal(native.gdiExecuted,false);assert.equal(native.realOsExecuted,false);
  assert.deepEqual(native.rngAfter,native.rngBefore);
  assert.deepEqual(native.rngAfterTitleOutput,native.rngBeforeTitleOutput);
  assert.deepEqual(native.records.map(r=>r.text),['  3:007','青符 / 140 %','*2.5','123%']);
  for(const row of native.records)assert.equal(row.returned,row.bytes);
  for(const name of ['HeapCreate','HeapAlloc','FlsAlloc','FlsGetValue','GetLastError','GetCurrentThreadId'])assert.ok(native.host.calls[name]>0,name);
  assert.equal(native.titleOutput.length,1);assert.equal(native.titleOutput[0].entry,'0x44e660');
  assert.equal(native.titleOutput[0].textHex,'90c29584'); // Actual CP932 bytes for 青符.
});
