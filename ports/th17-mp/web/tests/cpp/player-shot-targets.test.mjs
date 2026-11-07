import test from 'node:test';
import {verifyPlayerShots} from './player-shot-oracle.mjs';
test('TH17 homing and laser behavior preserve native enemy selection and target generation',()=>verifyPlayerShots({simulate:true,withTargets:true}));
