#!/usr/bin/env python3
"""Unicorn oracle：在原版 th16.exe 上跑成绩对象构造 0x4497e0（读 scoreth16.dat）与写盘 0x449a00。

用法：
    score-oracle.py <th16.exe> <out.json> [--load scoreth16.dat] [--poke OFF:HEX ...] [--unlock-all] [--rng-seed N]

步骤：DAT_004a6d88 = rng-seed → 0x4497e0（0x402440 读文件：给了 --load 即返回该字节，否则返回 NULL）
     → 导出 +8 起的 0x1a3a4 字节镜像（5 个 CR 块 + ST 块）→ 按 --poke 改镜像（偏移相对对象基址）
     → --unlock-all 时跑 0x44a930（Player Data 隐藏键序的全解锁）
     → 0x449a00（0x4027c0 打开文件改为直接成功，WriteFile 截获写出字节）→ 导出写出的文件与写盘后镜像。

输出 JSON：{"image": hex, "rng_seed": n, "saved": hex, "image_after_save": hex}
"""
import argparse, importlib.util, json, pathlib, struct

HERE = pathlib.Path(__file__).resolve().parent
spec = importlib.util.spec_from_file_location('anm_oracle', HERE / 'anm-oracle.py')
anm_oracle = importlib.util.module_from_spec(spec)
spec.loader.exec_module(anm_oracle)
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX

SCORE_CTOR, SCORE_SAVE, SCORE_UNLOCK_ALL = 0x4497E0, 0x449A00, 0x44A930
SCORE_SIZE = 0x1A3AC
DAT_SCORE = 0x4A6F0C
DAT_RNG_SEED = 0x4A6D88
DAT_WRITE_HANDLE = 0x49F270
IAT_WRITE_FILE, IAT_CLOSE_HANDLE = 0x48B094, 0x48B080
FAKE_HANDLE = 0x1234


class ScoreOracle(anm_oracle.Oracle):
    def __init__(self, exe, file_bytes):
        super().__init__(exe, 'scoreth16.dat', file_bytes if file_bytes is not None else b'')
        self.has_file = file_bytes is not None
        self.written = bytearray()
        self.replaced[0x402440] = (4, self.on_read_score)
        self.replaced[0x479A65] = (0, lambda o: None)                 # _chdir
        self.replaced[0x477FB2] = (0, lambda o: None)                 # free
        self.replaced[0x4027C0] = (0, self.on_open_for_write)         # fastcall(name)
        write_file = self.make_iat_stub(20, 20, self.on_write_file)
        close_handle = self.make_iat_stub(21, 4, lambda o: o.uc.reg_write(UC_X86_REG_EAX, 1))
        self.uc.mem_write(IAT_WRITE_FILE, struct.pack('<I', write_file))
        self.uc.mem_write(IAT_CLOSE_HANDLE, struct.pack('<I', close_handle))
        # LZSS 0x457b20 要跑上亿条指令：把逐指令 Python 钩子换成只挂在桩 / 替换入口上的单地址钩子
        self.uc.hook_del(self.code_hook)
        for addr in set(self.hooks) | set(self.replaced) | set(self.skips):
            self.uc.hook_add(UC_HOOK_CODE, self.on_code, begin=addr, end=addr)

    def make_iat_stub(self, index, ret_bytes, fn):
        addr = self.stub(index)
        self.uc.mem_write(addr, bytes([0xC2]) + struct.pack('<H', ret_bytes))
        self.hooks[addr] = fn
        return addr

    def on_read_score(self, o):
        size_ptr = self.uc.reg_read(anm_oracle.UC_X86_REG_EDX)
        if not self.has_file:
            if size_ptr:
                self.uc.mem_write(size_ptr, struct.pack('<I', 0))
            self.uc.reg_write(UC_X86_REG_EAX, 0)
            return
        self.on_read_file(o)

    def on_open_for_write(self, o):
        self.uc.mem_write(DAT_WRITE_HANDLE, struct.pack('<I', FAKE_HANDLE))
        self.uc.reg_write(UC_X86_REG_EAX, 0)

    def on_write_file(self, o):
        buf, n, written_ptr = self.arg(1), self.arg(2), self.arg(3)
        self.written += bytes(self.uc.mem_read(buf, n))
        if written_ptr:
            self.uc.mem_write(written_ptr, struct.pack('<I', n))
        self.uc.reg_write(UC_X86_REG_EAX, 1)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('exe')
    ap.add_argument('out')
    ap.add_argument('--load', default='')
    ap.add_argument('--poke', action='append', default=[], help='OFF:HEX，偏移相对成绩对象基址（十六进制）')
    ap.add_argument('--unlock-all', action='store_true')
    ap.add_argument('--rng-seed', type=int, default=0)
    a = ap.parse_args()
    data = pathlib.Path(a.load).read_bytes() if a.load else None
    o = ScoreOracle(pathlib.Path(a.exe), data)
    o.uc.mem_write(DAT_RNG_SEED, struct.pack('<H', a.rng_seed))
    score = o.alloc(SCORE_SIZE + 0x5000)   # 0x44a930 会越界写到对象尾后 +0x1a748 + i×0x9c（至 +0x1ef30）
    o.call(SCORE_CTOR, ecx=score, limit=0)
    o.uc.mem_write(DAT_SCORE, struct.pack('<I', score))
    image = bytes(o.uc.mem_read(score + 8, SCORE_SIZE - 8))
    rng_seed = struct.unpack('<H', o.uc.mem_read(DAT_RNG_SEED, 2))[0]
    for spec_text in a.poke:
        off, hex_bytes = spec_text.split(':', 1)
        o.uc.mem_write(score + int(off, 16), bytes.fromhex(hex_bytes))
    if a.unlock_all:
        o.call(SCORE_UNLOCK_ALL, limit=0)
    o.call(SCORE_SAVE, limit=0)
    after = bytes(o.uc.mem_read(score + 8, SCORE_SIZE - 8))
    pathlib.Path(a.out).write_text(json.dumps({
        'image': image.hex(), 'rng_seed': rng_seed, 'saved': bytes(o.written).hex(), 'image_after_save': after.hex(),
    }))


if __name__ == '__main__':
    main()
