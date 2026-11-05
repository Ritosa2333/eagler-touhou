"""Browser proof for TH10 multiplayer touch/analog ownership.

The live page owns seat 2. It deliberately does not pre-capture its local
FrameInput, so sdl_native_input -> Application::multiplayer_update must sample
the real TouchController path. A read-only fixture probe verifies that local
touch moves only seat 2. The same run then injects synchronized direct-touch
delta input for remote seat 1 and verifies that the remote pilot consumes it.
"""
import argparse
import json
from pathlib import Path

from playwright.sync_api import sync_playwright


parser = argparse.ArgumentParser()
parser.add_argument('--url', default='http://127.0.0.1:8138/')
parser.add_argument('--output', type=Path,
                    default=Path('artifacts/multiplayer-tests/browser-touch.json'))
args = parser.parse_args()

LOADOUTS = [[0, 0], [1, 1]]
SESSION_LOW = 0x71A4C209
SESSION_HIGH = 0x3B8D55E1


def call(page, expression, arg=None):
    return page.evaluate(expression, arg) if arg is not None else page.evaluate(expression)


def open_net(context, local):
    page = context.new_page()
    errors = []
    page.on('pageerror', lambda error: errors.append(str(error)))
    page.goto(args.url)
    page.wait_for_function('window.multiplayerSmoke !== undefined', timeout=120000)
    call(page,
         '(x)=>multiplayerSmoke.startNet(x[0],x[1],x[2],x[3],x[4],x[5])',
         [LOADOUTS, local, 1, 1234, SESSION_LOW, SESSION_HIGH])
    return page, errors


def exchange_barrier(left, right):
    left_hello = call(left, 'multiplayerSmoke.sessionPacket(1)')
    right_hello = call(right, 'multiplayerSmoke.sessionPacket(1)')
    assert call(left, '(p)=>multiplayerSmoke.applySession(p)', right_hello)
    assert call(right, '(p)=>multiplayerSmoke.applySession(p)', left_hello)
    assert call(left, 'multiplayerSmoke.markReady()')
    assert call(right, 'multiplayerSmoke.markReady()')
    left_ready = call(left, 'multiplayerSmoke.sessionPacket(2)')
    right_ready = call(right, 'multiplayerSmoke.sessionPacket(2)')
    assert call(left, '(p)=>multiplayerSmoke.applySession(p)', right_ready)
    assert call(right, '(p)=>multiplayerSmoke.applySession(p)', left_ready)
    assert call(left, 'multiplayerSmoke.canStart()')
    assert call(right, 'multiplayerSmoke.canStart()')


def net(page):
    return call(page, 'multiplayerSmoke.netStatus()')


def probe(page):
    return call(page, 'multiplayerSmoke.replayProbe()')


def pilot(values, seat):
    start = 16 + seat * 12
    return values[start:start + 12]


def submit_remote(page, sample=0):
    frame = net(page)[2]
    if isinstance(sample, dict):
        result = call(page,
                      '(x)=>multiplayerSmoke.submitRemoteInput(0,x[0],x[1])',
                      [frame, sample])
    else:
        result = call(page,
                      '(x)=>multiplayerSmoke.submitRemote(0,x[0],x[1])',
                      [frame, sample])
    assert result in (1, 2, 3, 4), (frame, sample, result, net(page))


def exact_tick(page, remote=0):
    if net(page)[1]:
        submit_remote(page, remote)
    call(page, 'multiplayerSmoke.netTicks(1)')


def stable_players(page):
    for _ in range(1200):
        exact_tick(page)
        status = net(page)
        values = probe(page)
        if status[9] and status[10] and status[3] >= 20:
            p0, p1 = pilot(values, 0), pilot(values, 1)
            if p0[2] == 1 and p1[2] == 1:
                return values
    raise AssertionError(('players never became active', net(page), probe(page)[:52]))


def positions(values):
    return [[pilot(values, seat)[0], pilot(values, seat)[1]] for seat in range(2)]


def authoritative(values):
    return values[1:28]


def submit_at(page, player, frame, sample=0):
    if isinstance(sample, dict):
        result = call(page,
                      '(x)=>multiplayerSmoke.submitRemoteInput(x[0],x[1],x[2])',
                      [player, frame, sample])
    else:
        result = call(page,
                      '(x)=>multiplayerSmoke.submitRemote(x[0],x[1],x[2])',
                      [player, frame, sample])
    assert result in (1, 2, 3, 4), (player, frame, sample, result, net(page))


