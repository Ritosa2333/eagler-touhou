// Diagnostic supplement to the strict chronological common comparator.
// Match exact (stage, input-frame) identities; never shift or discard a frame.
import {readFileSync,writeFileSync} from 'node:fs';
import {fields} from './adapter.mjs';
const arg=name=>process.argv[process.argv.indexOf(name)+1];
const read=path=>readFileSync(path,'utf8').trim().split('\n').filter(Boolean).map(s=>JSON.parse(s).words);
const original=read(arg('--expected')),candidate=read(arg('--actual'));
function index(rows){
 const result=new Map();
 for(const w of rows){const key=w[1]+':'+w[2];if(result.has(key))throw Error('Duplicate frame '+key);result.set(key,w);}
 return result;
}
const left=index(original),right=index(candidate),counts={},examples=[],missing=[];
let shared=0,different=0,firstGameplayDifference=null,firstNonLoadingDifference=null;
for(const [key,a] of left){
 const b=right.get(key);if(!b){if(missing.length<100)missing.push({side:'candidate',frame:key});continue;}
 shared++;const differences=[];
 if(a.length!==fields.length||b.length!==fields.length)throw Error('State schema mismatch');
 for(let i=0;i<fields.length;i++)if(a[i]!==b[i]){const field=fields[i];counts[field]=(counts[field]??0)+1;differences.push({field,expected:a[i],actual:b[i]});}
 if(differences.length){different++;if(examples.length<100)examples.push({frame:key,differences});
  if(!firstNonLoadingDifference&&!((a[5]|b[5])&0x1004))firstNonLoadingDifference={frame:key,differences};
  // These four fields may reflect asynchronous host loading. This classification
  // is diagnostic only: they remain differences and never produce a PASS.
  if(!firstGameplayDifference&&differences.some(d=>!['stageClock','secondaryClock','gameFlags','sessionFlags'].includes(d.field)))firstGameplayDifference={frame:key,differences};
 }
}
for(const key of right.keys())if(!left.has(key)&&missing.length<100)missing.push({side:'original',frame:key});
const result={purpose:'exact-frame diagnostic; not full replay acceptance',originalFrames:original.length,candidateFrames:candidate.length,sharedFrames:shared,differentFrames:different,fieldDifferenceCounts:counts,firstGameplayDifference,firstNonLoadingDifference,examples,missingFrameExamples:missing};
if(process.argv.includes('--output'))writeFileSync(arg('--output'),JSON.stringify(result,null,2)+'\n');
console.log(JSON.stringify({...result,examples:undefined,missingFrameExamples:undefined},null,2));
