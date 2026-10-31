"""Original431630 surface aliases and42f200/42f3b0 background pass order.

Real text.anm81/82 binds, STD17 factory/list and native draw dispatch execute.
Prepared COM textures expose distinct GetSurfaceLevel outputs. The Device's
SetRenderTarget/Clear calls and45c900 VM selection are recorded; CPU grid
vertices have their separate complete native fixture. No GPU pixels execute.
"""
import json, pathlib
ROOT = pathlib.Path(__file__).resolve().parents[2]
source = ROOT / 'scripts/reverse/sample-stage-background-effect.py'
ns = {'__file__': str(source), '__name__': 'stage_surface_native_definitions'}
exec(compile(source.read_text(encoding='utf-8').split('\nnative = StageNative()\n')[0], str(source), 'exec'), ns)
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import *
native = ns['StageNative']()
APP, TEXTURES = 0x4ce8e8, 0x20a0000
TEX2, TEX3, TEX_VTABLE = 0x20a0100, 0x20a0200, 0x20a0300
SURFACE2, SURFACE3, BACKBUFFER = 0x20a0400, 0x20a0500, 0x20a0600
MAIN2, MAIN3, FINAL = 0x20a1000, 0x20a2000, 0x20a3000
GET_SURFACE, GET_BACKBUFFER, SET_TARGET, CLEAR = [0x100d050+i*16 for i in range(4)]
SET_VIEWPORT = 0x100d0a0
records = []
phase = 'surface-bootstrap'
native.put(TEX_VTABLE + 0x48, 'I', GET_SURFACE)
for texture in [TEX2, TEX3]: native.put(texture, 'I', TEX_VTABLE)
resource = native.get(0x4cee70, 'I')[0]
native.put(resource + 0x120, 'I', TEXTURES)
native.put(TEXTURES + 2*20, 'I', TEX2)
native.put(TEXTURES + 3*20, 'I', TEX3)
native.put(APP + 0x588, 'I', resource)
native.put(APP + 0x1ac, '3I', 0, 0, 0)
native.put(APP + 0x1bc, '3I', MAIN2, MAIN3, FINAL)
native.put(ns['ns']['VTABLE'] + 0x48, 'I', GET_BACKBUFFER)
native.put(ns['ns']['VTABLE'] + 0x94, 'I', SET_TARGET)
native.put(ns['ns']['VTABLE'] + 0xac, 'I', CLEAR)
native.put(ns['ns']['VTABLE'] + 0xbc, 'I', SET_VIEWPORT)
native.put(0x4cede8, '4I', 13, 0, 422, 480)
native.put(ns['STAGE'] + 0x360c + 0xcc, '4I', 13, 0, 422, 480)
native.put(0x4cf2a8, 'I', 0xff000000)
for pointer in [MAIN2, MAIN3, FINAL]:
    native.call(0x4027e0, registers={UC_X86_REG_ECX: pointer})
