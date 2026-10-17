"""Late remote Pause press/release, menu control, and subsequent gameplay."""
import argparse
import json

from rollback_testkit import call, equal, fixture, hashes, net, state, submit, tick

parser = argparse.ArgumentParser()
parser.add_argument('--url', default='http://127.0.0.1:8138/')
parser.add_argument('--output', default='artifacts/multiplayer-tests/rollback-pause.json')
args = parser.parse_args()
report = {'scope': 'TH10 remote Pause edge rollback and host Resume'}

with fixture(args.url, report, args.output) as test:
    reference, late = test.pair()
    for page in (reference, late):
        test.advance(page, 59)
    equal(reference, late, 'warmup')
    start_stage_frame = state(reference)[7]
    for frame in range(60, 65):
        submit(reference, 1, frame, 8 if frame == 60 else 0)
        tick(reference)
        tick(late)
    for frame in range(60, 65):
        submit(late, 1, frame, 8 if frame == 60 else 0)
    assert net(late)[5] == 60, net(late)
    for page in (reference, late):
        submit(page, 1, 65)
        tick(page)
    equal(reference, late, 'corrected pause')
    lifecycle = call(late, 'multiplayerSmoke.lifecycle()')
    assert lifecycle[8] > 0 and lifecycle[9] >= 5, lifecycle
    assert state(late)[7] <= start_stage_frame + 1, (start_stage_frame, state(late))
    report['correctedPause'] = {'state': state(late), 'lifecycle': lifecycle, 'hashes': hashes(late)}
    # Let the real menu animation reach an interactive state. Only P1 supplies
    # confirm; P2's pause was merged into the native raw edge lane.
    for page in (reference, late):
        test.advance(page, 105)
    for page in (reference, late):
        frame = net(page)[2]
        assert call(page, '(f)=>multiplayerSmoke.captureLocal(f,1)', frame)
        submit(page, 1, frame)
        tick(page)
    equal(reference, late, 'host resume pressed')
    for frame in range(107, 150):
        for page in (reference, late):
            submit(page, 1, frame)
            tick(page)
        equal(reference, late, ('resume continuation', frame))
    assert state(late)[7] > start_stage_frame + 10, state(late)
    report['resumed'] = {'state': state(late), 'lifecycle': call(late, 'multiplayerSmoke.lifecycle()')}
print(json.dumps({'passed': report['passed']}))
