#!/usr/bin/env python3
"""Unicorn oracle：在原版 th16.exe 代码上跑乐曲命令队列（0x45ed00 入队 / 0x45e330 前段逐步执行），
把对串流播放器 DAT_004df470 的调用导出为文本，供 harness `music-script` 对拍。

用法：
    music-oracle.py <th16.exe> <thbgm.fmt> <ops> [--disabled]

ops 以逗号分隔（与 harness `music-script` 同口径）：
    q:<kind>:<track>:<name>   0x45ed00(kind, track, name)      入队（name 可为空）
    P                         0x45e330                         处理一步（输出 "ret <队头 kind>"）
    D                         do 0x45e330 while ret != 0       排空
    F:<0|1>                   置串流播放器 +0x9c（淡出进行中）
    T                         0x43dcf0                         淡出逐帧推进（0x43b3d0 帧首紧随 0x45e330）
    c                         输出当前曲名 DAT_004dc0f0

环境：DAT_004d9e1c（设备）非 0；DAT_004c12c1 = 乐曲开；DAT_004c12cc bit4（整曲预载）清零；
DAT_004db790 = thbgm.fmt；DAT_004de370 曲名槽清空；DAT_004d9e24（声音线程句柄）= 0。
串流播放器函数整体替换为桩，每次调用输出一行：
    stop <close>  open <曲目号>  seek <曲目号> <偏移>  fill <偏移>  play <循环> <偏移>  start
    pause  resume  volume <v>  loop <曲目号>  halt（淡出到底时 IDirectSoundBuffer::Stop）
0x45e330 case 5 写入的淡出帧数（+0x14 / +0x18 与 +0x1c 标志）在该步之后以 "fade <帧>" 输出；状态保留给 T 推进。
"""
import argparse, importlib.util, pathlib, struct, sys

HERE = pathlib.Path(__file__).resolve().parent
spec = importlib.util.spec_from_file_location('sound_oracle', HERE / 'sound-oracle.py')
sound_oracle = importlib.util.module_from_spec(spec)
spec.loader.exec_module(sound_oracle)
from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_ECX

MUSIC_ENQUEUE, SOUND_PROCESS, FADE_TICK = 0x45ED00, 0x45E330, 0x43DCF0
DAT_SOUND_MANAGER, DAT_STREAMER, DAT_FMT, DAT_BGM_ENABLED, DAT_CONFIG = 0x4D9E10, 0x4DF470, 0x4DB790, 0x4C12C1, 0x4C12CC
DAT_CURRENT_NAME, DAT_SLOT_NAMES, DAT_SOUND_THREAD = 0x4DC0F0, 0x4DE370, 0x4D9E24
ENTRY_SIZE = 0x34

# (地址, 栈清理字节, 名字, 栈参数个数)——thiscall 的 this 在 ECX
STREAMER = [
    (0x471270, 4, 'stop', 1), (0x470BB0, 4, 'open', 1), (0x4718A0, 8, 'seek', 2), (0x471720, 4, 'fill', 1),
    (0x470ED0, 0xC, 'play', 3), (0x471120, 0xC, 'start', 3), (0x4711F0, 4, 'volume', 1), (0x471B00, 4, 'loop', 1),
    (0x4712F0, 0, 'pause', 0), (0x471380, 0, 'resume', 0),
]


