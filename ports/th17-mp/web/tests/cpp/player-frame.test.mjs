import test from 'node:test';import {verifyPlayerFrames} from './player-frame-oracle.mjs';
test('TH17 named player preserves the native whole-player frame',()=>verifyPlayerFrames());
