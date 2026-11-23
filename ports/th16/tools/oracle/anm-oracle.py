#!/usr/bin/env python3
"""Unicorn oracle：在原版 th16.exe 代码上装载 ANM 文件、生成 VM 并逐帧执行，导出 VM 内存快照。

用法：
    anm-oracle.py <th16.exe> <name.anm> <anm-bytes-file> <scripts: all|3|0-20,40> <frames> <out.json> [--layer N]

装载一次后做内存检查点，每个脚本从同一状态开始（spawn → N 帧 tick → 快照）。

执行链（全部为原版代码，只桩掉堆分配 / 档案读取 / D3D 纹理 / 临界区 / Sleep）：
    0x46a3a0  AnmManager 构造（0x1c7fd90 字节，含两组 0x1fff 个 VM 池）
    0x46cf80  为 bank 槽 0 分配 ANM 文件对象 (0x13c) 并经 0x46cdd0 解析头部
    0x46d1c0  装载线程工作步：逐 entry 建纹理（桩）与精灵表（0x46d8a0 真算），末步初始化脚本模板并预跑一次 VM
    0x406380  spawn：从池取 VM、拷模板、置 layer、首次 0x45f980、挂进世界链表 (manager+0xdc)
    0x46e340  世界链表逐帧 tick（0x45f980 + 按 layer 建绘制链）

输出 JSON：{"runs":[{"script":k,"frames":[{"frame":n,"vms":[{"id":..,"addr":..,"bytes":hex(0x5fc)}...],"layers":[[vm ids by layer chain]...]}]}]}
VM 字节是 0x5fc 结构体原样内存；比较器按字段表取用（指针字段跳过）。
"""
import argparse, json, pathlib, struct, sys
import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE, UC_HOOK_MEM_UNMAPPED
from unicorn.x86_const import *

IMAGE_BASE = 0x400000
STACK = 0x00600000
STACK_SIZE = 0x100000
HEAP = 0x01000000
HEAP_SIZE = 0x18000000  # 堆只增不减：整局多关（每关 Stage / ECL / ANM 重建）需 > 128 MiB
STUB = 0x00700000
TEB = 0x7FFDD000
GDT = 0x80000000
SENTINEL = 0x00710000

MANAGER_CTOR, FILE_OPEN, LOADER_STEP, SPAWN, TICK = 0x46A3A0, 0x46CF80, 0x46D1C0, 0x406380, 0x46E340
VM_SIZE = 0x5FC
DAT_MANAGER = 0x4C0F48
DAT_ASCII_MANAGER = 0x4A6D98
DAT_BACKBUFFER_W = 0x4D9D2C  # +4 高度 +8 缩放系数（float）
ASCII_SLOT = 2
BANK_ARRAY = 0x184F4F0
WORLD_LIST = 0xDC
LAYER_HEADS = 0x1C6FC30
LAYER_COUNT = 0x24


