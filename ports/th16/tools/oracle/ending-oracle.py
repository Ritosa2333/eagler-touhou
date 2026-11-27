#!/usr/bin/env python3
"""Record the original TH16 story ending or a stage-clear transition."""
import argparse
import importlib.util
import json
from pathlib import Path
import struct


HERE = Path(__file__).resolve().parent
spec = importlib.util.spec_from_file_location('title_oracle', HERE / 'title-oracle.py')
title_oracle = importlib.util.module_from_spec(spec)
spec.loader.exec_module(title_oracle)
game_oracle = title_oracle.game_oracle

ENDING = 15
ENDING_EXIT = 16
ENDING_CREATE = 0x419640
ENDING_LOAD_WORKER = 0x41A310
ENDING_THREAD_CREATE = 0x402FB0
TITLE_DESTROY = 0x44AF50
DAT_ENDING = 0x4A6DBC
DAT_CONTINUES = 0x4A57B8


class EndingOracle(title_oracle.TitleOracle):
    def __init__(self, exe, dat_dir):
        self.ending_loads = []
        super().__init__(exe, dat_dir)

    def narrow_hooks(self):
        self.replaced[ENDING_THREAD_CREATE] = (8, self.defer_ending_load)
        super().narrow_hooks()

    def defer_ending_load(self, _):
        worker, arg = self.arg(0), self.arg(1)
        if worker != ENDING_LOAD_WORKER:
            raise RuntimeError(f'unexpected ending worker {worker:#x}')
        self.ending_loads.append(arg)
        self.uc.reg_write(game_oracle.UC_X86_REG_EAX, 0)

    def begin_ending(self, shot, difficulty, continues, state):
        def remember_state(_frame, calls):
            for call in calls:
                state.apply(call)

        self.boot_title()
        self.on_draw_frame = remember_state
        for _ in range(312):
            self.set_input(0)
            self.tick()
        if self.title_state()['sub'] != 2:
            raise RuntimeError('title did not reach the ready state')

        # The device state survives the scene transition. Preserve the prelude
        # and transition writes before emitting frame 1 of the ending.
        self.recorder.begin_frame()
        self.call(TITLE_DESTROY)
        for addr, value in ((game_oracle.DAT_SHOT, shot), (game_oracle.DAT_DIFFICULTY, difficulty),
                            (DAT_CONTINUES, continues), (game_oracle.DAT_SCENE_REQUEST, ENDING)):
            self.uc.mem_write(addr, struct.pack('<I', value))
        self.frame = 0
        self.call(ENDING_CREATE)
        remember_state(0, self.recorder.end_frame())
        if not self.u32(DAT_ENDING):
            raise RuntimeError('original ending did not initialize')

    def tick_ending(self, input_bits):
        loads, self.ending_loads = self.ending_loads, []
        for arg in loads:
            self.call(ENDING_LOAD_WORKER, args=(arg,))
        self.set_input(input_bits)
        game_oracle.GameOracle.tick(self)


class ExtraClearOracle(game_oracle.GameOracle):
    def __init__(self, exe, dat_dir, shot, *, stage=7, difficulty=4, lives=None):
        self.title_worker_pending = False
        self.transition_frame = None
        super().__init__(exe, dat_dir, stage=stage, shot=shot, difficulty=difficulty,
                         season=4 if stage == 7 else 0, lives=lives, draw=True)

    def narrow_hooks(self):
        self.replaced[0x47952B] = (0, self.defer_title_worker)
        self.replaced[0x42E4D0] = (0, self.pause_empty_audio_channels)
        super().narrow_hooks()

    def defer_title_worker(self, _):
        if self.arg(2) != 0x44ABC0:
            raise RuntimeError('unexpected thread in the Extra clear-to-title transition')
        self.title_worker_pending = True
        self.uc.reg_write(game_oracle.UC_X86_REG_EAX, 0)

    def pause_empty_audio_channels(self, _):
        # The headless device has no DirectSound buffers to stop.
        self.uc.mem_write(0x4D9E2C, struct.pack('<I', 0xffffffff))
        for channel in range(0x4DB894, 0x4DBFE4, 24):
            self.uc.mem_write(channel + 20, bytes(4))

    def switch_scene(self):
        if self.u32(game_oracle.DAT_SCENE_CURRENT) == 7 and self.u32(game_oracle.DAT_SCENE_REQUEST) == ENDING_EXIT:
            self.call(0x43CE10, ecx=game_oracle.DAT_UI_CAMERA)
            if self.u32(game_oracle.DAT_SCENE_CURRENT) != 4 or self.u32(game_oracle.DAT_SCENE_REQUEST) != 4:
                raise RuntimeError('the original Extra clear did not enter Title')
            self.transition_frame = self.frame + 1
        else:
            super().switch_scene()


