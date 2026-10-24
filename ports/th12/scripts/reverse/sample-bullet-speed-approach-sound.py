"""Actual20000000 shared453d90 sound-call gates, with audio as output boundary."""
import json, pathlib
ROOT = pathlib.Path(__file__).resolve().parents[2]
HOST = ROOT / 'scripts/reverse/sample-bullet-speed-approach.py'
exec(compile(HOST.read_text(encoding='utf-8').split('native = ApproachNative()')[0], str(HOST), 'exec'))


class SoundNative(ApproachNative):
    def __init__(self):
        self.sound_calls = []
        super().__init__()

    def hook(self, mu, address, size, unused):
        if address == 0x453d90:
            stack = mu.reg_read(UC_X86_REG_ESP)
            self.sound_calls.append({'entry': hex(address),
                'caller': hex(self.get(stack, 'I')[0]),
                'soundId': mu.reg_read(UC_X86_REG_EDX)})
        super().hook(mu, address, size, unused)


native = SoundNative()
cases = []
for name, prefix, sound, duration in [
    ('first-record-0-skips', 0, 27, 2),
    ('later-record-1-plays', 1, 27, 2),
    ('negative-sound-skips', 1, -1, 2),
    ('sound-zero-plays', 1, 0, 2),
    ('duration-zero-still-plays', 1, 27, 0),
    ('duration-negative-still-plays', 1, 27, -1),
]:
    commands = ([record(0x80000000)] if prefix else []) + [record(0x20000000, duration, .06)]
    native.seed(commands, speed=1)
    native.put(BULLET + 0x544, 'i', sound)
    native.sound_calls.clear()
    native.context = name
    native.call(0x40aa60, registers={UC_X86_REG_ECX: BULLET})
    cases.append({'name': name, 'prefixRecords': prefix, 'transformSound': sound,
                  'duration': duration, 'records': commands,
                  'active': native.get(BULLET + 0x528, 'I')[0],
                  'cursor': native.get(BULLET + 0x548, 'i')[0],
                  'soundCalls': native.sound_calls.copy()})
output = ROOT / 'tests/fixtures/bullet-speed-approach-sound-original.json'
output.write_text(json.dumps({'schema': 'th12-original-speed-approach-sound/1',
    'originalExeSha256': EXE_SHA256, 'wholeGameOracle': False, 'gpuExecuted': False,
    'scope': 'Actual40aa60→40b468→40abc9→40abd1/40abdd→453d90. Prepared Bullet slot and transformSound; actual sound entry/EDX/caller recorded, audio output boundary then returns without playing device sound. No dispatch/angle/delta/timer instruction is intercepted. No difficulty is read by this branch.',
    'cases': cases}, indent=2) + '\n', encoding='utf-8')
print(json.dumps({'output': str(output), 'cases': [
    {'name': c['name'], 'soundCalls': c['soundCalls']} for c in cases]}))
