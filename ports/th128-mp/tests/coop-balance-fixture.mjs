// Usage for native balance integrations: node <test> <world-input.json> <coop-core.mjs> <coop-core.wasm>
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {readFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
import {prepareA13Preview} from '../runtime/preview-world.mjs';

const sha = bytes => createHash('sha256').update(bytes).digest('hex');
export async function balanceFixture([pin, core, wasmPath] = process.argv.slice(2)) {
  const manifest = await readFile(pin);
  assert.equal(sha(manifest), '72c4743eca36478b724d300f4f1a07419717d9a19633158b027789e4cf85588b', 'case1529 world-input.json pin');
  const config = JSON.parse(manifest), read = async path => new Uint8Array(await readFile(path));
  const dat = await read(config.dat), wasm = await read(wasmPath);
  assert.equal(sha(dat), config.datSHA256);
  const paths = {...config, bullet9:config.type9, bullet32:config.type32,
    ...Object.fromEntries(Object.entries(config.bullets).map(([type, path]) => ['bullet'+type, path]))};
  const build = {coreSHA256:sha(wasm), resourceSHA256:config.datSHA256};
  const factory = (await import(pathToFileURL(resolve(core)))).default;
  const options = {resources:{build, read:k => k==='dat' ? dat : k==='wasm' ? wasm : read(paths[k])},
    factory, onFactory(){}, onWorldStart(){}, onTick(){}};
  const prepared = await prepareA13Preview(options);
  return {build, options, open:(players, overrides={}) => prepared.openCoopStage({players, stageId:3, difficulty:1,
    mode:'same-stage-coop', seed:34772, rollbackWindow:2, challengeMode:false, ...build, ...overrides})};
}
export const row = (frame, keys) => ({frame, inputs:keys.map((held, player) => ({player, held}))});
