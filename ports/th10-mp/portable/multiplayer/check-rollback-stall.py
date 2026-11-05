"""No logical progress means no extra authored Draw/ANM/RNG mutation."""
import argparse
import json

from rollback_testkit import fixture, hashes, net, state, submit, tick, equal

parser = argparse.ArgumentParser()
parser.add_argument('--url', default='http://127.0.0.1:8138/')
parser.add_argument('--output', default='artifacts/multiplayer-tests/rollback-stall.json')
args = parser.parse_args()
report = {'scope': 'TH10 prediction-ceiling stall and recovery; full owner comparison'}

with fixture(args.url, report, args.output) as test:
    reference, late = test.pair()
    for page in (reference, late):
        test.advance(page, 59)
    equal(reference, late, 'exact warmup')
    # All missing inputs are actually neutral; repeatedly waiting must not
    # mutate authored game/ANM state or resample an already captured frame.
    for _ in range(30):
        previous = net(late)[3]
        tick(late)
        if net(late)[3] == previous:
            break
    else:
        raise AssertionError('prediction ceiling did not stall')
    before = {'net': net(late), 'state': state(late), 'hashes': hashes(late)}
    tick(late, 20)
    after = {'net': net(late), 'state': state(late), 'hashes': hashes(late)}
    report.update(before=before, after=after)
    assert before == after, ('stalled simulation mutated', before, after)
    last = before['net'][3]
    test.advance(reference, last + 1)
    for frame in range(60, last + 2):
        submit(late, 1, frame)
    tick(late)
    equal(reference, late, 'after stall recovery')
print(json.dumps({'passed': report['passed']}))
