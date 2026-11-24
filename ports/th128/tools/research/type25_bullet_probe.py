import argparse
import hashlib
import json
import struct
from pathlib import Path

from bullet_probe import NativeBullet, ROOT, EXE_SHA256, datum, emulate, STACK_POINTER
from bullet_lifecycle_probe import bullet_state
from bullet_tick_probe import frame_input, semantic, ANIMATION_BOUNDARY, BULLET, SPRITE

EMITTER = 0x370000
VM = 0x3d0000
OWNER = 0x3e0000
DESTINATION = ROOT / 'local/research/type25-bullet-fixtures.json'
PROFILE = 0x4b40f0


def disassemble(native, start, end):
    import capstone
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    return [['0x%08x' % item.address, item.mnemonic, item.op_str]
            for item in decoder.disasm(native.read(start, end - start), start)]


class NativeType25(NativeBullet):
    def callback(self, color, source):
        vm = bytearray(0x500)
        owner = bytearray(0x11b8)
        struct.pack_into('<I', vm, 0x4e4, OWNER)
        struct.pack_into('<h', owner, 0x11b0, 25)
        struct.pack_into('<h', owner, 0x11b2, color)
        result = emulate([self.chunk(0x40ccb0, 0x40cce8)], 0x40ccb0,
                         [datum('vm', VM, bytes(vm)), datum('owner', OWNER, bytes(owner)),
                          self.constant(PROFILE, 0x120)],
                         registers={'ecx': VM, 'edx': source}, budget=64)
        return {'type': 25, 'color': color, 'source': source,
                'eax': result['registers']['eax'], 'stop': result['stop']}

    def spawn(self, color, source=0):
        emitter = bytearray(0x380)
        struct.pack_into('<2i', emitter, 0, 25, color)
        struct.pack_into('<i', emitter, 0x38, 1)
        struct.pack_into('<i', emitter, 0x48, 2)
        struct.pack_into('<I', emitter, 0x36c, 0x13)
        struct.pack_into('<i', emitter, 0x374, 40)
        image = bytearray(0x11b8)
        struct.pack_into('<I', image, 0, 0x13)
        struct.pack_into('<H', image, 0xa2a, 1)
        struct.pack_into('<h', image, 0x11b0, 25)
        struct.pack_into('<3f', image, 0x9d8, -128, 100, .1)
        struct.pack_into('<3f', image, 0x9e4, .5, 1.5, 0)
        result = emulate([self.chunk(0x4097d0, 0x4099df)], 0x4097d0,
                         [datum('emitter', EMITTER, bytes(emitter), writable=False),
                          datum('bullet', BULLET, bytes(image)),
                          self.constant(PROFILE, 0x120), self.constant(0x409a5c, 40),
                          self.constant(0x4a6680, 8)],
                         registers={'ebx': BULLET, 'edi': EMITTER, 'ebp': STACK_POINTER + 0x100},
                         stack={0x10c: struct.pack('<I', EMITTER)}, stops=[0x4099df], budget=2048)
        if result['stop']['reason'] != 'boundary':
            raise ValueError(result['stop'])
        after = bytes.fromhex(result['memory']['bullet']['after'])
        state = bullet_state(after)
        state['cancellationScript'] = struct.unpack_from('<i', after, 0xa1c)[0]
        state['profileValue'] = struct.unpack_from('<i', after, 0xa40)[0]
        state['profilePointer'] = '0x%08x' % struct.unpack_from('<I', after, 0x11ac)[0]
        state['type'] = struct.unpack_from('<h', after, 0x11b0)[0]
        state['secondaryEnabled'] = struct.unpack_from('<I', after, 0x9a0)[0] & 1
        return {'input': {'type': 25, 'color': color}, 'state': state, 'completedConstructor': False}

    def tick(self, **changes):
        values = frame_input(**changes)
        values['type'] = 25
        image = bytearray(0x11b8)
        struct.pack_into('<Ii', image, 0, values['flags'], values['cooldown'])
        struct.pack_into('<I', image, 0x430, SPRITE if values['hasSprite'] else 0)
        struct.pack_into('<i', image, 0x438, values['integer0'])
        struct.pack_into('<3f', image, 0x9d8, *values['position'])
        struct.pack_into('<3f', image, 0x9e4, *values['velocity'])
        struct.pack_into('<2f', image, 0x9f8, 6., 6.)
        struct.pack_into('<i', image, 0xa18, values['grace'])
        struct.pack_into('<I', image, 0xa20, values['activeExtensions'])
        struct.pack_into('<H', image, 0xa2a, values['motionState'])
        struct.pack_into('<i', image, 0xa3c, 1)
        struct.pack_into('<f', image, 0x1180, 1)
        struct.pack_into('<i', image, 0x1188, values['age'])
        struct.pack_into('<ifi', image, 0x119c, values['lifeClock'], values['lifeTime'], 0x4b6f58)
        struct.pack_into('<h', image, 0x11b0, 25)
        sprite = bytearray(0x48)
        struct.pack_into('<2f', sprite, 0x34, values['spriteHeight'], values['spriteWidth'])
        spans = ((0x408640, ANIMATION_BOUNDARY), (0x408560, 0x4085ca), (0x407fb0, 0x408055),
                 (0x409a90, 0x409aef), (0x40a843, 0x40a85e), (0x4728fd, 0x472907))
        constants = ((0x408d38, 20), (0x4a6588, 8), (0x4a65a8, 8), (0x4a6694, 12), (0x4a6600, 8),
                     (0x4a6608, 8), (0x4a65d8, 8), (0x4a6658, 8), (0x4b01f4, 4), (0x4a6718, 8), (0x4a6ad0, 8))
        result = emulate([self.chunk(start, end) for start, end in spans], 0x408640,
                         [datum('bullet', BULLET, bytes(image)), datum('sprite', SPRITE, bytes(sprite), writable=False),
                          datum('clock', 0x4b6f58, struct.pack('<f', 1), writable=False),
                          *[self.constant(start, size) for start, size in constants]],
                         registers={'ebx': BULLET}, stops=[ANIMATION_BOUNDARY], budget=2048)
        stop = result['stop']
        if stop['reason'] == 'boundary':
            continuation, boundary = 'NeedsAnimation', ANIMATION_BOUNDARY
        elif stop['reason'] == 'returned':
            continuation, boundary = 'Removed', 0x4086c6
        elif stop.get('target') in ('0x0043c410', '0x0043c570'):
            continuation, boundary = 'NeedsCollision', int(stop['address'], 16)
        else:
            raise ValueError(stop)
        after = bytes.fromhex(result['memory']['bullet']['after'])
        return {'input': values, 'next': semantic(after, values), 'continuation': continuation,
                'boundary': boundary, 'completedTick': False}


