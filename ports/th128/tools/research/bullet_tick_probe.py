import argparse
import hashlib
import json
import math
from pathlib import Path
import struct

from bullet_probe import NativeBullet, ROOT, RETURN_TRAP, STACK_POINTER, address, datum, emulate


BULLET, SPRITE = 0x390000, 0x3b0000
ANIMATION_BOUNDARY = 0x408d09
FLOAT_LIMIT = 16777216


def report_destination(path):
    destination = Path(path).resolve()
    allowed = ROOT / 'local/research'
    if (allowed.resolve() != allowed or destination.parent != allowed
            or destination.suffix != '.json' or not destination.name.startswith('bullet-tick')
            or destination.drive.lower() == 'e:'):
        raise ValueError('report must be owned private local/research/bullet-tick*.json')
    return destination


def scalar(value, *, motion=False):
    if (type(value) not in (float, int) or abs(value) > FLOAT_LIMIT or not math.isfinite(value)
            or (not motion and 0 < abs(value) < 2**-16)):
        raise ValueError('bounded finite float32 value required')
    return struct.unpack('<f', struct.pack('<f', value))[0]


def frame_input(**changes):
    values = dict(type=8, motionState=2, age=0, lifeClock=0, lifeTime=0,
                  integer0=0, flags=0x13, activeExtensions=0, grace=5, cooldown=0,
                  hasSprite=True, spriteWidth=16, spriteHeight=16, clock=1,
                  extensionListEmpty=True, position=(-128, -32, .1), velocity=(.25, .75, 0))
    if set(changes) - set(values):
        raise ValueError('unknown tick input')
    values.update(changes)
    for name in ('type', 'motionState', 'age', 'lifeClock', 'integer0', 'flags',
                 'activeExtensions', 'grace', 'cooldown'):
        if type(values[name]) is not int:
            raise ValueError('exact integer tick field required')
    if (values['type'] != 8 or values['motionState'] not in (1, 2, 3)
            or not 0 <= values['age'] <= 4096 or not 0 <= values['lifeClock'] <= 4096
            or not -2**31 <= values['integer0'] < 2**31
            or any(not 0 <= values[name] < 2**32 for name in ('flags', 'activeExtensions'))
            or any(not -4096 <= values[name] <= 4096 for name in ('grace', 'cooldown'))):
        raise ValueError('tick integer field outside the supported first-bullet budget')
    for name in ('hasSprite', 'extensionListEmpty'):
        if type(values[name]) is not bool:
            raise ValueError('exact boolean tick field required')
    for name in ('position', 'velocity'):
        vector = values[name]
        if not isinstance(vector, (tuple, list)) or len(vector) != 3:
            raise ValueError('exact three-component position/velocity required')
        values[name] = [scalar(value,motion=True) for value in vector]
        values[name + 'Bits'] = list(struct.unpack('<3I', struct.pack('<3f', *values[name])))
    for name in ('lifeTime', 'spriteWidth', 'spriteHeight', 'clock'):
        values[name] = scalar(values[name])
    if (values['clock'] != 1 or values['lifeTime'] < 0
            or values['spriteWidth'] < 0 or values['spriteHeight'] < 0):
        raise ValueError('only clock1 with nonnegative timer/sprite dimensions is supported')
    return values


def semantic(block, original):
    result = dict(original)
    for name, location, kind in (('flags', 0, 'I'), ('cooldown', 4, 'i'), ('integer0', 0x438, 'i'),
                                ('grace', 0xa18, 'i'), ('activeExtensions', 0xa20, 'I'),
                                ('motionState', 0xa2a, 'H'), ('age', 0x1188, 'i'),
                                ('lifeClock', 0x119c, 'i'), ('lifeTime', 0x11a0, 'f')):
        result[name] = struct.unpack_from('<' + kind, block, location)[0]
    result['hasSprite'] = struct.unpack_from('<I', block, 0x430)[0] != 0
    for name, location in (('position', 0x9d8), ('velocity', 0x9e4)):
        result[name] = list(struct.unpack_from('<3f', block, location))
        result[name + 'Bits'] = list(struct.unpack_from('<3I', block, location))
    return result


