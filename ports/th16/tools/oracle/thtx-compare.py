#!/usr/bin/env python3
"""把 AnmResource::decode_thtx 的 BGRA8 输出与 thanm -x16 提取出的 PNG 逐像素对照。

用法：
    thtx-compare.py <harness> <th16.dat> <anm-dump-dir> <png-root> name.anm [name.anm ...]

<anm-dump-dir>/<name>.txt 是 `thanm -l16` 的文本，<png-root>/<name>/ 是 `thanm -x16` 的输出目录。
同名 entry 会被 thanm 合成到同一张 PNG（按 xOffset/yOffset 摆放），因此按 entry 的 x/y 裁剪后再比较。
格式 1/3/5/7 全部走 thanm 自己的转换（thanm 输出 RGBA PNG），我们输出 BGRA8，因此比较时交换 B/R。
退出码 0 = 全部一致；每条不一致都打印第一处差异的像素坐标。
"""
import re
import subprocess
import sys
from pathlib import Path

from PIL import Image

harness, dat, dump_dir, png_root, *anms = sys.argv[1:]
failures = 0
checked = 0
for anm in anms:
    base = anm[:-4]
    dump = Path(dump_dir, base + '.txt').read_text(encoding='utf-8', errors='replace')
    entries = re.findall(r'^entry entry(\d+) \{\n\s+version: \d+,\n\s+name: "([^"]*)",', dump, re.M)
    for index, name in entries:
        raw = Path('/tmp/th16-thtx-%s-%s.bgra' % (base, index))
        proc = subprocess.run([harness, 'anm-texture', dat, anm, index, str(raw)], capture_output=True, text=True)
        if proc.returncode != 0:
            if 'entry has no texture' in proc.stderr:
                continue
            print('%s entry %s: harness failed: %s' % (anm, index, proc.stderr.strip()))
            failures += 1
            continue
        fmt, w, h, x, y = (int(v) for v in proc.stdout.split())
        ours = Image.frombytes('RGBA', (w, h), raw.read_bytes(), 'raw', 'BGRA')
        png = Path(png_root, base, name)
        if not png.exists():
            print('%s entry %s: thanm PNG missing: %s' % (anm, index, png))
            failures += 1
            continue
        theirs = Image.open(png).convert('RGBA').crop((x, y, x + w, y + h))
        a, b = ours.tobytes(), theirs.tobytes()
        if fmt == 3:
            # RGB565 的 5/6 位通道扩展到 8 位：thanm 只复制最低位（00001b→00001111b），
            # 我们按 GPU 规则复制高位（v<<3 | v>>2），与 D3D9/GLES 采样 R5G6B5 的结果一致。
            # 两者的高 5/6/5 位必定相同，因此只比较源位。
            mask = bytes((0xf8, 0xfc, 0xf8, 0xff)) * (w * h)
            a = bytes(p & m for p, m in zip(a, mask))
            b = bytes(p & m for p, m in zip(b, mask))
        checked += 1
        if a == b:
            continue
        failures += 1
        first = next(i for i in range(min(len(a), len(b))) if a[i] != b[i]) // 4
        print('%s entry %s (%s fmt=%d %dx%d @%d,%d): first diff at pixel (%d,%d) ours=%s thanm=%s' % (
            anm, index, name, fmt, w, h, x, y, first % w, first // w, a[first * 4:first * 4 + 4].hex(), b[first * 4:first * 4 + 4].hex()))
print('thtx-compare: %d textures checked, %d mismatches' % (checked, failures))
sys.exit(1 if failures else 0)
