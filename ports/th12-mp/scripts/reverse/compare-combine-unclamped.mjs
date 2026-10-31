// Uses the independent actual-runtime harness produced by
// tests/enemy-manager-motion.test.mjs; never overwrites production Wasm.
import fs from 'node:fs';
import path from 'node:path';
import {pathToFileURL} from 'node:url';
import {createHash} from 'node:crypto';
const root=path.resolve(import.meta.dirname,'../..');
const fixture=JSON.parse(fs.readFileSync(path.join(root,'tests/fixtures/enemy-combine-unclamped-native.json'),'utf8'));
const wasm=path.join(root,'artifacts/enemy-manager-motion-tests/core.wasm');
const create=(await import(pathToFileURL(path.join(path.dirname(wasm),'core.mjs')))).default;
const c=await create(),failures=[];
for(const [index,row] of fixture.cases.entries()){
  c._setup(1,0,0,0,0,0,0,0,0,0,0);
  c._restore_position(...row.old.slice(0,2),...row.absolute.slice(0,2),...row.relative.slice(0,2),0,0,0);
  c._combine();
  const pointer=c._snapshot()/4,actual=[...c.HEAPF32.subarray(pointer,pointer+30)];
  const bits=new Uint32Array(c.HEAPF32.buffer);
  const actualBits=[bits[pointer+18],bits[pointer+19]],expectedBits=row.combinedBits.slice(0,2);
  if(JSON.stringify(actualBits)!==JSON.stringify(expectedBits)||
    JSON.stringify(actual.slice(0,2))!==JSON.stringify(row.absoluteAfter.slice(0,2))||
    JSON.stringify(actual.slice(9,11))!==JSON.stringify(row.relativeAfter.slice(0,2)))
    failures.push({index,actualPosition:actual.slice(18,20),nativePosition:row.combined.slice(0,2),
      actualBits,expectedBits,actualAbsolute:actual.slice(0,2),nativeAbsolute:row.absoluteAfter.slice(0,2),
      actualRelative:actual.slice(9,11),nativeRelative:row.relativeAfter.slice(0,2)});
}
const report={schema:'th12-combine-unclamped-comparison/1',originalExeSha256:fixture.originalExeSha256,
  partial:true,acceptedReplayGolden:false,wholeGameOracle:false,
  candidateHarness:'artifacts/enemy-manager-motion-tests/core.wasm',
  candidateSha256:createHash('sha256').update(fs.readFileSync(wasm)).digest('hex'),
  testedAxes:'XY only; Enemy has no independent absoluteZ/relativeZ fields',
  samples:fixture.cases.length,failures};
fs.writeFileSync(path.join(root,'artifacts/reverse/enemy-combine-unclamped-comparison.json'),JSON.stringify(report,null,2)+'\n');
console.log(JSON.stringify(report,null,2));
if(failures.length)process.exitCode=1;