class Oracle:
    def __init__(self, exe: pathlib.Path, anm_name: str, anm_bytes: bytes, extra=()):
        # extra：[(slot, name, bytes)]，装载主文件之前先装入的 ANM（如 AsciiManager 的 ascii.anm → 槽 2）
        pe = pefile.PE(str(exe))
        self.pe = pe
        uc = Uc(UC_ARCH_X86, UC_MODE_32)
        self.uc = uc
        size = (pe.OPTIONAL_HEADER.SizeOfImage + 0xFFFF) & ~0xFFFF
        uc.mem_map(IMAGE_BASE, size)
        uc.mem_write(IMAGE_BASE, pe.header)
        for s in pe.sections:
            uc.mem_write(IMAGE_BASE + s.VirtualAddress, s.get_data())
        uc.mem_map(STACK, STACK_SIZE)
        uc.mem_map(HEAP, HEAP_SIZE)
        uc.mem_map(STUB, 0x1000)
        uc.mem_map(SENTINEL, 0x1000)
        uc.mem_map(TEB, 0x1000)
        self.heap_top = HEAP + 0x1000
        self.anm_name = anm_name
        self.anm_bytes = anm_bytes
        self.extra = list(extra)
        self.files = {anm_name: anm_bytes}
        for _slot, name, data in self.extra:
            self.files[name] = data
        self.read_calls = 0
        self.textures = []
        self.errors = []
        self.setup_fs()
        self.setup_stubs()
        uc.hook_add(UC_HOOK_MEM_UNMAPPED, self.on_unmapped)

    # ---- 环境 ----
    def setup_fs(self):
        uc = self.uc
        uc.mem_map(GDT, 0x1000)
        def entry(base, limit, access, flags):
            v = limit & 0xFFFF
            v |= (base & 0xFFFFFF) << 16
            v |= (access & 0xFF) << 40
            v |= ((limit >> 16) & 0xF) << 48
            v |= (flags & 0xF) << 52
            v |= ((base >> 24) & 0xFF) << 56
            return struct.pack('<Q', v)
        # 全部 ring0 平坦段（4 GiB，粒度 4K）：1=代码 2=数据/栈；14=FS → TEB
        code = 0x80 | 0x10 | 0x08 | 0x02
        data = 0x80 | 0x10 | 0x02
        uc.mem_write(GDT + 1 * 8, entry(0, 0xFFFFF, code, 0xC))
        uc.mem_write(GDT + 2 * 8, entry(0, 0xFFFFF, data, 0xC))
        uc.mem_write(GDT + 14 * 8, entry(TEB, 0xFFF, data, 0x4))
        uc.reg_write(UC_X86_REG_GDTR, (0, GDT, 0x1000, 0))
        uc.reg_write(UC_X86_REG_CS, 0x08)
        for reg in (UC_X86_REG_DS, UC_X86_REG_ES, UC_X86_REG_SS, UC_X86_REG_GS):
            uc.reg_write(reg, 0x10)
        uc.reg_write(UC_X86_REG_FS, 0x70)
        uc.mem_write(TEB + 0x00, struct.pack('<I', 0xFFFFFFFF))  # ExceptionList
        uc.mem_write(TEB + 0x18, struct.pack('<I', TEB))         # Self

    def alloc(self, size, align=16):
        size = (size + align - 1) & ~(align - 1)
        p = self.heap_top
        self.heap_top += size
        if self.heap_top > HEAP + HEAP_SIZE:
            raise RuntimeError('oracle heap exhausted')
        return p

    def stub(self, index):
        return STUB + index * 16

    def setup_stubs(self):
        uc = self.uc
        self.hooks = {}
        # 桩区：每个桩 16 字节，形如 ret imm16；语义由 Python 钩子在桩入口完成
        def make_stub(index, ret_bytes, fn):
            addr = self.stub(index)
            uc.mem_write(addr, bytes([0xC2]) + struct.pack('<H', ret_bytes) if ret_bytes else bytes([0xC3]))
            self.hooks[addr] = fn
            return addr
        # 导入表替换：Sleep / EnterCriticalSection / LeaveCriticalSection
        iat = {0x48B0A0: 4, 0x48B070: 4, 0x48B074: 4}
        for i, (slot, n) in enumerate(iat.items()):
            addr = make_stub(1 + i, n, lambda o: None)
            uc.mem_write(slot, struct.pack('<I', addr))
        # D3D 伪纹理 vtable：0x44 GetLevelDesc(this, level, desc*)、0x48 GetSurfaceLevel(this, level, out*)
        self.tex_vtable = self.alloc(0x100)
        get_level_desc = make_stub(8, 0xC, self.on_get_level_desc)
        get_surface_level = make_stub(9, 0xC, self.on_get_surface_level)
        release = make_stub(10, 4, lambda o: o.uc.reg_write(UC_X86_REG_EAX, 0))
        generic = make_stub(11, 0, lambda o: o.uc.reg_write(UC_X86_REG_EAX, 0))
        for i in range(0x40):
            uc.mem_write(self.tex_vtable + i * 4, struct.pack('<I', generic))
        # IUnknown：AddRef(this) / Release(this) 为 stdcall 1 参（RET 4）；0x46d770 卸载 ANM 时逐纹理 Release
        uc.mem_write(self.tex_vtable + 4, struct.pack('<I', release))
        uc.mem_write(self.tex_vtable + 8, struct.pack('<I', release))
        uc.mem_write(self.tex_vtable + 0x44, struct.pack('<I', get_level_desc))
        uc.mem_write(self.tex_vtable + 0x48, struct.pack('<I', get_surface_level))
        self.surface_vtable = self.alloc(0x40)
        for i in range(0x10):
            uc.mem_write(self.surface_vtable + i * 4, struct.pack('<I', generic))
        uc.mem_write(self.surface_vtable + 8, struct.pack('<I', release))
        self.fake_surface = self.alloc(0x10)
        uc.mem_write(self.fake_surface, struct.pack('<I', self.surface_vtable))
        # 镜像内函数的整体替换（在函数入口拦截，手动 ret）
        self.replaced = {
            0x477FA7: (0, self.on_malloc),            # malloc(size) cdecl
            0x4749AC: (0, self.on_malloc),            # operator new(size) cdecl
            0x4749DF: (0, lambda o: None),            # operator delete
            0x477920: (0, self.on_memset),            # memset(dst, c, n) cdecl（30 MiB 清零在模拟器中过慢）
            0x402440: (4, self.on_read_file),         # fastcall(name, size*, flag) ret 4
            0x4090D0: (0, self.on_format_name),       # thiscall sprintf(buf, "%s", name) cdecl
            0x402AA0: (8, self.on_error),             # thiscall log(fmt, text) ret 8
            0x470240: (0, lambda o: None),            # 线程亲和/让步
            0x401300: (8, self.on_register),          # 注册到 supervisor 更新链（oracle 直接调 tick，不需要）
            0x4013B0: (8, self.on_register),          # 注册到 supervisor 绘制链
            0x46CB60: (0x14, self.on_create_texture), # THTX → D3D 纹理（stdcall 5 args）
            0x46C920: (0x1C, self.on_create_texture), # PNG → D3D 纹理
            0x46CD30: (0x10, self.on_create_texture), # '@' 运行时纹理
            0x46CD80: (0x0C, self.on_create_texture), # '@R' 渲染目标纹理
        }
        # 代码内跳过：构造尾部 IDirect3DDevice9::SetTexture(0, NULL)（DAT_004c10d8 设备在 oracle 中不存在）
        self.skips = {0x46B741: 0x46B751}
        self.code_hook = uc.hook_add(UC_HOOK_CODE, self.on_code)

    def on_unmapped(self, uc, access, address, size, value, user_data):
        eip = uc.reg_read(UC_X86_REG_EIP)
        self.errors.append('unmapped access %#x (size %d) at eip %#x' % (address, size, eip))
        return False

    def on_code(self, uc, address, size, user_data):
        fn = self.hooks.get(address)
        if fn is not None:
            fn(self)
            return
        skip = self.skips.get(address)
        if skip is not None:
            uc.reg_write(UC_X86_REG_EIP, skip)
            return
        rep = self.replaced.get(address)
        if rep is not None:
            cleanup, handler = rep
            handler(self)
            esp = uc.reg_read(UC_X86_REG_ESP)
            ret = struct.unpack('<I', uc.mem_read(esp, 4))[0]
            uc.reg_write(UC_X86_REG_ESP, esp + 4 + cleanup)
            uc.reg_write(UC_X86_REG_EIP, ret)

    # ---- 栈参数 ----
    def arg(self, n):
        esp = self.uc.reg_read(UC_X86_REG_ESP)
        return struct.unpack('<I', self.uc.mem_read(esp + 4 + n * 4, 4))[0]

    def cstr(self, addr):
        out = bytearray()
        while True:
            c = self.uc.mem_read(addr, 1)[0]
            if c == 0 or len(out) > 512:
                return out.decode('cp932', errors='replace')
            out.append(c)
            addr += 1

    # ---- 桩语义 ----
    def on_malloc(self, o):
        size = self.arg(0)
        self.uc.reg_write(UC_X86_REG_EAX, self.alloc(max(size, 1)))

    def on_register(self, o):
        self.uc.reg_write(UC_X86_REG_EAX, 0)

    def on_memset(self, o):
        dst, value, n = self.arg(0), self.arg(1) & 0xFF, self.arg(2)
        if n:
            self.uc.mem_write(dst, bytes([value]) * n)
        self.uc.reg_write(UC_X86_REG_EAX, dst)

    def on_read_file(self, o):
        name_ptr = self.uc.reg_read(UC_X86_REG_ECX)
        size_ptr = self.uc.reg_read(UC_X86_REG_EDX)
        name = self.cstr(name_ptr)
        self.read_calls += 1
        if name in self.files:
            data = self.files[name]
        else:
            # 外部贴图等：纹理创建已桩掉，只需非空缓冲
            data = b'\0' * 64
            self.errors.append('note: read of %s stubbed' % name)
        p = self.alloc(len(data) + 16)
        self.uc.mem_write(p, data)
        if size_ptr:
            self.uc.mem_write(size_ptr, struct.pack('<I', len(data)))
        self.uc.reg_write(UC_X86_REG_EAX, p)

    def on_format_name(self, o):
        buf = self.arg(0)
        name = self.cstr(self.arg(2))
        self.uc.mem_write(buf, name.encode('cp932') + b'\0')

    def on_error(self, o):
        text = self.cstr(self.arg(1))
        self.errors.append('game error log: ' + text)
        raise RuntimeError('original code reported error: ' + text)

    def on_create_texture(self, o):
        slot = self.arg(0)  # 纹理槽 (0x18 字节)：[0]=IDirect3DTexture9*, [4]=hdr ptr
        hdr = struct.unpack('<I', self.uc.mem_read(slot + 0x10, 4))[0]
        w, h = struct.unpack('<HH', self.uc.mem_read(hdr + 0xA, 4)) if hdr else (0, 0)
        obj = self.alloc(0x20)
        self.uc.mem_write(obj, struct.pack('<IHH', self.tex_vtable, w, h))
        self.uc.mem_write(slot, struct.pack('<I', obj))
        self.textures.append((w, h))
        self.uc.reg_write(UC_X86_REG_EAX, w * h * 4)

    def on_get_level_desc(self, o):
        this = self.arg(0)
        desc = self.arg(2)
        w, h = struct.unpack('<HH', self.uc.mem_read(this + 4, 4))
        self.uc.mem_write(desc + 0x18, struct.pack('<II', w, h))
        self.uc.reg_write(UC_X86_REG_EAX, 0)

    def on_get_surface_level(self, o):
        out = self.arg(2)
        self.uc.mem_write(out, struct.pack('<I', self.fake_surface))
        self.uc.reg_write(UC_X86_REG_EAX, 0)

    # ---- 调用 ----
    def call(self, fn, ecx=0, edx=0, args=(), limit=50_000_000):
        uc = self.uc
        esp = STACK + STACK_SIZE - 0x1000 - 4 * len(args)
        uc.mem_write(esp, struct.pack('<I', SENTINEL) + b''.join(struct.pack('<I', a & 0xFFFFFFFF) for a in args))
        uc.reg_write(UC_X86_REG_ESP, esp)
        uc.reg_write(UC_X86_REG_ECX, ecx)
        uc.reg_write(UC_X86_REG_EDX, edx)
        self.errors = [e for e in self.errors if e.startswith('note')]
        try:
            uc.emu_start(fn, SENTINEL, count=limit)
        except Exception as exc:
            raise RuntimeError('call %#x faulted: %s eip=%#x errors=%s' % (
                fn, exc, uc.reg_read(UC_X86_REG_EIP), self.errors[-4:])) from None
        if uc.reg_read(UC_X86_REG_EIP) != SENTINEL:
            raise RuntimeError('call %#x did not return: eip=%#x errors=%s' % (fn, uc.reg_read(UC_X86_REG_EIP), self.errors))
        return uc.reg_read(UC_X86_REG_EAX)

    def u32(self, addr):
        return struct.unpack('<I', self.uc.mem_read(addr, 4))[0]

    def boot(self):
        self.manager = self.alloc(0x1C7FD90)
        self.call(MANAGER_CTOR, ecx=self.manager)
        self.uc.mem_write(DAT_MANAGER, struct.pack('<I', self.manager))
        # 0x45b280 的窗口配置：后备缓冲 640×480、缩放 1.0（'@R' 渲染目标 entry 的宽高在 0x46d3b0 被改写为此值）
        self.uc.mem_write(DAT_BACKBUFFER_W, struct.pack('<IIf', 640, 480, 1.0))
        # AsciiManager（DAT_004a6d98）：ins_300 负下标回退到其 +0x19240 的 ascii.anm；oracle 只给出这一字段
        self.ascii_manager = self.alloc(0x19260)
        self.uc.mem_write(DAT_ASCII_MANAGER, struct.pack('<I', self.ascii_manager))
        for slot, name, _data in self.extra:
            f = self.load_file(slot, name)
            if slot == ASCII_SLOT:
                self.uc.mem_write(self.ascii_manager + 0x19240, struct.pack('<I', f))
        self.file = self.load_file(0, self.anm_name)

    def load_file(self, slot, name_text):
        name = self.alloc(64)
        self.uc.mem_write(name, name_text.encode('ascii') + b'\0')
        f = self.call(FILE_OPEN, ecx=self.manager, args=(slot, name))
        if not f:
            raise RuntimeError('file open failed: %s' % self.errors)
        self.uc.mem_write(f + 0x4A * 4, struct.pack('<I', 1))
        steps = 0
        while self.u32(f + 0x4A * 4) != 0:
            self.call(LOADER_STEP, args=(f,))
            steps += 1
            if steps > 10000:
                raise RuntimeError('loader did not finish')
        self.loader_steps = steps
        return f

    def spawn(self, script, layer):
        out_id = self.alloc(16)
        out_vm = self.alloc(16)
        self.call(SPAWN, ecx=self.file, args=(out_id, script, layer, out_vm))
        return self.u32(out_id), self.u32(out_vm)

    def tick(self):
        self.call(TICK, ecx=self.manager)

    def snapshot(self):
        vms = []
        node = self.u32(self.manager + WORLD_LIST)
        while node:
            vm = self.u32(node)
            vms.append({'addr': vm, 'id': self.u32(vm + 0x538), 'script': self.u32(vm + 0x24),
                        'bytes': bytes(self.uc.mem_read(vm, VM_SIZE)).hex()})
            node = self.u32(node + 4)
        layers = []
        for layer in range(LAYER_COUNT):
            head = self.manager + LAYER_HEADS + layer * VM_SIZE
            chain = []
            vm = self.u32(head + 0x5A8)
            while vm and len(chain) < 4096:
                chain.append(self.u32(vm + 0x538))
                vm = self.u32(vm + 0x5A8)
            layers.append(chain)
        return vms, layers


    # ---- 装载完成后的检查点：多脚本批量运行时每个脚本都从同一状态开始 ----
    def checkpoint(self):
        uc = self.uc
        self.saved_ctx = uc.context_save()
        self.saved_regions = [(addr, bytes(uc.mem_read(addr, n))) for addr, n in (
            (IMAGE_BASE, (self.pe.OPTIONAL_HEADER.SizeOfImage + 0xFFFF) & ~0xFFFF),
            (HEAP, self.heap_top - HEAP), (STACK, STACK_SIZE), (TEB, 0x1000))]
        self.saved_heap_top = self.heap_top
        self.saved_textures = list(self.textures)

    def restore(self):
        uc = self.uc
        uc.context_restore(self.saved_ctx)
        for addr, data in self.saved_regions:
            uc.mem_write(addr, data)
        self.heap_top = self.saved_heap_top
        self.textures = list(self.saved_textures)
        self.errors = []

    def script_count(self):
        # ANM 文件对象（0x46cdd0 填充）：+0x110 entry 数 +0x114 脚本总数 +0x118 精灵总数
        return self.u32(self.file + 0x114)

    def run_script(self, script, layer, frames):
        vm_id, _vm = self.spawn(script, layer)
        out = []
        vms, layers = self.snapshot()
        out.append({'frame': 0, 'vms': vms, 'layers': layers})
        for n in range(1, frames + 1):
            self.tick()
            vms, layers = self.snapshot()
            out.append({'frame': n, 'vms': vms, 'layers': layers})
        return {'script': script, 'layer': layer, 'spawnedId': vm_id, 'notes': self.errors, 'frames': out}


