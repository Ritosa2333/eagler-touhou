import {resolve} from 'node:path';
import {pathToFileURL,fileURLToPath} from 'node:url';
const common=process.env.EAGLER_COMMON_ROOT??fileURLToPath(new URL('../../../eagler-common/',import.meta.url));
const {digestCanonicalJson,recordsFromSegments}=await import(pathToFileURL(resolve(common,'testkit/replay-verifier/adapter.mjs')));
export const fields=['active','stage','replayFrame','stageClock','secondaryClock','gameFlags','input','pressed','released','rng','rngState','playerXBits','playerYBits','playerState','fixedX','fixedY','focused','scoreLow','scoreHigh','power','lives','lifeFragments','bombFragments','bombs','sessionFlags','dialogue','speed0','speed1','speed2','speed3','graze','logicalScaleBits','pointValue','specialItems','pointRatio','overlayMeter'];
export function ticksFromRows(rows){
 let previous=null;
 return rows.map(({words:w})=>{
   if(!Array.isArray(w)||w.length!==36||w.some(v=>!Number.isInteger(v)||v<0||v>0xffffffff)||w[0]!==1)throw Error('Invalid TH20 state words');
   if(!previous||previous[1]!==w[1]){if(w[2]!==1)throw Error('Missing first replay sample');}
   else if(w[2]!==previous[2]+1)throw Error('Missing or reordered replay sample');
   if(w[31]!==0x3f800000)throw Error('Logical viewport mismatch: capture requires 640x480 scale 1');
   previous=w;
   const player=w.slice(11,17),economy=[...w.slice(17,24),...w.slice(32,36)],rng=w.slice(9,11);
   return {stage:w[1],replaySampleIndex:w[2]-1,clockDisposition:'recorded-input',
    appliedInput:{current:w[6],pressed:w[7],released:w[8]},
    clocks:{stage:w[3],secondary:w[4]},
    scalars:Object.fromEntries(fields.slice(5,36).map((name,i)=>[name,w[i+5]])),
    categories:{rng:digestCanonicalJson(rng),player:digestCanonicalJson(player),economy:digestCanonicalJson(economy)}};
 });
}
export function recordsFromCapture(receipt,rows){
 if(receipt.diagnosticOverrides)throw Error('Diagnostic overrides are not ordinary replay parity evidence');
 if(receipt.recordedScoreAudit?.status==='mismatch')throw Error('Playback score differs from immutable replay stage checkpoints');
 const ticks=ticksFromRows(rows),groups=[];
 if(!ticks.length)throw Error('Empty replay capture');
 if(receipt.ticks!==ticks.length)throw Error('Receipt/row count mismatch');
 if(receipt.errors?.length||receipt.exceptions?.length)throw Error('Capture contains runtime exceptions');
 for(const tick of ticks){let group=groups.at(-1);if(!group||group.stage!==tick.stage){group={stage:tick.stage,ticks:[]};groups.push(group);}group.ticks.push(tick);}
 return recordsFromSegments({comparisonIdentity:{game:'th20',profile:'jp-1.00c/ordinary-replay-frame-v2',replaySha256:receipt.replaySha256,executableSha256:receipt.executableSha256,resourceSha256:receipt.resourceSha256,stateSchema:'th20/replay-state-words/v2',traceCodec:'jsonl/v1',digestAlgorithm:'sha256-truncated-128/canonical-json-v1'},
  coverage:{requiredCategories:['rng','player','economy'],optionalCategories:[]},
  provenance:{provider:receipt.provider,candidateWasmSha256:receipt.candidateWasmSha256??null},
  segments:groups.map((g,i)=>({segmentId:`stage-${g.stage}#${i}`,route:`stage-${g.stage}`,ticks:g.ticks,complete:i<groups.length-1||receipt.complete,reason:receipt.reason})),runReason:receipt.reason});
}
export const commonRoot=common;
