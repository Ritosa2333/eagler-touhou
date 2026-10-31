"""Execute TH12 1.00b draw wrappers and font queue viewport selection.

This is an independent, GPU-free scheduling probe, not a rendering Oracle.
The original layer dispatch, intrusive layer traversal, queued-font filtering,
viewport choice, and Player body draw entry execute. Matrix calculation, quad
submission, and D3D device methods are observable boundaries. The prepared
layer lists and font records are explicit inputs, not captured full-game state.
"""
import json
import pathlib
import struct
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT.parent / 'tools/python'))
import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
from unicorn.x86_const import *
from native_anm_resource import check_original, EXE_SHA256

EXE = ROOT.parent / 'th12/th12.exe'
check_original(EXE)
PE = pefile.PE(str(EXE))
MANAGER = 0x3000000
DEVICE = 0x2000000
VTABLE = 0x2001000
FONT = 0x2100000
PLAYER = 0x2200000
VIEWPORT_METHOD = 0x100d000
RENDER_STATE_METHOD = 0x100d010
CALLBACK = 0x100d020
CAMERAS = [0x4ceaec, 0x4cec04, 0x4ced1c]


class Native:
    def __init__(self, renderer_flags=0x4000):
        self.mu = Uc(UC_ARCH_X86, UC_MODE_32)
        for base, size in [(0, 0x10000),
                           (0x400000, (PE.OPTIONAL_HEADER.SizeOfImage + 4095) & ~4095),
                           (0x1000000, 0x10000), (0x2000000, 0x400000),
                           (MANAGER, 0x900000)]:
            self.mu.mem_map(base, size)
        self.mu.mem_write(0x400000, PE.get_memory_mapped_image())
        self.mu.reg_write(UC_X86_REG_FPCW, 0x37f)
        self.mu.reg_write(UC_X86_REG_MXCSR, 0x1f80)
        self.records = []
        self.registrations = []
        self.heap = 0x2350000
        self.context = ''
        self.put(0x4ce8cc, 'I', MANAGER)
        self.put(0x4ce8f0, 'I', DEVICE)
        self.put(DEVICE, 'I', VTABLE)
        self.put(VTABLE + 0xbc, 'I', VIEWPORT_METHOD)
        self.put(VTABLE + 0xe4, 'I', RENDER_STATE_METHOD)
        self.put(0x4ce8e8 + 0x590, 'I', renderer_flags)
        self.mu.hook_add(UC_HOOK_CODE, self.hook)
        self.call(0x430be0, regs={UC_X86_REG_ECX: 0x4ce8e8})
        self.set_camera(2)

    def put(self, address, fmt, *values):
        self.mu.mem_write(address, struct.pack('<' + fmt, *values))

    def get(self, address, fmt):
        return list(struct.unpack('<' + fmt,
                                 self.mu.mem_read(address, struct.calcsize('<' + fmt))))

    def set_camera(self, index):
        self.put(0x4cee34, 'I', CAMERAS[index])
        self.put(0x4cee38, 'I', index)

    def viewport(self):
        camera = self.get(0x4cee34, 'I')[0]
        return self.get(camera + 0xcc, '4I')

    def event(self, kind, **values):
        self.records.append({'context': self.context, 'kind': kind,
                             'camera': self.get(0x4cee38, 'I')[0],
                             'viewport': self.viewport(), **values})

    def ret(self, pop=0, value=0):
        sp = self.mu.reg_read(UC_X86_REG_ESP)
        target = self.get(sp, 'I')[0]
        self.mu.reg_write(UC_X86_REG_ESP, sp + 4 + pop)
        self.mu.reg_write(UC_X86_REG_EAX, value)
        self.mu.reg_write(UC_X86_REG_EIP, target)

    def hook(self, mu, address, size, data):
        sp = mu.reg_read(UC_X86_REG_ESP)
        if address in [0x430a70, 0x430910, 0x45a3c0]:
            self.ret()
        elif address == VIEWPORT_METHOD:
            pointer = self.get(sp + 8, 'I')[0]
            # Native updates its current-camera index *after* this call.
            self.event('set-viewport', applied=self.get(pointer, '4I'))
            self.ret(8)
        elif address == RENDER_STATE_METHOD:
            state, value = self.get(sp + 8, 'II')
            self.event('render-state', state=state, value=value)
            self.ret(12)
        elif address == 0x45c900:
            self.event('anm-draw', vm=mu.reg_read(UC_X86_REG_EAX),
                       layer=self.get(mu.reg_read(UC_X86_REG_EAX) + 0x20, 'i')[0])
            self.ret(4)
        elif address == CALLBACK:
            self.event('custom-draw', vm=mu.reg_read(UC_X86_REG_ECX))
            self.ret()
        elif address == 0x4018f0:
            font, record = self.get(sp + 4, 'II')
            self.event('font-draw', index=(record - font - 0x97c) // 0x138,
                       queue=self.get(record + 0x128, 'i')[0],
                       cameraFlag=self.get(record + 0x11c, 'i')[0])
            self.ret(8)
        elif address == 0x46c9ea:
            size = self.get(sp + 4, 'I')[0]
            result = self.heap
            self.heap += (size + 15) & ~15
            if self.heap >= 0x2400000:
                raise RuntimeError('Constructed allocation pool exhausted')
            self.ret(value=result)
        elif address == 0x462420:
            node = mu.reg_read(UC_X86_REG_ESI)
            self.registrations.append({'callback': f'{self.get(node + 8, "I")[0]:08x}',
                                       'priority': mu.reg_read(UC_X86_REG_EBX)})
            self.ret()

    def call(self, address, args=(), regs=None):
        sp, stop = 0x100e000, 0x100f000
        self.put(sp, 'I' * (len(args) + 1), stop, *args)
        self.mu.reg_write(UC_X86_REG_ESP, sp)
        for register, value in (regs or {}).items():
            self.mu.reg_write(register, value)
        try:
            self.mu.emu_start(address, stop, count=100000)
        except Exception as error:
            raise RuntimeError(f'{address:x} failed at {self.mu.reg_read(UC_X86_REG_EIP):x}') from error
        if self.mu.reg_read(UC_X86_REG_EIP) != stop:
            raise RuntimeError('Original instruction budget exhausted')

    def layer_chain(self, layer):
        base = 0x2300000 + layer * 0x2000
        # Inherited parent and child have distinct draw-list membership. The
        # list, not the parent pointer, decides whether a child is submitted.
        for index in range(4):
            vm = base + index * 0x500
            self.put(vm + 0x1c, 'I', vm + 0x500 if index < 3 else 0)
            self.put(vm + 0x20, 'i', layer)
            self.put(vm + 0x47c, 'I', 0x10000003 if index == 1 else 3)
        self.put(base + 0x48c, 'I', CALLBACK)
        self.put(base + 0x3c, 'I', base + 3 * 0x500)
        self.put(MANAGER + 0x8856e4 + layer * 0x4b4, 'I', base)
        return base

    def original_layer_registration(self):
        # Start after ANM construction and its separate logic registrations.
        # EBP=3 is the original registration flag; EDI is its constructed
        # manager. All allocation, callback assignment and priority instructions
        # execute through the last layer31 registration, before device init.
        self.mu.reg_write(UC_X86_REG_ESP, 0x100e000)
        self.mu.reg_write(UC_X86_REG_EDI, MANAGER)
        self.mu.reg_write(UC_X86_REG_EBP, 3)
        self.mu.emu_start(0x45e1b7, 0x45e9ea, count=100000)
        if self.mu.reg_read(UC_X86_REG_EIP) != 0x45e9ea:
            raise RuntimeError('Original layer-registration block did not complete')
        return {row['callback']: row['priority'] for row in self.registrations}


