"""Valid original shooter births, full-clear tint and Primary27 RNG samples.

Executes40a250's real color remap/bullet ANM and40cf60(1), then460fe0.
Constructed CPU fixture with host allocator/resource boundaries, no provider
changes, runtime edits, GPU rendering, original edits or accepted goldens.
"""
import hashlib, json, pathlib
ROOT = pathlib.Path(__file__).resolve().parents[2]
sampler = ROOT / 'scripts/reverse/sample-ufo-summon-effect.py'
namespace = {'__file__': str(sampler), '__name__': 'valid_full_clear_color_definitions'}
exec(compile(sampler.read_text(encoding='utf-8').split('\nscenarios = []\n')[0], str(sampler), 'exec'), namespace)
Native = namespace['Native']
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import *
BULLETS, ITEMS, BONUS, FONT = 0x2400000, 0x5000000, 0x2012000, 0x2015000
SHOOTER, PLAYER = 0x2018000, 0x2100000
cases = []
for kind, color, origin in [(0, 5, [-80, 120]), (17, 4, [0, 160]), (26, 2, [80, 200])]:
    native = Native(0x1234)
    native.mu.mem_map(ITEMS, 0x800000)
    ascii_raw = (ROOT / 'local/retail/ascii.anm').read_bytes()
    ascii_resource = namespace['install_resource'](native.mu, ascii_raw, 0x6700000, bank_id=11)
    native.put(0x4b43b8, 'I', FONT)
    native.put(FONT + 0x18fb4, 'I', ascii_resource['resource'])
    native.put(0x4b43c8, 'I', BULLETS)
    native.put(0x4b44f0, 'I', ITEMS)
    native.put(0x4b43cc, 'I', BONUS)
    native.put(0x4b4514, 'I', PLAYER)
    native.put(PLAYER + 0x97c, '3f', 0, 400, 0)
    native.put(BULLETS + 0x10, 'I', BULLETS + 0x64)
    native.put(BULLETS + 0x64 + 2000 * 0x9f8 + 0x532, 'H', 5)
    native.put(BULLETS + 0x44, '4f', 0, 224, 0, 384)
    native.put(BULLETS + 0x54, 'f', 448)
    native.put(BULLETS + 0x4debdc, 'I', native.resource['resource'])
    native.put(SHOOTER, 'HH', kind, color)
    native.put(SHOOTER + 4, '3f', *origin, .1)
    native.put(SHOOTER + 0x10, '4f', 0, 0, 0, 0)
    native.put(SHOOTER + 0x1f8, 'HHH', 1, 1, 1)
    native.put(SHOOTER + 0x200, 'Iii', 0x83, 22, 40)
    before = native.rng()
    native.call(0x40a250, (BULLETS, SHOOTER, 0, 0, 0))
    bullet = BULLETS + 0x64
    sprite = native.get(bullet + 0x3fc, 'I')[0] # Embedded VM+3f4, as40d1a1 reads.
    birth = {'position': native.get(bullet + 0x4bc, '3f'),
             'typeColor': native.get(bullet + 0x9f4, 'HH'),
             'sprite': native.get(bullet + 8 + 0x3e4, 'h')[0],
             'spriteWidth': native.get(sprite + 0x38, 'f')[0],
             'rng': native.rng()}
    point_births = []
    def observe(mu, address, size, unused):
        sp = mu.reg_read(UC_X86_REG_ESP)
        point_kind, position = native.get(sp + 4, '2I')
        point_births.append({'type': point_kind, 'position': native.get(position, '3f')})
    native.mu.hook_add(UC_HOOK_CODE, observe, begin=0x4273f0, end=0x4273f0)
    native.call(0x40cf60, (1,))
    clear = {'rng': native.rng(), 'effects': native.nodes(), 'pointBirths': point_births}
    native.tick = 0
    native.call(0x460fe0, registers={UC_X86_REG_EDI: namespace['MANAGER']})
    primary = {'rng': native.rng(), 'effects': native.nodes()}
    cases.append({'type': kind, 'color': color, 'origin': origin, 'mode': 1, 'speed': 0,
                  'flags': 0x83, 'fireSound': 22, 'transformSound': 40,
                  'seed': 0x1234, 'rngBefore': before,
                  'bulletBirth': birth, 'afterClear': clear, 'afterPrimary27': primary})
output = ROOT / 'tests/fixtures/full-clear-valid-colors-native.json'
output.write_text(json.dumps({'schema': 'th12-native-full-clear-valid-colors/1',
    'originalExeSha256': namespace['EXE_SHA256'], 'bulletAnmSha256': hashlib.sha256(namespace['RAW']).hexdigest(),
    'asciiAnmSha256': hashlib.sha256(ascii_raw).hexdigest(), 'gpuExecuted': False,
    'wholeGameOracle': False, 'acceptedReplayGolden': False,
    'boundary': 'Actual40a250 with valid type0/color5, type17/color4, type26/color2 shooters, real454ee0/40d430 ANM remap and original bullet birth update; actual40cf60(1)/4273f0 and subsequent460fe0 Primary27. Zero-speed mode1 single bullets, default shooter flags0x83/fireSound22/transformSound40, original resources and prepared host allocations. Constructor called directly, so emitter wrapper/audio output omitted. No gameplay instruction replacements. Player/Enemy/Item manager updates omitted; no GPU draw.',
    'cases': cases}, indent=2) + '\n', encoding='utf-8')
print(json.dumps({'output': str(output), 'cases': [{'type': c['type'], 'color': c['color'],
    'spriteWidth': c['bulletBirth']['spriteWidth'], 'sprite': c['bulletBirth']['sprite'],
    'tints': [hex(e['primaryColor']) for e in c['afterClear']['effects']],
    'rng': c['afterPrimary27']['rng']} for c in cases]}))
