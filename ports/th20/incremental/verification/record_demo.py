"""Observe stable game-field samples from original built-in demo replays.

The target is never written or suspended. Each candidate is bracketed by replay
identity/frame reads and accepted only after identical reads at least 2 ms apart.
This reduces torn-state observations; it does not assert complete frame capture.
"""
from __future__ import annotations
import argparse
import datetime
import hashlib
import json
from pathlib import Path
import struct
import time
from inspect_process import Remote


def observe(remote):
    base = remote.base
    ptr = remote.value(0x1c60fc)
    mode = remote.value(0x1ba5d4)
    if not ptr or not mode & 0x20:
        return None
    head = remote.read(ptr + 0x250, 0x110)
    frame, _, stage = struct.unpack_from('<iii', head)
    filename = head[16:].split(b'\0', 1)[0].decode('cp932', errors='replace').replace('\\', '/').rsplit('/', 1)[-1]
    if filename not in {'demo1.rpy', 'demo2.rpy', 'demo3.rpy', 'demo4.rpy'} or frame < 0:
        return None
    side = remote.read(base + 0x1ba5f0, 0xf0)
    fields = {'score': struct.unpack_from('<Q', side)[0]}
    field_offsets = dict(character=0x08, story_stone=0x0c, narrow_shot_stone=0x10,
                        wide_shot_stone=0x14, assist_stone=0x18, power=0x30,
                        point_value=0x44, hyper_meter=0x4c, summon_meter=0x5c,
                        stone_level_red=0x74, stone_level_blue=0x78,
                        stone_level_yellow=0x7c, stone_level_green=0x80,
                        life_stocks=0xb8, life_fragments=0xc0, bomb_stocks=0xcc,
                        bomb_fragments=0xd0, graze=0xe4)
    fields.update({name: struct.unpack_from('<i', side, offset)[0] for name, offset in field_offsets.items()})
    fields['replay_input_bits'] = remote.value(0x1b8b4c)
    after_ptr = remote.value(0x1c60fc)
    if after_ptr != ptr or remote.read(ptr + 0x250, 0x110) != head:
        return None
    return dict(replay=filename, frame=frame, stage=stage, fields=fields)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('pid', type=int)
    parser.add_argument('--seconds', type=float, default=65)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    remote = Remote(args.pid)
    report = dict(format_version=2, pid=args.pid, executable=remote.modules[0],
                  executable_sha256=hashlib.sha256(Path(remote.modules[0]['path']).read_bytes()).hexdigest(),
                  started_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                  method='ReadProcessMemory only; unchanged replay header around each field read; identical candidates >=2ms apart',
                  frame_evidence='analysis/ghidra/pseudocode/00507b70.c: ReplayMgr+0x250 increment after recorded input',
                  limitations=['Only listed fields are compared; no bullet/entity or image/audio hash.',
                               'Asynchronous stable observations are not synchronized full-frame execution traces.',
                               'Missing or unstable frames are omitted, never interpolated.'], samples=[])
    started = time.monotonic()
    previous = None
    previous_time = 0
    seen = set()
    errors = 0
    try:
        while time.monotonic() - started < args.seconds:
            now = time.monotonic()
            try:
                current = observe(remote)
            except (OSError, ValueError):
                current = None
                errors += 1
            if current is not None and current == previous and now - previous_time >= 0.002:
                key = (current['replay'], current['stage'], current['frame'])
                if key not in seen:
                    seen.add(key)
                    report['samples'].append(current)
            else:
                previous = current
                previous_time = now
            time.sleep(0.001)
    finally:
        remote.close()
    report['read_errors_during_transitions'] = errors
    report['elapsed_seconds'] = time.monotonic() - started
    report['ended_utc'] = datetime.datetime.now(datetime.timezone.utc).isoformat()
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2), encoding='utf-8')
    counts = {name: sum(x['replay'] == name for x in report['samples']) for name in sorted({x['replay'] for x in report['samples']})}
    print(json.dumps(dict(output=str(args.output), samples=len(report['samples']), demos=counts), indent=2))


if __name__ == '__main__':
    main()
