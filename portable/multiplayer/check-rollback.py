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


def enemy_debug(page):
    return call(page, 'multiplayerSmoke.enemyDebug()')


def authoritative(values):
    # Both runs use identical loadouts, local seat and allocation bootstrap.
    # Require the full owner composite, including visual RNG and authored ANM.
    # This is an identical-layout rollback oracle, not a cross-device pointer-
    # independent hash. A future cross-device schema must preserve equivalent
    # semantic coverage instead of dropping a divergent owner.
    return values[1:28]


def submit(page, frame, buttons=REMOTE):
    result = call(page,
                  '(x)=>multiplayerSmoke.submitRemote(x[0],x[1],x[2])',
                  [1, frame, buttons])
    assert result in (1, 2, 3, 4), (frame, buttons, result, net(page))


report = {'passed': False, 'scope': 'TH10 browser title rollback', 'samples': []}
with sync_playwright() as p:
    browser = p.chromium.launch(headless=True, args=['--enable-unsafe-swiftshader'])
    try:
        helper_context = browser.new_context(service_workers='block')
        helper, helper_errors = open_net(helper_context, 1)

        reference_context = browser.new_context(service_workers='block')
        reference, reference_errors = open_net(reference_context, 0)
        exchange_barrier(reference, helper)
        print('rollback-check: reference barrier ready', flush=True)

        # A second seat-1 gate peer is used because SessionGate state is
        # intentionally one start lifecycle, not a packet factory.
        late_peer_context = browser.new_context(service_workers='block')
        late_peer, late_peer_errors = open_net(late_peer_context, 1)
        late_context = browser.new_context(service_workers='block')
        late, late_errors = open_net(late_context, 0)
        exchange_barrier(late, late_peer)
        print('rollback-check: late barrier ready', flush=True)

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
        print('rollback-check: reference frame 5 exact', flush=True)

        # The late copy deliberately receives no remote samples and advances
        # five speculative frames (0..4) from neutral prediction.
        for _ in range(900):
            tick(late)
            if net(late)[3] >= 4:
                break
        assert net(late)[3] == 4, ('late run prediction frontier', net(late), status(late))
        late_last = 4
        print('rollback-check: late frame 4 predicted', flush=True)

        # The late copy used neutral predictions.  Supplying the real Right
        # samples must request rollback at the earliest divergent frame.
        for frame in range(late_last + 1):
            submit(late, frame)
        before = net(late)
        assert before[5] == 0, ('expected rollback from frame zero', before)
        print('rollback-check: rollback requested', before, flush=True)

        # Also supply the next exact frame to both before advancing.  The late
        # tick performs restore + resimulation before its new forward frame.
        next_frame = late_last + 1
        submit(late, next_frame)
        print('rollback-check: entering rollback tick', flush=True)
        tick(late)
        print('rollback-check: rollback tick returned', flush=True)
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
        assert authoritative(post_reference_hash) == authoritative(post_late_hash), (
            'post rollback authoritative mismatch',
            authoritative(post_reference_hash), authoritative(post_late_hash),
            post_reference_hash, post_late_hash)

        # Continue with exact inputs until a mature Stage 1 frontier. Record
        # visible owner state at several points before the second correction.
        index = 0
        while net(reference)[2] < 240:
            rf = net(reference)[2]
            lf = net(late)[2]
            assert rf == lf, (rf, lf)
            submit(reference, rf)
            submit(late, lf)
            tick(reference)
            tick(late)
            if index % 30 == 0:
                print('rollback-check: continuation', index, rf, flush=True)
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
            reference_status = status(reference)
            late_status = status(late)
            reference_hash = canonical(reference)
            late_hash = canonical(late)
            assert reference_status == late_status, (
                rf, reference_status, late_status, reference_hash, late_hash,
                net(reference), net(late))
            assert authoritative(reference_hash) == authoritative(late_hash), (
                rf, authoritative(reference_hash), authoritative(late_hash),
                reference_hash, late_hash, net(reference), net(late),
                enemy_debug(reference), enemy_debug(late))
            index += 1

        # Second correction: the last confirmed direction is Right. Reference
        # receives a Left turn exactly at frame 240; late intentionally misses
        # five samples and predicts the previous direction before the common
        # core's short direction-prediction window expires.
        for frame in range(240,245):
            submit(reference, frame, 0x40)
            tick(reference)
            tick(late)
        assert net(reference)[3] == 244 and net(late)[3] == 244, (
            net(reference), net(late))
        for frame in range(240,245):
            submit(late, frame, 0x40)
        second_before = net(late)
        assert second_before[5] == 240, ('expected mature rollback', second_before)
        submit(reference,245,0x40);submit(late,245,0x40)
        tick(reference);tick(late)
        second_after = net(late)
        assert second_after[5] == -1 and second_after[3] == net(reference)[3], (
            second_before, second_after, net(reference))
        second_ref_status=status(reference);second_late_status=status(late)
        second_ref_hash=canonical(reference);second_late_hash=canonical(late)
        assert second_ref_status == second_late_status, (
            'mature rollback visible mismatch', second_ref_status, second_late_status)
        assert authoritative(second_ref_hash) == authoritative(second_late_hash), (
            'mature rollback authoritative mismatch', authoritative(second_ref_hash),
            authoritative(second_late_hash), second_ref_hash, second_late_hash,
            enemy_debug(reference), enemy_debug(late))
        report['matureRollbackBefore']=second_before
        report['matureRollbackAfter']=second_after

        # Keep running corrected Left input to prove the restored world remains
        # stable after the immediate comparison.
        while net(reference)[2] < 360:
            rf=net(reference)[2];lf=net(late)[2];assert rf==lf,(rf,lf)
            submit(reference,rf,0x40);submit(late,lf,0x40)
            tick(reference);tick(late)
            reference_status=status(reference);late_status=status(late)
            reference_hash=canonical(reference);late_hash=canonical(late)
            assert reference_status==late_status,(rf,reference_status,late_status)
            assert authoritative(reference_hash)==authoritative(late_hash),(
                rf,authoritative(reference_hash),authoritative(late_hash),
                reference_hash,late_hash,enemy_debug(reference),enemy_debug(late))

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
