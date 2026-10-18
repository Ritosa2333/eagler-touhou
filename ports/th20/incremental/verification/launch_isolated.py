"""Launch a workspace game build with process-local APPDATA for manual UI QA.

This is a test harness, not the delivered game launcher. It does not launch the
game in the original source directory and never changes the caller's environment.
"""
from __future__ import annotations
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('exe', type=Path)
    parser.add_argument('--profile', choices=('baseline', 'rebuilt'), required=True)
    args = parser.parse_args()
    exe = args.exe.resolve(strict=True)
    if not exe.is_relative_to(ROOT) or exe.suffix.lower() != '.exe':
        raise ValueError('Only a workspace executable may be launched by this test harness')
    appdata = HERE / (args.profile + '_appdata')
    if not (appdata / 'ShanghaiAlice' / 'th20' / 'th20.cfg').is_file():
        raise RuntimeError('Prepare isolated profiles first with preflight.py --prepare')
    env = os.environ.copy()
    env['APPDATA'] = str(appdata)
    process = subprocess.Popen([str(exe)], cwd=exe.parent, env=env,
                               stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL,
                               stderr=subprocess.DEVNULL)
    report = dict(pid=process.pid, executable=str(exe), working_directory=str(exe.parent),
                  appdata=str(appdata), utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                  sha256=hashlib.sha256(exe.read_bytes()).hexdigest())
    target = ROOT / 'reports' / f'playable_{args.profile}_launch.json'
    target.write_text(json.dumps(report, indent=2), encoding='utf-8')
    print(json.dumps(report, indent=2))


if __name__ == '__main__':
    main()
