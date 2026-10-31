import test from 'node:test';
import assert from 'node:assert/strict';
import {replayStageLoadout} from '../src/multiplayer.mjs';

test('multiplayer replay keeps its recorded machine loadout through each stage transition',()=>{
 const recorded={character:2,shot:1},meta={difficulty:4,recordedPlayer:1,loadouts:[{character:0,shot:0},recorded,{character:1,shot:1}]};
 assert.deepEqual(replayStageLoadout(meta),[recorded,meta.loadouts[0],4]);
 assert.deepEqual(replayStageLoadout(null),[undefined,undefined,undefined]);
});
