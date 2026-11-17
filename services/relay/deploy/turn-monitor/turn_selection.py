#!/usr/bin/env python3
"""Publish one preferred healthy TURN for new allocations."""
import datetime
import json
import pathlib
import sys
import time


def health(node, paths):
    fresh = [p for p in paths if p['target'] == node and p.get('ageSeconds', 100000) < 900]
    # Require corroboration before failing over. One source must have
    # completed its normal consecutive-failure threshold.
    bad_sources = {p['source'] for p in fresh if p.get('status') in ('observing', 'degraded')}
    if len(bad_sources) >= 2 and any(p.get('status') == 'degraded' for p in fresh):
        return 'degraded'
    good = any(p.get('status') == 'healthy' and p.get('complete')
               and p.get('lossPercent', 100) <= 1 and p.get('sendDropped') == 0
               and p.get('goodRounds', 0) >= 3 for p in fresh)
    if good and not bad_sources:
        return 'healthy'
    return 'unknown'


def choose(priority, current, paths, cooldown=False, capacity=None):
    states = {node: health(node, paths) for node in priority}
    for node, sample in (capacity or {}).items():
        if node in states and sample.get('overloaded'):
            states[node] = 'saturated'
    if current not in states:
        current = priority[0]
    if cooldown and states[current] not in ('degraded', 'saturated'):
        return current, states
    for node in priority:
        if node == current and states[node] not in ('degraded', 'saturated'):
            return current, states
        if states[node] == 'healthy':
            return node, states
    return current, states


def main(config_path):
    import fcntl

    config = json.loads(pathlib.Path(config_path).read_text())
    with pathlib.Path(config['selectionState'] + '.lock').open('a') as lock:
        fcntl.flock(lock, fcntl.LOCK_EX)
        select(config)


def select(config):
    from monitor import atomic_json

    matrix = json.loads(pathlib.Path(config['summary']).read_text())
    updated = datetime.datetime.fromisoformat(matrix['updatedAt']).timestamp()
    if time.time() - updated > 180:
        raise RuntimeError('Quality matrix is stale; retain current TURN')
    state_path = pathlib.Path(config['selectionState'])
    state = json.loads(state_path.read_text()) if state_path.exists() else {'selected': config['priority'][0], 'switchedAt': 0}
    capacity = {}
    if config.get('capacityState'):
        try:
            capacity = json.loads(pathlib.Path(config['capacityState']).read_text())
        except (OSError, ValueError):
            # Keep an established overload latch when telemetry is unavailable.
            capacity = state.get('capacity', {})
    selected, states = choose(config['priority'], state['selected'], matrix['paths'],
                              time.time() - state['switchedAt'] < 600, capacity)
    if selected != state['selected']:
        state['selected'] = selected
        state['switchedAt'] = time.time()
    state.update(evaluatedAt=time.time(), health=states, capacity=capacity)
    node = next(node for node in config['nodes'] if node['id'] == selected)
    value = {'urls': node['urls'], 'sharedSecret': node['secret']}
    active = pathlib.Path(config['activeConfig'])
    if not active.exists() or json.loads(active.read_text()) != value:
        atomic_json(active, value)
        active.chmod(0o600)
    atomic_json(state_path, state)
    print(json.dumps({'selected': selected, 'health': states}))


if __name__ == '__main__':
    main(sys.argv[1])
