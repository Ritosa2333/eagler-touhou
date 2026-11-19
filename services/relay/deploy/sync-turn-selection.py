#!/usr/bin/env python3
"""Apply a private TURN selection to the TLS bridge and Relay configuration."""
import fcntl
import hashlib
import json
import os
import pathlib
import re
import subprocess
import sys
import tempfile
import time


def atomic(path, data):
    path = pathlib.Path(path)
    fd, temporary = tempfile.mkstemp(dir=path.parent)
    try:
        with os.fdopen(fd, 'wb') as output:
            output.write(data)
            output.flush()
            os.fsync(output.fileno())
        os.chmod(temporary, 0o600)
        os.replace(temporary, path)
    finally:
        if os.path.exists(temporary):
            os.unlink(temporary)


def encoded(value):
    return (json.dumps(value, separators=(',', ':')) + '\n').encode()


def validate(value, config, now):
    if value.get('schema') != 'eagler/turn-selection/1':
        raise ValueError('Unknown TURN selection schema')
    node = config['nodes'].get(value.get('selected'))
    if not node:
        raise ValueError('Unknown selected node')
    age = now - float(value['evaluatedAt'])
    if not -30 <= age <= config.get('maxAgeSeconds', 180):
        raise ValueError('TURN selection is stale')
    if value.get('urls') != node['urls']:
        raise ValueError('Selected node and TURN URLs differ')
    secret = value.get('sharedSecret')
    if not isinstance(secret, str) or not 1 <= len(secret) <= 4096:
        raise ValueError('Invalid TURN credential configuration')
    if config.get('requireTlsTarget') and value.get('tlsTarget') != node['target']:
        raise ValueError('TLS target has not been published with selection')
    return node


def fetch(config):
    target = pathlib.Path(config['downloadFile']) if config.get('downloadFile') else None
    command = [arg.replace('{download}', str(target)) for arg in config['fetchCommand']]
    result = subprocess.run(command, capture_output=True, timeout=20)
    if result.returncode:
        # SSH/rsync diagnostics may contain paths, never echo the secret payload.
        raise RuntimeError('Selection fetch failed with exit ' + str(result.returncode))
    data = target.read_bytes() if target else result.stdout
    if len(data) > 16384:
        raise ValueError('TURN selection exceeds size limit')
    return json.loads(data)


def turn_backups(value, config, primary_urls):
    candidates = value.get('backups', [])
    if not isinstance(candidates, list):
        raise ValueError('Invalid exported TURN pool')
    allowed = {tuple(node['urls']) for node in config['nodes'].values()}
    if any(not isinstance(node, dict) or not isinstance(node.get('urls'), list)
           or not all(isinstance(url, str) for url in node['urls'])
           or tuple(node['urls']) not in allowed for node in candidates):
        raise ValueError('Unexpected TURN node in exported pool')
    if config.get('backupFile'):
        local = json.loads(pathlib.Path(config['backupFile']).read_bytes())
        if not isinstance(local, list):
            raise ValueError('Invalid local TURN pool')
        candidates = [*candidates, *local]
    health = value.get('health', {})
    blocked_urls = {url for key, node in config['nodes'].items()
                    if key != value['selected'] and health.get(key) in ('degraded', 'saturated')
                    for url in node['urls']}
    seen = set(primary_urls) | blocked_urls
    backups = []
    for node in candidates:
        urls = node.get('urls') if isinstance(node, dict) else None
        secret = node.get('sharedSecret') if isinstance(node, dict) else None
        if (not isinstance(urls, list) or not 1 <= len(urls) <= 8
                or any(not isinstance(url, str) or not re.fullmatch(r'turns?:[^\s]+', url) for url in urls)
                or not isinstance(secret, str) or not 1 <= len(secret) <= 4096):
            raise ValueError('Invalid TURN pool entry')
        new_urls = [url for url in dict.fromkeys(urls) if url not in seen]
        if new_urls:
            backups.append({'urls': new_urls, 'sharedSecret': secret})
            seen.update(new_urls)
    if len(backups) > 3:
        raise ValueError('TURN pool exceeds relay capacity')
    return backups


def apply(value, config, run=subprocess.run, clock=time.time):
    now = clock()
    node = validate(value, config, now)
    active_path = pathlib.Path(config['activeConfig'])
    previous = json.loads(active_path.read_text()) if active_path.exists() else None
    active = {'urls': [*value['urls'], config['tlsUrl']], 'sharedSecret': value['sharedSecret']}
    backups = turn_backups(value, config, active['urls'])
    if backups:
        active['backups'] = backups
    state_path = pathlib.Path(config['stateFile'])
    state = json.loads(state_path.read_text()) if state_path.exists() else {}
    if value['evaluatedAt'] < state.get('evaluatedAt', 0):
        raise ValueError('TURN selection moved backwards')
    bridge = config.get('tlsBridge')
    bridge_changed = False
    if bridge:
        path = pathlib.Path(bridge['config'])
        before = path.read_bytes()
        text, count = re.subn(r'(?m)^\s*proxy_pass [^\r\n]+;$',
                              '        proxy_pass ' + node['target'] + ';', before.decode())
        if count != 1:
            raise ValueError('Expected exactly one TURN TLS target')
        if text.encode() != before:
            atomic(path, text.encode())
            try:
                run(['systemctl', 'reload', bridge['unit']], check=True, capture_output=True, timeout=10)
            except Exception:
                atomic(path, before)
                run(['systemctl', 'reload', bridge['unit']], check=False, capture_output=True, timeout=10)
                raise RuntimeError('TLS reload failed; retained previous TURN selection')
            bridge_changed = True
    if previous != active:
        atomic(active_path, encoded(active))
    if config.get('exportFile'):
        exported = {**value, 'tlsTarget': node['target']}
        # Share the publication lock with the static site exporter.
        with open(config['exportLock'], 'a') as lock:
            fcntl.flock(lock, fcntl.LOCK_EX)
            atomic(config['exportFile'], encoded(exported))
    state = {'selected': value['selected'], 'evaluatedAt': value['evaluatedAt'],
             'lastSuccess': now, 'tlsTarget': node['target'],
             'configurationHash': hashlib.sha256(encoded(active)).hexdigest()}
    atomic(state_path, encoded(state))
    return {'selected': value['selected'], 'ageSeconds': round(now - value['evaluatedAt'], 1),
            'configurationChanged': previous != active, 'tlsReloaded': bridge_changed}


def main():
    config = json.loads(pathlib.Path(sys.argv[1]).read_text())
    with open(config['lockFile'], 'a') as lock:
        fcntl.flock(lock, fcntl.LOCK_EX)
        value = fetch(config)
        if '--validate-only' in sys.argv:
            validate(value, config, time.time())
            turn_backups(value, config, [*value['urls'], config['tlsUrl']])
            print(json.dumps({'selected': value['selected'], 'valid': True}))
        else:
            print(json.dumps(apply(value, config)))


if __name__ == '__main__':
    try:
        main()
    except Exception as error:
        # Keep exception text independent of private configuration values.
        print('TURN selection sync failed: ' + type(error).__name__, file=sys.stderr)
        sys.exit(1)
