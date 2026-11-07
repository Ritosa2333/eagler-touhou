import test from 'node:test';
import {verifyPlayerShots} from './player-shot-oracle.mjs';
test('TH17 player shot lifecycles preserve motion, lasers and animation queues',()=>verifyPlayerShots({simulate:true}));
