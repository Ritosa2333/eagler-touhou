#!/usr/bin/env python3
"""IDirect3DDevice9 录制桩：game-oracle.py --draw 时替换伪设备，把原版绘制链（0x4015a0）对设备的每次调用记成
可对拍的数据流——渲染状态 / 纹理阶段状态 / 采样器状态 / 纹理 / 变换矩阵 / 视口 / 渲染目标 / Clear，以及每次
DrawPrimitive(UP) 时的"生效状态 + 顶点字节"。移植版 harness `game-draw` 产出同口径数据，由 draw-compare.py 对拍。

纹理身份：ANM 纹理由 0x46d3b0（装载 ANM 第 n 张纹理）入口钩子记下 (bank, entry)，随后 0x46cb60 / 0x46c920 /
0x46cd30 / 0x46cd80 建伪纹理对象时绑定；SetTexture 记录为 [槽号, entry 序号]。非 ANM 的 CreateTexture（文字贴图、
截屏）记为 ["tex", 序号]。渲染目标 '@R' 纹理各有独立伪表面，SetRenderTarget 据此记录目标纹理。

顶点缓冲：CreateVertexBuffer 返回带 Lock / Unlock 的伪对象（0x46b900 的 mode 8 九锚点四边形 0x2d0 字节），
DrawPrimitive 从当前 SetStreamSource 的伪缓冲取字节。

每帧输出一条 JSON：{"frame": n, "calls": [...]}，调用项：
    ["rs", state, value] ["tss", stage, type, value] ["ss", sampler, type, value] ["tex", stage, id|null]
    ["fvf", fvf] ["xf", state, [16 个 float 位型]] ["vp", [x, y, w, h, minz 位型, maxz 位型]] ["rt", id]
    ["clear", flags, color, rect|null] ["begin"] ["end"]
    ["draw", prim_type, prim_count, fvf, stride, 顶点 u32 字数组]（DrawPrimitiveUP 与 DrawPrimitive 同口径：
    后者按 start/count 从流缓冲切出）
"""
import struct

from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_ESP

