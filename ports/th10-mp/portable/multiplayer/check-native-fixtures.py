"""Controlled initial conditions; actual native world and rollback thereafter.

Requires the separately linked --multiplayer-fixtures diagnostic Runtime.
No fixture command is replayed during rollback; setup is at a confirmed fence.
"""
import argparse
import json

from rollback_testkit import call, equal, fixture, hashes, net, state, submit, tick

parser = argparse.ArgumentParser()
parser.add_argument('--url', default='http://127.0.0.1:8140/')
parser.add_argument('--case', choices=['all', 'deathbomb', 'rescue', 'pickup', 'power-transfer', 'no-bomb', 'wipe-cancel', 'laser-close', 'retry', 'stage'], default='all')
parser.add_argument('--output', default='artifacts/multiplayer-tests/native-fixtures.json')
args = parser.parse_args()
report = {'scope': 'TH10 controlled native initial-state fixtures; full owner comparison', 'cases': []}


def prepare(test, kind, local=0, difficulty=4):
    a, b = test.pair(difficulty=difficulty, local=local)
    for page in (a, b):
        test.advance(page, 59)
        probe = call(page, 'multiplayerSmoke.replayProbe()')
        assert probe[16] == -32 and probe[28] == 32, ('multiplayer spawn overlap', probe[16], probe[28])
        assert call(page, '(k)=>multiplayerSmoke.fixture(k)', kind), (kind, state(page))
    equal(a, b, 'fixture initial state')
    return a, b


def advance_pair(a, b, frame, remote, buttons=0):
    for page in (a, b):
        assert net(page)[2] == frame, (frame, net(page))
        submit(page, remote, frame, buttons)
        tick(page)
    equal(a, b, ('exact', frame))


def evidence(page):
    return {'state': state(page), 'net': net(page), 'hashes': hashes(page),
            'audio': call(page, 'multiplayerSmoke.audioStatus()'),
            'fixture': call(page, 'multiplayerSmoke.fixtureStatus()'),
            'lifecycle': call(page, 'multiplayerSmoke.lifecycle()')}


