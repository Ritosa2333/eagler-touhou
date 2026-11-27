#!/usr/bin/env python3
"""Unicorn 标题 oracle：在原版 th16.exe 上走启动幕之后的标题创建与主菜单。

路径（全部为原版代码，桩同 game-oracle.py）：
    0x44a9f0  标题对象构造（DAT_004a6f20）
    0x44ac70  装 title.anm / title_v.anm，登记更新链 6（0x44af80）与绘制链 0x45（0x44b530），链先禁用
    0x43b300  启动幕绘制链：首帧生成 sig.anm 脚本 0，+0x638 每绘帧 +1
    0x44abe1  +0x638 ≥ 0xb4 之后卸 sig.anm 并启用标题更新链（0x44abc0 等待循环的后半）
    0x401460 / 0x4015a0  每帧更新链与绘制链（含 0x44af80 / 0x44b530）

帧号与 harness `scene-boot <frames>` 对齐：第 N 帧快照是第 N 次 logic step 之后的状态
（启动幕 180 帧后 title_live，主菜单可输入在第 312 帧，sub==2）。

用法：
    title-oracle.py <th16.exe> <dat 目录> <frames> <out.json> [--input "f:held,..."] [--debug]
                    [--draw-out path] [--draw-from N]
缺的导入按 game-oracle 抛错，不猜返回值。
"""
import argparse, importlib.util, json, pathlib, struct, sys

HERE = pathlib.Path(__file__).resolve().parent
spec = importlib.util.spec_from_file_location('game_oracle', HERE / 'game-oracle.py')
game_oracle = importlib.util.module_from_spec(spec)
spec.loader.exec_module(game_oracle)
GameOracle = game_oracle.GameOracle
anm_oracle = game_oracle.anm_oracle

TITLE_CTOR, TITLE_INIT, TITLE_ARM = 0x44A9F0, 0x44AC70, 0x44ABE1
BOOT_DRAW, BOOT_READY = 0x43B300, 0x43B290
DAT_BOOT, DAT_TITLE = 0x4A6EE4, 0x4A6F20
TITLE_SIZE, BOOT_SIZE = 0x5E00, 0x63C
BOOT_HOLD = 0xB4
ASCII_ENTRIES, ASCII_STRIDE, ASCII_COUNT = 0xC08, 0x138, 0x19208
HELP_BANK_WORKER = 0x42E760
HELP_PAGE_WORKER = 0x42E7C0