# IDirect3DDevice9 vtable 偏移 → (名称, stdcall 参数字节数含 this)
DEVICE_METHODS = {
    0x00: ('QueryInterface', 12), 0x04: ('AddRef', 4), 0x08: ('Release', 4),
    0x0C: ('TestCooperativeLevel', 4), 0x10: ('GetAvailableTextureMem', 4), 0x14: ('EvictManagedResources', 4),
    0x18: ('GetDirect3D', 8), 0x1C: ('GetDeviceCaps', 8), 0x20: ('GetDisplayMode', 12),
    0x24: ('GetCreationParameters', 8), 0x28: ('SetCursorProperties', 16), 0x2C: ('SetCursorPosition', 16),
    0x30: ('ShowCursor', 8), 0x34: ('CreateAdditionalSwapChain', 12), 0x38: ('GetSwapChain', 12),
    0x3C: ('GetNumberOfSwapChains', 4), 0x40: ('Reset', 8), 0x44: ('Present', 20), 0x48: ('GetBackBuffer', 20),
    0x4C: ('GetRasterStatus', 12), 0x50: ('SetDialogBoxMode', 8), 0x54: ('SetGammaRamp', 16),
    0x58: ('GetGammaRamp', 12), 0x5C: ('CreateTexture', 36), 0x60: ('CreateVolumeTexture', 40),
    0x64: ('CreateCubeTexture', 32), 0x68: ('CreateVertexBuffer', 28), 0x6C: ('CreateIndexBuffer', 28),
    0x70: ('CreateRenderTarget', 36), 0x74: ('CreateDepthStencilSurface', 36), 0x78: ('UpdateSurface', 20),
    0x7C: ('UpdateTexture', 12), 0x80: ('GetRenderTargetData', 12), 0x84: ('GetFrontBufferData', 12),
    0x88: ('StretchRect', 24), 0x8C: ('ColorFill', 16), 0x90: ('CreateOffscreenPlainSurface', 28),
    0x94: ('SetRenderTarget', 12), 0x98: ('GetRenderTarget', 12), 0x9C: ('SetDepthStencilSurface', 8),
    0xA0: ('GetDepthStencilSurface', 8), 0xA4: ('BeginScene', 4), 0xA8: ('EndScene', 4), 0xAC: ('Clear', 28),
    0xB0: ('SetTransform', 12), 0xB4: ('GetTransform', 12), 0xB8: ('MultiplyTransform', 12),
    0xBC: ('SetViewport', 8), 0xC0: ('GetViewport', 8), 0xC4: ('SetMaterial', 8), 0xC8: ('GetMaterial', 8),
    0xCC: ('SetLight', 12), 0xD0: ('GetLight', 12), 0xD4: ('LightEnable', 12), 0xD8: ('GetLightEnable', 12),
    0xDC: ('SetClipPlane', 12), 0xE0: ('GetClipPlane', 12), 0xE4: ('SetRenderState', 12),
    0xE8: ('GetRenderState', 12), 0xEC: ('CreateStateBlock', 12), 0xF0: ('BeginStateBlock', 4),
    0xF4: ('EndStateBlock', 8), 0xF8: ('SetClipStatus', 8), 0xFC: ('GetClipStatus', 8), 0x100: ('GetTexture', 12),
    0x104: ('SetTexture', 12), 0x108: ('GetTextureStageState', 16), 0x10C: ('SetTextureStageState', 16),
    0x110: ('GetSamplerState', 16), 0x114: ('SetSamplerState', 16), 0x118: ('ValidateDevice', 8),
    0x11C: ('SetPaletteEntries', 12), 0x120: ('GetPaletteEntries', 12), 0x124: ('SetCurrentTexturePalette', 8),
    0x128: ('GetCurrentTexturePalette', 8), 0x12C: ('SetScissorRect', 8), 0x130: ('GetScissorRect', 8),
    0x134: ('SetSoftwareVertexProcessing', 8), 0x138: ('GetSoftwareVertexProcessing', 4),
    0x13C: ('SetNPatchMode', 8), 0x140: ('GetNPatchMode', 4), 0x144: ('DrawPrimitive', 16),
    0x148: ('DrawIndexedPrimitive', 32), 0x14C: ('DrawPrimitiveUP', 20), 0x150: ('DrawIndexedPrimitiveUP', 36),
    0x154: ('ProcessVertices', 28), 0x158: ('CreateVertexDeclaration', 12), 0x15C: ('SetVertexDeclaration', 8),
    0x160: ('GetVertexDeclaration', 8), 0x164: ('SetFVF', 8), 0x168: ('GetFVF', 8),
    0x16C: ('CreateVertexShader', 12), 0x170: ('SetVertexShader', 8), 0x174: ('GetVertexShader', 8),
    0x178: ('SetVertexShaderConstantF', 16), 0x17C: ('GetVertexShaderConstantF', 16),
    0x180: ('SetVertexShaderConstantI', 16), 0x184: ('GetVertexShaderConstantI', 16),
    0x188: ('SetVertexShaderConstantB', 16), 0x18C: ('GetVertexShaderConstantB', 16),
    0x190: ('SetStreamSource', 20), 0x194: ('GetStreamSource', 20), 0x198: ('SetStreamSourceFreq', 12),
    0x19C: ('GetStreamSourceFreq', 12), 0x1A0: ('SetIndices', 8), 0x1A4: ('GetIndices', 8),
    0x1A8: ('CreatePixelShader', 12), 0x1AC: ('SetPixelShader', 8), 0x1B0: ('GetPixelShader', 8),
    0x1B4: ('SetPixelShaderConstantF', 16), 0x1B8: ('GetPixelShaderConstantF', 16),
    0x1BC: ('SetPixelShaderConstantI', 16), 0x1C0: ('GetPixelShaderConstantI', 16),
    0x1C4: ('SetPixelShaderConstantB', 16), 0x1C8: ('GetPixelShaderConstantB', 16),
    0x1CC: ('DrawRectPatch', 16), 0x1D0: ('DrawTriPatch', 16), 0x1D4: ('DeletePatch', 8), 0x1D8: ('CreateQuery', 12),
}

