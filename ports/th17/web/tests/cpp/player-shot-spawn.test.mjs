import test from 'node:test';
import {verifyPlayerShots} from './player-shot-oracle.mjs';
test('TH17 nine shot variants preserve original firing, spawning and delayed slash sources',()=>verifyPlayerShots());