class MusicOracle(sound_oracle.SoundOracle):
    def __init__(self, exe, fmt, enabled):
        super().__init__(exe, 100, True, set())
        uc = self.uc
        self.fmt = self.alloc(len(fmt) + ENTRY_SIZE)
        uc.mem_write(self.fmt, fmt + b'\0' * ENTRY_SIZE)
        uc.mem_write(DAT_FMT, struct.pack('<I', self.fmt))
        self.streamer = self.alloc(0x100)
        uc.mem_write(self.streamer, b'\0' * 0x100)
        # +0xc 文件读取器（+0x90 = 当前 thbgm.fmt 条目，0x45e330 case 2 第 3 步按它的 +0x1c 判断是否循环）
        self.reader = self.alloc(0x100)
        uc.mem_write(self.reader, b'\0' * 0x100)
        uc.mem_write(self.streamer + 0xC, struct.pack('<I', self.reader))
        # +0x4 DirectSound 缓冲指针数组（+0x10 个数保持 0，case 2 第 5 步因此不取它）：0x43dcf0 淡出到底调首个缓冲的 vtable +0x48 Stop
        self.ds_buffer = self.alloc(0x10)
        ds_vtable = self.alloc(0x60)
        halt_stub = self.stub(70)
        uc.mem_write(halt_stub, bytes([0xC2, 0x04, 0x00]))
        self.hooks[halt_stub] = lambda o: o.on_halt()
        uc.mem_write(ds_vtable + 0x48, struct.pack('<I', halt_stub))
        uc.mem_write(self.ds_buffer, struct.pack('<I', ds_vtable))
        buffers = self.alloc(0x10)
        uc.mem_write(buffers, struct.pack('<I', self.ds_buffer))
        uc.mem_write(self.streamer + 0x4, struct.pack('<I', buffers))
        uc.mem_write(DAT_STREAMER, struct.pack('<I', self.streamer))
        uc.mem_write(DAT_BGM_ENABLED, bytes([1 if enabled else 0]))
        uc.mem_write(DAT_CONFIG, bytes([0]))
        uc.mem_write(DAT_CURRENT_NAME, b'\0' * 0x100)
        uc.mem_write(DAT_SLOT_NAMES, b'\0' * 0x1000)
        uc.mem_write(DAT_SOUND_THREAD, struct.pack('<I', 0))
        for addr, cleanup, name, argc in STREAMER:
            self.replaced[addr] = (cleanup, (lambda n, c: (lambda o: o.on_streamer(n, c)))(name, argc))

    def entry_index(self, addr):
        return (addr - self.fmt) // ENTRY_SIZE

    def on_streamer(self, name, argc):
        args = [self.arg(i) for i in range(argc)]
        if name == 'stop':
            # 0x471270 真身会把淡出模式 +0x1c 清零（桩替换后需手工复现）
            self.uc.mem_write(self.streamer + 0x1C, struct.pack('<I', 0))
            self.log.append('stop %d' % args[0])
        elif name == 'open':
            self.uc.mem_write(self.reader + 0x90, struct.pack('<I', args[0]))
            self.log.append('open %d' % self.entry_index(args[0]))
        elif name == 'seek':
            self.uc.mem_write(self.reader + 0x90, struct.pack('<I', args[0]))
            self.log.append('seek %d %d' % (self.entry_index(args[0]), args[1]))
        elif name == 'loop':
            self.log.append('loop %d' % self.entry_index(args[0]))
        elif name == 'pause':
            self.uc.mem_write(self.streamer + 0x50, struct.pack('<II', 0, 1))
            self.log.append('pause')
        elif name == 'resume':
            self.uc.mem_write(self.streamer + 0x50, struct.pack('<II', 1, 0))
            self.log.append('resume')
        elif name == 'start':
            self.uc.mem_write(self.streamer + 0x50, struct.pack('<I', 1))
            self.log.append('start')
        else:
            self.log.append(' '.join([name] + [str(struct.unpack('<i', struct.pack('<I', a))[0]) for a in args]))
        self.uc.reg_write(UC_X86_REG_EAX, 0)

    def on_halt(self):
        self.log.append('halt')
        self.uc.reg_write(UC_X86_REG_EAX, 0)

    def current_name(self):
        return self.cstr(DAT_CURRENT_NAME)

    def fade_state(self):
        return struct.unpack('<iii', self.uc.mem_read(self.streamer + 0x14, 12))

    def process_once(self):
        before = self.fade_state()
        self.call(SOUND_PROCESS)
        ret = self.uc.reg_read(UC_X86_REG_EAX)
        remaining, total, mode = self.fade_state()
        if mode == 1 and (remaining, total, mode) != before:
            self.log.append('fade %d' % total)
        self.log.append('ret %d' % ret)
        return ret

    def run(self, ops):
        for op in ops.split(','):
            if not op:
                continue
            parts = op.split(':')
            kind = parts[0]
            if kind == 'q':
                name = ':'.join(parts[3:]).encode() + b'\0'
                buf = self.alloc(len(name))
                self.uc.mem_write(buf, name)
                self.call(MUSIC_ENQUEUE, ecx=DAT_SOUND_MANAGER, args=(int(parts[1]), int(parts[2]) & 0xFFFFFFFF, buf))
            elif kind == 'P':
                self.process_once()
            elif kind == 'D':
                for _ in range(64):
                    if self.process_once() == 0:
                        break
            elif kind == 'F':
                self.uc.mem_write(self.streamer + 0x9C, struct.pack('<I', int(parts[1])))
            elif kind == 'T':
                self.call(FADE_TICK)
            elif kind == 'c':
                self.log.append('current %s' % (self.current_name() or '-'))
            else:
                raise SystemExit('unknown op %r' % op)
            self.log.append('-- ' + op)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('exe')
    ap.add_argument('fmt')
    ap.add_argument('ops')
    ap.add_argument('--disabled', action='store_true')
    a = ap.parse_args()
    o = MusicOracle(pathlib.Path(a.exe), pathlib.Path(a.fmt).read_bytes(), not a.disabled)
    o.run(a.ops)
    sys.stdout.write('\n'.join(o.log) + '\n')


if __name__ == '__main__':
    main()
