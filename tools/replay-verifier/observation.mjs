import {resolve} from 'node:path';
import {pathToFileURL, fileURLToPath} from 'node:url';
import {readFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
const common=resolve(process.env.EAGLER_COMMON_ROOT||fileURLToPath(new URL('../../../eagler-common',import.meta.url)));
const {TraceCapture}=await import(pathToFileURL(resolve(common,'testkit/replay-verifier/capture-file.mjs')));
const sha=b=>createHash('sha256').update(b).digest('hex');
export const categories=['player','economy','rng','entities'];
export function identity(root,raw) {
  const target=JSON.parse(readFileSync(resolve(root,'target.json')));
  return {game:'th15',profile:'jp-1.00b/world-replay-v1',replaySha256:sha(raw),executableSha256:target.sha256,
    resourceSha256:sha(readFileSync(resolve(root,target.executable.replace(/\.exe$/,'.dat')))),
    stateSchema:'th15/authoritative-selected-state/v1',traceCodec:'jsonl/v1',digestAlgorithm:'sha256-truncated-128/canonical-json-v1'};
}
export function capture(path,identity,provider,provenance={}) {return new TraceCapture(path,{identity,provider,categories,provenance});}
export function row(stage,frame,words,{pieces=null,counts=false}={}) {
  const entities=counts?words.slice(12,14):[words[12]];
  return {stage,replaySampleIndex:frame,appliedInput:words[1],clocks:{stageFrame:words[0]},scalars:{stage},
    categories:{player:words.slice(2,5),economy:[...words.slice(5,10),...(pieces||[])],rng:words.slice(10,12),entities}};
}
export function pair(directory,id,identity) {
  return Object.fromEntries(['original','candidate'].map(side=>[side,capture(resolve(directory,`${id}.${side}.jsonl`),identity,
    side==='original'?'th15/retail-x86-world':'th15/current-wasi-core')]));
}
