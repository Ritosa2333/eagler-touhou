import test from 'node:test';
import {verifyPlayerShots} from './player-shot-oracle.mjs';
test('TH17 option angle and random scatter preserve both native random streams',()=>verifyPlayerShots({dynamicAngles:true}));
