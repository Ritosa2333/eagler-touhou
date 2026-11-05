"""Original40aa60 ETEX10000000 only changes the VM blend bits.

Use the SHA-pinned native ETEX environment, execute the actual dispatch, and
record flags/rotation/cursor. This is a constructed CPU fixture, not a replay.
"""
import json
import pathlib

ROOT = pathlib.Path(__file__).resolve().parents[2]
source = ROOT / 'scripts/reverse/sample-etex-boundaries.py'
ns = {'__file__': str(source), '__name__': 'native_etex_blend_probe'}
exec(compile(source.read_text(encoding='utf-8').split('native = Native()')[0], str(source), 'exec'), ns)
from unicorn.x86_const import UC_X86_REG_ECX
rows = []
for flags in [3, 0xe3, 0x20000003, 0x200000e3, 0x0080040f]:
    for enabled in [0, 1, 2, -1]:
        native = ns['Native']()
        native.seed([{'kind': 0x10000000, 'c': enabled}])
        bullet = ns['BULLET']
        native.put(bullet + 0x484, 'I', flags)
        native.put(bullet + 0x34, 'f', 1.25)
        native.call(0x40aa60, registers={UC_X86_REG_ECX: bullet})
        rows.append({'beforeFlags': flags, 'operandC': enabled,
                     'afterFlags': native.get(bullet + 0x484, 'I')[0],
                     'rotationZ': native.get(bullet + 0x34, 'f')[0],
                     'cursor': native.get(bullet + 0x548, 'i')[0],
                     'active': native.get(bullet + 0x528, 'I')[0]})
target = ROOT / 'tests/fixtures/etex-blend-native.json'
target.write_text(json.dumps({'schema': 'th12-etex-blend-native/1',
                              'originalExeSha256': ns['EXE_SHA256'],
                              'entry': '0040aa60', 'handler': '0040b4fd',
                              'scope': 'constructed original ETEX dispatch, no candidate code',
                              'cases': rows}, indent=2) + '\n')
print(json.dumps({'output': str(target), 'cases': len(rows)}))
