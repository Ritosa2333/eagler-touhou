import test from 'node:test';
import assert from 'node:assert/strict';
import {setRoomRules} from '../web/native-room-rules.mjs';
test('room difficulty and seed reach native initialization',()=>{
 const calls=[],engine={_th03_classic_room_rank(v){calls.push(['rank',v]);return 1;},_th03_classic_room_seed(v){calls.push(['seed',v]);}};
 for(let difficulty=0;difficulty<4;difficulty++){setRoomRules(engine,difficulty,1234);assert.deepEqual(calls.splice(0),[['rank',difficulty],['seed',1234]]);}
 setRoomRules(engine);assert.deepEqual(calls.splice(0),[['rank',1],['seed',0]]);
 assert.throws(()=>setRoomRules(engine,4,1),/难度/);assert.throws(()=>setRoomRules(engine,1,-1),/种子/);
});
