import test from 'node:test';
import assert from 'node:assert/strict';
import {execFileSync} from 'node:child_process';

test('large input inventories finish writing to a pipe before the builder exits',()=>{
  const helper=new URL('./runtime-inputs.mjs',import.meta.url).href;
  const flags=Array.from({length:20000},(_,n)=>'-DPIPE_INVENTORY_'+n+'='+('x'.repeat(64)));
  const output=execFileSync(process.execPath,['--input-type=module','-e',`
    import {printRuntimeInputs} from ${JSON.stringify(helper)};
    const flags=Array.from({length:20000},(_,n)=>'-DPIPE_INVENTORY_'+n+'='+('x'.repeat(64)));
    await printRuntimeInputs({workspace:process.cwd(),root:process.cwd(),sources:[],headers:[],flags});
    throw Error('Input inspection continued into the compiler');
  `,'--','--print-inputs'],{encoding:'utf8',maxBuffer:8*1024*1024});
  assert.ok(Buffer.byteLength(output)>1024*1024);
  const report=JSON.parse(output);assert.equal(report.schema,'eagler-touhou/runtime-inputs/1');
  assert.deepEqual(report.compilerFlags,flags);
});
