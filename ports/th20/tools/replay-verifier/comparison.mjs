import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
import {recordsFromCapture,commonRoot} from './adapter.mjs';
const {compareTraces}=await import(pathToFileURL(resolve(commonRoot,'testkit/replay-verifier/compare.mjs')));

export async function compareCaptures(expected,actual){
 const left=Array.from(recordsFromCapture(expected.receipt,expected.rows));
 const right=Array.from(recordsFromCapture(actual.receipt,actual.rows));
 const result=await compareTraces(left,right);
 // A timed-out collector ending before the other collector is missing coverage,
 // not a gameplay divergence. Keep every tick and compare all shared samples.
 if(result.status==='DIVERGED'&&result.reason==='record-type'){
   const sequence=result.firstDifference.sequence;
   const a=left.find(r=>r.sequence===sequence),b=right.find(r=>r.sequence===sequence);
   const incompleteEnd=r=>r?.type==='segment-end'&&r.complete===false;
   if((incompleteEnd(a)&&b?.type==='tick')||(incompleteEnd(b)&&a?.type==='tick'))
     {
       const {firstDifference,...rest}=result;
       return {...rest,status:'INCOMPLETE',reason:'capture-ended-before-peer',
         coverageBoundary:firstDifference,
         expectedTicks:expected.receipt.ticks,actualTicks:actual.receipt.ticks};
     }
 }
 return result;
}
