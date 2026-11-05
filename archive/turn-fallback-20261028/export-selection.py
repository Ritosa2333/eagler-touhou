#!/usr/bin/env python3
"""Read the selector's coherent result through a restricted SSH command."""
import fcntl
import json
import pathlib


def export(config_path):
    config = json.loads(pathlib.Path(config_path).read_text())
    with open(config['selectionState'] + '.lock', 'a') as lock:
        fcntl.flock(lock, fcntl.LOCK_SH)
        state = json.loads(pathlib.Path(config['selectionState']).read_text())
        active = json.loads(pathlib.Path(config['activeConfig']).read_text())
        node = next(n for n in config['nodes'] if n['id'] == state['selected'])
        if active != {'urls': node['urls'], 'sharedSecret': node['secret']}:
            raise ValueError('Selection and active configuration differ')
        return dict(schema='eagler/turn-selection/1', selected=state['selected'],
                    evaluatedAt=state['evaluatedAt'], switchedAt=state['switchedAt'],
                    health=state['health'], **active)


if __name__ == '__main__':
    print(json.dumps(export('/etc/eagler-turn-monitor/selection-config.json')))
