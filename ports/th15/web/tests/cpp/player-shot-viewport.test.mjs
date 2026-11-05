import test from 'node:test';import {verifyPlayerShots} from './player-shot-oracle.mjs';
test('TH15 actual centered 384x448 playfield preserves native shot clipping for all four characters',async()=>{await verifyPlayerShots(true,false,false,false,true);});