def observe(mu, address, size, user):
    sp = mu.reg_read(UC_X86_REG_ESP)
    if address == GET_SURFACE:
        texture, level, destination = native.get(sp+4, '3I')
        surface = SURFACE2 if texture == TEX2 else SURFACE3
        native.put(destination, 'I', surface)
        records.append({'phase': phase, 'kind': 'get-surface', 'textureEntry': 2 if texture == TEX2 else 3,
                        'level': level, 'destination': hex(destination), 'surface': hex(surface)})
        native.ret(0, 12)
    elif address == GET_BACKBUFFER:
        destination = native.get(sp+20, 'I')[0]
        native.put(destination, 'I', BACKBUFFER)
        records.append({'phase': phase, 'kind': 'get-backbuffer', 'destination': hex(destination), 'surface': hex(BACKBUFFER)})
        native.ret(0, 20)
    elif address == SET_TARGET:
        target, surface = native.get(sp+8, '2I')
        records.append({'phase': phase, 'kind': 'set-target', 'slot': target, 'surface': hex(surface)})
        native.ret(0, 12)
    elif address == CLEAR:
        count, rectangle, flags, color = native.get(sp+8, '4I')
        records.append({'phase': phase, 'kind': 'clear', 'flags': flags, 'color': color,
                        'rectangles': [native.get(rectangle+i*16, '4i') for i in range(count)]})
        native.ret(0, 28)
    elif address == SET_VIEWPORT:
        native.ret(0, 8)
    elif address == 0x430a70:
        # View-matrix library preparation is outside this surface-order query.
        native.ret()
    elif address == 0x45c900:
        pointer = mu.reg_read(UC_X86_REG_EAX)
        records.append({'phase': phase, 'kind': 'draw-vm', 'pointer': hex(pointer),
                        'id': native.get(pointer, 'I')[0], 'script': native.get(pointer+0x3ea, 'h')[0],
                        'sprite': native.get(pointer+0x3e4, 'h')[0], 'layer': native.get(pointer+0x20, 'i')[0],
                        'position': native.get(pointer+0x424, '3f'), 'engine': native.get(pointer+0x430, '3f'),
                        'scale': native.get(pointer+0x40, '2f'), 'dimensions': native.get(pointer+0x58, '2f'),
                        'flags': native.get(pointer+0x47c, 'I')[0], 'primary': native.get(pointer+0x3bc, 'I')[0]})
        native.ret(0, 4)
for at in [GET_SURFACE, GET_BACKBUFFER, SET_TARGET, CLEAR, SET_VIEWPORT, 0x430a70, 0x45c900]:
    native.mu.hook_add(UC_HOOK_CODE, observe, begin=at, end=at)
native.call(0x431630, registers={UC_X86_REG_EAX: APP})
aliases = {'surface2': hex(native.get(APP+0x1ac, 'I')[0]),
           'surface3': hex(native.get(APP+0x1b0, 'I')[0]),
           'backbuffer': hex(native.get(APP+0x1b4, 'I')[0]),
           'mainScripts': [native.get(pointer+0x3ea, 'h')[0] for pointer in [MAIN2, MAIN3, FINAL]],
           'mainSprites': [native.get(pointer+0x3e4, 'h')[0] for pointer in [MAIN2, MAIN3, FINAL]]}
for tick in range(2565):
    native.tick = tick
    native.call(0x403ec0, registers={UC_X86_REG_ECX: ns['STAGE']})
    native.call(0x460fe0, registers={UC_X86_REG_EDI: ns['ns']['MANAGER']})
before = native.rng()
for priority, entry in [(0,0x42f0b0),(12,0x42f200),(14,0x42f3b0),(37,0x42f290),(39,0x42f410),(67,0x42f320),(68,0x42f470)]:
    phase = 'draw-priority-'+str(priority)
    native.call(entry, registers={UC_X86_REG_ECX: APP})
assert native.rng() == before
output = ROOT / 'tests/fixtures/stage-surface-pipeline-native.json'
output.write_text(json.dumps({'schema': 'th12-native-stage-surface-pipeline/1',
    'originalExeSha256': ns['ns']['EXE_SHA256'], 'wholeGameOracle': False, 'gpuExecuted': False,
    'scope': 'Actual431630 selects real text.anm texture2/3 surfaces and binds81/82/81. Original stage05 STD12 and Primary27 execute through2564.42f0b0/42f200/42f3b0/42f290/42f410/42f320/42f470 dispatch real Target/Clear/ANM layer selections. Prepared COM GetSurfaceLevel/GetBackBuffer outputs, recorded Device methods,430a70 view-library boundary and45c900 draw-selection boundary; no GPU pixels or whole-game acceptance.',
    'aliases': aliases, 'records': records, 'rngBeforeDraw': before, 'rngAfterDraw': native.rng()}, indent=2)+'\n', encoding='utf-8')
print(json.dumps({'output': str(output), 'aliases': aliases, 'records': len(records)}))
