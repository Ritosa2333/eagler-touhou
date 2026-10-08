"""Original Bomb constructors, handles, registered Camera14 and ANM27.

Bootstraps the existing original selected-manager provider without running its
replay loop, then runs two isolated Bombs per real loadout. No Enemy/Player
updates or accepted replay golden are generated. Original callbacks/ANM/RNG and
native allocator/resource/CRT host boundaries remain those of the provider.
"""
import argparse, json, pathlib, runpy, sys
ROOT = pathlib.Path(__file__).resolve().parents[2]
parser = argparse.ArgumentParser()
parser.add_argument('--case', choices=['sanae-b', 'marisa-a', 'all'], default='all')
parser.add_argument('--draw-reset', action='store_true')
parser.add_argument('--visual-fields', action='store_true')
parser.add_argument('--audit-other-loadouts', action='store_true')
parser.add_argument('--mesh-enabled', action='store_true')
parser.add_argument('--tracked-main', action='store_true')
arguments = parser.parse_args()
records = []
names = ['reimu-a', 'reimu-b', 'marisa-b', 'sanae-a'] if arguments.audit_other_loadouts else ['sanae-b', 'marisa-a']
for name in names:
    if not arguments.audit_other_loadouts and arguments.case not in [name, 'all']: continue
    fixture_path = ROOT / f'artifacts/reverse/enemy-input-bomb-lifecycle-{name}.json'
    if arguments.audit_other_loadouts:
        controlled = json.loads((ROOT / 'artifacts/reverse/enemy-input-bomb-lifecycle-sanae-b.json').read_text())
        controlled['character'], controlled['shot'] = {'reimu-a':(0,0),'reimu-b':(0,1),'marisa-b':(1,1),'sanae-a':(2,0)}[name]
        controlled['scope'] = 'Controlled synthetic character/shot header for original Bomb constructor; no replay-input execution or accepted replay claim'
        fixture_path.write_text(json.dumps(controlled),encoding='utf-8')
    original_argv = sys.argv
    sys.argv = [str(ROOT / 'scripts/reverse/native-enemy-provider.py'), '--frames', '0',
        '--native-player', '--native-items', '--native-bullets', '--native-lasers',
        '--active-collision', '--capture-text-output', '--omit-animation-capture',
        '--instruction-budget', '20000000', '--input', str(fixture_path),
        '--output', str(ROOT / f'artifacts/reverse/bomb-bootstrap-{name}.json')]
    try: native = runpy.run_path(sys.argv[0])
    finally: sys.argv = original_argv
    mu, put, get, call = (native[key] for key in ['v', 'put', 'get', 'call'])
    from unicorn import UC_HOOK_CODE
    from unicorn.x86_const import UC_X86_REG_ESP, UC_X86_REG_ESI, UC_X86_REG_ECX, UC_X86_REG_EAX, UC_X86_REG_EDI, UC_X86_REG_EDX
    PLAYER, BOMB, ANM, SCHEDULER = native['PLAYER'], native['bomb_manager'], 0x5000000, get(0x4ce89c, 'I')[0]
    # Explicit unpaused application context. The full GameScene constructor is
    # outside this local fixture; actual4527f0 requires a nonnull flags holder.
    put(0x4b44e8, 'I', 0x20d0000)
    put(0x20d0060, 'I', 0)
    put(0x4ce55c, 'I', 0)
    if arguments.mesh_enabled:
        # Original40ff10's CPU mesh allocation gate, needed by Reimu A's
        # damage-free backdrop/RNG path. No GPU submission is executed.
        put(0x4cea94, 'I', 1)
    put(PLAYER + 0x97c, '3f', 20., 300., 0.)
    put(0x4ce568, 'II', 42, 0)
    events, phase, frame = [], 'startup', -1
    resource_names = {pointer: name for name, pointer in native['resource_by_name'].items()}
    def colors(pointer):
        return {'primary': get(pointer + 0x3bc, 'I')[0], 'secondary': get(pointer + 0x3c0, 'I')[0],
                'primaryAlphaInterpolator': list(get(pointer + 0x134, '4i')),
                'primaryAlphaDuration': get(pointer + 0x158, 'i')[0],
                'secondaryAlphaInterpolator': list(get(pointer + 0x270, '4i')),
                'secondaryAlphaDuration': get(pointer + 0x294, 'i')[0]}
    def observe(uc, address, size, user):
        sp = mu.reg_read(UC_X86_REG_ESP)
        if address == 0x454d10:
            resource = mu.reg_read(UC_X86_REG_ECX)
            events.append({'frame': frame, 'phase': phase, 'kind': 'anmBind',
                           'resource': resource_names.get(resource), 'script': get(sp + 8, 'i')[0],
                           'rngBefore': list(get(0x4ce568, 'II'))})
        elif address == 0x464440:
            if mu.reg_read(UC_X86_REG_ESI) == 0x4ce568:
                events.append({'frame': frame, 'phase': phase, 'kind': 'gameRng32',
                               'caller': hex(get(sp, 'I')[0]), 'before': list(get(0x4ce568, 'II'))})
        elif address in [0x40cd00, 0x4285f0, 0x40caa0, 0x4286f0]:
            events.append({'frame': frame, 'phase': phase, 'kind': 'clear', 'entry': hex(address)})
        elif address in [0x407297, 0x4072c5] and arguments.visual_fields:
            events.append({'frame': frame, 'phase': phase, 'kind': 'marisaFrontAlpha',
                           'entry': hex(address), 'colors': colors(mu.reg_read(UC_X86_REG_EAX))})
        elif address == 0x452a60:
            events.append({'frame': frame, 'phase': phase, 'kind': 'cameraConstructor',
                           'caller': hex(get(sp, 'I')[0]), 'arguments': list(get(sp + 8, '6i'))})
    for address in [0x454d10, 0x464440, 0x40cd00, 0x4285f0, 0x40caa0, 0x4286f0, 0x452a60]:
        mu.hook_add(UC_HOOK_CODE, observe, begin=address, end=address)
    if arguments.visual_fields:
        for address in [0x407297, 0x4072c5]:
            mu.hook_add(UC_HOOK_CODE, observe, begin=address, end=address)
    def cameras():
        rows, node = [], get(SCHEDULER + 0x18, 'I')[0]
        while node:
            owner, next_node = get(node, 'II')
            if get(owner, 'i')[0] == 14 and get(owner + 8, 'I')[0]:
                rows.append((owner, get(owner + 8, 'I')[0], get(owner + 0x20, 'I')[0]))
            node = next_node
        return rows
    def tick_cameras():
        for owner, callback, data in cameras():
            mu.reg_write(UC_X86_REG_ECX, data)
            result = call(callback)
            events.append({'frame': frame, 'phase': phase, 'kind': 'cameraCallback',
                'entry': hex(callback), 'timer': list(get(data + 0x30, 'iif')), 'result': result,
                'parameters': list(get(data + 0x1c, '4i'))})
            if result == 7:
                # Dispatcher4625a0 invokes this registration's destructor;
                # Camera452960 unregisters both callbacks through452f00.
                destructor = get(owner + 0x10, 'I')[0]
                if destructor:
                    mu.reg_write(UC_X86_REG_ECX, data)
                    call(destructor)
            elif result == 0:
                mu.reg_write(UC_X86_REG_ECX, owner)
                mu.reg_write(UC_X86_REG_EDX, SCHEDULER)
                call(0x462890)
    def resolve(handle):
        if not handle: return None
        mu.reg_write(UC_X86_REG_EDX, ANM)
        pointer = call(0x461920, handle)
        if not pointer: return None
        result = {'id': get(pointer, 'I')[0], 'script': get(pointer + 0x3ea, 'H')[0],
                  'clock': get(pointer + 0x6c, 'i')[0], 'flags': get(pointer + 0x47c, 'I')[0]}
        if arguments.visual_fields: result.update(colors(pointer))
        return result
    tracked_main = 0
    def snap():
        result = {'frame': frame, 'state': get(BOMB + 0x3c, 'i')[0], 'age': get(BOMB + 0x18, 'i')[0],
                'handles': list(get(BOMB + 0x40, '3I')), 'main': resolve(get(BOMB + 0x40, 'I')[0]),
                'background': resolve(get(BOMB + 0x48, 'I')[0]), 'rng': list(get(0x4ce568, 'II')),
                'cameraOffsets': list(get(0x4cee04, '2f')), 'cameraCount': len(cameras())}
        if arguments.tracked_main:
            # Independent original461920 query of the birth-time handle;
            # Bomb may release its current handle before ANM's tail retires.
            result['trackedMain'] = resolve(tracked_main)
        return result
    frames = []
    for frame in range(710):
        native['code_hook'].__globals__['frame'] = frame
        phase = 'camera14'
        tick_cameras()
        if frame in [0, 350]:
            phase = 'bombStart'
            call(0x406bf0)
            tracked_main = get(BOMB + 0x40, 'I')[0]
        phase = 'bomb17'
        mu.reg_write(UC_X86_REG_EAX, BOMB)
        call(0x406ce0)
        phase = 'primary27'
        mu.reg_write(UC_X86_REG_EDI, ANM)
        call(0x460fe0)
        frames.append(snap())
        if arguments.draw_reset:
            # Execute the real Draw42 write-zero block, stopping before
            #4611d0's draw calls. It pushesEDI; preserve the fixture stack.
            prior_stack = mu.reg_read(UC_X86_REG_ESP)
            mu.emu_start(0x460e40, 0x460e62)
            mu.reg_write(UC_X86_REG_ESP, prior_stack)
    records.append({'name': name, 'loadout': native['fixture']['character'] * 2 + native['fixture']['shot'],
        'fixtures': ['Actual selected-manager startup at replay header; no gameplay input ticks',
                     'isolated player position20,300; Enemy/Bullet/Player managers not advanced',
                     'explicit unpaused application context flags0; only actual registered priority14 callbacks',
                     'actual Bomb17 then PrimaryANM27; original constructors/Leaf callbacks and game RNG'],
        'initialSeed': 42, 'starts': [0, 350], 'frames': frames, 'events': events,
        'resourceNames': list(native['resource_by_name'])})
    output = ROOT / ('artifacts/reverse/bomb-lifecycle-other-mesh-tracked-original.json' if arguments.audit_other_loadouts and arguments.mesh_enabled and arguments.tracked_main
                     else 'artifacts/reverse/bomb-lifecycle-other-mesh-original.json' if arguments.audit_other_loadouts and arguments.mesh_enabled
                     else 'artifacts/reverse/bomb-lifecycle-other-loadouts-original.json' if arguments.audit_other_loadouts
                     else 'artifacts/reverse/bomb-lifecycle-visual-original.json' if arguments.visual_fields
                     else 'artifacts/reverse/bomb-lifecycle-draw-reset-original.json' if arguments.draw_reset
                     else 'artifacts/reverse/bomb-lifecycle-original.json')
    output.write_text(json.dumps({'schema': 'th12-native-bomb-lifecycle/4' if arguments.tracked_main
                                 else 'th12-native-bomb-lifecycle/3' if arguments.visual_fields
                                 else 'th12-native-bomb-lifecycle/2' if arguments.draw_reset else 'th12-native-bomb-lifecycle/1',
        'originalExeSha256': native['SHA'], 'wholeGameOracle': False, 'gpuExecuted': False,
        'meshAllocationGate': get(0x4cea94,'I')[0], 'meshGateSyntheticDisplayEnvironment': arguments.mesh_enabled,
        'draw42ResetExecuted': arguments.draw_reset, 'draw42Block': '460e40..460e62 original four float writes; stop before4611d0' if arguments.draw_reset else None,
        'records': records}, indent=2) + '\n', encoding='utf-8')
    print(json.dumps({'name': name, 'output': str(output), 'bindings': [event for event in events if event['kind'] == 'anmBind' and event['phase'] == 'bombStart'],
        'stopped': [f['frame'] for f in frames if f['state'] == 0 and (f['frame'] == 0 or frames[f['frame']-1]['state'] != 0)],
        'finalRng': frames[-1]['rng'], 'cameraTicks': sum(event['kind'] == 'cameraCallback' for event in events)}), flush=True)