profiles = []
for flags in [0, 0x4000]:
    n = Native(flags)
    profiles.append({'rendererFlags': flags,
                     'viewports': [n.get(camera + 0xcc, '4I') for camera in CAMERAS]})

n = Native()
registered = n.original_layer_registration()
layers = []
for layer in list(range(26)) + [31, 30]:
    n.context = f'layer{layer}'
    base = n.layer_chain(layer)
    entry = (0x460c50 + layer * 0x10 if layer < 4 else
             0x460c90 if layer == 4 else
             0x460d10 + (layer - 5) * 0x10 if layer < 20 else
             {20: 0x460e40, 21: 0x460e70, 22: 0x460e30,
              23: 0x460e20, 24: 0x460e10, 25: 0x460e00,
              30: 0x460f00, 31: 0x460f50}[layer])
    start = len(n.records)
    n.call(entry, regs={UC_X86_REG_ECX: MANAGER})
    layers.append({'layer': layer, 'entry': f'{entry:08x}',
                   'priority': registered[f'{entry:08x}'],
                   'inputChain': [base + index * 0x500 for index in range(4)],
                   'events': n.records[start:], 'cameraAfter': n.get(0x4cee38, 'I')[0]})

font_cases = []
for queue in [0, 1]:
    for camera_flags in [[0], [1], [1, 0, 1], [0, 1, 0]]:
        n = Native()
        n.context = f'font-queue{queue}-cameras{camera_flags}'
        n.put(FONT + 0x18f7c, 'i', len(camera_flags) * 2)
        # Interleaved opposite-queue entries must be skipped by the original.
        inputs = []
        for index, camera_flag in enumerate(camera_flags):
            for opposite in [False, True]:
                slot = index * 2 + int(opposite)
                record = FONT + 0x97c + slot * 0x138
                row = {'queue': 1 - queue if opposite else queue,
                       'cameraFlag': 1 - camera_flag if opposite else camera_flag}
                n.put(record + 0x128, 'i', row['queue'])
                n.put(record + 0x11c, 'i', row['cameraFlag'])
                inputs.append(row)
        n.call(0x401790, [FONT, queue])
        font_cases.append({'queue': queue, 'priority': 69 if queue == 0 else 45,
                           'inputs': inputs, 'events': n.records,
                           'cameraAfter': n.get(0x4cee38, 'I')[0]})

