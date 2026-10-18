#!/usr/bin/env node
import {readFileSync} from 'node:fs';
import {dirname,resolve} from 'node:path';
import {fileURLToPath} from 'node:url';

const here=dirname(fileURLToPath(import.meta.url)),root=resolve(here,'../../th09_web/artifacts/replay-verifier');
const results=[0,1,2].map(index=>JSON.parse(readFileSync(resolve(root,`demo${index}.json`),'utf8')));
if(results.some((result,index)=>result.id!==`demo${index}`||result.status!=='PASS'))throw Error('TH09 quick Demo verifier is incomplete or failed');
for(const result of results)process.stdout.write(`${result.id}: ${result.status} (${result.comparedTicks} ticks)\n`);
process.stdout.write(`TH09 Replay verifier: PASS (${results.reduce((sum,result)=>sum+result.comparedTicks,0)} fixed ticks)\n`);