class NativeBulletTick(NativeBullet):
    def tick(self, *, collision_misses=0, measure_helper_return=False, collision_environment='miss', after_hit=False, **changes):
        if (type(collision_misses) is not int or not 0 <= collision_misses <= 2
                or type(measure_helper_return) is not bool
                or collision_environment not in ('miss', 'suppressed')):
            raise ValueError('bounded actual-helper execution options required')
        values = frame_input(**changes)
        if type(after_hit) is not bool or (after_hit and (collision_misses or measure_helper_return or values['motionState']!=3)):
            raise ValueError('independent after-hit suffix requires explicit hit state and no fabricated prior calls')
        block = bytearray(0x11b8)
        struct.pack_into('<Ii', block, 0, values['flags'], values['cooldown'])
        struct.pack_into('<I', block, 0x430, SPRITE if values['hasSprite'] else 0)
        struct.pack_into('<i', block, 0x438, values['integer0'])
        struct.pack_into('<3f', block, 0x9d8, *values['position'])
        struct.pack_into('<3f', block, 0x9e4, *values['velocity'])
        struct.pack_into('<2f', block, 0x9f8, 2.4, 2.4)
        struct.pack_into('<iI', block, 0xa18, values['grace'], 0)
        struct.pack_into('<I', block, 0xa20, values['activeExtensions'])
        struct.pack_into('<H', block, 0xa2a, values['motionState'])
        struct.pack_into('<i', block, 0xa3c, 1)
        if not values['extensionListEmpty']:
            struct.pack_into('<I', block, 0xa64 + 0x2c, 1)
        struct.pack_into('<f', block, 0x1180, 1)
        struct.pack_into('<i', block, 0x1188, values['age'])
        struct.pack_into('<ifi', block, 0x119c, values['lifeClock'], values['lifeTime'], 0x4b6f58)
        if after_hit:struct.pack_into('<i',block,0x1198,values['lifeClock']-1)
        struct.pack_into('<h', block, 0x11b0, 8)
        if semantic(bytes(block), values) != values:
            raise ValueError('declared tick input does not match original-memory field layout')
        sprite = bytearray(0x48)
        struct.pack_into('<2f', sprite, 0x34, values['spriteHeight'], values['spriteWidth'])
        spans = [(0x408640, ANIMATION_BOUNDARY), (0x408560, 0x4085ca), (0x407fb0, 0x408055)]
        data = [datum('bullet', BULLET, bytes(block)), datum('sprite', SPRITE, bytes(sprite), writable=False),
                datum('clock', 0x4b6f58, struct.pack('<f', values['clock']), writable=False)]
        for start, size in ((0x408d38, 20), (0x4a6588, 8), (0x4a65a8, 8), (0x4a6694, 12),
                            (0x4a6600, 8), (0x4a6608, 8), (0x4a65d8, 8), (0x4a6658, 8)):
            data.append(self.constant(start, size))
        if values['activeExtensions'] == 0 and values['extensionListEmpty']:
            spans.extend(((0x409a90, 0x409aef), (0x40a843, 0x40a85e), (0x4728fd, 0x472907)))
            for start, size in ((0x4b01f4, 4), (0x4a6718, 8), (0x4a6ad0, 8)):
                data.append(self.constant(start, size))
        stops = [ANIMATION_BOUNDARY]
        collision_inputs = None
        if collision_misses:
            from bullet_collision_probe import NativeCollision
            player = (1000, 1000) if collision_environment == 'miss' else (0, 0)
            bounds = (998, 998, 1002, 1002) if collision_environment == 'miss' else (-2, -2, 2, 2)
            collision_data, collision_inputs, _, _ = NativeCollision.state(
                self, values['position'][:2], (2.4, 2.4), player_position=player, bounds=bounds, counter=1,
                state=2 if collision_environment == 'suppressed' else 0)
            collision_data += NativeCollision.constants_data(self)
            existing = {region['address']: region for region in data}
            for region in collision_data:
                if region['address'] not in existing:
                    data.append(region)
                    existing[region['address']] = region
                elif region['bytes'] != existing[region['address']]['bytes']:
                    raise ValueError('conflicting collision probe constant')
            spans += [(0x43c410, 0x43c56e), (0x43c570, 0x43c658)]
            if measure_helper_return:
                stops.append(0x408814 if values['motionState'] == 2 and collision_misses == 1 else 0x408aa4)
            elif collision_misses == 1 and values['motionState'] == 2:
                stops += [0x408a44, 0x408a80, 0x408a9f]
        chunks = [self.chunk(start, end) for start, end in spans]
        registers={'ebx':BULLET}
        stack=None
        entry=0x408640
        if after_hit:
            entry=0x408c93
            registers.update(ebp=STACK_POINTER+0x28)
            stack={0:struct.pack('<2I',0,0),0x28:struct.pack('<2I',0,RETURN_TRAP)}
        result = emulate(chunks, entry, data, registers=registers,
                         stops=stops, budget=2048,stack=stack)
        if after_hit:result['independentAfterHitSuffix']=True
        final = bytes.fromhex(result['memory']['bullet']['after'])
        stop = result['stop']
        if measure_helper_return:
            if stop['reason'] != 'boundary' or int(stop['address'], 16) not in (0x408814, 0x408aa4):
                raise ValueError(f'actual helper did not reach its return measurement boundary: {stop}')
            collision_inputs['centerBits']=list(struct.unpack_from('<2I',final,0x9d8))
            result.update(helperReturn=result['registers']['eax'], collisionInput=collision_inputs,
                          completedTick=False, declaredCode=[f'{start:#x}:{end:#x}' for start, end in spans])
            result.pop('memory')
            return result
        if stop['reason'] == 'boundary' and int(stop['address'], 16) == ANIMATION_BOUNDARY:
            continuation, boundary = 'NeedsAnimation', ANIMATION_BOUNDARY
        elif stop['reason'] == 'returned' and result['registers']['eax'] == 0xffffffff:
            continuation, boundary = 'Removed', 0x4086c6
        elif stop['reason'] == 'boundary' and int(stop['address'], 16) in (0x408a44, 0x408a80, 0x408a9f):
            continuation, boundary = 'NeedsCollision', int(stop['address'], 16)
        elif stop['reason'] == 'unsupported_call':
            if stop['target'] in ('0x0043c410', '0x0043c570'):
                continuation = 'NeedsCollision'
            elif stop['target'] in ('0x0040ccf0', '0x00409a90', '0x0040b6b0'):
                continuation = 'NeedsExtension'
            else:
                raise ValueError(f'unknown original call is outside supported prefix classification: {stop}')
            boundary = int(stop['address'], 16)
        else:
            raise ValueError(f'unexpected original-byte prefix stop: {stop}')
        result.update(input=values, next=semantic(final, values), continuation=continuation, boundary=boundary,
                      previousLifeClock=struct.unpack_from('<i', final, 0x1198)[0], completedTick=False,
                      declaredCode=[f'{start:#x}:{end:#x}' for start, end in spans],
                      bulletBeforeSha256=hashlib.sha256(block).hexdigest(),
                      bulletAfterSha256=hashlib.sha256(final).hexdigest())
        result.pop('memory')
        return result


