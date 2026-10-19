import test from 'node:test';
import assert from 'node:assert/strict';
import {assertPlayerObservationSeats} from './room-spectator.mjs';

test('neutral input from the wrong player cannot establish spectator isolation',()=>{
  const states=[0,1,2].map(localSeat=>({observation:{localSeat,input:{value:0}}}));
  assert.doesNotThrow(()=>assertPlayerObservationSeats(states));
  states[2].observation.localSeat=0;
  assert.throws(()=>assertPlayerObservationSeats(states),/P3.*another seat/);
  delete states[2].observation.localSeat;
  assert.throws(()=>assertPlayerObservationSeats(states),/P3.*another seat/);
  assert.throws(()=>assertPlayerObservationSeats(states.slice(0,1)),/every player/);
});
