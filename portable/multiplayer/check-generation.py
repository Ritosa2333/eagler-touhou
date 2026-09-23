"""Native P1/P2 retry, fresh frame-zero gate and old-wire-packet rejection."""
import argparse
import json
from rollback_testkit import barrier, call, fixture, hashes, net, state, submit, tick

parser = argparse.ArgumentParser()
parser.add_argument('--url', default='http://127.0.0.1:8140/')
parser.add_argument('--output', default='artifacts/multiplayer-tests/generation.json')
args = parser.parse_args()
report = {'scope': 'TH10 native retry with two real roles and common wire packets; no browser transport'}


def generation(page):
    return call(page, 'multiplayerSmoke.generationStatus()')


def compare(pages, frame):
    values = [state(page) for page in pages]
    assert values[0][:2]+values[0][3:] == values[1][:2]+values[1][3:], (frame, values)
    digests = [hashes(page) for page in pages]
    report['last'] = {'frame': frame, 'states': values, 'hashes': digests}
    assert digests[0][1:28] == digests[1][1:28], report['last']
    audio = [call(page, 'multiplayerSmoke.audioStatus()') for page in pages]
    assert audio[0] == audio[1], (frame, audio)


with fixture(args.url, report, args.output) as test:
    pages = [test.open(local=seat, difficulty=4) for seat in range(2)]
    barrier(pages)
    for page in pages:
        test.advance(page, 59)
        assert call(page, 'multiplayerSmoke.fixture(5)')
    old_hello = call(pages[0], 'multiplayerSmoke.sessionPacket(1)')
    old_input = call(pages[0], 'multiplayerSmoke.inputPacket(1,59)')
    original = generation(pages[0])
    for frame in range(60, 65):
        for seat, page in enumerate(pages):
            submit(page, 1-seat, frame);tick(page)
        compare(pages, frame)
    for page in pages:
        assert call(page, 'multiplayerSmoke.lifecycle()')[2] == 13
        for _ in range(200):
            tick(page)
            if generation(page)[1] == 1:
                break
        current = generation(page)
        assert current[1] == 1 and not current[8], current
        assert current[4:6] != original[4:6], (original, current)
        assert net(page)[2:4] == [0,-1], net(page)
        before = {'state': state(page), 'hash': hashes(page), 'net': net(page)}
        tick(page, 20)
        after = {'state': state(page), 'hash': hashes(page), 'net': net(page)}
        assert before == after, ('fresh generation advanced without peer', before, after)
        if page == pages[1]:
            for packet in (old_hello, old_input):
                assert call(page, '(p)=>multiplayerSmoke.applyPacket(p)', packet) == 2
            assert net(page)[4] == -1 and net(page)[2:4] == [0,-1], net(page)
    generations = [generation(page) for page in pages]
    assert generations[0][4:8] == generations[1][4:8], generations
    report['freshGate'] = generations
    barrier(pages)
    for frame in range(30):
        packets = []
        for seat, page in enumerate(pages):
            assert call(page, '(x)=>multiplayerSmoke.captureLocal(...x)', [frame, 0x80 if seat==0 else 0x40])
            packets.append(call(page, '(x)=>multiplayerSmoke.inputPacket(...x)', [1-seat,frame,frame+1,frame]))
        for seat,page in enumerate(pages):
            assert call(page, '(p)=>multiplayerSmoke.applyPacket(p)', packets[1-seat]) == 1
            tick(page)
        compare(pages, frame)
    assert generation(pages[0])[8] == generation(pages[1])[8] == 1
    assert state(pages[0])[7] >= 20, state(pages[0])
print(json.dumps({'passed': report['passed']}))
