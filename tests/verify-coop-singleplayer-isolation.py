#!/usr/bin/env python3
"""Compare optimized ordinary-build objects against the audited Eagler base.

Uses the same temporary input path and current headers for both compiles. New
header members are MP-guarded. This is object identity, not asset/runtime QA.
"""
from pathlib import Path
import hashlib
import os
import subprocess
import tempfile

root=Path(__file__).resolve().parents[1]
six='TH06_MULTI_MAX_PLAYERS' in (root/'src/Multiplayer.hpp').read_text()
base='b3df2df27dd6b31fde2ec96f8fa881742563decc' if six else '77af369be0b9282338111cb6e4007439d6497ebc'
cxx=os.environ.get('EMXX','em++')
files=['Player.cpp','ItemManager.cpp','GameManager.cpp','AsciiManager.cpp','BulletManager.cpp']
with tempfile.TemporaryDirectory(prefix='coop-sp-identity-') as tmp:
    source=Path(tmp)/'same-input.cpp'
    obj=Path(tmp)/'same-output.o'
    for name in files:
        outputs=[]
        old=subprocess.check_output(['git','show',f'{base}:src/{name}'],cwd=root)
        new=(root/'src'/name).read_bytes()
        for text in [old,new]:
            source.write_bytes(text)
            subprocess.run([cxx,'-std=c++20','-O2','-Isrc','-Ithird_party/eagler-common/include',
                            '-c',str(source),'-o',str(obj)],cwd=root,check=True)
            outputs.append(obj.read_bytes())
        assert outputs[0]==outputs[1],f'ordinary object changed: {name}'
        print(f'PASS ordinary wasm object identical: {name} sha256={hashlib.sha256(outputs[1]).hexdigest()}',flush=True)
