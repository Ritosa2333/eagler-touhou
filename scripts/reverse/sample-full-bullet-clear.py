"""Read-only original ECL510's40cf60 full-clear/ANM96 CPU behavior.

Constructed original managers and physical bullets. Executes actual viewport
selection, item9 births, flags, effect factory/color mapping and targeted next
Bullet21 visits. Host allocation boundaries reuse the ANM effect sampler.
Never changes game source, original files, replays or accepted goldens.
"""
import hashlib, json, pathlib, re
ROOT = pathlib.Path(__file__).resolve().parents[2]
sampler = ROOT / 'scripts/reverse/sample-ufo-summon-effect.py'
namespace = {'__file__': str(sampler), '__name__': 'native_full_clear_definitions'}
exec(compile(sampler.read_text(encoding='utf-8').split('\nscenarios = []\n')[0], str(sampler), 'exec'), namespace)
Native = namespace['Native']
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import *
BULLETS, ITEMS, BONUS, FONT = 0x2400000, 0x5000000, 0x2012000, 0x2015000
rows = [
    {'slot': 0, 'position': [0, 128], 'hitbox': [4, 4], 'phase': 1, 'bodyScript': 35, 'color': 0},
    {'slot': 1, 'position': [-193.5, 128], 'hitbox': [4, 4], 'phase': 2, 'bodyScript': 35, 'color': 1},
    {'slot': 2, 'position': [-198, 128], 'hitbox': [14, 14], 'phase': 1, 'bodyScript': 75, 'color': 2},
    {'slot': 3, 'position': [-194, 128], 'hitbox': [4, 4], 'phase': 1, 'bodyScript': 35, 'color': 3},
    {'slot': 4, 'position': [0, -1.9409205913543701], 'hitbox': [4, 4], 'phase': 1, 'bodyScript': 35, 'color': 4},
    {'slot': 5, 'position': [0, 128], 'hitbox': [4, 4], 'phase': 3, 'bodyScript': 35, 'color': 5},
    {'slot': 6, 'position': [0, 128], 'hitbox': [4, 4], 'phase': 0, 'bodyScript': 35, 'color': 6},
    {'slot': 7, 'position': [260, 128], 'hitbox': [4, 4], 'phase': 2, 'bodyScript': 35, 'color': 7},
    {'slot': 8, 'position': [0, 450], 'hitbox': [4, 4], 'phase': 2, 'bodyScript': 69, 'color': 8},
    {'slot': 9, 'position': [40, 220], 'hitbox': [10, 10], 'phase': 1, 'bodyScript': 69, 'color': 9},
]
cases = []
for convert, card, active in [(1, 0, False), (0, 0, False), (1, 95, True),
                               (1, 96, True), (1, 99, True), (1, 96, False)]:
    native = Native(0x1234)
    native.mu.mem_map(ITEMS, 0x800000)
    ascii_raw = (ROOT / 'local/retail/ascii.anm').read_bytes()
    ascii_resource = namespace['install_resource'](native.mu, ascii_raw, 0x6700000, bank_id=11)
    native.put(0x4b43b8, 'I', FONT)
    native.put(FONT + 0x18fb4, 'I', ascii_resource['resource'])
    native.put(0x4b43c8, 'I', BULLETS)
    native.put(0x4b44f0, 'I', ITEMS)
    native.put(0x4b43cc, 'I', BONUS)
    native.put(BONUS + 0x78, 'II', card, int(active))
    native.put(BULLETS + 0x44, '4f', 0, 224, 0, 384)
    native.put(BULLETS + 0x54, 'f', 448)
    native.put(BULLETS + 0x4debdc, 'I', native.resource['resource'])
    for row in rows:
        at = BULLETS + 0x64 + row['slot'] * 0x9f8
        native.put(at, 'Ii', 2, 9)
        native.put(at + 0x532, 'H', row['phase'])
        native.put(at + 0x4bc, '3f', *row['position'], .1)
        native.put(at + 0x4dc, '2f', *row['hitbox'])
        native.put(at + 0x4e4, 'iifII', 80, 81, 81, 0x4b2ed0, 1)
        native.put(at + 0x9f6, 'h', row['color'])
        native.call(0x454d10, (at + 8, row['bodyScript']), {UC_X86_REG_ECX: native.resource['resource']})
    point_births = []
    def observe(mu, address, size, unused):
        sp = mu.reg_read(UC_X86_REG_ESP)
        kind, position = native.get(sp + 4, '2I')
        point_births.append({'type': kind, 'position': native.get(position, '3f'),
                             'sourceSlot': (position - BULLETS - 0x64 - 0x4bc) // 0x9f8})
    native.mu.hook_add(UC_HOOK_CODE, observe, begin=0x4273f0, end=0x4273f0)
    before = native.rng()
    native.call(0x40cf60, (convert,))
    def bullet_state(row):
        at = BULLETS + 0x64 + row['slot'] * 0x9f8
        return {'slot': row['slot'], 'flags': native.get(at, 'I')[0],
                'phase': native.get(at + 0x532, 'H')[0], 'word4': native.get(at + 4, 'i')[0],
                'timer': native.get(at + 0x4e4, 'iif'), 'interrupt': native.get(at + 8 + 0x3c4, 'h')[0],
                'position': native.get(at + 0x4bc, '3f')}
    after = [bullet_state(row) for row in rows]
    effects = native.nodes()
    for row in rows:
        at = BULLETS + 0x64 + row['slot'] * 0x9f8
        if native.get(at, 'I')[0] & 8:
            native.call(0x4099e0, registers={UC_X86_REG_ESI: at})
    cases.append({'convert': bool(convert), 'card': card, 'activeSpell': active,
                  'rngBefore': before, 'rngAfter': native.rng(), 'afterClear': after,
                  'pointBirths': point_births, 'effects': effects,
                  'afterSelectedNextBulletVisit': [bullet_state(row) for row in rows]})
output = ROOT / 'tests/fixtures/full-bullet-clear-native.json'
result = {'schema': 'th12-native-full-bullet-clear/1', 'originalExeSha256': namespace['EXE_SHA256'],
          'bulletAnmSha256': hashlib.sha256(namespace['RAW']).hexdigest(),
          'asciiAnmSha256': hashlib.sha256(ascii_raw).hexdigest(),
          'wholeGameOracle': False, 'acceptedReplayGolden': False, 'gpuExecuted': False,
          'boundary': 'Actual40cf60/4273f0/454d10/461250/4099e0 with a constructed384x448 BulletManager viewport, ten physical bullet slots and original embedded bullet.anm bodies. Only selected flag8 next-visits are invoked. Host malloc/free and resource tables use sample-ufo-summon-effect.py; no gameplay function stubs, no GPU draw. Enemy/Player/UFO/Item updates omitted.',
          'initialBullets': rows, 'cases': cases}
output.write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
print(json.dumps({'output': str(output), 'cases': [{'convert': r['convert'], 'card': r['card'], 'activeSpell': r['activeSpell'],
    'flagged': [b['slot'] for b in r['afterClear'] if b['flags'] & 8],
    'pointSlots': [p['sourceSlot'] for p in r['pointBirths']],
    'effectScripts': [e['script'] for e in r['effects']]} for r in cases]}))
