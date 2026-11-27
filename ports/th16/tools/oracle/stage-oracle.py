#!/usr/bin/env python3
"""Unicorn oracle：在原版 th16.exe 代码上创建 StageManager（0x409db0）、首帧 0x40add0、逐帧 0x409e50，导出关卡状态。

用法：
    stage-oracle.py <th16.exe> <stage 1..7> <stNN.std bytes> <stNNwl.anm bytes> <text.anm bytes> <frames> <out.json>
    stage-oracle.py --digest <out.json...>        → 打印与 harness `stage-digest` 同口径的 FNV-1a 64 摘要

执行链（原版代码；桩同 anm-oracle.py，另加 0x46d020 AnmManager::load_file → 预装 bank 槽指针）：
    0x46a3a0  AnmManager 构造；背景 ANM 经 0x46cf80 / 0x46d1c0 预装进槽 3 + (stage & 1)
    0x409db0  operator new(0x66f0) → 0x409670 预构造（8 层精灵 VM + 面 VM 表）→ 0x4097c0：
              DAT_004a5790 关卡号 → 0x40ac30 读 STD（0x402440 桩）+ 0x46d020（桩）→ 相机模板 0x4c168c 拷贝 → 链登记（桩）
    0x40add0  面 VM init_from_template（0x407b20）+ 首跑（0x45f980）
    0x409e50  每帧：facing_norm、0x40aed0 面 VM、0x40b3b0 脚本/摇晃、层精灵 VM tick、0x40c4a0 畸变

输出 JSON 与 harness `stage-run` 同构：
    {"stage":N,"frames":[{"frame":n,"timer":[cur,cur_f_bits],"ins":byte_offset,"rocking":[mode,timer],"stage_frame":..,"flags":..,"bg":..,
      "cam":{"pos":[bits×3],"facing":..,"up":..,"facing_norm":..,"pos_offset":..,"facing_offset":..,"fov":bits},
      "fog":[near,far,c0,c1,c2,c3,color],"layer_ids":[8],"radius_sq":bits,"faces":[hex 0x568 …],"layers":[hex 0x568 × 8]}]}
"""
import importlib.util, json, math, pathlib, struct, sys

HERE = pathlib.Path(__file__).resolve().parent
spec = importlib.util.spec_from_file_location('anm_oracle', HERE / 'anm-oracle.py')
anm_oracle = importlib.util.module_from_spec(spec)
spec.loader.exec_module(anm_oracle)
Oracle = anm_oracle.Oracle
from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_ECX

STAGE_CREATE, STAGE_START, STAGE_UPDATE = 0x409DB0, 0x40ADD0, 0x409E50
ANM_LOAD_FILE = 0x46D020
IAT_VEC3_NORMALIZE = 0x48B264
DAT_STAGE_NUMBER = 0x4A5790
DAT_STAGE_MANAGER = 0x4A6DA0
DAT_CAMERA_TEMPLATE = 0x4C168C
DAT_BG_COLOR = 0x4C1B0C
BANK_ARRAY = 0x184F4F0
VM_SNAPSHOT = 0x568
VM_SIZE = 0x5FC

# StageManager 偏移
OFF_TIMER, OFF_INS, OFF_ROCK_MODE, OFF_ROCK_TIMER = 0x0C, 0x20, 0x24, 0x28
OFF_CAMERA, OFF_LAYER_VMS, OFF_LAYER_IDS, OFF_RADIUS = 0x1EC, 0x31C, 0x32FC, 0x331C
OFF_FACE_VMS, OFF_STD, OFF_FLAGS, OFF_FRAME = 0x669C, 0x66A4, 0x66C4, 0x66E0


def f32(v):
    return struct.unpack('<f', struct.pack('<f', v))[0]


