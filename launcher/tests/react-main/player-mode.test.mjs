import test from 'node:test';
import assert from 'node:assert/strict';
import {classifyPlayerMode, playerUsesGameInput} from '../../app/models/player-mode.ts';

test('player purpose separates both playable modes from spectators and Replay, including multiplayer recordings', () => {
  for (const [input, mode, usesInput] of [
    [{}, 'singleplayer', true],
    [{multiplayer: true}, 'multiplayer', true],
    [{multiplayer: true, spectator: true}, 'spectator', false],
    [{replay: true}, 'replay', false],
    [{multiplayer: true, replay: true}, 'replay', false],
    [{multiplayer: true, spectator: true, replay: true}, 'replay', false],
  ]) {
    assert.equal(classifyPlayerMode(input), mode);
    assert.equal(playerUsesGameInput(mode), usesInput);
  }
});
