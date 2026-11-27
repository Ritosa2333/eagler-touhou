#!/usr/bin/env python3
"""Unicorn oracle：在原版 th16.exe 代码上跑音效请求队列，导出 IDirectSoundBuffer 调用序列。

用法：
    sound-oracle.py <th16.exe> <ops> [--master N] [--disabled] [--missing v,v,...]

ops 以逗号分隔（与 harness `sound-script` 同口径）：
    p:<id>        0x45e150(id)          播放（声像 0）
    a:<id>:<x>    0x45e1f0(id, x)       按 x（float，XMM2）定位播放
    s:<id>        0x45e2a0(id)          停止（id < 0 立即停全部）
    P             0x45e330              帧尾消费队列（music 队列为空，直接落到音效段）
    S             0x42e4d0              暂停
    R             0x440bd0              恢复

环境：0x45d510 的声部表照原样布置——78 个声部（DAT_004db894，0x18 字节）各挂一只假缓冲
（--missing 列出的声部不挂），+4 = -1、+8 = 声部号匹配的 DAT_004a2a30 行、+0xc = 声部号；
队列 DAT_004d9e2c 置 -1；DAT_004d9e1c（设备）非 0、DAT_004c12c2 = 音效开、DAT_004df4a0 = 音效音量。
假缓冲方法（stdcall）：+0x24 GetStatus、+0x30 Play、+0x34 SetCurrentPosition、+0x3c SetVolume、
+0x40 SetPan、+0x48 Stop；Play 置“在播”、Stop 清除。每次调用输出一行 "<voice> <方法> [参数]"。
"""
import argparse, importlib.util, pathlib, struct, sys

HERE = pathlib.Path(__file__).resolve().parent
spec = importlib.util.spec_from_file_location('anm_oracle', HERE / 'anm-oracle.py')
anm_oracle = importlib.util.module_from_spec(spec)
spec.loader.exec_module(anm_oracle)
from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_XMM2

SOUND_PLAY, SOUND_PLAY_AT, SOUND_STOP = 0x45E150, 0x45E1F0, 0x45E2A0
SOUND_PROCESS, SOUND_SUSPEND, SOUND_RESUME = 0x45E330, 0x42E4D0, 0x440BD0
DAT_VOICES, DAT_QUEUE = 0x4DB894, 0x4D9E2C
DAT_DEFINITIONS = 0x4A2A30
VOICES, DEFINITION_ROWS = 0x4E, 0x4E
DAT_DEVICE, DAT_ENABLED, DAT_MASTER, DAT_CS_ENABLED, DAT_MUSIC_QUEUE = 0x4D9E1C, 0x4C12C2, 0x4DF4A0, 0x4C10B6, 0x4DC1F0

METHODS = {0x24: ('status', 8), 0x30: ('play', 0x10), 0x34: ('position', 8), 0x3C: ('volume', 8), 0x40: ('pan', 8), 0x48: ('stop', 4)}


class SoundOracle(anm_oracle.Oracle):
    def __init__(self, exe, master, enabled, missing):
        super().__init__(exe, 'none.anm', b'')
        self.log = []
        self.playing = {}
        self.buffer_voice = {}
        uc = self.uc
        vtable = self.alloc(0x80)
        for i, (off, (name, ret)) in enumerate(METHODS.items()):
            addr = self.stub(40 + i)
            uc.mem_write(addr, bytes([0xC2]) + struct.pack('<H', ret))
            self.hooks[addr] = (lambda n: (lambda o: o.on_method(n)))(name)
            uc.mem_write(vtable + off, struct.pack('<I', addr))
        rows = {}
        for r in range(DEFINITION_ROWS):
            voice = struct.unpack('<i', uc.mem_read(DAT_DEFINITIONS + r * 0x14, 4))[0]
            rows.setdefault(voice, DAT_DEFINITIONS + r * 0x14)
        for v in range(VOICES):
            buf = 0
            if v not in missing:
                buf = self.alloc(0x10)
                uc.mem_write(buf, struct.pack('<I', vtable))
                self.buffer_voice[buf] = v
            uc.mem_write(DAT_VOICES + v * 0x18, struct.pack('<IiIIiI', buf, -1, rows[v], v, 0, 0))
        uc.mem_write(DAT_QUEUE, struct.pack('<12i', *([-1] * 12)))
        uc.mem_write(DAT_DEVICE, struct.pack('<I', 1))
        uc.mem_write(DAT_ENABLED, bytes([1 if enabled else 0]))
        uc.mem_write(DAT_MASTER, struct.pack('<i', master))
        uc.mem_write(DAT_CS_ENABLED, bytes([0]))
        uc.mem_write(DAT_MUSIC_QUEUE, struct.pack('<I', 0))

    def on_method(self, name):
        voice = self.buffer_voice[self.arg(0)]
        if name == 'status':
            self.uc.mem_write(self.arg(1), struct.pack('<I', 1 if self.playing.get(voice) else 0))
            self.log.append('%d status' % voice)
        elif name == 'play':
            self.playing[voice] = True
            self.log.append('%d play %d' % (voice, self.arg(3)))
        elif name == 'stop':
            self.playing[voice] = False
            self.log.append('%d stop' % voice)
        else:
            self.log.append('%d %s %d' % (voice, name, struct.unpack('<i', struct.pack('<I', self.arg(1)))[0]))
        self.uc.reg_write(UC_X86_REG_EAX, 0)

    def run(self, ops):
        for op in ops.split(','):
            if not op:
                continue
            parts = op.split(':')
            kind = parts[0]
            if kind == 'p':
                self.call(SOUND_PLAY, args=(int(parts[1]), 0))
            elif kind == 'a':
                bits = struct.unpack('<I', struct.pack('<f', float(parts[2])))[0]
                self.uc.reg_write(UC_X86_REG_XMM2, bits)
                self.call(SOUND_PLAY_AT, args=(int(parts[1]),))
            elif kind == 's':
                self.call(SOUND_STOP, args=(int(parts[1]),))
            elif kind == 'P':
                self.call(SOUND_PROCESS)
            elif kind == 'S':
                self.call(SOUND_SUSPEND)
            elif kind == 'R':
                self.call(SOUND_RESUME)
            else:
                raise SystemExit('unknown op %r' % op)
            self.log.append('-- ' + op)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('exe')
    ap.add_argument('ops')
    ap.add_argument('--master', type=int, default=100)
    ap.add_argument('--disabled', action='store_true')
    ap.add_argument('--missing', default='')
    a = ap.parse_args()
    missing = {int(v) for v in a.missing.split(',') if v}
    o = SoundOracle(pathlib.Path(a.exe), a.master, not a.disabled, missing)
    o.run(a.ops)
    sys.stdout.write('\n'.join(o.log) + '\n')


if __name__ == '__main__':
    main()