def probe(executable):
    native = NativeType25(executable)
    try:
        if hashlib.sha256(native.executable.read_bytes()).hexdigest() != EXE_SHA256:
            raise ValueError('type25 probe requires the pinned Japanese executable')
        payload = native.read(PROFILE, 0x120)
        words = list(struct.unpack_from('<72i', payload, 0))
        script = words[0]
        secondary = words[0x11c // 4]
        hitbox = struct.unpack_from('<f', payload, 0x104)[0]
        auxiliary = struct.unpack_from('<f', payload, 0x108)[0]
        if (script, secondary, hitbox, auxiliary) != (102, 103, 6.0, 22.0):
            raise ValueError('unexpected type25 profile identity')
        callback_cases = [native.callback(0, source) for source in (0, 1, 457, 588)]
        spawn_cases = [native.spawn(color) for color in (0, 1, 15)]
        tick_cases = [native.tick(**changes) for changes in
                      ({}, {'age': 7}, {'age': 8}, {'motionState': 1}, {'motionState': 3})]
        return {
            'schema': 'th128-original-type25-bullet/1',
            'executable': {'sha256': EXE_SHA256,
                           'sha256After': hashlib.sha256(native.executable.read_bytes()).hexdigest()},
            'profile': {
                'address': '0x004b40f0', 'bytes': len(payload), 'hex': payload.hex(),
                'sha256': hashlib.sha256(payload).hexdigest(), 'script': script,
                'secondaryScript': secondary, 'hitboxSize': hitbox, 'auxiliarySize': auxiliary,
                'appearance': words[0x10c // 4], 'selector': words[0x110 // 4],
                'freezePolicy': words[0x118 // 4], 'word114': words[0x114 // 4],
                'identitySpriteRemap': words[1] < 0,
                'colorSlots': [[words[1 + color * 4 + sprite] for sprite in range(4)] for color in range(16)],
                'cancellationWords': [words[4 + color * 4] for color in range(16)]},
            'code': {
                'dedicatedSpawn': disassemble(native, 0x40a990, 0x40aa72),
                'genericSpawnProfile': disassemble(native, 0x4097d0, 0x4097f4)
                                      + disassemble(native, 0x40988c, 0x4098f3),
                'genericSpawnCancellation': disassemble(native, 0x409873, 0x409889),
                'callback': disassemble(native, 0x40ccb0, 0x40cce8),
                'updateSecondaryOrder': disassemble(native, 0x408d09, 0x408d31),
                'freezePolicy': disassemble(native, 0x40eb04, 0x40eb3b)},
            'profileModeTable': [hex(struct.unpack_from('<I', native.read(0x409a5c + 4 * index, 4))[0])
                                 for index in range(10)],
            'callbackCases': callback_cases,
            'spawnCases': spawn_cases,
            'tickCases': tick_cases,
            'conclusions': {
                'spawn': 'generic ECL spawn reads profile 0x4b40f0 and stores type25 script102, hitbox6, auxiliary22, appearance2 and cancellation371',
                'callback': 'callback returns the source sprite unchanged for type25 because profile word1 is negative',
                'secondary': 'generic spawn exposes secondary script 103 through the profile; the common update ticks primary then secondary only while primary has not ended',
                'freeze': 'profile +0x118 is 1, and the native freeze site rejects policy 1 before any age check; appearance +0x10c is not read there',
                'word114': 'profile word at +0x114 is 60 but no consumer or lifetime naming is proven in this scope'},
            'windowsGameProcessLaunched': False, 'nativeCpuTrace': False,
            'generatorSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    finally:
        native.image.close()


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', required=True)
    arguments = parser.parse_args()
    report = probe(arguments.exe)
    DESTINATION.write_text(json.dumps(report, indent=1) + '\n', encoding='utf-8', newline='\n')
    print('recorded type25 profile/callback/spawn/tick and %d callback cases' % len(report['callbackCases']))
