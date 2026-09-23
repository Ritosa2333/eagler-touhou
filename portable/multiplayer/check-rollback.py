"""Focused browser proof for TH10 title rollback.

Runs two identical local-seat-0 games.  The reference receives remote input
before each logical frame.  The late run predicts its first five frames, then
receives the same remote samples and must rewind/resimulate to the same visible
gameplay state.  This is deliberately below real transport; it validates the
title rollback owner before WebRTC/WebSocket is allowed to mask mistakes.
"""
import argparse
import json
from pathlib import Path

from playwright.sync_api import sync_playwright


parser = argparse.ArgumentParser()
parser.add_argument('--url', default='http://127.0.0.1:8138/')
parser.add_argument('--output', type=Path,
                    default=Path('artifacts/multiplayer-tests/browser-rollback.json'))
args = parser.parse_args()

LOADOUTS = [[0, 0], [1, 1]]
REMOTE = 0x80  # native Right
SESSION_LOW = 0x13572468
SESSION_HIGH = 0x24681357


def call(page, expression, arg=None):
    return page.evaluate(expression, arg) if arg is not None else page.evaluate(expression)


def exchange_barrier(host, peer):
    host_hello = call(host, 'multiplayerSmoke.sessionPacket(1)')
    peer_hello = call(peer, 'multiplayerSmoke.sessionPacket(1)')
    assert call(host, '(p)=>multiplayerSmoke.applySession(p)', peer_hello)
    assert call(peer, '(p)=>multiplayerSmoke.applySession(p)', host_hello)
    assert call(host, 'multiplayerSmoke.markReady()')
    assert call(peer, 'multiplayerSmoke.markReady()')
    host_ready = call(host, 'multiplayerSmoke.sessionPacket(2)')
    peer_ready = call(peer, 'multiplayerSmoke.sessionPacket(2)')
    assert call(host, '(p)=>multiplayerSmoke.applySession(p)', peer_ready)
    assert call(peer, '(p)=>multiplayerSmoke.applySession(p)', host_ready)
    assert call(host, 'multiplayerSmoke.canStart()')
    assert call(peer, 'multiplayerSmoke.canStart()')


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


def tick(page):
    return call(page, 'multiplayerSmoke.netTicks(1)')


def status(page):
    return call(page, 'multiplayerSmoke.status()')


def net(page):
    return call(page, 'multiplayerSmoke.netStatus()')


def canonical(page):
    return call(page, 'multiplayerSmoke.canonical()')


def submit(page, frame, buttons=REMOTE):
    result = call(page,
                  '(x)=>multiplayerSmoke.submitRemote(x[0],x[1],x[2])',
                  [1, frame, buttons])
    assert result != 0, (frame, buttons, result, net(page))


report = {'passed': False, 'scope': 'TH10 browser title rollback', 'samples': []}
with sync_playwright() as p:
    browser = p.chromium.launch(headless=True, args=['--enable-unsafe-swiftshader'])
    try:
        helper_context = browser.new_context(service_workers='block')
        helper, helper_errors = open_net(helper_context, 1)

        reference_context = browser.new_context(service_workers='block')
        reference, reference_errors = open_net(reference_context, 0)
        exchange_barrier(reference, helper)

        # A second seat-1 gate peer is used because SessionGate state is
        # intentionally one start lifecycle, not a packet factory.
        late_peer_context = browser.new_context(service_workers='block')
        late_peer, late_peer_errors = open_net(late_peer_context, 1)
        late_context = browser.new_context(service_workers='block')
        late, late_errors = open_net(late_context, 0)
        exchange_barrier(late, late_peer)

        # Drive the reference independently to frame 5.  Submit the exact
        # remote sample before every outer tick; before gameplay activates this
        # simply re-submits frame zero and is harmless.  The reference must not
        # itself rely on prediction or it is not an authoritative comparison.
        for _ in range(900):
            frame = net(reference)[2]
            submit(reference, frame)
            tick(reference)
            if net(reference)[3] >= 5:
                break
        assert net(reference)[3] == 5, ('reference frontier', net(reference), status(reference))
        assert net(reference)[4] == 5, ('reference not fully confirmed', net(reference))

        # The late copy deliberately receives no remote samples and advances
        # five speculative frames (0..4) from neutral prediction.
        for _ in range(900):
            tick(late)
            if net(late)[3] >= 4:
                break
        assert net(late)[3] == 4, ('late run prediction frontier', net(late), status(late))
        late_last = 4

        # The late copy used neutral predictions.  Supplying the real Right
        # samples must request rollback at the earliest divergent frame.
        for frame in range(late_last + 1):
            submit(late, frame)
        before = net(late)
        assert before[5] == 0, ('expected rollback from frame zero', before)

        # Also supply the next exact frame to both before advancing.  The late
        # tick performs restore + resimulation before its new forward frame.
        next_frame = late_last + 1
        submit(late, next_frame)
        tick(late)
        after = net(late)
        assert after[5] == -1, ('rollback request not cleared', before, after)
        assert after[3] == net(reference)[3], (net(reference), after)
        post_reference = status(reference)
        post_late = status(late)
        report['postRollbackStatus'] = {
            'reference': post_reference,
            'late': post_late,
            'netReference': net(reference),
            'netLate': after,
        }
        assert post_reference == post_late, (
            'post rollback mismatch', post_reference, post_late,
            net(reference), after)
        post_reference_hash = canonical(reference)
        post_late_hash = canonical(late)
        report['postRollbackCanonical'] = {
            'reference': post_reference_hash,
            'late': post_late_hash,
        }
        assert post_reference_hash == post_late_hash, (
            'post rollback canonical mismatch', post_reference_hash, post_late_hash)

        # Continue with exact inputs long enough to exercise bullets/enemies
        # after correction.  Record visible owner state at several frontiers.
        for _ in range(180):
            rf = net(reference)[2]
            lf = net(late)[2]
            assert rf == lf, (rf, lf)
            submit(reference, rf)
            submit(late, lf)
            tick(reference)
            tick(late)
            if rf in (10, 30, 60, 120, 180):
                report['samples'].append({
                    'frame': rf,
                    'reference': status(reference),
                    'late': status(late),
                    'netReference': net(reference),
                    'netLate': net(late),
                })
            # Current public diagnostic covers player/economy/lifecycle.  A
            # deeper canonical owner hash is added after this gate is stable.
            assert status(reference) == status(late), (rf, status(reference), status(late))
            assert canonical(reference) == canonical(late), (rf, canonical(reference), canonical(late))

        assert not (helper_errors + reference_errors + late_peer_errors + late_errors), {
            'helper': helper_errors, 'reference': reference_errors,
            'latePeer': late_peer_errors, 'late': late_errors,
        }
        report['passed'] = True
        report['rollbackBefore'] = before
        report['rollbackAfter'] = after
    finally:
        browser.close()
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(json.dumps(report, indent=2), encoding='utf-8')

print(json.dumps({'passed': report['passed'], 'samples': len(report['samples'])}))