def record_extra_clear(args, events):
    state = game_oracle.draw_compare.OracleState()
    oracle = ExtraClearOracle(args.exe, args.dat_dir, args.shot)
    oracle.boot()
    oracle.uc.mem_write(0x4C11CC, struct.pack('<I', 21))  # D3DFMT_A8R8G8B8
    with args.draw_ops.open('w', encoding='utf-8') as stream:
        def record(frame, calls):
            ops = state.fold(calls)
            if frame >= args.draw_from:
                stream.write(json.dumps({'frame': frame, 'ops': ops}, separators=(',', ':')) + '\n')

        oracle.on_draw_frame = record
        for frame in range(args.frames):
            if frame == args.extra_clear_at:
                oracle.uc.mem_write(0x4A57B0, struct.pack('<I', args.clear_score))
                oracle.call(0x42E150)
            bits = int(frame % 2 == 0) if args.z_pulses else events.get(frame, 0)
            oracle.set_input(bits)
            oracle.tick()
            if oracle.title_worker_pending:
                oracle.title_worker_pending = False
                oracle.call(0x44ABC0)

    errors = [error for error in oracle.errors if not error.startswith('note')]
    if errors or oracle.transition_frame is None:
        raise RuntimeError(f'original Extra clear reference did not finish: {errors}')
    print(json.dumps({'frames_recorded': args.frames - args.draw_from + 1,
                      'title_frame': oracle.transition_frame}))


class NormalClearOracle(ExtraClearOracle):
    defer_ending_load = EndingOracle.defer_ending_load

    def __init__(self, exe, dat_dir, shot, difficulty, lives):
        self.ending_loads = []
        self.draw_state = game_oracle.draw_compare.OracleState()
        super().__init__(exe, dat_dir, shot, stage=6, difficulty=difficulty, lives=lives)

    def narrow_hooks(self):
        self.replaced[ENDING_THREAD_CREATE] = (8, self.defer_ending_load)
        super().narrow_hooks()

    def switch_scene(self):
        if (self.u32(game_oracle.DAT_SCENE_CURRENT) == 7
                and self.u32(game_oracle.DAT_SCENE_REQUEST) == ENDING):
            self.recorder.begin_frame()
            self.call(0x43CE10, ecx=game_oracle.DAT_UI_CAMERA)
            for call in self.recorder.end_frame():
                self.draw_state.apply(call)
            if self.u32(game_oracle.DAT_SCENE_CURRENT) != ENDING:
                raise RuntimeError('the original stage 6 clear did not enter Ending')
            self.transition_frame = self.frame + 1
        else:
            super().switch_scene()

    def tick_frame(self, bits):
        loads, self.ending_loads = self.ending_loads, []
        for arg in loads:
            self.call(ENDING_LOAD_WORKER, args=(arg,))
        self.set_input(bits)
        self.tick()