class StageOracle(Oracle):
    def __init__(self, exe, stage, std_bytes, anm_name, anm_bytes, text_bytes):
        super().__init__(exe, anm_name, anm_bytes)
        self.files['text.anm'] = text_bytes
        self.stage = stage
        self.slot = 3 + (stage & 1)
        self.std_name = 'st%02d.std' % stage
        self.files[self.std_name] = std_bytes
        self.replaced[ANM_LOAD_FILE] = (8, self.on_anm_load_file)  # stdcall(slot, name) ret 8
        # d3dx9_43.dll 不在镜像内：D3DXVec3Normalize(out*, in*) stdcall ret 8，以 float32 x/|v| 实现
        addr = self.stub(20)
        self.uc.mem_write(addr, bytes([0xC2, 0x08, 0x00]))
        self.hooks[addr] = self.on_vec3_normalize
        self.uc.mem_write(IAT_VEC3_NORMALIZE, struct.pack('<I', addr))

    def on_vec3_normalize(self, o):
        out, src = self.arg(0), self.arg(1)
        x, y, z = struct.unpack('<3f', self.uc.mem_read(src, 12))
        length = f32(math.sqrt(f32(f32(f32(x * x) + f32(y * y)) + f32(z * z))))
        if length != 0.0:
            x, y, z = f32(x / length), f32(y / length), f32(z / length)
        self.uc.mem_write(out, struct.pack('<3f', x, y, z))
        self.uc.reg_write(UC_X86_REG_EAX, out)

    def on_anm_load_file(self, o):
        # AnmManager::load_file(slot, name)：原版同步等待装载线程；oracle 已预装，直接返回 bank 槽指针
        slot = self.arg(0)
        name = self.cstr(self.arg(1))
        if name != self.anm_name or slot != self.slot:
            raise RuntimeError('unexpected ANM load %s → slot %d' % (name, slot))
        self.uc.reg_write(UC_X86_REG_EAX, self.u32(self.manager + BANK_ARRAY + slot * 4))

    def boot(self):
        self.manager = self.alloc(0x1C7FD90)
        self.call(anm_oracle.MANAGER_CTOR, ecx=self.manager)
        self.uc.mem_write(anm_oracle.DAT_MANAGER, struct.pack('<I', self.manager))
        self.uc.mem_write(anm_oracle.DAT_BACKBUFFER_W, struct.pack('<IIf', 640, 480, 1.0))
        self.ascii_manager = self.alloc(0x19260)
        self.uc.mem_write(anm_oracle.DAT_ASCII_MANAGER, struct.pack('<I', self.ascii_manager))
        # 原版启动 0x43adc0 先把 text.anm 装进槽 0；StageManager 的全零层精灵 VM（file_slot 0）每帧推进其脚本 0
        self.load_file(0, 'text.anm')
        self.file = self.load_file(self.slot, self.anm_name)
        # 0x43cb10 为 3D 相机 0x4c168c 设的模板：fov π/6、视口 (116,4,408,472)、kind 3（StageManager 构造后只保留这些）
        cam = bytearray(0x12C)
        struct.pack_into('<f', cam, 0x15 * 4, 0.5235988)
        struct.pack_into('<4I2f', cam, 0x38 * 4, 116, 4, 408, 472, 0.0, 1.0)
        struct.pack_into('<I', cam, 0x3E * 4, 3)
        self.uc.mem_write(DAT_CAMERA_TEMPLATE, bytes(cam))
        self.uc.mem_write(DAT_STAGE_NUMBER, struct.pack('<I', self.stage))
        # 0x43b520（Supervisor 初始化）把场地清色 DAT_004c1b0c 置为不透明黑；关卡脚本 ins 13 再改写
        self.uc.mem_write(DAT_BG_COLOR, struct.pack('<I', 0xFF000000))
        name = self.alloc(64)
        self.uc.mem_write(name, self.std_name.encode('ascii') + b'\0')
        self.stage_manager = self.call(STAGE_CREATE, ecx=name)
        if not self.stage_manager:
            raise RuntimeError('StageManager creation failed: %s' % self.errors)
        if self.u32(DAT_STAGE_MANAGER) != self.stage_manager:
            raise RuntimeError('DAT_004a6da0 not set')
        std = self.u32(self.stage_manager + OFF_STD)
        self.face_count = struct.unpack('<h', self.uc.mem_read(std + 2, 2))[0]
        self.call(STAGE_START)

    def tick(self):
        self.call(STAGE_UPDATE, ecx=self.stage_manager)

    def f32bits(self, addr):
        return self.u32(addr)

    def vec3(self, addr):
        return [self.u32(addr + 4 * i) for i in range(3)]

    def snapshot(self, frame):
        sm = self.stage_manager
        cam = sm + OFF_CAMERA
        faces_ptr = self.u32(sm + OFF_FACE_VMS)
        return {
            'frame': frame,
            'timer': [struct.unpack('<i', self.uc.mem_read(sm + OFF_TIMER + 4, 4))[0], self.u32(sm + OFF_TIMER + 8)],
            'ins': struct.unpack('<i', self.uc.mem_read(sm + OFF_INS, 4))[0],
            'rocking': [struct.unpack('<i', self.uc.mem_read(sm + OFF_ROCK_MODE, 4))[0],
                        struct.unpack('<i', self.uc.mem_read(sm + OFF_ROCK_TIMER + 4, 4))[0]],
            'stage_frame': struct.unpack('<i', self.uc.mem_read(sm + OFF_FRAME, 4))[0],
            'flags': self.u32(sm + OFF_FLAGS),
            'bg': self.u32(DAT_BG_COLOR),
            'cam': {
                'pos': self.vec3(cam), 'facing': self.vec3(cam + 0xC), 'up': self.vec3(cam + 0x18),
                'facing_norm': self.vec3(cam + 0x24), 'pos_offset': self.vec3(cam + 0x3C), 'facing_offset': self.vec3(cam + 0x48),
                'fov': self.u32(cam + 0x15 * 4),
            },
            'fog': [self.u32(cam + (0x44 + i) * 4) for i in range(7)],
            'layer_ids': [struct.unpack('<i', self.uc.mem_read(sm + OFF_LAYER_IDS + 4 * i, 4))[0] for i in range(8)],
            'radius_sq': self.u32(sm + OFF_RADIUS),
            'faces': [bytes(self.uc.mem_read(faces_ptr + i * VM_SIZE, VM_SNAPSHOT)).hex() for i in range(self.face_count)],
            'layers': [bytes(self.uc.mem_read(sm + OFF_LAYER_VMS + i * VM_SIZE, VM_SNAPSHOT)).hex() for i in range(8)],
        }


