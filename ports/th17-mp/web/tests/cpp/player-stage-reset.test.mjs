import test from 'node:test';import {verifyPlayerFrames} from './player-frame-oracle.mjs';
test('TH17 stage transitions preserve player/options/charge/reward resets',()=>verifyPlayerFrames({stageReset:true}));
