#!/usr/bin/env python3
"""Read the preferred TURN and eligible alternatives under the selector lock."""
import fcntl
import json
import pathlib
import sys


def export(config_path):
    config = json.loads(pathlib.Path(config_path).read_text())
    with open(config['selectionState'] + '.lock', 'a') as lock:
        fcntl.flock(lock, fcntl.LOCK_SH)
        state = json.loads(pathlib.Path(config['selectionState']).read_text())
        active = json.loads(pathlib.Path(config['activeConfig']).read_text())
        nodes = {node['id']: node for node in config['nodes']}
        selected = state['selected']
        preferred = nodes[selected]
        if active != {'urls': preferred['urls'], 'sharedSecret': preferred['secret']}:
            raise ValueError('Selection and active configuration differ')
        backups = [
            {'urls': nodes[node]['urls'], 'sharedSecret': nodes[node]['secret']}
            for node in config['priority']
            if node != selected and state['health'].get(node) not in ('degraded', 'saturated')
        ]
        return dict(schema='eagler/turn-selection/1', selected=selected,
                    evaluatedAt=state['evaluatedAt'], switchedAt=state['switchedAt'],
                    health=state['health'], backups=backups, **active)


if __name__ == '__main__':
    config_path = sys.argv[1] if len(sys.argv) > 1 else '/etc/eagler-turn-monitor/selection-config.json'
    print(json.dumps(export(config_path)))