def probe_hit_suffix(executable):
    native=NativeBulletTick(executable)
    cases=[]
    try:
        for name,changes in (('half',{}),('full',{'motionState':1}),('cooldown',{'cooldown':3}),
                ('bounds',{'position':(400,400,.1),'grace':0}),('no-sprite',{'position':(400,400,.1),'grace':0,'hasSprite':False}),
                ('negative-cooldown',{'cooldown':-2})):
            pending=native.tick(age=8,**changes)
            if pending['continuation']!='NeedsCollision':raise ValueError('expected actual pending collision prefix')
            state={key:value for key,value in pending['next'].items() if not key.endswith('Bits')}
            state['motionState']=3
            expected=native.tick(after_hit=True,**state)
            cases.append({'id':name,'pending':pending,'expected':expected})
        after=hashlib.sha256(native.executable.read_bytes()).hexdigest()
        return {'schema':'th128-original-byte-bullet-hit-suffix/1','nativeCpuTrace':False,'windowsGameProcessLaunched':False,
                'executable':{'sha256':hashlib.sha256(native.binary).hexdigest(),'sha256After':after,'unchanged':after==hashlib.sha256(native.binary).hexdigest()},
                'caseCount':len(cases),'cases':cases,'code':native.code,'constants':native.constants,
                'limitations':['Independent real tail entries with explicit hit state and synthetic caller stack.',
                               'No collision, IRQ, cancellation-effect or player-hit return is fabricated or executed by these suffixes.',
                               'Caller must complete every required hit-side effect before using this continuation.']}
    finally:native.image.close()


