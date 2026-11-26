#!/usr/bin/env python3
"""Run the original game with bounded-memory state digests.

Uses GameOracle and its unmodified digest functions. Each function consumes
the same snapshots in order through a bounded iterator. Only explicitly
requested checkpoint frames are retained, rather than an entire replay.
"""
import argparse
from concurrent.futures import ThreadPoolExecutor
import importlib.util
import json
from pathlib import Path
from queue import Full, Queue
import struct
import sys

spec = importlib.util.spec_from_file_location('th16_game_oracle', Path(__file__).with_name('game-oracle.py'))
game = importlib.util.module_from_spec(spec)
sys.modules[spec.name] = game
spec.loader.exec_module(game)

DIGESTS = {
    'digest': game.frame_digest, 'enemies': game.enemy_digest,
    'enemy_vms': game.enemy_vm_digest, 'bullets': game.bullet_digest,
    'items': game.item_digest, 'spell': game.spell_digest,
    'lasers': game.laser_digest, 'bombs': game.bomb_digest,
    'menu': game.menu_digest, 'sound': game.sound_digest,
}


class DigestStream:
    def __init__(self):
        self.pool = ThreadPoolExecutor(max_workers=len(DIGESTS))
        self.queues = {key: Queue(maxsize=2) for key in DIGESTS}
        self.tasks = {key: self.pool.submit(function, self.records(self.queues[key]))
                      for key, function in DIGESTS.items()}

    @staticmethod
    def records(queue):
        while True:
            record = queue.get()
            if record is None:
                return
            yield record

    def append(self, record):
        for key, queue in self.queues.items():
            while True:
                if self.tasks[key].done():
                    self.tasks[key].result()
                    raise RuntimeError(f'Digest consumer {key} stopped before end of stream')
                try:
                    queue.put(record, timeout=0.1)
                    break
                except Full:
                    continue

    def finish(self):
        for key, queue in self.queues.items():
            while not self.tasks[key].done():
                try:
                    queue.put(None, timeout=0.1)
                    break
                except Full:
                    continue
        try:
            return {key: task.result() for key, task in self.tasks.items()}
        finally:
            self.pool.shutdown()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--check-snapshot', help='Verify stream versus batch on an existing oracle capture')
    parser.add_argument('exe', nargs='?')
    parser.add_argument('dat_dir', nargs='?')
    parser.add_argument('frames', type=int, nargs='?')
    parser.add_argument('out', nargs='?')
    for key, default in [('stage', 1), ('shot', 0), ('season', 0), ('difficulty', 1),
                         ('seed', 0), ('replay-slot', -1), ('lives', None), ('mode', 0)]:
        parser.add_argument('--' + key, type=int, default=default)
    parser.add_argument('--input', default='')
    parser.add_argument('--refill-lives', action='store_true')
    for key in ['enemies', 'bullets', 'items', 'lasers', 'bombs']:
        parser.add_argument('--' + key, action='store_true')
    parser.add_argument('--keep-frame', action='append', type=int, default=[])
    args = parser.parse_args()
    if args.check_snapshot:
        frames = json.loads(Path(args.check_snapshot).read_text())['frames']
        stream = DigestStream()
        for frame in frames:
            stream.append(frame)
        actual = stream.finish()
        expected = {key: function(frames) for key, function in DIGESTS.items()}
        if actual != expected:
            raise RuntimeError(f'Stream/batch mismatch: {actual} != {expected}')
        print(f'Stream/batch identical: {len(frames)} frames, {len(DIGESTS)} digests')
        return 0
    if None in (args.exe, args.dat_dir, args.frames, args.out):
        parser.error('exe, dat_dir, frames and out are required')
    oracle = game.GameOracle(Path(args.exe), args.dat_dir, args.stage, args.shot, args.season,
                             args.difficulty, args.seed, False, args.replay_slot, args.lives, args.mode)
    for field, option in [('enemy', 'enemies'), ('bullet', 'bullets'), ('item', 'items'),
                          ('laser', 'lasers'), ('bomb', 'bombs')]:
        setattr(oracle, field + '_snap', getattr(args, option))
    oracle.normalize_laser_graze_z()
    oracle.boot()
    events = game.parse_input(args.input)
    stream = DigestStream()
    held = 0
    checkpoints = []
    wanted = set(args.keep_frame)
    try:
        for frame in range(args.frames):
            held = events.get(frame, held)
            oracle.set_input(held)
            if args.refill_lives and args.lives is not None and oracle.i32(0x4A57F4) < args.lives:
                oracle.uc.mem_write(0x4A57F4, struct.pack('<i', args.lives))
            oracle.tick()
            if oracle.scene_exit is not None:
                break
            record = oracle.snapshot()
            stream.append(record)
            if frame + 1 in wanted:
                checkpoints.append(record)
    finally:
        digests = stream.finish()
    errors = [e for e in oracle.errors if not e.startswith('note')]
    Path(args.out).write_text(json.dumps({
        'stage': args.stage, 'shot': args.shot, 'season': args.season,
        'difficulty': args.difficulty, 'input': args.input,
        'scene_exit': oracle.scene_exit,
        'exit_frame': oracle.frame if oracle.scene_exit is not None else None,
        'frames': checkpoints,
        'digests': {key: f'{value:016x}' for key, value in digests.items()},
    }, separators=(',', ':')))
    print('game-oracle: ' + ' '.join(f'{key}={value:016x}' for key, value in digests.items()) + f' errors={errors}')
    return 1 if errors else 0


if __name__ == '__main__':
    sys.exit(main())