def record_normal_clear(args, events):
    oracle = NormalClearOracle(args.exe, args.dat_dir, args.shot, args.difficulty, args.refill_lives)
    oracle.boot()
    oracle.uc.mem_write(0x4C11CC, struct.pack('<I', 21))  # D3DFMT_A8R8G8B8
    gameplay = game_oracle.parse_input('@' + str(args.stage6_input)) if args.stage6_input else None
    held = 0
    frames_recorded = 0
    with args.draw_ops.open('w', encoding='utf-8') as stream:
        def record(frame, calls):
            nonlocal frames_recorded
            if oracle.transition_frame is None:
                for call in calls:
                    oracle.draw_state.apply(call)
            elif oracle.u32(game_oracle.DAT_SCENE_REQUEST) == ENDING:
                ops = oracle.draw_state.fold(calls)
                if frame >= args.draw_from:
                    stream.write(json.dumps({'frame': frame, 'ops': ops}, separators=(',', ':')) + '\n')
                    frames_recorded += 1

        oracle.on_draw_frame = record
        for frame in range(args.frames):
            if frame == args.normal_clear_at:
                oracle.uc.mem_write(0x4A57B0, struct.pack('<I', args.clear_score))
                oracle.call(0x42E150)
            bits = events.get(frame, 0)
            if oracle.transition_frame is None:
                if gameplay is not None:
                    held = gameplay.get(frame, held)
                    bits = held
                if args.refill_lives is not None and oracle.i32(0x4A57F4) < args.refill_lives:
                    oracle.uc.mem_write(0x4A57F4, struct.pack('<i', args.refill_lives))
            elif args.z_pulses:
                bits = int((frame - oracle.transition_frame + 1) % 2 == 0)
            oracle.tick_frame(bits)
            if oracle.transition_frame is not None and oracle.u32(game_oracle.DAT_SCENE_REQUEST) == ENDING_EXIT:
                break
    errors = [error for error in oracle.errors if not error.startswith('note')]
    if errors or oracle.transition_frame is None or oracle.u32(game_oracle.DAT_SCENE_REQUEST) != ENDING_EXIT:
        raise RuntimeError(f'original story clear reference did not finish: {errors}')
    print(json.dumps({'frames_recorded': frames_recorded, 'ending_from': oracle.transition_frame,
                      'exit_frame': oracle.frame}))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('exe', type=Path)
    parser.add_argument('dat_dir', type=Path)
    parser.add_argument('frames', type=int)
    parser.add_argument('draw_ops', type=Path, help='folded JSONL for tools/draw-digest.mjs')
    parser.add_argument('--shot', type=int, choices=range(4), default=0)
    parser.add_argument('--difficulty', type=int, choices=range(4), default=1)
    parser.add_argument('--continues', type=int, choices=range(10), default=0)
    parser.add_argument('--draw-from', type=int, default=1, help='first one-based frame to write')
    clear = parser.add_mutually_exclusive_group()
    clear.add_argument('--extra-clear-at', type=int, help='award an Extra clear at this zero-based step')
    clear.add_argument('--normal-clear-at', type=int, help='award a stage 6 clear at this zero-based step')
    parser.add_argument('--stage6-input', type=Path, help='held-input script played until the stage 6 ending begins')
    parser.add_argument('--refill-lives', type=int, choices=range(1, 9), help='refill stage 6 lives to this count')
    parser.add_argument('--clear-score', '--extra-score', dest='clear_score', type=int, default=800000,
                        help='score just before the clear bonus')
    inputs = parser.add_mutually_exclusive_group()
    inputs.add_argument('--input', default='', help='zero-based one-frame inputs, e.g. 121:0x1')
    inputs.add_argument('--z-pulses', action='store_true', help='press Z on alternate frames')
    args = parser.parse_args()
    if not 1 <= args.draw_from <= args.frames:
        parser.error('draw-from must be between 1 and frames')
    events = title_oracle.parse_input(args.input)
    clear_step = args.normal_clear_at if args.normal_clear_at is not None else args.extra_clear_at
    if clear_step is not None:
        if not 0 <= clear_step < args.frames or not 0 <= args.clear_score <= 999999999:
            parser.error('the clear step or score is out of range')
    if args.extra_clear_at is not None:
        if args.stage6_input is not None or args.refill_lives is not None:
            parser.error('--stage6-input and --refill-lives are for stage 6')
        record_extra_clear(args, events)
        return
    if args.normal_clear_at is not None or args.stage6_input is not None:
        record_normal_clear(args, events)
        return
    if args.refill_lives is not None:
        parser.error('--refill-lives requires --stage6-input or --normal-clear-at')

    state = game_oracle.draw_compare.OracleState()
    oracle = EndingOracle(args.exe, args.dat_dir)
    oracle.begin_ending(args.shot, args.difficulty, args.continues, state)
    with args.draw_ops.open('w', encoding='utf-8') as stream:
        def record(frame, calls):
            if oracle.u32(game_oracle.DAT_SCENE_REQUEST) == ENDING:
                ops = state.fold(calls)
                if frame >= args.draw_from:
                    stream.write(json.dumps({'frame': frame, 'ops': ops}, separators=(',', ':')) + '\n')

        oracle.on_draw_frame = record
        for frame in range(args.frames):
            bits = int(frame % 2 == 0) if args.z_pulses else events.get(frame, 0)
            oracle.tick_ending(bits)
            request = oracle.u32(game_oracle.DAT_SCENE_REQUEST)
            if request != ENDING:
                if request != ENDING_EXIT:
                    raise RuntimeError(f'unexpected ending exit request {request}')
                break
    errors = [error for error in oracle.errors if not error.startswith('note')]
    if errors:
        raise RuntimeError(f'original ending oracle reported errors: {errors}')
    print(json.dumps({
        'frames_recorded': max(0, oracle.frame - int(request == ENDING_EXIT) - args.draw_from + 1),
        'exit_frame': oracle.frame if request == ENDING_EXIT else None,
        'messages': [name for name in oracle.files if name.endswith('.msg') and name.startswith(('e0', 'staff'))],
    }))


if __name__ == '__main__':
    main()