def probe_resume(executable):
    native = NativeBulletTick(executable)
    cases = []
    specifications = [
        ('half-circle', {}, 1), ('half-rectangle', {'flags': 3}, 1),
        ('half-scaled-circle', {'flags': 0x53}, 1), ('half-scaled-rectangle', {'flags': 0x43}, 1),
        ('full-circle', {'motionState': 1}, 1), ('full-rectangle', {'motionState': 1, 'flags': 3}, 1),
        ('full-scaled-rectangle', {'motionState': 1, 'flags': 0x43}, 1),
        ('pending-counter-edge', {'lifeClock': 4096}, 1),
        ('transition-circle', {'integer0': 1}, 2), ('transition-rectangle', {'integer0': 1, 'flags': 3}, 2),
        ('miss-to-bounds-remove', {'position': (300, 500, 0), 'grace': 0}, 1),
        ('miss-to-extension', {'activeExtensions': 0x1000}, 1),
        ('miss-transition-unknown-extension', {'integer0': 1, 'extensionListEmpty': False}, 1),
        ('suppressed-circle', {'position': (0, 0, 0), 'velocity': (0, 0, 0)}, 1),
        ('suppressed-rectangle', {'flags': 3, 'position': (0, 0, 0), 'velocity': (0, 0, 0)}, 1)]
    try:
        for name, changes, count in specifications:
            changes = {'age': 8, 'lifeClock': 8, 'lifeTime': 8.25, 'cooldown': 3, **changes}
            pending = native.tick(**changes)
            environment = 'suppressed' if name.startswith('suppressed') else 'miss'
            for miss in range(1, count + 1):
                if pending['continuation'] != 'NeedsCollision':
                    raise ValueError('collision resume golden must start at a real collision call')
                measured = native.tick(collision_misses=miss, measure_helper_return=True,
                                       collision_environment=environment, **changes)
                if measured['helperReturn'] != 0:
                    raise ValueError(f'{name} actual collision helper returned {measured["helperReturn"]}, not no-effect zero')
                expected = native.tick(collision_misses=miss, collision_environment=environment, **changes)
                cases.append({'id': f'{name}-resume{miss}', 'pending': pending, 'expected': expected,
                              'helperMeasurement': measured, 'completedTick': False})
                pending = expected
        after = hashlib.sha256(native.executable.read_bytes()).hexdigest()
        return {'schema': 'th128-original-byte-bullet-tick-resume-fixtures/1',
                'nativeCpuTrace': False, 'windowsGameProcessLaunched': False,
                'executable': {'sha256': hashlib.sha256(native.binary).hexdigest(), 'sha256After': after,
                               'unchanged': native.binary == native.executable.read_bytes()},
                'code': native.code, 'constants': native.constants, 'cases': cases, 'caseCount': len(cases),
                'limitations': ['Actual collision helper bytes execute using the collision worker probe data builder.',
                                'Only measured zero helper returns are resumed; no return stub or register override is used.',
                                'Damage, hit effects, graze side effects, ANM and complete bullet ticks remain unexecuted.']}
    finally:
        native.image.close()


