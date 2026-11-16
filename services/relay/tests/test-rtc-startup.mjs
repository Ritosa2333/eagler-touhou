import assert from 'node:assert/strict';
import test from 'node:test';
import {createRtcStartup} from '../src/rtc-startup.mjs';

function fixture(primaryMayUseSwarm = true) {
  let time = 0;
  const timers = new Map(), events = [];
  const startup = createRtcStartup({
    timeout:7500, primaryMayUseSwarm, now:() => time,
    wait(handler, ms) {const id = Symbol(); timers.set(id, {at:time + ms, handler}); return id;},
    cancel:id => timers.delete(id),
    openTurn:() => events.push(['turn',time]),
    select:serial => events.push(['select',serial,time]),
    complete:serial => events.push(['release',serial,time]),
    fail:() => events.push(['failed',time]),
  });
  startup.start();
  return {
    startup, events,
    at(next) {
      for (;;) {
        const scheduled = [...timers].filter(([, item]) => item.at <= next).sort((a,b) => a[1].at - b[1].at)[0];
        if (!scheduled) break;
        const [id, {at,handler}] = scheduled;
        timers.delete(id);
        time = at;
        handler();
      }
      time = next;
    },
    reportBoth(serial, p95, {direct=false, received=6}={}) {
      for (const player of [0,1]) startup.report(player, serial, {p95,direct,sent:6,received});
    },
  };
}

test('quick direct releases without TURN allocation after both players acknowledge', () => {
  const f=fixture();
  f.at(400);
  f.reportBoth(0, 20, {direct:true});
  assert.deepEqual(f.events,[['select',0,400]]);
  f.startup.acknowledge(0,0);
  f.startup.acknowledge(0,0);
  f.startup.acknowledge(1,1);
  assert.equal(f.events.length,1);
  f.startup.acknowledge(1,0);
  assert.deepEqual(f.events.at(-1),['release',0,400]);
});

test('dead primary opens first-party TURN and releases before the ICE deadline', () => {
  const f=fixture();
  f.at(200);
  f.startup.report(0,0,null);
  assert.deepEqual(f.events,[['turn',200]]);
  f.at(980);
  f.reportBoth(1,40);
  assert.deepEqual(f.events.at(-1),['select',1,980]);
});

test('first-party TURN wins when either endpoint reports a high Swarm RTT', () => {
  const f=fixture();
  f.at(740);
  f.startup.report(0,0,{p95:45,sent:6,received:6});
  f.startup.report(1,0,{p95:400,sent:6,received:6});
  assert.deepEqual(f.events,[['turn',500]]);
  f.at(960);
  f.reportBoth(1,75);
  assert.deepEqual(f.events.at(-1),['select',1,960]);
});

test('first-party TURN has priority over healthy Swarm when both qualify', () => {
  const f=fixture();
  f.at(750);
  f.reportBoth(0,45);
  f.at(1350);
  assert.deepEqual(f.events,[['turn',500]]);
  f.reportBoth(1,70);
  assert.deepEqual(f.events.at(-1),['select',1,1350]);
});

test('Swarm starts if TURN remains unconfirmed after its probe window', () => {
  const f=fixture();
  f.at(750);
  f.reportBoth(0,45);
  f.at(1999);
  assert.deepEqual(f.events,[['turn',500]]);
  f.at(2000);
  assert.deepEqual(f.events.at(-1),['select',0,2000]);
});

test('healthy Swarm starts immediately when TURN probes show high latency', () => {
  const f=fixture();
  f.at(750);
  f.reportBoth(0,45);
  f.at(960);
  f.reportBoth(1,300);
  assert.deepEqual(f.events.at(-1),['select',0,960]);
});

test('ordinary primary TURN does not wait through the Swarm fallback window', () => {
  const f=fixture(false);
  f.at(750);
  f.reportBoth(0,45);
  f.at(1100);
  assert.deepEqual(f.events.at(-1),['select',0,1100]);
});

test('a degraded Swarm keeps waiting for first-party TURN after three seconds', () => {
  const f=fixture();
  f.at(1250);
  f.reportBoth(0,300);
  f.at(3750);
  assert.deepEqual(f.events,[['turn',500]]);
  f.reportBoth(1,62);
  assert.deepEqual(f.events.at(-1),['select',1,3750]);
});

test('use the only proven route at the deadline and require both acknowledgements', () => {
  const f=fixture();
  f.at(1250);
  f.reportBoth(0,300);
  f.at(7500);
  assert.deepEqual(f.events.at(-1),['select',0,7500]);
  f.startup.acknowledge(0,0);
  f.at(9500);
  assert.deepEqual(f.events.at(-1),['failed',9500]);
});

test('all dead paths time out and zero-return paths are never selected', () => {
  const f=fixture();
  f.at(1300);
  f.reportBoth(0,null,{received:0});
  f.at(7499);
  assert.deepEqual(f.events,[['turn',500]]);
  f.at(7500);
  assert.deepEqual(f.events.at(-1),['failed',7500]);
});
