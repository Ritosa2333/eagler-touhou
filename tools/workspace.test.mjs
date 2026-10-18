import assert from 'node:assert/strict';
import { existsSync } from 'node:fs';
import { resolve } from 'node:path';
import test from 'node:test';
import { selectedPaths } from './workspace.mjs';

const portPaths = paths => paths.filter(path => path.startsWith('ports/'));

test('TH06 and TH07 use one source directory for normal and multiplayer builds', () => {
  for (const game of ['th06', 'th07']) {
    const expected = [`ports/${game}`];
    assert.deepEqual(portPaths(selectedPaths(game, 'normal')), expected);
    assert.deepEqual(portPaths(selectedPaths(game, 'multiplayer')), expected);
    assert.deepEqual(portPaths(selectedPaths(game, 'both')), expected);
    assert.equal(existsSync(resolve(import.meta.dirname, '..', expected[0])), true);
  }
});

test('TH09 maps both product modes to its shared source directory', () => {
  const expected = ['ports/th09'];
  assert.deepEqual(portPaths(selectedPaths('th09', 'normal')), expected);
  assert.deepEqual(portPaths(selectedPaths('th09', 'multiplayer')), expected);
  assert.deepEqual(portPaths(selectedPaths('th09', 'both')), expected);
  assert.equal(existsSync(resolve(import.meta.dirname, '..', expected[0])), true);
  assert.equal(expected.includes('ports/th09-mp'), false);
});

test('TH10 multiplayer selects its separate source directory', () => {
  assert.deepEqual(portPaths(selectedPaths('th10', 'normal')), ['ports/th10']);
  const multiplayerSources = portPaths(selectedPaths('th10', 'multiplayer'));
  assert.deepEqual(multiplayerSources, ['ports/th10-mp']);
  assert.equal(existsSync(resolve(import.meta.dirname, '..', multiplayerSources[0])), true);
});

test('unknown build variants are rejected', () => {
  assert.throws(() => selectedPaths('th10', 'preview'), /没有 preview 构建/);
});