def probe(executable):
    native = NativeBulletTick(executable)
    cases = []

    def run(name, **changes):
        if len(cases) >= 256:
            raise ValueError('tick case budget exceeded')
        result = native.tick(**changes)
        result['id'] = name
        cases.append(result)

    try:
        for age in range(10):
            run(f'spawn-age{age}', age=age, lifeClock=age, lifeTime=age)
        for age in range(8):
            run(f'first-eight-step{age}', age=age, lifeClock=age + 3, lifeTime=age + .25,
                position=(-128 + age * .125, -32 + age * .375, .1))
        for grace in (-2, -1, 0, 1, 5):
            for cooldown in (-2, 0, 1, 5):
                run(f'timers-{grace}-{cooldown}', grace=grace, cooldown=cooldown, position=(0, 0, 0))
        for index, position in enumerate(((-202, 0, 0), (-201.99998474121094, 0, 0),
                (202, 0, 0), (201.99998474121094, 0, 0), (0, -84, 0), (0, -83.99999237060547, 0),
                (0, 468, 0), (0, 467.9999694824219, 0), (0, 0, 1000))):
            run(f'bounds-edge{index}', grace=0, cooldown=3, position=position, velocity=(0, 0, 0),
                spriteWidth=20, spriteHeight=40)
        for grace in (0, 1):
            run(f'bounds-grace{grace}', grace=grace, position=(1000, 1000, 0))
            run(f'bounds-no-sprite{grace}', grace=grace, position=(1000, 1000, 0), hasSprite=False)
            run(f'bounds-skip-flag{grace}', grace=grace, position=(1000, 1000, 0), activeExtensions=0x100)
        for flags in (0x1b, 8):
            run(f'early-remove-{flags}', flags=flags, age=7, lifeClock=12, lifeTime=12.75, cooldown=4)
        for integer in (1, -1):
            for flags in (0x11, 0x13):
                run(f'transition-{integer}-{flags}', integer0=integer, flags=flags,
                    velocity=(1.0000001192092896, -.75, .125), lifeClock=4, lifeTime=4.5)
        run('transition-two-rounded-moves', integer0=1, flags=0x11,
            position=(16777216, -16777216, 0), velocity=(1, -1, -.125), hasSprite=False)
        run('transition-unknown-empty-list', integer0=1, flags=0x11, extensionListEmpty=False)
        run('transition-active-extension', integer0=1, activeExtensions=1)
        run('normal-full-step', motionState=1, flags=0x11)
        run('normal-needs-collision', motionState=1, flags=0x13)
        run('normal-needs-extension', motionState=1, activeExtensions=1)
        run('age8-collision-disabled', age=8, flags=0x11)
        run('age8-rectangle-call', age=8, flags=3)
        run('age8-scaled-circle-call', age=8, flags=0x53)
        run('early-extension-scale', activeExtensions=0x400000)
        run('late-extension', activeExtensions=0x1000)
        run('late-extension-without-sprite', activeExtensions=0x1000, hasSprite=False)
        run('age8-collision-before-transition', age=8, integer0=1)
        run('signed-zero-motion', position=(-0.0, 0.0, -0.0), velocity=(-0.0, -0.0, 0.0))
        smallest=struct.unpack('<f',struct.pack('<I',1))[0]
        run('tiny-vertical-velocity',position=(0,359,.1),velocity=(-4.371138828673793e-8,1,0))
        run('motion-cancellation-residue',position=(-126,2**-22,.1),velocity=(.3,.9,0))
        run('smallest-subnormal-half-step',position=(0,0,.1),velocity=(smallest,-smallest,0))
        run('smallest-subnormal-full-step',position=(smallest,-smallest,.1),velocity=(smallest,-smallest,0),motionState=1,flags=0x11)
        run('tiny-motion-zero-cross',position=(1e-8,-1e-8,.1),velocity=(-2e-8,2e-8,0))
        for age in (0,8,25):
            run(f'hit-state-half-motion-{age}',motionState=3,age=age,lifeClock=age,lifeTime=age)
        run('hit-state-negative-motion',motionState=3,position=(0,0,.1),velocity=(-.5,-1,0))
        run('hit-state-bounds-retire',motionState=3,position=(400,400,.1),velocity=(0,0,0),grace=0)
        run('hit-state-sprite-absent',motionState=3,hasSprite=False,position=(400,400,.1),velocity=(0,0,0),grace=0)
        for start, end in ((0x408241, 0x408258), (0x4153ae, 0x4153c5), (0x4091a0, 0x4091c8),
                           (0x408d50, 0x408e69), (0x408d09, 0x408d35), (0x4592b5, 0x4592c1)):
            native.chunk(start, end)
        after = hashlib.sha256(native.executable.read_bytes()).hexdigest()
        return {'schema': 'th128-original-byte-bullet-tick-fixtures/1',
                'nativeCpuTrace': False, 'windowsGameProcessLaunched': False,
                'executable': {'sha256': hashlib.sha256(native.binary).hexdigest(), 'sha256After': after,
                               'unchanged': native.binary == native.executable.read_bytes()},
                'scheduler': {'bulletPriority': 24, 'enemyPriority': 21,
                              'registrationPriorityLoad': '0x00408241', 'registrationCall': '0x00408253',
                              'callback': '0x004091a0', 'managerCall': '0x004091c1',
                              'manager': '0x00408d50', 'bodyCall': '0x00408dbf', 'slotCount': 2000,
                              'ascendingStride': 0x11b8, 'ageIncrement': '0x00408e22',
                              'ageUpdateOnlyAfterBodySuccessWhenBodyRuns': True,
                              'bodySkippedWhenControlBit400': True},
                'code': native.code, 'constants': native.constants, 'cases': cases, 'caseCount': len(cases),
                'limitations': ['Original-byte bounded Unicorn emulation, not native Windows CPU execution.',
                                'No original game process, collision helper, nonempty ETEX dispatcher or ANM tick is executed.',
                                'Empty extension sentinel and real security-cookie check are executed only when explicitly declared.',
                                'Manager scheduling/priority/age order is static-byte evidence, not a 2000-bullet execution trace.',
                                'Only type8 states1/2/3, bounded finite inputs, clock1 and declared sprite metadata are supported.']}
    finally:
        native.image.close()


