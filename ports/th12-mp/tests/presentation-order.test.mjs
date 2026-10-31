import test from 'node:test';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {runNativeCpp} from '../../../launcher/tests/support/native-cpp.mjs';
test('SDL draw ordering preserves ties, priority fallback and frame-local identity reuse',async()=>{
 const source=await readFile(resolve(import.meta.dirname,'native/sdl-presentation-order.cpp'),'utf8');
 const output=await runNativeCpp(source,{includes:[resolve(import.meta.dirname,'../portable/sdl')]});
 assert.match(output,/priority buckets and epoch lookup match/);
});
