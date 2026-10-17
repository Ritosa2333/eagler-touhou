import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {pathToFileURL,fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
const common = resolve(process.env.EAGLER_COMMON_ROOT || fileURLToPath(new URL('../../../eagler-common', import.meta.url)));
const {TraceCapture} = await import(pathToFileURL(resolve(common, 'testkit/replay-verifier/capture-file.mjs')));
export const economyAddresses = [0x4a56e4,0x4a56e8,0x4a56f0,0x4a56f4,0x4a5718,0x4a571c,0x4a5720,0x4a5744,0x4a5748,0x4a574c,0x4a5754,0x4a576c];
export const motionFields = [[0,0x87c,12],[12,0x888,8],[20,0x890,16],[36,0x8a0,12],[48,0x8ac,12],[60,0x8b8,8],[68,0x914,8],[76,0x91c,12],[88,0x8d20,4],[92,0x95c,4],[96,0x8ba0,16],[112,0x8bc8,4],[116,0x8bc4,4],[120,0x7c90,4],[124,0x8c14,4]];
const sha = data => createHash('sha256').update(data).digest('hex');
const categories = ['economy','player','rng','entities'];
export function captures(directory, id, identity, provenance) {
  return Object.fromEntries(['original','candidate'].map(provider => [provider,
    new TraceCapture(resolve(directory, id + '.' + provider + '.jsonl'), {identity,
      categories, provider: provider === 'original' ? 'th11/retail-x86-world' : 'th11/current-wasi-core', provenance})]));
}
export function identity(root, raw) {
  const target = JSON.parse(readFileSync(resolve(root, 'target.json')));
  return {game:'th11',profile:'jp-1.00a/world-replay-v1',replaySha256:sha(raw),
    executableSha256:target.sha256,resourceSha256:sha(readFileSync(resolve(root,target.executable.replace(/\.exe$/,'.dat')))),
    stateSchema:'th11/world-selected-state/v1',traceCodec:'jsonl/v1',digestAlgorithm:'sha256-truncated-128/canonical-json-v1'};
}
export function nativeRow(m, frame) {
  const player=m.u32(0x4a8eb4), economy=economyAddresses.map(address=>m.u32(address));
  return {stage:m.u32(0x4a5728),replaySampleIndex:frame,
    appliedInput:[m.u32(0x4c93c0),m.u32(0x4c93cc)],clocks:{worldFrame:frame},scalars:{stage:m.u32(0x4a5728)},
    categories:{economy,player:motionFields.map(([,offset,size])=>Buffer.from(m.bytes(player+offset,size)).toString('hex')),
      rng:Buffer.from(m.bytes(0x4c2f00,8)).toString('hex'),
      entities:[m.u32(m.u32(0x4a8d7c)+0x70),m.u32(m.u32(0x4a8d68)+0x5c)]}};
}
export function candidateRow(c, session, frame) {
  const view=new DataView(c.memory.buffer), motion=c.battle_data(session,2), input=c.battle_data(session,3);
  return {stage:c.game_session_value(session,2),replaySampleIndex:frame,
    appliedInput:[view.getUint32(input,true),view.getUint32(input+4,true)],clocks:{worldFrame:frame},scalars:{stage:c.game_session_value(session,2)},
    categories:{economy:Array.from(new Uint32Array(c.memory.buffer,c.battle_data(session,0),12)),
      player:motionFields.map(([offset,,size])=>Buffer.from(new Uint8Array(c.memory.buffer,motion+offset,size)).toString('hex')),
      rng:Buffer.from(new Uint8Array(c.memory.buffer,c.anm_env_rng(c.game_session_animations(session),0),8)).toString('hex'),
      entities:[c.game_session_value(session,7),c.game_session_value(session,8)]}};
}