def main(argv=None):
    parser = argparse.ArgumentParser(description='Bounded original-byte TH128 pre-animation bullet update')
    parser.add_argument('--exe', required=True)
    parser.add_argument('--output', default=str(ROOT / 'local/research/bullet-tick-fixtures.json'))
    parser.add_argument('--resume-output', default=str(ROOT / 'local/research/bullet-tick-resume-fixtures.json'))
    parser.add_argument('--hit-output',default=str(ROOT/'local/research/bullet-tick-hit-suffix-fixtures.json'))
    arguments = parser.parse_args(argv)
    destination = report_destination(arguments.output)
    resume_destination = report_destination(arguments.resume_output)
    hit_destination=report_destination(arguments.hit_output)
    if len({resume_destination,destination,hit_destination})!=3:
        raise ValueError('prefix, resume and hit reports require separate output paths')
    report = probe(arguments.exe)
    if not report['executable']['unchanged']:
        raise ValueError('read-only executable digest changed')
    resume = probe_resume(arguments.exe)
    if not resume['executable']['unchanged']:
        raise ValueError('read-only executable digest changed')
    hit=probe_hit_suffix(arguments.exe)
    if not hit['executable']['unchanged']:raise ValueError('read-only executable digest changed')
    destination.parent.mkdir(parents=True, exist_ok=True)
    destination.write_text(json.dumps(report, ensure_ascii=False, indent=2) + '\n', encoding='utf8')
    resume_destination.write_text(json.dumps(resume, ensure_ascii=False, indent=2) + '\n', encoding='utf8')
    hit_destination.write_text(json.dumps(hit,ensure_ascii=False,indent=2)+'\n',encoding='utf8')
    print(f'{report["caseCount"]} original-byte prefixes; no collision/ANM return or complete bullet tick invented')
    print(f'{resume["caseCount"]} resumes after actual measured collision-helper zero returns; no stubs')


if __name__ == '__main__':
    main()
