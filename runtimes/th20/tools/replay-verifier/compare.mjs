import {readFileSync,writeFileSync,mkdirSync} from 'node:fs';
import {resolve,dirname} from 'node:path';
import {compareCaptures} from './comparison.mjs';
const arg=n=>{const i=process.argv.indexOf(n);if(i<0)throw Error('Missing '+n);return process.argv[i+1];};
function load(path){const receipt=JSON.parse(readFileSync(path,'utf8'));const rows=readFileSync(receipt.rows,'utf8').trim().split('\n').filter(Boolean).map(JSON.parse);return {receipt,rows};}
const result=await compareCaptures(load(arg('--expected')),load(arg('--actual')));
const output=resolve(arg('--output'));mkdirSync(dirname(output),{recursive:true});writeFileSync(output,JSON.stringify(result,null,2)+'\n');console.log(JSON.stringify(result,null,2));
if(result.status!=='PASS')process.exitCode=1;
