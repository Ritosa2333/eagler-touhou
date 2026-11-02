import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';
import {assertCliArguments} from '../testing/cli-options.mjs';

const root = resolve(import.meta.dirname, '../..');
const readConfig = async name => JSON.parse(await readFile(resolve(root, 'launcher/config', name), 'utf8'));

export function buildRuntimeTestTargets(products, builds, workspace) {
  assert.deepEqual(Object.keys(builds.games).sort(), Object.keys(products).sort(), 'Build games must match the product catalog');
  return Object.entries(products).flatMap(([game, product]) => {
    const build = builds.games[game];
    assert.equal(!!build.variants.multiplayer, !!product.multiplayerRuntime, `${game}: multiplayer build mismatch`);
    return ['normal', ...(product.multiplayerRuntime ? ['multiplayer'] : [])].map(variant => {
      const owner = build.variants[variant]?.workspaceRepository ?? build.workspaceRepository;
      assert.ok(build.variants[variant] && workspace.repositories[owner], `${game}/${variant}: missing source owner`);
      return {game, variant, directory: workspace.repositories[owner], inputProfile: product.inputProfile ?? 'standard',
        gameplay: variant === 'multiplayer' ? product.multiplayer?.gameplay : null};
    });
  });
}

export async function runtimeTestTargets({game, variant = 'all', gameplay, inputProfile} = {}) {
  assert.ok(['normal', 'multiplayer', 'all'].includes(variant), `Unknown variant: ${variant}`);
  let selected;
  if (game !== undefined) {
    selected = game.split(',');
    assert.ok(selected.every(id => Object.hasOwn(PRODUCT_GAMES, id)), `Unknown game selection: ${game}`);
    assert.equal(new Set(selected).size, selected.length, 'Duplicate game selection');
  }
  const targets = buildRuntimeTestTargets(PRODUCT_GAMES, await readConfig('runtime-builds.json'), await readConfig('workspace.json'))
    .filter(target => (variant === 'all' || target.variant === variant) && (!selected || selected.includes(target.game)) &&
      (!gameplay || target.gameplay === gameplay) && (!inputProfile || target.inputProfile === inputProfile));
  for (const id of selected ?? []) assert.ok(targets.some(target => target.game === id), `${id}: no applicable Runtime for this test`);
  assert.ok(targets.length, 'No selected Runtime');
  return targets;
}

export const runtimeTargetKey = target => `${target.game}/${target.variant}`;

export function selectRuntimeInputs(config, required) {
  assert.ok(Array.isArray(required) && required.length, 'Required Runtime selection must be nonempty');
  assert.ok(Array.isArray(config.targets), 'Private manifest must contain targets');
  const inputs = new Map(), ids = new Set();
  for (const target of config.targets) {
    assert.ok(Object.hasOwn(PRODUCT_GAMES, target.game) && ['normal', 'multiplayer'].includes(target.variant), 'Unknown manifest Runtime');
    assert.ok(target.variant !== 'multiplayer' || PRODUCT_GAMES[target.game].multiplayerRuntime, `${target.game}: no multiplayer product`);
    const key = runtimeTargetKey(target);
    assert.ok(!inputs.has(key), `Duplicate manifest Runtime: ${key}`);
    assert.ok(typeof target.id === 'string' && /^[a-zA-Z0-9_-]+$/.test(target.id) && !ids.has(target.id), 'Missing, invalid or duplicate Runtime id');
    for (const name of ['runtime', 'entry', 'data']) assert.ok(typeof target[name] === 'string' && target[name].length, `${key}: missing ${name}`);
    inputs.set(key, target); ids.add(target.id);
  }
  const missing = required.map(runtimeTargetKey).filter(key => !inputs.has(key));
  assert.deepEqual(missing, [], `Missing required Runtime inputs: ${missing.join(', ')}`);
  return required.map(target => inputs.get(runtimeTargetKey(target)));
}

export function resultCoverage(requiredIds, results) {
  assert.ok(requiredIds.length && new Set(requiredIds).size === requiredIds.length, 'Expected cases must be nonempty and unique');
  const ids = results.map(row => row.id), duplicates = ids.filter((id, i) => ids.indexOf(id) !== i);
  const missing = requiredIds.filter(id => !ids.includes(id)), unexpected = ids.filter(id => !requiredIds.includes(id));
  return {required: requiredIds.length, executed: results.length, passed: results.filter(row => row.passed === true).length,
    missing, unexpected, duplicates, complete: !missing.length && !unexpected.length && !duplicates.length &&
      results.every(row => row.passed === true)};
}

if (process.argv[1] && import.meta.url === pathToFileURL(resolve(process.argv[1])).href) {
  assertCliArguments(process.argv.slice(2), {values: ['game', 'variant', 'gameplay', 'input-profile']});
  const arg = name => process.argv.find(value => value.startsWith(`--${name}=`))?.slice(name.length + 3);
  console.log(JSON.stringify(await runtimeTestTargets({game: arg('game'), variant: arg('variant'), gameplay: arg('gameplay'), inputProfile: arg('input-profile')})));
}
