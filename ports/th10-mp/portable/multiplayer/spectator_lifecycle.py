"""Focused real-WASM lifecycle checks, separate from stage canonical fixtures.

Players use the existing deterministic native test pump. Source-idle checks
hold that pump with healthy real relay sockets; they are not Pause-key or
physical-device acceptance. Normal playback is checked per 1/60 native pump.
"""
import time


def run(*, pages, viewer, setup, call, advance_sources, url, spectator_url,
        spectator_id, loadouts, session, report, navigation_only=False):
    result = report['lifecycle'] = {'entries': [], 'sourceIdleFixture': 'source native pumps held; live relay sockets'}

    def state():
        value = call(viewer, 'multiplayerSmoke.spectatorPlaybackState()')
        assert not call(viewer, 'multiplayerSmoke.networkError()'), value
        return value

    def tick():
        return call(viewer, 'multiplayerSmoke.spectatorTick()')['playback']

    def control(action, frame=0):
        assert call(viewer, 'v=>multiplayerSmoke.spectatorPlaybackControl(...v)', [action, frame])

    def wait(predicate, label, timeout=30):
        deadline = time.monotonic() + timeout
        while time.monotonic() < deadline:
            value = tick()
            if value and predicate(value):
                return value
            viewer.wait_for_timeout(5)
        raise AssertionError((label, state()))

    def identity():
        actual = call(viewer, """async()=>{const bytes=await(await fetch('/th10-sdl.wasm',{cache:'no-store'})).arrayBuffer();
          return [...new Uint8Array(await crypto.subtle.digest('SHA-256',bytes))].map(v=>v.toString(16).padStart(2,'0')).join('');}""")
        expected = call(viewer, 'multiplayerSmoke.identity()')['wasmSha256']
        assert actual == expected, (actual, expected)
        return actual

    def pacing(label, presentations=90):
        start = state()['position']; deltas = []; wall = time.monotonic()
        for _ in range(presentations):
            before = state()
            after = tick()
            delta = after['position'] - before['position']
            assert 0 <= delta <= 1, (label, before, after)
            if before['position'] < before['edge']:
                assert delta == 1, (label, before, after)
            deltas.append(delta)
            viewer.wait_for_timeout(1000 / 60)
        assert sum(deltas) > 0, (label, state())
        return {'presentations': presentations, 'ticks': sum(deltas), 'maxTicksPerPresentation': max(deltas),
                'seconds': time.monotonic() - wall, 'start': start, 'end': state()['position']}

    # Retain enough real confirmed input to interrupt a seek before its target.
    result['phase'] = 'source-history'
    history_deadline = time.monotonic() + 300
    next_history_sample = time.monotonic()
    history_frames = 300 if navigation_only else 1200
    while min(call(page, 'multiplayerSmoke.netStatus()')[3] for page in pages) < history_frames:
        assert time.monotonic() < history_deadline, ('source history deadline',
            [call(page, 'multiplayerSmoke.netStatus()') for page in pages])
        advance_sources(pages)
        if time.monotonic() >= next_history_sample:
            print('lifecycle source history', [call(page, 'multiplayerSmoke.netStatus()') for page in pages], flush=True)
            next_history_sample = time.monotonic() + 10
        pages[0].wait_for_timeout(1)
    assert call(viewer, 'v=>multiplayerSmoke.spectatorConnect(...v)', [spectator_url, spectator_id])
    first = wait(lambda s: not s['seeking'], 'initial checkpoint')
    assert not first['following'] and first['position'] == first['min'], first
    result['entries'].append({'position': first['position'], 'received': first['received'], 'wasmSha256': identity()})
    result['initialPacing'] = pacing('initial historical playback', 10 if navigation_only else 90)

    # The Boss button sends the same frame Seek as the timeline. Exercise
    # forward navigation and a rewind through the real native stage loader.
    result['phase'] = 'native-navigation'
    origin = state()['min']; control('pause'); navigation = []
    for offset in (45, 90, 45, 0, 90):
        control('seek', origin + offset)
        reached = wait(lambda s: not s['seeking'], 'native mark seek')
        assert reached['paused'] and reached['position'] == origin + offset, reached
        navigation.append(reached['position'])
    result['nativeNavigation'] = navigation
    control('play')
    if navigation_only:
        result['resumedPacing'] = pacing('native navigation resume', 10)
        control('live'); target = state()['target']
        assert tick()['seeking']
        for _ in range(4):
            advance_sources(pages)
        during = tick()
        assert during['seeking'] and during['edge'] > target, during
        control('live')
        assert state()['target'] == target, ('repeated Live changed target', target, state())
        reached = wait(lambda s: not s['seeking'], 'fixed navigation live target')
        assert reached['position'] == target and reached['following'], reached
        result['fixedLiveTarget'] = target
        result['afterLivePacing'] = pacing('native navigation live resume', 10)
        assert not report['errors'], report['errors']
        result['phase'] = 'complete'; result['passed'] = True; report['passed'] = True
        return

    result['phase'] = 'cancel-seek'
    target = state()['edge']; control('seek', target)
    during = tick(); assert during['seeking'] and during['position'] < target, during
    control('pause'); stopped = state()
    assert stopped['paused'] and not stopped['seeking'] and stopped['position'] < target, stopped
    for _ in range(10):
        assert tick()['position'] == stopped['position']
    result['seekPause'] = {'target': target, 'stoppedAt': stopped['position']}
    control('play'); result['resumedPacing'] = pacing('historical resume', 30)

    result['phase'] = 'live-budget'
    control('live'); target = state()['target']
    during = tick(); assert during['seeking'], during
    for _ in range(4):
        advance_sources(pages)
    control('live')
    assert state()['target'] == target, ('repeated Live changed target', target, state())
    settled = wait(lambda s: s['following'] and not s['seeking'], 'fixed live target')
    assert settled['position'] == target, (target, settled)
    result['fixedLiveTarget'] = target
    # Deliver a burst ahead of the viewer. Following may wait but cannot
    # reactivate multi-input catch-up merely because the edge has advanced.
    deltas = []
    for _ in range(90):
        for _ in range(4):
            advance_sources(pages)
        before = state(); after = tick(); delta = after['position'] - before['position']
        assert 0 <= delta <= 1 and not after['seeking'], (before, after)
        deltas.append(delta)
    assert sum(deltas) > 0
    result['followingBudget'] = {'presentations': len(deltas), 'ticks': sum(deltas),
                                'maxTicksPerPresentation': max(deltas), 'extraLag': state()['edge'] - state()['position']}
    frontier = min(min(call(page, 'multiplayerSmoke.netStatus()')[i] for i in (3, 4)) for page in pages) + 1
    # Publication runs before the native forward tick. Flush the final recorded
    # input on one more callback without manufacturing another captured input.
    for page in pages:
        call(page, 'multiplayerSmoke.netTicks(1)')
    wait(lambda s: s['received'] >= frontier, 'final confirmed history delivery')
    result['confirmedFrontier'] = frontier
    control('live'); wait(lambda s: s['following'] and not s['seeking'] and s['position'] == s['edge'], 'second fixed target')

    def idle(seconds, paused, feed_sources=False):
        before = state(); source_before = [call(page, 'multiplayerSmoke.netStatus()')[3] for page in pages]
        deadline = time.monotonic() + seconds
        while time.monotonic() < deadline:
            if feed_sources:
                advance_sources(pages)
            after = tick()
            assert after['position'] == before['position'], (before, after)
            assert after['paused'] == paused
            assert call(viewer, 'globalThis.__eaglerPeerTransport?.relay?.readyState') == 1
            viewer.wait_for_timeout(50)
        source_after = [call(page, 'multiplayerSmoke.netStatus()')[3] for page in pages]
        assert all(call(page, 'globalThis.__eaglerPeerTransport?.relay?.readyState') == 1 for page in pages)
        if feed_sources:
            assert after['received'] > before['received'], (before, after)
        else:
            assert source_after == source_before, (source_before, source_after)
        return {'seconds': seconds, 'position': after['position'], 'receivedBefore': before['received'],
                'received': after['received'], 'socketOpen': True, 'sourceFramesBefore': source_before,
                'sourceFramesAfter': source_after}

    control('pause'); result['phase'] = 'local-pause'
    result['localPause'] = idle(17, True, feed_sources=True)

    # Same browser page is deliberately retired before each new native world.
    # The mounted Launcher matrix separately covers permanent iframe ownership.
    for attempt in range(2):
        result['phase'] = 'reentry-' + str(attempt + 1)
        viewer.goto('about:blank')
        call(setup, """()=>{const ws=__th10SpectatorLobbySockets.at(-1);
          ws.send(JSON.stringify({type:'leave-spectator'}));ws.send(JSON.stringify({type:'spectate'}));}""")
        setup.wait_for_timeout(100)
        viewer.goto(url); viewer.wait_for_function('window.multiplayerSmoke !== undefined', timeout=120000)
        call(viewer, 'multiplayerSmoke.setSpectatorCount(1)')
        call(viewer, 'v=>multiplayerSmoke.startNet(...v)', [loadouts, 0, 1, 1234, *session])
        assert call(viewer, 'v=>multiplayerSmoke.spectatorConnect(...v)', [spectator_url, spectator_id])
        entered = wait(lambda s: not s['seeking'], 'same-run native reentry')
        assert not entered['following'], entered
        result['entries'].append({'attempt': attempt + 1, 'position': entered['position'],
                                  'received': entered['received'], 'wasmSha256': identity()})
        pacing('reentry historical playback', 30)
    # Finish with quiet publishers. No claim is made about restarting frozen
    # players; their independent confirmed-input watchdog stays unchanged.
    frontier = min(min(call(page, 'multiplayerSmoke.netStatus()')[i] for i in (3, 4)) for page in pages) + 1
    for page in pages:
        call(page, 'multiplayerSmoke.netTicks(1)')
    wait(lambda s: s['received'] >= frontier, 'final source publication')
    control('live')
    wait(lambda s: s['following'] and not s['seeking'] and s['position'] == s['edge'], 'quiet live edge')
    result['phase'] = 'healthy-source-idle'
    result['sourceIdle'] = idle(17, False)
    assert not report['errors'], report['errors']
    result['phase'] = 'complete'; result['passed'] = True
    report['passed'] = True