with fixture(args.url, report, args.output) as test:
    cases = ['deathbomb', 'rescue', 'pickup', 'power-transfer', 'no-bomb', 'wipe-cancel', 'laser-close', 'retry', 'stage'] if args.case == 'all' else [args.case]
    for name in cases:
        print('native-fixture:', name, 'begin', flush=True)
        result = {'case': name, 'passed': False}
        report['cases'].append(result)
        if name == 'deathbomb':
            a, b = prepare(test, 1)
            assert state(a)[24] == 4, state(a)
            for frame in range(60, 72):
                submit(a, 1, frame, 2 if frame == 60 else 0)
                tick(a); tick(b)
            result['predicted'] = evidence(b)
            assert state(b)[22] < 0, ('native death did not occur', state(b))
            for frame in range(60, 72):
                submit(b, 1, frame, 2 if frame == 60 else 0)
            assert net(b)[5] == 60, net(b)
            advance_pair(a, b, 72, 1)
            corrected = state(b)
            assert corrected[22] == 0 and corrected[23] == 60 and corrected[24] == 1, corrected
            for frame in range(73, 100):
                advance_pair(a, b, frame, 1)
            result['corrected'] = evidence(b)
        elif name == 'rescue':
            a, b = prepare(test, 2, local=1)
            assert call(a, 'multiplayerSmoke.rulesProbe()')[0] == 1
            for frame in range(60, 149):
                advance_pair(a, b, frame, 0, 4)
            assert call(b, 'multiplayerSmoke.fixtureStatus()')[4] == 89
            for frame in range(149, 154):
                submit(a, 0, frame, 0); tick(a); tick(b)
            predicted = state(b);result['predicted'] = evidence(b)
            assert predicted[10] == 2 and predicted[22] >= 0, predicted
            for frame in range(149, 154):
                submit(b, 0, frame, 0)
            assert net(b)[5] == 149, net(b)
            advance_pair(a, b, 154, 0)
            assert state(b)[10] == 3 and state(b)[22] == -1 and state(b)[24] == 3, state(b)
            assert state(b)[11] == 80 and state(b)[23] == 0, state(b)
            result['cancelledRescue'] = evidence(b)
            for frame in range(155, 244):
                advance_pair(a, b, frame, 0, 4)
            for page in (a, b):
                assert call(page, 'multiplayerSmoke.fixture(7)')
            advance_pair(a, b, 244, 0, 4)
            revived = call(b, 'multiplayerSmoke.rulesProbe()')
            assert revived[2] >= 279 and revived[4] >= 1200, revived
            for frame in range(245, 265):
                advance_pair(a, b, frame, 0, 4)
            assert state(b)[10] == 2 and state(b)[22] >= 0, state(b)
            assert state(b)[11] == 80 and state(b)[23] == 50 and state(b)[22] == 0, state(b)
            probe = call(b, 'multiplayerSmoke.rulesProbe()')
            assert probe[0] == 2 and probe[4] >= 1200, probe
            result['completedRescue'] = evidence(b)
        elif name == 'pickup':
            a, b = prepare(test, 3)
            for frame in range(60, 65):
                submit(a, 1, frame, 0x40);tick(a);tick(b)
            result['predicted'] = evidence(b)
            for frame in range(60, 65):
                submit(b, 1, frame, 0x40)
            advance_pair(a, b, 65, 1)
            assert state(b)[23] > 0 and state(b)[11] == 0, state(b)
            for frame in range(66, 90):
                advance_pair(a, b, frame, 1)
            result['corrected'] = evidence(b)
        elif name == 'power-transfer':
            a, b = prepare(test, 8)
            initial = state(a)
            assert initial[11] == 0 and initial[23] == 40, initial
            frame = 60
            for tap in range(5):
                advance_pair(a, b, frame, 1, 1); frame += 1
                if tap != 4:
                    advance_pair(a, b, frame, 1, 0); frame += 1
            after_gesture = state(a)
            assert after_gesture[23] == 20, ('donor did not spend 20 Power', after_gesture)
            # The committed event spawns a real native kind-4 big-P targeted to
            # P1.  Let Item::update perform homing/pickup and prove that the
            # recipient, not the donor, receives the native +20 reward.
            for _ in range(90):
                if state(a)[11] == 20:
                    break
                advance_pair(a, b, frame, 1, 0); frame += 1
            final = state(a)
            assert final[11] == 20 and final[23] == 20, ('native Power transfer incomplete', final)
            equal(a, b, 'native power transfer')
            result['afterGesture'] = after_gesture
            result['completed'] = evidence(b)
        elif name == 'no-bomb':
            # Normal Bomb: 0.95 Power cannot pay TH10's native 1.00 cost.
            a, b = prepare(test, 9)
            initial = state(a)
            assert initial[23] == 19 and initial[24] == 1, initial
            advance_pair(a, b, 60, 1, 2)
            for frame in range(61, 66):
                advance_pair(a, b, frame, 1, 0)
            alive = state(a)
            assert alive[23] == 19 and alive[24] == 1 and alive[22] == 0, (
                'sub-1.00 normal Bomb changed native state', alive)

            # Deathbomb: the same insufficient Power cannot cancel the hit.
            a, b = prepare(test, 10)
            assert state(a)[23] == 19 and state(a)[24] == 4, state(a)
            frame = 60
            advance_pair(a, b, frame, 1, 2); frame += 1
            while frame < 108:
                advance_pair(a, b, frame, 1, 0); frame += 1
            dead = state(a)
            assert dead[22] == -1 and dead[23] == 0 and dead[24] == 3, (
                'sub-1.00 deathbomb incorrectly rescued pilot', dead)
            assert dead[10] == 2, ('final death minted a survivor life', dead)
            result['normalBombRejected'] = alive
            result['deathbombRejected'] = evidence(b)
        elif name == 'wipe-cancel':
            a, b = prepare(test, 5)
            for frame in range(60, 65):
                submit(a, 1, frame, 8 if frame == 60 else 0);tick(a);tick(b)
            predicted = call(b, 'multiplayerSmoke.lifecycle()')
            # MP now uses native Game Over results, not the retired Continue /
            # Retry screen 13. Prove the speculative results flow really ran
            # before the late input cancels it; do not weaken the undo oracle.
            assert predicted[1] == predicted[2] == 7 and predicted[3] & 0x10, predicted
            assert call(b, 'multiplayerSmoke.fixtureStatus()')[2], 'wipe did not become pending'
            result['predictedWipe'] = evidence(b)
            for frame in range(60, 65):
                submit(b, 1, frame, 8 if frame == 60 else 0)
            assert net(b)[5] == 60, net(b)
            advance_pair(a, b, 65, 1)
            lifecycle = call(b, 'multiplayerSmoke.lifecycle()')
            assert lifecycle[1] == lifecycle[2] == 7, lifecycle
            assert not call(b, 'multiplayerSmoke.fixtureStatus()')[2]
            for frame in range(66, 90):
                advance_pair(a, b, frame, 1)
            result['cancelledWipe'] = evidence(b)
        elif name == 'laser-close':
            a, b = prepare(test, 4)
            for page in (a, b):
                assert call(page, 'multiplayerSmoke.fixtureStatus()')[11] > 0
                # Exercise actual Application/World/native LaserManager teardown,
                # rather than letting browser-context disposal hide bad frees.
                assert call(page, 'multiplayerSmoke.close()')
        elif name == 'retry':
            a, b = prepare(test, 5)
            for frame in range(60, 65):
                advance_pair(a, b, frame, 1)
            for page in (a, b):
                lifecycle = call(page, 'multiplayerSmoke.lifecycle()')
                assert lifecycle[2] != 13, ('multiplayer wipe entered Continue/Retry screen', lifecycle)
                assert lifecycle[4] == 6, ('multiplayer wipe did not open native Game Over results', lifecycle)
                assert call(page, 'multiplayerSmoke.generationStatus()')[1] == 0
            equal(a, b, 'wipe ends without Continue')
            result['afterWipe'] = evidence(b)
        elif name == 'stage':
            a, b = prepare(test, 6, difficulty=1)
            # Real native stage completion/load; the old background then fades
            # under GameSession. Catch its physical lifetime, not just positions.
            for _ in range(150):
                for page in (a, b):
                    submit(page, 1, net(page)[2]);tick(page)
                assert net(a)[3] == net(b)[3], (net(a), net(b))
                old = call(a, 'multiplayerSmoke.fixtureStatus()')
                # Native old-background fade counts DOWN from thirty. Start
                # while three authored Draws remain, before logical deletion.
                if old[15] and 0 < old[16] <= 3:
                    break
            else:
                raise AssertionError(('old-stage fade boundary not reached', evidence(a), evidence(b)))
            equal(a, b, 'before old-stage retirement')
            start = net(a)[2]
            for frame in range(start, start+5):
                submit(a, 1, frame, 0x80);tick(a);tick(b)
            result['speculativeRetirement'] = evidence(b)
            assert not call(b, 'multiplayerSmoke.fixtureStatus()')[15], evidence(b)
            assert call(b, 'multiplayerSmoke.fixtureStatus()')[17] > 0, evidence(b)
            for frame in range(start, start+5):
                submit(b, 1, frame, 0x80)
            assert net(b)[5] == start, net(b)
            advance_pair(a, b, start+5, 1)
            assert call(b, 'multiplayerSmoke.fixtureStatus()')[17] == 0, evidence(b)
            # A newer rollback after reclamation cannot resurrect an old
            # physical retirement record and free the graph twice.
            for frame in range(start+6, start+15):
                advance_pair(a, b, frame, 1)
            for frame in range(start+15, start+20):
                submit(a, 1, frame, 0x40);tick(a);tick(b)
            for frame in range(start+15, start+20):
                submit(b, 1, frame, 0x40)
            advance_pair(a, b, start+20, 1)
            result['afterSecondCorrection'] = evidence(b)
            assert call(a, 'multiplayerSmoke.close()') and call(b, 'multiplayerSmoke.close()')
        result['passed'] = True
        print('native-fixture:', name, 'PASS', flush=True)
        test.close_contexts()

print(json.dumps({'passed': report['passed'], 'cases': len(report['cases'])}))
