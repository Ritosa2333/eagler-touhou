import {dirname,join,resolve} from 'node:path';
import {fileURLToPath,pathToFileURL} from 'node:url';

const here=dirname(fileURLToPath(import.meta.url));
const repo=resolve(here,'../..');
const commonRoot=resolve(process.env.EAGLER_COMMON_ROOT||join(repo,'..','eagler-common'));
const {compareTraces}=await import(pathToFileURL(join(commonRoot,'testkit/replay-verifier/compare.mjs')));
const {digestCanonicalJson,recordsFromSegments}=await import(pathToFileURL(join(commonRoot,'testkit/replay-verifier/adapter.mjs')));

export const TH09_EXECUTABLE_SHA256='10350095bcf95edb59e03bee9849a2dc8a7714b4927ad5909c569c550fce6822';
export const TH09_RESOURCE_SHA256='73a344a2cb7b1113831058010758f03d6d20e7710d889a95f249046fb8fa5ea0';

function ticks(rows){
 return rows.map(row=>({
  replaySampleIndex:row.frame,
  clockDisposition:'advanced',
  appliedInput:row.players.map(player=>({held:player.held,pressed:player.pressed})),
  clocks:{replayFrame:row.frame},
  scalars:{flags:row.flags},
  categories:{
   rng:digestCanonicalJson(row.rng),
   player0:digestCanonicalJson(row.players[0]),
   player1:digestCanonicalJson(row.players[1]),
  },
 }));
}

export function recordsFromRows(fixture,rows,provider){
 return recordsFromSegments({
  comparisonIdentity:{
   game:'th09',profile:'jp-1.50a/demo-lockstep-v1',replaySha256:fixture.replaySha256,
   executableSha256:TH09_EXECUTABLE_SHA256,resourceSha256:TH09_RESOURCE_SHA256,
   stateSchema:'th09/demo-lockstep-state/v1',traceCodec:'in-memory/v1',
   digestAlgorithm:'sha256-truncated-128/canonical-json-v1',
  },
  coverage:{requiredCategories:['rng','player0','player1'],optionalCategories:[]},
  provenance:{provider},runReason:'demo-complete',
  segments:[{segmentId:`${fixture.id}#0`,route:fixture.id,ticks:ticks(rows),reason:'demo-complete'}],
 });
}

export function compareDemoRows(fixture,expected,actual){
 return compareTraces(
  recordsFromRows(fixture,expected,'th09-1.50a/native-lockstep'),
  recordsFromRows(fixture,actual,'th09-eagler/wasi-session'),
 );
}