def parse_scripts(spec, count):
    if spec == 'all':
        return list(range(count))
    out = []
    for part in spec.split(','):
        if '-' in part:
            lo, hi = part.split('-')
            out.extend(range(int(lo), int(hi) + 1))
        else:
            out.append(int(part))
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('exe')
    ap.add_argument('anm_name')
    ap.add_argument('anm_file')
    ap.add_argument('scripts', help="脚本下标：'all' / '3' / '0-20,40'")
    ap.add_argument('frames', type=int)
    ap.add_argument('out')
    ap.add_argument('--layer', type=int, default=0)
    ap.add_argument('--count', type=int, default=None, help='脚本总数（all 时使用；缺省读取文件对象）')
    ap.add_argument('--extra', action='append', default=[], help='先装入的 ANM：slot:name.anm:path（如 2:ascii.anm:dat/ascii.anm）')
    a = ap.parse_args()
    extra = []
    for spec in a.extra:
        slot, name, path = spec.split(':', 2)
        extra.append((int(slot), name, pathlib.Path(path).read_bytes()))
    o = Oracle(pathlib.Path(a.exe), a.anm_name, pathlib.Path(a.anm_file).read_bytes(), extra)
    o.boot()
    o.checkpoint()
    count = a.count if a.count is not None else o.script_count()
    runs = []
    failures = []
    for script in parse_scripts(a.scripts, count):
        o.restore()
        try:
            runs.append(o.run_script(script, a.layer, a.frames))
        except RuntimeError as exc:
            # 需要 oracle 尚未提供的外部子系统（如 EffectManager）：记录并继续，由调用方决定如何处置
            failures.append({'script': script, 'error': str(exc)})
            print('anm-oracle: script %d failed: %s' % (script, exc), file=sys.stderr)
    result = {'source': 'th16.exe v1.00a', 'anm': a.anm_name, 'layer': a.layer, 'frames': a.frames,
              'loaderSteps': o.loader_steps, 'textures': o.saved_textures, 'runs': runs, 'failures': failures}
    pathlib.Path(a.out).write_text(json.dumps(result))
    print('anm-oracle: %s %d scripts × %d frames → %s' % (a.anm_name, len(runs), a.frames, a.out))


if __name__ == '__main__':
    main()
