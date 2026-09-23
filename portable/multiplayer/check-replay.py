"""Native all-seat save -> fresh native Replay menu -> authoritative playback.

The diagnostic fixture invokes the real native writer only. It does not modify
gameplay initial conditions, fake a Replay decoder, or inject playback input.
This initial gate compares every exposed player/economy value and both RNGs;
it explicitly reports that the legacy pointer-heavy hashes are not portable.
"""
import argparse
import hashlib
import json
from pathlib import Path
from rollback_testkit import barrier, call, fixture, net, submit, tick

parser = argparse.ArgumentParser()
parser.add_argument('--url', default='http://127.0.0.1:8140/')
parser.add_argument('--output', default='artifacts/multiplayer-tests/replay-native.json')
parser.add_argument('--cases', choices=('roundtrip', 'lifecycle', 'all'), default='all')
args = parser.parse_args()
report = {'scope': 'TH10 native multiseat file/menu roundtrip; local algorithm harness, not transport',
          'coverage': ['every exposed player/economy word', 'script RNG seed/calls', 'visual RNG seed/calls'],
          'notClaimed': ['portable complete-world hash', 'later-stage seeking', 'Launcher persistence'],
          'cases': []}


def inputs(frame, players):
    values = []
    for seat in range(players):
        direction = (0x40 if (frame // 40 + seat) % 2 else 0x80) if frame >= 80 else 0
        values.append(direction | (1 if frame >= 160 else 0) | (4 if frame % 120 < 70 else 0))
    if frame == 350:
        values[-1] |= 2
    return values


def record(test, players=2, lag=0, last=599):
    loadouts = [[0, 0], [1, 1], [0, 2]][:players]
    pages = [test.open(local=seat, loadouts=loadouts, difficulty=4, history=1 + seat % 2)
             for seat in range(players)]
    barrier(pages)
    page = pages[-1]
    trace = []
    last_seen = -1
    sent = [-1] * players
    for _ in range(last + 1400):
        n = net(page)
        if n[3] >= last:
            break
        frame = n[2]
        values = inputs(frame, players)
        assert call(page, '(x)=>multiplayerSmoke.captureLocal(...x)', [frame, values[-1]])
        for seat in range(players - 1):
            through = frame - lag
            for remote in range(sent[seat] + 1, through + 1):
                submit(page, seat, remote, inputs(remote, players)[seat])
            sent[seat] = max(sent[seat], through)
        tick(page)
        now = net(page)
        if lag == 0 and now[3] != last_seen and now[3] >= 0:
            trace.append(call(page, 'multiplayerSmoke.replayObservation()'))
            last_seen = now[3]
    assert net(page)[3] == last, net(page)
    # One final fully confirmed frame performs all outstanding reconciliation
    # through the real loop; no uncomputed or predicted tail is called saved.
    frame = last + 1
    values = inputs(frame, players)
    assert call(page, '(x)=>multiplayerSmoke.captureLocal(...x)', [frame, values[-1]])
    for seat in range(players - 1):
        for remote in range(sent[seat] + 1, frame + 1):
            submit(page, seat, remote, inputs(remote, players)[seat])
    tick(page)
    assert net(page)[3] == frame and net(page)[4] >= frame and net(page)[5] == -1
    final = call(page, 'multiplayerSmoke.replayObservation()')
    assert final['state'][7] >= 300, ('recording never reached active gameplay', final)
    if lag == 0:
        trace.append(final)
    assert call(page, 'multiplayerSmoke.saveReplay()')
    data = call(page, 'multiplayerSmoke.replayBytes()')
    assert bytes(data[:8]) == b'EAGLRPY1'
    status = call(page, 'multiplayerSmoke.replayStatus()')
    assert status[3] == frame + 1, status
    for peer in pages:
        peer.close()
    return data, trace, final


def viewer(test, data, expected):
    context = test.browser.new_context(service_workers='block')
    test.contexts.append(context)
    page = context.new_page()
    page.on('pageerror', lambda error: test.errors.append(str(error)))
    page.goto(args.url)
    page.wait_for_function('window.multiplayerSmoke !== undefined', timeout=120000)
    test.identities.append(call(page, 'multiplayerSmoke.identity()'))
    assert call(page, 'multiplayerSmoke.historyProfile(2)')
    call(page, '(b)=>multiplayerSmoke.viewerStart(b)', data)
    call(page, 'multiplayerSmoke.viewerTicks(400)')

    def press(code, wait=55):
        call(page, '(c)=>multiplayerSmoke.key(c,true)', code)
        call(page, 'multiplayerSmoke.viewerTicks(2)')
        call(page, '(c)=>multiplayerSmoke.key(c,false)', code)
        call(page, '(n)=>multiplayerSmoke.viewerTicks(n)', wait)

    press('KeyZ')  # prompt -> native main menu, MP viewer selects Replay
    press('KeyZ')  # native Replay list
    before_files = call(page, 'multiplayerSmoke.savedFiles()')
    press('KeyZ')  # slot -> native stage list
    press('KeyZ')  # native selection invokes native title transition
    # Real physical device events must not become additional player input in
    # an offline all-seat Replay. Escape remains the viewer's explicit exit.
    for code in ('ArrowLeft', 'ShiftLeft', 'KeyX', 'KeyZ'):
        call(page, '(c)=>multiplayerSmoke.key(c,true)', code)
    for _ in range(20):
        status = call(page, 'multiplayerSmoke.replayStatus()')
        if status[6]:
            break
        call(page, 'multiplayerSmoke.viewerTicks(100)')
    status = call(page, 'multiplayerSmoke.replayStatus()')
    assert status[2] == status[6] == status[8] == status[9] == 1, status
    trace = call(page, 'multiplayerSmoke.takeReplayTrace()')
    assert len(trace) == len(expected), (len(trace), len(expected), status, trace[:1], trace[-1:])
    for actual, original in zip(trace, expected):
        assert actual['frame'] == original['frame']
        for key in ('state', 'rng'):
            assert actual[key] == original[key], {'firstMismatch': actual['frame'], 'category': key,
                'expected': original[key], 'actual': actual[key]}
    assert call(page, 'multiplayerSmoke.savedFiles()') == before_files, 'Playback changed saved files'
    for code in ('ArrowLeft', 'ShiftLeft', 'KeyX', 'KeyZ'):
        call(page, '(c)=>multiplayerSmoke.key(c,false)', code)
    press('Escape', 100)
    assert call(page, 'multiplayerSmoke.replayStatus()')[8] == 0
    assert call(page, 'multiplayerSmoke.savedFiles()') == before_files, 'Viewer exit changed saved files'
    page.close()
    return {'frames': len(trace), 'readOnly': True, 'nativeMenu': True, 'differentLocalHistory': True}


def record_retry(test, players):
    """Pause and use the native restart shortcut; no state-writing fixture.

    Only the receiver's inputs are submitted directly. New-generation helpers
    complete the genuine session gate, exactly as in the existing local-core
    tests. The file must reproduce the native teardown/rebuild from its inputs.
    """
    loadouts = [[0, 0], [1, 1], [0, 2]][:players]
    pages = [test.open(local=seat, loadouts=loadouts, difficulty=4, history=1 + seat % 2)
             for seat in range(players)]
    barrier(pages)
    page = pages[-1]
    trace, transitions, active_generation = [], [], 0
    for _ in range(2400):
        generation = call(page, 'multiplayerSmoke.generationStatus()')
        n = net(page)
        if generation[1] == 1 and n[3] >= 199:
            break
        if generation[1] != active_generation:
            assert generation[1] == 1 and n[2:4] == [0, -1], (generation, n)
            active_generation = generation[1]
            helpers = [test.open(local=seat, loadouts=loadouts, difficulty=4,
                                seed=generation[6], session_id=generation[4:6])
                       for seat in range(players - 1)]
            barrier(helpers + [page])
            transitions.append({'generation': generation, 'cursor': len(trace)})
        before = call(page, 'multiplayerSmoke.replayStatus()')[4]
        if call(page, 'multiplayerSmoke.canStart()'):
            frame = net(page)[2]
            values = inputs(frame, players)
            if active_generation == 0:
                if frame == 300:
                    values[0] = 8
                elif frame == 315:
                    values[0] = 0x4000
            assert call(page, '(x)=>multiplayerSmoke.captureLocal(...x)', [frame, values[-1]])
            for seat in range(players - 1):
                submit(page, seat, frame, values[seat])
        tick(page)
        after = call(page, 'multiplayerSmoke.replayStatus()')[4]
        if after != before:
            assert after == before + 1
            observation = call(page, 'multiplayerSmoke.replayObservation()')
            trace.append(observation)
    assert active_generation == 1 and net(page)[3] == 199, (
        'native restart not completed', net(page), call(page, 'multiplayerSmoke.lifecycle()'))
    assert transitions and len(trace) > 500, transitions
    assert call(page, 'multiplayerSmoke.saveReplay()')
    data = call(page, 'multiplayerSmoke.replayBytes()')
    return data, trace, transitions


with fixture(args.url, report, args.output) as test:
    for players in ((2, 3) if args.cases in ('roundtrip', 'all') else ()):
        data, trace, final = record(test, players=players)
        for lag in (0, 4):
            if lag:
                data, _, corrected = record(test, players=players, lag=lag)
                for key in ('state', 'rng'):
                    assert corrected[key] == final[key], ('corrected recording differs', players, key)
            case = {'name': f'{players}p-native-file-menu-roundtrip-lag{lag}', 'fileBytes': len(data),
                    'fileSha256': hashlib.sha256(bytes(data)).hexdigest(), 'recordedFrames': len(trace)}
            path = Path(args.output).parent / (case['name'] + '.rpyx')
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(bytes(data))
            case['replayFile'] = str(path)
            report['cases'].append(case)
            case.update(viewer(test, data, trace))
            case['passed'] = True
            print(json.dumps(case), flush=True)
        test.close_contexts()
    for players in ((2, 3) if args.cases in ('lifecycle', 'all') else ()):
        data, trace, transitions = record_retry(test, players)
        case = {'name': f'{players}p-native-restart-file-menu-roundtrip',
                'fileBytes': len(data), 'fileSha256': hashlib.sha256(bytes(data)).hexdigest(),
                'recordedFrames': len(trace), 'transitions': transitions}
        path = Path(args.output).parent / (case['name'] + '.rpyx')
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_bytes(bytes(data))
        case['replayFile'] = str(path)
        report['cases'].append(case)
        case.update(viewer(test, data, trace))
        case['passed'] = True
        print(json.dumps(case), flush=True)
        test.close_contexts()
print(json.dumps({'passed': report['passed'], 'report': args.output}), flush=True)