class TitleOracle(GameOracle):
    def __init__(self, exe, dat_dir, debug=False):
        # 绘制链里的合成 / 纹理面需要 DeviceRecorder 的 GetBackBuffer，即使不落盘也要装上
        super().__init__(exe, dat_dir, debug=debug, draw=True)
        self.help_workers_pending = []
        self.manual_page_reads = []
        self.title_armed = False
        self.boot_obj = 0

    def narrow_hooks(self):
        # 0x43c5b0 的 ret 8 清理两个参数；目录和正文装载函数在下一帧前执行。
        self.replaced[game_oracle.THREAD_CREATE] = (8, self.on_title_loader)
        super().narrow_hooks()

    def on_title_loader(self, _):
        worker = self.arg(0)
        if worker in (HELP_BANK_WORKER, HELP_PAGE_WORKER):
            self.help_workers_pending.append(worker)
        self.uc.reg_write(game_oracle.UC_X86_REG_EAX, 0)

    def on_read_file(self, oracle):
        super().on_read_file(oracle)
        if self.uc.reg_read(game_oracle.UC_X86_REG_EAX):
            name = self.cstr(self.uc.reg_read(game_oracle.UC_X86_REG_ECX))
            if name.startswith('help_') and name.endswith('.png'):
                self.manual_page_reads.append(name)

    def boot_title(self):
        """Supervisor 环境 + 启动幕资源 + 0x44ac70。标题更新链保持禁用，等启动幕画满 0xb4 帧。"""
        uc = self.uc
        self.loaded_banks = {}
        self.chain_manager = self.alloc(0x100)
        uc.mem_write(self.chain_manager, b'\0' * 0x100)
        uc.mem_write(game_oracle.DAT_CHAIN_MANAGER, struct.pack('<I', self.chain_manager))
        self.manager = self.alloc(0x1C7FD90)
        self.call(anm_oracle.MANAGER_CTOR, ecx=self.manager)
        uc.mem_write(anm_oracle.DAT_MANAGER, struct.pack('<I', self.manager))
        uc.mem_write(anm_oracle.DAT_BACKBUFFER_W, struct.pack('<IIf', 640, 480, 1.0))
        # 0x45a8a0 绘制段要用的批量顶点缓冲（0x43b520 → 0x46b900）
        self.call(0x46B900)
        for fn, priority in ((0x43D140, 1), (0x43D400, 0xE), (0x43D2F0, 0xF), (0x43D5A0, 0x19), (0x43D4A0, 0x1A),
                             (0x43D6F0, 0x2B), (0x43D640, 0x2C), (0x43D820, 0x38), (0x43D7C0, 0x39), (0x43D870, 0x55)):
            node = self.call(0x401730, args=(fn,))
            uc.mem_write(node + 4, struct.pack('<I', self.u32(node + 4) | 2))
            uc.mem_write(node + 0x24, struct.pack('<I', game_oracle.DAT_UI_CAMERA))
            self.call(0x4013B0, args=(node, priority))
        uc.mem_write(game_oracle.DAT_GAME_SPEED, struct.pack('<f', 1.0))
        uc.mem_write(game_oracle.DAT_BG_COLOR, struct.pack('<I', 0xFF000000))
        self.call(game_oracle.CAMERA_INIT, ecx=game_oracle.DAT_UI_CAMERA)
        overlay = self.alloc(0x88)
        uc.mem_write(overlay, b'\0' * 0x88)
        uc.mem_write(overlay, struct.pack('<I', 2))
        uc.mem_write(game_oracle.DAT_OVERLAY, struct.pack('<I', overlay))
        for i in range(4):
            vm = self.alloc(anm_oracle.VM_SIZE)
            self.call(game_oracle.VM_CTOR, ecx=vm)
            uc.mem_write(game_oracle.DAT_LOADING_VMS + i * 4, struct.pack('<I', vm))
        # 0x43adc0：AsciiManager、text.anm、0x43ad20（front / effect / bullet）、成绩默认块
        self.ascii_manager = self.alloc(0x19254)
        self.call(game_oracle.ASCII_CTOR, ecx=self.ascii_manager)
        self.call(game_oracle.ASCII_INIT, ecx=self.ascii_manager)
        uc.mem_write(anm_oracle.DAT_ASCII_MANAGER, struct.pack('<I', self.ascii_manager))
        uc.mem_write(game_oracle.DAT_TEXT_BANK, struct.pack('<I', self.call(game_oracle.ANM_LOAD_FILE, args=(0, self.cstring('text.anm')))))
        if self.call(game_oracle.FRONT_INIT) != 0:
            raise RuntimeError('0x43ad20 failed: %s' % self.errors[-4:])
        self.score = self.alloc(0x1A3AC)
        self.call(game_oracle.SCORE_CTOR, ecx=self.score)
        uc.mem_write(game_oracle.DAT_SCORE, struct.pack('<I', self.score))
        # sig.anm → 槽 1，启动幕对象（0x43b1f0 的字段：+0x62c 银行、+0x630/+0x634 首帧生成、+0x638 帧计数）
        sig = self.call(game_oracle.ANM_LOAD_FILE, args=(1, self.cstring('sig.anm')))
        self.boot_obj = self.alloc(BOOT_SIZE)
        uc.mem_write(self.boot_obj, b'\0' * BOOT_SIZE)
        uc.mem_write(self.boot_obj, struct.pack('<I', 2))
        uc.mem_write(self.boot_obj + 0x62C, struct.pack('<III', sig, 1, 1))
        uc.mem_write(DAT_BOOT, struct.pack('<I', self.boot_obj))
        boot_draw = self.call(0x401730, args=(BOOT_DRAW,))
        uc.mem_write(boot_draw + 4, struct.pack('<I', self.u32(boot_draw + 4) | 2))
        uc.mem_write(boot_draw + 0x24, struct.pack('<I', self.boot_obj))
        self.call(0x4013B0, args=(boot_draw, 0x44))
        # 0x43b290：合成 VM、打开 Ascii 链、清配置 bit13。场景请求 4 由本 oracle 自己驱动，不跑 0x43ce10
        self.call(BOOT_READY, ecx=self.boot_obj)
        # 0x44a9f0 / 0x44ac70：标题对象与 title*.anm。更新链保持禁用，等到启动幕结束再 0x44abe1
        title = self.alloc(TITLE_SIZE)
        self.call(TITLE_CTOR, ecx=title)
        if self.u32(DAT_TITLE) != title:
            raise RuntimeError('DAT_004a6f20 not set by 0x44a9f0')
        rc = self.call(TITLE_INIT)
        if rc != 0:
            raise RuntimeError('0x44ac70 failed rc=%#x errors=%s' % (rc, self.errors[-4:]))
        uc.mem_write(0x4D9D50, struct.pack('<ii', 320, 16))
        self.frame = 0
        self.title_armed = False

    def arm_title(self):
        """0x44abe1：启动幕画满 0xb4 帧后卸 sig、启用标题更新链。"""
        if self.i32(self.boot_obj + 0x638) < BOOT_HOLD:
            raise RuntimeError('arm_title before boot hold (%d)' % self.i32(self.boot_obj + 0x638))
        self.call(TITLE_ARM)
        self.title_armed = True

    def ascii_lines(self):
        n = self.i32(self.ascii_manager + ASCII_COUNT)
        if n < 0 or n > 0x140:
            raise RuntimeError('ascii count %d' % n)
        lines = []
        for i in range(n):
            entry = self.ascii_manager + ASCII_ENTRIES + i * ASCII_STRIDE
            text = self.cstr(entry)
            x, y = struct.unpack('<ff', self.uc.mem_read(entry + 0x100, 8))
            color = self.u32(entry + 0x10C)
            shadow = self.uc.mem_read(entry + 0x124, 1)[0]
            lines.append('ascii %.1f %.1f %08x %d [%s]' % (x, y, color, 1 if shadow else 0, text))
        return lines

    def title_state(self):
        title = self.u32(DAT_TITLE)
        return {
            'screen': self.i32(title + 0x18),
            'sub': self.i32(title + 0x20),
            'cursor': self.i32(title + 0x24),
            'timer': self.i32(title + 0x2B0),
            'boot': self.i32(self.boot_obj + 0x638),
            'live': 1 if self.title_armed else 0,
        }

    def tick(self):
        workers, self.help_workers_pending = self.help_workers_pending, []
        for worker in workers:
            self.call(worker)
        self.sfx_log = []
        self.sfx_pans = []
        if self.run_draw:
            self.call(0x465A80, ecx=self.manager)
            self.uc.mem_write(0x4C17B8, struct.pack('<I', 0x4C1560))
            self.call(0x43C710, args=(0x4C1560,))
            self.device_call(0xBC, 0x4C1560 + 0xE0)
            self.uc.mem_write(0x4C17BC, struct.pack('<I', 2))
        self.call(game_oracle.CHAIN_RUN_UPDATE, ecx=self.chain_manager)
        calls = None
        if self.run_draw:
            self.recorder.begin_frame()
            self.device_call(0xA4)
            self.call(0x465A30)
            self.uc.mem_write(0x4C1ADC, b'\xff')
            self.call(0x43C4F0, ecx=0x4C10D0)
            self.call(game_oracle.CHAIN_RUN_DRAW)
            self.call(0x465A80, ecx=self.manager)
            self.device_call(0x104, 0, 0)
            self.device_call(0xA8)
            calls = self.recorder.end_frame()
        # 与 GameSession::logic_step 相同：绘制把启动幕计数到 0xb4 之后才启用标题更新链，下一帧才跑 0x44af80
        if not self.title_armed and self.i32(self.boot_obj + 0x638) >= BOOT_HOLD:
            self.arm_title()
        self.frame += 1
        if calls is not None and self.on_draw_frame is not None:
            self.on_draw_frame(self.frame, calls)
        state = self.title_state()
        state['frame'] = self.frame
        state['sfx'] = [sid for sid, _ret in self.sfx_log]
        state['ascii'] = self.ascii_lines()
        return state