n = Native()
n.context = 'player-body-layer99'
n.put(PLAYER + 0xa28, 'i', 1)
n.put(PLAYER + 0x14 + 0x20, 'i', 99)
n.call(0x437600, regs={UC_X86_REG_EAX: PLAYER})
body = {'entry': '00437600', 'priority': 24, 'events': n.records}

out = {'schema': 'th12-native-draw-scheduling-v1', 'originalExeSha256': EXE_SHA256,
       'scope': 'Constructed native CPU draw-wrapper inputs, not full-frame pixels',
       'boundaries': ['430a70/430910 matrix and device transforms omitted',
                      '45a3c0 batch flush omitted', 'D3D SetViewport/RenderState recorded',
                      '45c900/4018f0 geometry submissions recorded without execution',
                      '46c9ea constructed allocations;462420 registration recorded'],
       'nativeLayerRegistrations': [{'callback': callback, 'priority': priority}
                                    for callback, priority in registered.items()],
       'cameraProfiles': profiles, 'layers': layers, 'fontQueues': font_cases,
       'playerBody': body,
       'bulletGroups': [struct.unpack('<i', PE.get_memory_mapped_image()[0xaf348 + i * 0xd0:
                                                                      0xaf34c + i * 0xd0])[0]
                        for i in range(31)]}
target = ROOT / 'tests/fixtures/render-schedule-native.json'
target.write_text(json.dumps(out, ensure_ascii=False, indent=2), encoding='utf-8')
print(json.dumps({'output': str(target), 'layerCases': len(layers),
                  'fontCases': len(font_cases), 'cameraProfiles': len(profiles)}))
