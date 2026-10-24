"""Original414b0f..414c01 death visual and actual primary scheduler.

The sampled basic-block path ends before item drops/audio. No ANM allocation,
binding, child factory, RNG or scheduler is intercepted. This is a visual-logic
fixture, not a whole-game or GPU golden.
"""
import pathlib, sys, struct, json, hashlib
ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT.parent / 'tools/python'))
import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32
from unicorn.x86_const import *
from native_anm_resource import check_original, EXE_SHA256, install_resource
exe = ROOT.parent / 'th12/th12.exe'
check_original(exe)
pe = pefile.PE(str(exe))
raw = (ROOT / 'local/retail/bullet.anm').read_bytes()
records = []
for script in [81, 84, 87, 90]:
    mu = Uc(UC_ARCH_X86, UC_MODE_32)
    for base, size in [(0x400000, (pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095),
                       (0x1000000, 0x10000), (0x2000000, 0x1000000)]:
        mu.mem_map(base, size)
    mu.mem_write(0x400000, pe.get_memory_mapped_image())
    manager, resource, enemy, em, stack, stop = 0x2000000, 0x2a00000, 0x2b00000, 0x2c00000, 0x100e000, 0x100f000
    put = lambda at, fmt, *v: mu.mem_write(at, struct.pack('<'+fmt, *v))
    read = lambda at, fmt: list(struct.unpack('<'+fmt, mu.mem_read(at, struct.calcsize('<'+fmt))))
    install_resource(mu, raw, resource, 0)
    put(0x4ce8cc, 'I', manager); put(0x4b43dc, 'I', em); put(em+0x40, 'I', resource)
    put(0x4b2ed0, 'f', 1); put(0x4ce568, 'II', 0x1234, 0); put(0x4ce560, 'II', 0xbeef, 0)
    put(enemy+0x26bc, 'ii', script, 0); put(enemy+0x1074, '3f', -20, 120, 7)
    mu.reg_write(UC_X86_REG_FPCW, 0x37f); mu.reg_write(UC_X86_REG_MXCSR, 0x1f80)
    put(stack, 'I', stop); mu.reg_write(UC_X86_REG_ESP, stack); mu.reg_write(UC_X86_REG_ESI, enemy)
    mu.emu_start(0x414b0f, 0x414c01, count=1000000)
    if mu.reg_read(UC_X86_REG_EIP) != 0x414c01: raise RuntimeError('Native death path budget')
    def snapshot():
        primary = []; head = read(manager+0x8856b8, 'I')[0]
        while head:
            obj, head = read(head, 'II'); children=[]; child=read(obj+0x14,'I')[0]
            while child:
                c, child=read(child,'II'); children.append(read(c+0x3ea,'h')[0])
                if len(children)>100:raise RuntimeError('Native child list budget')
            primary.append({'script':read(obj+0x3ea,'h')[0], 'clock':read(obj+0x6c,'i')[0],
                'pendingInterrupt':read(obj+0x3c4,'h')[0], 'scale':read(obj+0x40,'2f'),
                'position':read(obj+0x424,'3f'), 'engine':read(obj+0x430,'3f'),
                'offset':read(obj+0x43c,'3f'), 'rotation':read(obj+0x24,'3f'),
                'float0':read(obj+0x40c,'f')[0], 'attached':bool(read(obj+0x18,'I')[0]),
                'children':children})
            if len(primary)>100:raise RuntimeError('Native primary list budget')
        return {'gameSeedCounter':read(0x4ce568,'II'), 'displaySeedCounter':read(0x4ce560,'II'), 'primary':primary}
    frames=[snapshot()]
    for tick in range(40):
        put(stack,'I',stop);mu.reg_write(UC_X86_REG_ESP,stack);mu.reg_write(UC_X86_REG_EDI,manager)
        mu.emu_start(0x460fe0,stop,count=1000000)
        if mu.reg_read(UC_X86_REG_EIP)!=stop:raise RuntimeError('Native scheduler budget')
        frames.append(snapshot())
    records.append({'script':script, 'engine':[204,136,7], 'layer':4, 'frames':frames})
out=ROOT/'tests/fixtures/enemy-death-anm-original.json'
out.write_text(json.dumps({'schema':'th12-original-enemy-death-anm/1', 'originalExeSha256':EXE_SHA256,
    'resourceSha256':hashlib.sha256(raw).hexdigest(), 'resource':'bullet.anm', 'coreBank':0,
    'resourceBindingEvidence':'Native412c60 copies BulletManager+4debdc into EnemyManager+40',
    'provider':'Actual414b0f..414c01,454d10,4621c0,461250,455630,460fe0 with original ANM resources',
    'wholeGameOracle':False,'gpuExecuted':False,'records':records},indent=2)+'\n',encoding='utf8')
print(json.dumps({'output':str(out),'cases':len(records),'frames':sum(len(r['frames']) for r in records)}))