def frame_digest(frames, digest=0xCBF29CE484222325):
    """harness `stage-digest` 的规范字节流 FNV-1a 64（小端 u32 头 + 面 VM / 层精灵 VM 原始字节）"""
    mask = (1 << 64) - 1
    def feed(data):
        nonlocal digest
        for b in data:
            digest = ((digest ^ b) * 0x100000001B3) & mask
    for f in frames:
        head = [f['frame'], f['timer'][0], f['timer'][1], f['ins'], f['rocking'][0], f['rocking'][1], f['stage_frame'], f['flags'], f['bg']]
        cam = f['cam']
        words = head + cam['pos'] + cam['facing'] + cam['up'] + cam['facing_norm'] + cam['pos_offset'] + cam['facing_offset'] + [cam['fov']]
        words += f['fog'] + f['layer_ids'] + [f['radius_sq']]
        feed(b''.join(struct.pack('<I', w & 0xFFFFFFFF) for w in words))
        for vm in f['faces']:
            feed(bytes.fromhex(vm))
        for vm in f['layers']:
            feed(bytes.fromhex(vm))
    return digest


def main():
    if len(sys.argv) >= 3 and sys.argv[1] == '--digest':
        # 已有完整快照 → 摘要（给 tests/golden/stage.json）
        for path in sys.argv[2:]:
            data = json.loads(pathlib.Path(path).read_text())
            print('stage=%d frames=%d faces=%d digest=%016x' % (data['stage'], len(data['frames']), len(data['frames'][0]['faces']), frame_digest(data['frames'])))
        return 0
    if len(sys.argv) < 8:
        print(__doc__)
        return 2
    exe, stage, std_path, anm_path, text_path, frames, out = sys.argv[1:8]
    stage = int(stage)
    anm_name = 'st%02dwl.anm' % stage
    oracle = StageOracle(pathlib.Path(exe), stage, pathlib.Path(std_path).read_bytes(), anm_name, pathlib.Path(anm_path).read_bytes(),
                         pathlib.Path(text_path).read_bytes())
    oracle.boot()
    result = {'stage': stage, 'std': oracle.std_name, 'anm': anm_name, 'frames': []}
    for f in range(int(frames)):
        oracle.tick()
        result['frames'].append(oracle.snapshot(f + 1))
    pathlib.Path(out).write_text(json.dumps(result, separators=(',', ':')))
    notes = [e for e in oracle.errors if not e.startswith('note')]
    print('stage %d: %d frames, %d face VMs, digest=%016x errors=%s' % (stage, int(frames), oracle.face_count, frame_digest(result['frames']), notes[:3]))
    return 0


if __name__ == '__main__':
    sys.exit(main())