def parse_input(spec):
    events = {}
    if spec:
        for part in spec.split(','):
            if not part:
                continue
            frame, value = part.split(':')
            events[int(frame)] = int(value, 0)
    return events


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('exe')
    ap.add_argument('dat_dir')
    ap.add_argument('frames', type=int)
    ap.add_argument('out')
    ap.add_argument('--input', default='')
    ap.add_argument('--debug', action='store_true')
    ap.add_argument('--draw-out', default='')
    ap.add_argument('--draw-from', type=int, default=1)
    args = ap.parse_args()
    oracle = TitleOracle(pathlib.Path(args.exe), args.dat_dir, debug=args.debug)
    oracle.boot_title()
    print('title booted chains=%s banks=%s' % (
        [(p, hex(cb)) for p, cb, _ctx in oracle.chain_list()], oracle.loaded_banks), file=sys.stderr)
    events = parse_input(args.input)
    draw_file = None
    if args.draw_out:
        draw_file = open(args.draw_out, 'w')
        carried = {}

        def write_draw(frame, calls):
            if frame < args.draw_from:
                for call in calls:
                    if call[0] in ('rs', 'fvf', 'rt', 'vp'):
                        key = (call[0], call[1]) if call[0] == 'rs' else (call[0],)
                    elif call[0] in ('tss', 'ss'):
                        key = (call[0], call[1], call[2])
                    elif call[0] in ('tex', 'xf'):
                        key = (call[0], call[1])
                    else:
                        continue
                    carried.pop(key, None)
                    carried[key] = call
                return
            rec = {'frame': frame}
            if carried:
                rec['prefix'] = list(carried.values())
                carried.clear()
            rec['calls'] = calls
            draw_file.write(json.dumps(rec, separators=(',', ':')) + '\n')

        oracle.on_draw_frame = write_draw
    held = 0
    frames = []
    for index in range(args.frames):
        if index in events:
            held = events[index]
        oracle.set_input(held)
        frames.append(oracle.tick())
        if args.debug and (index < 3 or index + 1 in (180, 181, 312)):
            st = frames[-1]
            print('f=%d screen=%d sub=%d cursor=%d boot=%d live=%d sfx=%s ascii=%d' % (
                st['frame'], st['screen'], st['sub'], st['cursor'], st['boot'], st['live'], st['sfx'], len(st['ascii'])),
                  file=sys.stderr)
    pathlib.Path(args.out).write_text(json.dumps({
        'frames': frames, 'manual_page_reads': oracle.manual_page_reads,
    }, separators=(',', ':')))
    last = frames[-1]
    print('frame=%d screen=%d sub=%d cursor=%d boot=%d live=%d sfx=%s ascii=%d' % (
        last['frame'], last['screen'], last['sub'], last['cursor'], last['boot'], last['live'],
        ','.join(str(s) for s in last['sfx']), len(last['ascii'])))
    if draw_file is not None:
        draw_file.close()
    return 0


if __name__ == '__main__':
    sys.exit(main())
