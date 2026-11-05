"""Run actual P1/P2 local roles with the same captured input timeline.

This is transport-neutral, not a WebRTC claim. It specifically catches local
HUD/economy selection leaking into shared simulation, which two seat-0 copies
of a rollback fixture cannot expose.
"""
import argparse
import json

from rollback_testkit import barrier, call, fixture, hashes, net, state, submit, tick

parser = argparse.ArgumentParser()
parser.add_argument('--url', default='http://127.0.0.1:8138/')
parser.add_argument('--output', default='artifacts/multiplayer-tests/peer-seats.json')
args = parser.parse_args()
report = {'scope': 'TH10 two real local roles; exact input; full owner comparison'}


def buttons(seat, frame):
    value = (0x80 if seat == 0 else 0x40) if 30 <= frame < 45 else 0
    if 20 <= frame < 90:
        value |= 1
    if seat == 1 and 45 <= frame < 55:
        value |= 4
    if seat == 1 and frame == 60:
        value |= 2
    return value


def compare(pages, frame):
    states = [state(page) for page in pages]
    assert states[0][2] == 0 and states[1][2] == 1, states
    normalized = [values[:2] + values[3:] for values in states]
    values = [hashes(page) for page in pages]
    report['last'] = {'frame': frame, 'states': states, 'hashes': values}
    assert normalized[0] == normalized[1], report['last']
    assert values[0][1:28] == values[1][1:28], report['last']
    audio = [call(page, 'multiplayerSmoke.audioStatus()') for page in pages]
    report['last']['audio'] = audio
    assert audio[0] == audio[1], report['last']


with fixture(args.url, report, args.output) as test:
    pages = [test.open(local=seat, difficulty=4) for seat in range(2)]
    barrier(pages)
    for seat, page in enumerate(pages):
        assert call(page, '(x)=>multiplayerSmoke.captureLocal(...x)', [0, buttons(seat, 0)])
        submit(page, 1 - seat, 0, buttons(1 - seat, 0))
        for _ in range(1000):
            tick(page)
            if net(page)[3] == 0:
                break
        assert net(page)[3] == 0, net(page)
    compare(pages, 0)
    for frame in range(1, 130):
        for seat, page in enumerate(pages):
            assert net(page)[2] == frame, net(page)
            assert call(page, '(x)=>multiplayerSmoke.captureLocal(...x)', [frame, buttons(seat, frame)])
            submit(page, 1 - seat, frame, buttons(1 - seat, frame))
            tick(page)
        compare(pages, frame)
    report['bombPower'] = state(pages[0])[23]
    assert report['bombPower'] <= 60, report['bombPower']
print(json.dumps({'passed': report['passed']}))
