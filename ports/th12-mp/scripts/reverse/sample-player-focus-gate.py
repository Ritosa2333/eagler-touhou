"""Original C598 focus gate, including restored focus and live-enemy count.

Runs full4364f0 with original ANM resources. SHT options are explicitly absent,
normal/focused speeds are explicit synthetic fixed-point inputs. No existing
fixture, replay, original asset or runtime is changed; no GPU is executed.
"""
import json
import pathlib
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts/reverse'))
source = (ROOT / 'scripts/reverse/sample-player-focus.py').read_text(encoding='utf-8').split('frames = []')[0]
rows = []
cases = [(age, focus, 1) for age in [0, 1, 3, 4, 10] for focus in [0, 1]]
cases += [(age, 1, 0) for age in [4, 10]]
for age, restored, count in cases:
    ns = {'__file__': str(ROOT / 'scripts/reverse/sample-player-focus.py')}
    exec(compile(source, 'original-focus-gate-bootstrap', 'exec'), ns)
    ns['frame'] = age
    put, read, call, player = ns['put'], ns['read'], ns['call'], ns['PLAYER']
    put(ns['ENEMY'] + 0x70, 'i', count)
    put(player + 0x990, '4i', 512, 256, 362, 181)
    put(player + 0xc598, 'i', restored)
    put(player + 0xa48, 'i', age)
    put(player + 0x988, 'ii', 0, 400 * 128)
    put(0x4d49d0, 'I', 0x88)
    before = read(0x4ce568, 'II')
    call(0x4364f0, {ns['UC_X86_REG_EDI']: player})
    rows.append({'initial': {'totalAge': age, 'restoredFocus': restored,
                             'liveEnemyCount': count, 'held': 0x88,
                             'speedsFixed': [512, 256, 362, 181],
                             'positionFixed': [0, 51200]},
                 'after': {'focused': read(player + 0xc598, 'i')[0],
                           'positionFixed': read(player + 0x988, 'ii'),
                           'totalAge': read(player + 0xa48, 'i')[0],
                           'markerLinked': bool(read(player + 0x8258, 'I')[0]),
                           'rngUnchanged': before == read(0x4ce568, 'II')}})
output = ROOT / 'tests/fixtures/player-focus-gate-original.json'
output.write_text(json.dumps({'schema': 'th12-original-player-focus-gate/1',
    'originalExeSha256': ns['EXE_SHA256'], 'gpuExecuted': False, 'wholeGameOracle': False,
    'provider': 'Actual4364f0,454d10,4615a0 and original bullet/pl00 ANM; explicit no-option synthetic Player, fixed speeds and total age; no instruction hooks.',
    'timerEvidence': '436100 initializes totalAge+a48;437475 common Player16 tail advances it; miss/respawn reset only stateAge+a34.',
    'rows': rows}, indent=2) + '\n', encoding='utf-8')
print(json.dumps({'cases': len(rows), 'rngUnchanged': all(r['after']['rngUnchanged'] for r in rows), 'output': str(output)}))