report = {'passed': False, 'scope': 'TH10 browser multiplayer touch ownership'}
with sync_playwright() as p:
    browser = p.chromium.launch(headless=True, args=['--enable-unsafe-swiftshader'])
    try:
        live_context = browser.new_context(service_workers='block')
        peer_context = browser.new_context(service_workers='block')
        live, live_errors = open_net(live_context, 1)
        peer, peer_errors = open_net(peer_context, 0)
        exchange_barrier(live, peer)
        initial = stable_players(live)
        report['initial'] = positions(initial)

        # Launcher joystick mode must flow through the live local-input capture,
        # not through a diagnostic multiplayer_capture_local call.
        call(live, 'multiplayerSmoke.touchMode(3)')
        call(live, '(x)=>multiplayerSmoke.touchControls(false,false,0,0,x,0)', 32767)
        before_joystick = probe(live)
        for _ in range(8):
            exact_tick(live)
        after_joystick = probe(live)
        call(live, 'multiplayerSmoke.touchControls(false,false,0,0,0,0)')
        j0_before, j1_before = pilot(before_joystick, 0), pilot(before_joystick, 1)
        j0_after, j1_after = pilot(after_joystick, 0), pilot(after_joystick, 1)
        assert j1_after[0] > j1_before[0] + 2.0, (j1_before, j1_after)
        assert abs(j0_after[0] - j0_before[0]) < 0.01, (j0_before, j0_after)
        report['localJoystick'] = {
            'before': positions(before_joystick), 'after': positions(after_joystick)}

        # Direct drag uses an absolute browser target, but the authoritative
        # logical frame is a fresh corrected-position delta on every tick.
        call(live, 'multiplayerSmoke.touchMode(0)')
        call(live, 'multiplayerSmoke.touchOptions(true,false,1)')
        current = pilot(probe(live), 1)
        direction = 1 if current[0] < 100 else -1
        call(live, '(x)=>multiplayerSmoke.touch(0,41,x,0.5)', 0.5)
        exact_tick(live)
        before_direct = probe(live)
        call(live, '(x)=>multiplayerSmoke.touch(1,41,x,0.5)',
             0.58 if direction > 0 else 0.42)
        for _ in range(8):
            exact_tick(live)
        after_direct = probe(live)
        call(live, 'multiplayerSmoke.touch(2,41,0.5,0.5)')
        d0_before, d1_before = pilot(before_direct, 0), pilot(before_direct, 1)
        d0_after, d1_after = pilot(after_direct, 0), pilot(after_direct, 1)
        assert direction * (d1_after[0] - d1_before[0]) > 2.0, (d1_before, d1_after)
        assert abs(d0_after[0] - d0_before[0]) < 0.01, (d0_before, d0_after)
        report['localDirectTouch'] = {
            'before': positions(before_direct), 'after': positions(after_direct)}

        # Unlimited touch is only a Result/Replay penalty after it actually
        # moves gameplay. Enabling the option alone must not mark the run.
        assert call(live, 'multiplayerSmoke.fixtureStatus()')[18] == 0
        call(live, 'multiplayerSmoke.touchOptions(true,true,1)')
        call(live, 'multiplayerSmoke.touch(0,42,0.5,0.5)')
        exact_tick(live)
        call(live, 'multiplayerSmoke.touch(1,42,0.62,0.5)')
        for _ in range(4):
            exact_tick(live)
        call(live, 'multiplayerSmoke.touch(2,42,0.62,0.5)')
        assert call(live, 'multiplayerSmoke.fixtureStatus()')[18] == 1
        report['unlimitedMarker'] = True

        # Now keep local touch neutral and feed remote seat 1 a synchronized
        # DirectTouchDelta. Only the remote pilot may consume that lane.
        before_remote = probe(live)
        remote = {'analogMode': 3, 'x': 400.0, 'y': 0.0, 'touchUsed': True}
        for _ in range(6):
            exact_tick(live, remote)
        after_remote = probe(live)
        r0_before, r1_before = pilot(before_remote, 0), pilot(before_remote, 1)
        r0_after, r1_after = pilot(after_remote, 0), pilot(after_remote, 1)
        assert r0_after[0] > r0_before[0] + 2.0, (r0_before, r0_after)
        assert abs(r1_after[0] - r1_before[0]) < 0.01, (r1_before, r1_after)
        report['remoteDirectTouchDelta'] = {
            'before': positions(before_remote), 'after': positions(after_remote)}

        # Touch Bomb must remain the native deathbomb action. Fixture 1 enters
        # P2's real eight-tick deathbomb window at a confirmed boundary; the
        # bomb below is produced by TouchController -> live local capture.
        assert call(live, 'multiplayerSmoke.fixture(1)')
        before_bomb = call(live, 'multiplayerSmoke.status()')
        assert before_bomb[24] == 4, before_bomb
        call(live, 'multiplayerSmoke.touchOptions(true,false,1)')
        call(live, 'multiplayerSmoke.touchControls(false,false,1,0,0,0)')
        for _ in range(12):
            exact_tick(live)
        after_bomb = call(live, 'multiplayerSmoke.status()')
        assert after_bomb[22] == 0 and after_bomb[23] == 60 and after_bomb[24] == 1, after_bomb
        report['touchDeathbomb'] = {'before': before_bomb, 'after': after_bomb}

        # Late synchronized analog input must use the same rollback path as
        # buttons. The reference gets exact DirectTouchDelta from frame zero;
        # the late copy predicts neutral, receives the same samples later, and
        # must restore/resimulate to the exact authoritative owner hash.
        ref_peer_context = browser.new_context(service_workers='block')
        reference_context = browser.new_context(service_workers='block')
        late_peer_context = browser.new_context(service_workers='block')
        late_context = browser.new_context(service_workers='block')
        ref_peer, ref_peer_errors = open_net(ref_peer_context, 1)
        reference, reference_errors = open_net(reference_context, 0)
        late_peer, late_peer_errors = open_net(late_peer_context, 1)
        late, late_errors = open_net(late_context, 0)
        exchange_barrier(reference, ref_peer)
        exchange_barrier(late, late_peer)
        analog = {'analogMode': 3, 'x': 300.0, 'y': 0.0, 'touchUsed': True}
        for _ in range(900):
            frame = net(reference)[2]
            submit_at(reference, 1, frame, analog)
            call(reference, 'multiplayerSmoke.netTicks(1)')
            if net(reference)[3] >= 5:
                break
        assert net(reference)[3] == 5 and net(reference)[4] == 5, net(reference)
        for _ in range(900):
            call(late, 'multiplayerSmoke.netTicks(1)')
            if net(late)[3] >= 4:
                break
        assert net(late)[3] == 4, net(late)
        for frame in range(5):
            submit_at(late, 1, frame, analog)
        assert net(late)[5] == 0, net(late)
        submit_at(late, 1, 5, analog)
        call(late, 'multiplayerSmoke.netTicks(1)')
        assert net(late)[5] == -1 and net(late)[3] == net(reference)[3], (net(reference), net(late))
        ref_hash = call(reference, 'multiplayerSmoke.canonical()')
        late_hash = call(late, 'multiplayerSmoke.canonical()')
        assert authoritative(ref_hash) == authoritative(late_hash), (ref_hash, late_hash)
        assert positions(probe(reference)) == positions(probe(late)), (positions(probe(reference)), positions(probe(late)))
        report['analogRollback'] = {
            'frame': net(late)[3], 'reference': ref_hash, 'late': late_hash}
        assert not (ref_peer_errors + reference_errors + late_peer_errors + late_errors), {
            'refPeer': ref_peer_errors, 'reference': reference_errors,
            'latePeer': late_peer_errors, 'late': late_errors}

        report['net'] = net(live)
        report['canonical'] = call(live, 'multiplayerSmoke.canonical()')
        report['errors'] = {'live': live_errors, 'peer': peer_errors}
        assert not live_errors and not peer_errors, report['errors']
        report['passed'] = True
    finally:
        browser.close()

args.output.parent.mkdir(parents=True, exist_ok=True)
args.output.write_text(json.dumps(report, indent=2), encoding='utf-8')
print(json.dumps(report, indent=2), flush=True)
