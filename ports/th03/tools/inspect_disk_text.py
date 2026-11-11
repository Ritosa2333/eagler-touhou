"""Locate original Japanese startup/pause messages without booting the disk."""
import gzip
from pathlib import Path
from prepare_assets import files

ROOT = Path(__file__).resolve().parents[1]
data = gzip.decompress((ROOT / 'web/disks/3-jp.hdi.gz').read_bytes())
for path, offsets, size in files(data):
    raw = b''.join(data[at:at + 8192] for at in offsets)[:size]
    if path.endswith(('.BAT', 'CONFIG.SYS')):
        print(path, raw.decode('cp932', errors='replace'))
    for text in ('STOP', 'ＳＴＯＰ', '方がいい', 'ポーズ', '一時停止'):
        needle = text.encode('cp932')
        at = raw.find(needle)
        while at >= 0:
            context = raw[max(0, at - 150):at + 180].decode('cp932', errors='replace')
            clean = ''.join(c if c.isprintable() or c in '\r\n' else ' ' for c in context)
            print(f'{path} {at:#x} [{text}] {clean}')
            at = raw.find(needle, at + len(needle))