# FVF → 顶点步长（原版只用这四种）
FVF_STRIDE = {0x44: 0x14, 0x144: 0x1C, 0x142: 0x18, 0x102: 0x14}


class DeviceRecorder:
    def __init__(self, oracle):
        self.o = oracle
        self.uc = oracle.uc
        self.calls = []           # 本帧调用序列
        self.enabled = False      # 只在绘制阶段记录（更新链里 0x43c4f0 / 0x465a80 等也会碰设备）
        self.textures = {}        # 伪纹理对象地址 → 身份（[slot, entry] 或 ["tex", n]）
        self.surfaces = {}        # 伪表面地址 → 所属纹理对象地址
        self.buffers = {}         # 伪顶点 / 索引缓冲对象地址 → 数据区地址
        self.stream = None        # SetStreamSource(0) 的 (缓冲地址, offset, stride)
        self.anonymous_textures = 0
        self.pending_anm_texture = None   # 0x46d3b0 记下的 (bank 地址, entry 序号)
        self.backbuffer = oracle.alloc(0x10)
        self.surfaces[self.backbuffer] = 'backbuffer'
        self.surface_pixels = {}
        self.install()

    # ---- 安装 ----
    def install(self):
        o = self.o
        uc = self.uc
        # 设备 vtable：全部方法先指向"返回 0"，再逐个换成记录桩
        vt = o.alloc(0x200)
        for off, (name, nbytes) in DEVICE_METHODS.items():
            handler = getattr(self, 'm_' + name, None)
            fn = (lambda h: (lambda _o: self.dispatch(h)))(handler) if handler else (lambda _o: self.ret(0))
            uc.mem_write(vt + off, struct.pack('<I', o.make_stub2(nbytes, fn)))
        dev = o.alloc(0x10)
        uc.mem_write(dev, struct.pack('<I', vt))
        uc.mem_write(0x4C10D8, struct.pack('<I', dev))
        self.device = dev
        # 伪纹理 vtable：GetLevelDesc / GetSurfaceLevel 走录制器（每张纹理独立表面）；Release 返回 0
        self.tex_vtable = o.alloc(0x100)
        generic = o.make_stub2(0, lambda _o: self.ret(0))
        release = o.make_stub2(4, lambda _o: self.ret(0))
        for i in range(0x40):
            uc.mem_write(self.tex_vtable + i * 4, struct.pack('<I', generic))
        uc.mem_write(self.tex_vtable + 4, struct.pack('<I', release))
        uc.mem_write(self.tex_vtable + 8, struct.pack('<I', release))
        # IDirect3DResource9::PreLoad 也要清理栈上的 this。
        uc.mem_write(self.tex_vtable + 0x24, struct.pack('<I', release))
        uc.mem_write(self.tex_vtable + 0x44, struct.pack('<I', o.make_stub2(0xC, lambda _o: self.tex_get_level_desc())))
        uc.mem_write(self.tex_vtable + 0x48, struct.pack('<I', o.make_stub2(0xC, lambda _o: self.tex_get_surface_level())))
        # Manual 清除可锁定的目录正文表面。提供真实尺寸和独立的临时像素缓冲。
        self.surface_vtable = o.alloc(0x80)
        for i in range(0x20):
            uc.mem_write(self.surface_vtable + i * 4, struct.pack('<I', generic))
        uc.mem_write(self.surface_vtable + 8, struct.pack('<I', release))
        uc.mem_write(self.surface_vtable + 0x30, struct.pack('<I', o.make_stub2(8, lambda _o: self.surface_get_desc())))
        uc.mem_write(self.surface_vtable + 0x34, struct.pack('<I', o.make_stub2(16, lambda _o: self.surface_lock())))
        uc.mem_write(self.surface_vtable + 0x38, struct.pack('<I', o.make_stub2(4, lambda _o: self.ret(0))))
        uc.mem_write(self.backbuffer, struct.pack('<I', self.surface_vtable))
        # 伪缓冲 vtable：Lock(this, offset, size, out**, flags) RET 0x14；Unlock(this) RET 4；Release RET 4
        self.buffer_vtable = o.alloc(0x80)
        for i in range(0x20):
            uc.mem_write(self.buffer_vtable + i * 4, struct.pack('<I', generic))
        uc.mem_write(self.buffer_vtable + 8, struct.pack('<I', release))
        uc.mem_write(self.buffer_vtable + 0x2C, struct.pack('<I', o.make_stub2(0x14, lambda _o: self.buffer_lock())))
        uc.mem_write(self.buffer_vtable + 0x30, struct.pack('<I', o.make_stub2(4, lambda _o: self.ret(0))))
        # ANM 纹理：0x46d3b0(bank, entry, ...) 入口观察（搬走序言 push ebp / mov ebp,esp / sub esp,0x88）→
        # 绑定随后创建的纹理对象；'@' / '@R' 运行时纹理没有 THTX 头，尺寸取自参数
        o.splice_hook(0x46D3B0, bytes.fromhex('558bec81ec88000000'), lambda _o: self.on_anm_texture_entry())
        for addr, size_from_args in ((0x46CB60, False), (0x46C920, False), (0x46CD30, True), (0x46CD80, True)):
            o.replaced[addr] = (o.replaced[addr][0],
                                (lambda f: (lambda _o: self.on_create_anm_texture(f)))(size_from_args))

    # ---- 工具 ----
    def ret(self, value):
        self.uc.reg_write(UC_X86_REG_EAX, value)

    def arg(self, n):
        return self.o.arg(n)

    def dispatch(self, handler):
        handler()

    def record(self, item):
        if self.enabled:
            self.calls.append(item)

    def u32(self, addr):
        return struct.unpack('<I', self.uc.mem_read(addr, 4))[0]

    def texture_id(self, obj):
        if obj == 0:
            return None
        return self.textures.get(obj, ['unknown', obj])

    # ---- 纹理 ----
    def on_anm_texture_entry(self):
        # 0x46d3b0(int* bank, uint entry, int, void*, int* thtx)：cdecl，参数从 esp+4 起
        bank, entry = self.arg(0), self.arg(1)
        self.pending_anm_texture = (bank, entry)

    def bank_slot(self, bank):
        o = self.o
        base = o.manager + 0x184F4F0   # BANK_ARRAY
        for slot in range(0x20):
            if self.u32(base + slot * 4) == bank:
                return slot
        return -1

    def on_create_anm_texture(self, size_from_args):
        # 0x46cb60 / 0x46c920(slot*, ...)：slot 为纹理槽 (0x18 字节)，+0x10 = THTX 头（+0xa 宽、+0xc 高）；
        # 0x46cd30 / 0x46cd80(slot*, w, h, ...)：尺寸在参数里
        slot = self.arg(0)
        if size_from_args:
            w, h = self.arg(1), self.arg(2)
        else:
            hdr = self.u32(slot + 0x10)
            w, h = struct.unpack('<HH', self.uc.mem_read(hdr + 0xA, 4)) if hdr else (0, 0)
        obj = self.new_texture(w, h)
        self.uc.mem_write(slot, struct.pack('<I', obj))
        if self.pending_anm_texture is not None:
            bank, entry = self.pending_anm_texture
            self.textures[obj] = [self.bank_slot(bank), entry]
            self.pending_anm_texture = None
        self.o.textures.append((w, h))
        self.ret(w * h * 4)

    def new_texture(self, w, h):
        obj = self.o.alloc(0x20)
        surface = self.o.alloc(0x10)
        self.uc.mem_write(surface, struct.pack('<I', self.surface_vtable))
        self.uc.mem_write(obj, struct.pack('<IHHI', self.tex_vtable, w, h, surface))
        self.surfaces[surface] = obj
        self.textures[obj] = ['tex', self.anonymous_textures]
        self.anonymous_textures += 1
        return obj

    def tex_get_level_desc(self):
        this, desc = self.arg(0), self.arg(2)
        w, h = struct.unpack('<HH', self.uc.mem_read(this + 4, 4))
        self.uc.mem_write(desc + 0x18, struct.pack('<II', w, h))
        self.ret(0)

    def tex_get_surface_level(self):
        this, out = self.arg(0), self.arg(2)
        self.uc.mem_write(out, struct.pack('<I', self.u32(this + 8)))
        self.ret(0)

    def surface_size(self, surface):
        owner = self.surfaces[surface]
        if owner == 'backbuffer':
            return struct.unpack('<II', self.uc.mem_read(0x4D9D2C, 8))
        return struct.unpack('<HH', self.uc.mem_read(owner + 4, 4))

    def surface_get_desc(self):
        width, height = self.surface_size(self.arg(0))
        # 临时缓冲采用 A8R8G8B8；录制器对照的是绘制调用，不比对表面像素。
        self.uc.mem_write(self.arg(1), struct.pack('<8I', 21, 1, 0, 0, 0, 0, width, height))
        self.ret(0)

    def surface_lock(self):
        surface, out, rect = self.arg(0), self.arg(1), self.arg(2)
        if rect:
            raise RuntimeError('surface lock: partial rectangles are not supported')
        width, height = self.surface_size(surface)
        pitch = width * 4
        if surface not in self.surface_pixels:
            self.surface_pixels[surface] = self.o.alloc(pitch * height)
        self.uc.mem_write(out, struct.pack('<II', pitch, self.surface_pixels[surface]))
        self.ret(0)

    def m_CreateTexture(self):
        # (this, w, h, levels, usage, format, pool, IDirect3DTexture9** out, handle*)
        w, h, out = self.arg(1), self.arg(2), self.arg(7)
        obj = self.new_texture(w, h)
        self.uc.mem_write(out, struct.pack('<I', obj))
        self.ret(0)

    # ---- 缓冲 ----
    def new_buffer(self, size, out):
        obj = self.o.alloc(0x10)
        data = self.o.alloc(size + 16)
        self.uc.mem_write(obj, struct.pack('<II', self.buffer_vtable, size))
        self.buffers[obj] = data
        self.uc.mem_write(out, struct.pack('<I', obj))
        self.ret(0)

    def m_CreateVertexBuffer(self):
        # (this, length, usage, fvf, pool, out**, handle*)
        self.new_buffer(self.arg(1), self.arg(5))

    def m_CreateIndexBuffer(self):
        self.new_buffer(self.arg(1), self.arg(5))

    def buffer_lock(self):
        # (this, offset, size, void** out, flags)
        this, offset, out = self.arg(0), self.arg(1), self.arg(3)
        self.uc.mem_write(out, struct.pack('<I', self.buffers[this] + offset))
        self.ret(0)

    def m_SetStreamSource(self):
        # (this, stream, vb*, offset, stride)
        if self.arg(1) == 0:
            vb = self.arg(2)
            self.stream = (self.buffers.get(vb), self.arg(3), self.arg(4))
        self.ret(0)

    # ---- 状态 ----
    def m_SetRenderState(self):
        self.record(['rs', self.arg(1), self.arg(2)])
        self.ret(0)

    def m_SetTextureStageState(self):
        self.record(['tss', self.arg(1), self.arg(2), self.arg(3)])
        self.ret(0)

    def m_SetSamplerState(self):
        self.record(['ss', self.arg(1), self.arg(2), self.arg(3)])
        self.ret(0)

    def m_SetTexture(self):
        self.record(['tex', self.arg(1), self.texture_id(self.arg(2))])
        self.ret(0)

    def m_SetFVF(self):
        self.fvf = self.arg(1)
        self.record(['fvf', self.fvf])
        self.ret(0)

    def m_SetTransform(self):
        m = struct.unpack('<16I', self.uc.mem_read(self.arg(2), 64))
        self.record(['xf', self.arg(1), list(m)])
        self.ret(0)

    def m_SetViewport(self):
        vp = struct.unpack('<6I', self.uc.mem_read(self.arg(1), 24))
        self.record(['vp', list(vp)])
        self.ret(0)

    def m_SetRenderTarget(self):
        # 目标记为 "backbuffer" 或所属纹理身份（'@R' 场地渲染目标 = text.anm [0,2] / [0,3]）
        surface = self.arg(2)
        owner = self.surfaces.get(surface, ['unknown-surface', surface])
        self.record(['rt', self.texture_id(owner) if isinstance(owner, int) else owner])
        self.ret(0)

    def m_GetRenderTarget(self):
        self.uc.mem_write(self.arg(2), struct.pack('<I', self.backbuffer))
        self.ret(0)

    def m_GetBackBuffer(self):
        # (this, swapchain, index, type, IDirect3DSurface9** out)：0x43d970 / 0x43ba40 取后台缓冲表面存入 DAT_004c1284
        self.uc.mem_write(self.arg(4), struct.pack('<I', self.backbuffer))
        self.ret(0)

    def m_Clear(self):
        # (this, count, rects*, flags, color, z, stencil)
        count, rects, flags, color = self.arg(1), self.arg(2), self.arg(3), self.arg(4)
        rect = list(struct.unpack('<4i', self.uc.mem_read(rects, 16))) if count and rects else None
        self.record(['clear', flags, color, rect])
        self.ret(0)

    def m_BeginScene(self):
        self.record(['begin'])
        self.ret(0)

    def m_EndScene(self):
        self.record(['end'])
        self.ret(0)

    def m_SetVertexShader(self):
        self.ret(0)

    # ---- 绘制 ----
    fvf = 0

    def vertex_words(self, data_addr, stride, count):
        raw = self.uc.mem_read(data_addr, stride * count)
        return list(struct.unpack('<%dI' % (len(raw) // 4), bytes(raw)))

    @staticmethod
    def vertex_count(prim_type, prim_count):
        # D3DPT_POINTLIST 1 / LINELIST 2 / LINESTRIP 3 / TRIANGLELIST 4 / TRIANGLESTRIP 5 / TRIANGLEFAN 6
        return {1: prim_count, 2: prim_count * 2, 3: prim_count + 1, 4: prim_count * 3,
                5: prim_count + 2, 6: prim_count + 2}[prim_type]

    def m_DrawPrimitiveUP(self):
        # (this, type, prim_count, data*, stride)
        prim_type, prim_count, data, stride = self.arg(1), self.arg(2), self.arg(3), self.arg(4)
        n = self.vertex_count(prim_type, prim_count)
        self.record(['draw', prim_type, prim_count, self.fvf, stride, self.vertex_words(data, stride, n)])
        self.ret(0)

    def m_DrawPrimitive(self):
        # (this, type, start_vertex, prim_count)：顶点取自 SetStreamSource(0) 的伪缓冲
        prim_type, start, prim_count = self.arg(1), self.arg(2), self.arg(3)
        n = self.vertex_count(prim_type, prim_count)
        data, offset, stride = self.stream
        self.record(['draw', prim_type, prim_count, self.fvf, stride,
                     self.vertex_words(data + offset + start * stride, stride, n)])
        self.ret(0)

    # ---- 帧 ----
    def begin_frame(self):
        self.calls = []
        self.enabled = True

    def end_frame(self):
        self.enabled = False
        calls, self.calls = self.calls, []
        return calls
