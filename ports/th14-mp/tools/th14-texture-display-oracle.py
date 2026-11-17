"""Compare TH14 ANM texture allocation at 640, 960 and 1280 with retail 1.00b.

Retail texture allocation and upload are recorded at the D3D boundary; the
candidate uses the actual GLES build's TextureImage and ImageResample objects.
"""
import argparse
import importlib
import json
import os
from pathlib import Path
import struct
import subprocess
import sys
import tempfile

from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_EIP, UC_X86_REG_ESP

ui = importlib.import_module('th14-ui-oracle')
ROOT, RUNTIME = ui.ROOT, ui.RUNTIME


def native_rows(image):
    retail = ui.Retail(image)
    machine = retail.machine
    current, uploads, result = {}, [], {}
    arena = ui.DATA + 0x8000

    def allocate(size):
        nonlocal arena
        p = arena
        arena = (arena + size + 15) & ~15
        machine.mem_write(p, bytes(size))
        return p

    def get(p):
        return retail.words(p, 1)[0]

    def ret(value, argc):
        sp = machine.reg_read(UC_X86_REG_ESP)
        machine.reg_write(UC_X86_REG_EAX, value)
        machine.reg_write(UC_X86_REG_ESP, sp + 4 + argc * 4)
        machine.reg_write(UC_X86_REG_EIP, get(sp))

    host = importlib.import_module('th14-native-physical-texture-host')
    host.install(machine, allocate, current, uploads, get, retail.put, ret)
    owner, header, tex = ui.DATA, ui.DATA + 0x100, ui.DATA + 0x200
    for display in (640, 960, 1280):
        retail.put(0x4f7a6c, ui.float_word(display/640))
        for flag in (0, 1):
            for odd in (0, 1):
                width, height, pw, ph = (n+odd for n in (16,20,12,16))
                chunk = bytearray(64)
                chunk[32], chunk[34] = 1, flag
                pixels = bytes((i*73+29) & 255 for i in range(pw*ph*4))
                machine.mem_write(owner, bytes(0x40))
                retail.put(owner+0x10, header)
                machine.mem_write(header, bytes(chunk))
                machine.mem_write(tex, struct.pack('<4s4HI', b'THTX',0,1,pw,ph,len(pixels))+pixels)
                current.update(chunk=chunk, index=0, textures={})
                uploads.clear()
                retail.run(0x47cb70,owner,tex,1,width,height)
                row, = uploads
                assert row['sourceRect']==[0,0,pw,ph] and row['sourceSha256']==ui.digest(pixels)
                assert row['filter'] in (1, 0x30004)
                result[display, flag, odd] = row
    return result


def candidate_rows(build, sdk):
    receipt = json.loads((build/'build.json').read_bytes())
    assert ui.digest((build/'th14-sdl.wasm').read_bytes()) == receipt['sha256']
    for path, expected in receipt['sourceBindings'].items():
        assert ui.digest((ROOT/path).read_bytes())==expected, path
    objects=[]
    for source in ('TextureImage.cpp', 'ImageResample.cpp'):
        name, = [name for name in receipt['objects'] if name.endswith('_'+source+'.o')]
        obj = build/name
        assert ui.digest(obj.read_bytes())==receipt['objects'][name], name
        objects.append(obj)
    flags = [f for f in receipt['flags'] if f not in ('--profiling-funcs', '--use-port=sdl3')]
    fixture = RUNTIME/'th12_web/tests/cpp/th14-texture-display-oracle.cpp'
    with tempfile.TemporaryDirectory(prefix='texture-display-',dir=build) as temp:
        module = Path(temp)/'probe.mjs'
        runner = Path(temp)/'run.mjs'
        env = {**os.environ, 'EMSDK':str(sdk), 'EM_CONFIG':str(sdk/'.emscripten'),
               'TMP':temp,'TMPDIR':temp,'TEMP':temp}
        subprocess.run([sys.executable,str(sdk/'upstream/emscripten/emcc.py'),*flags,
                        '-Ith12_web/cpp/game',str(fixture),*map(str,objects),
                        '-sDEFAULT_TO_CXX=1','-sENVIRONMENT=node','-sEXIT_RUNTIME=1',
                        '-o',str(module)],check=True,cwd=RUNTIME,env=env)
        runner.write_text("import make from './probe.mjs';\nawait make();\n",encoding='utf8')
        lines = subprocess.check_output(['node',str(runner)],text=True).splitlines()
    return {tuple(words[:3]): words[3:] for words in (list(map(int,line.split())) for line in lines)}


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--exe', type=Path, required=True)
    parser.add_argument('--build', type=Path, required=True)
    parser.add_argument('--emsdk', type=Path, required=True)
    parser.add_argument('--output', type=Path)
    a=parser.parse_args()
    image = importlib.import_module('th-anm-handlers').handlers(14,a.exe)[0]
    assert ui.digest(image.data)=='f08e885e28d2e0fc8c579e88034dd7de8f2a648d8f08ce781cf08eadc9ffe353'
    native=native_rows(image)
    candidate=candidate_rows(a.build.resolve(),a.emsdk.resolve())
    assert candidate.keys()==native.keys()
    differences=[]
    for key,row in native.items():
        expected=[row['width'],row['height'],int(row['filter']==1)]
        if candidate[key]!=expected:
            differences.append({'case':key,'retail':expected,'candidate':candidate[key]})
    report={'passed':not differences,'cases':len(native),'differences':differences,
            'rows':[{'display':key[0],'flagged':bool(key[1]),'odd':bool(key[2]),
                     'allocation':[row['width'],row['height']],'upload':row['destinationRect'][2:],
                     'retailFilter':row['filter'],'candidate':candidate[key]} for key,row in native.items()]}
    if a.output:
        a.output.write_text(json.dumps(report,indent=2)+'\n',encoding='utf8')
    print(json.dumps({k:report[k] for k in ('passed','cases','differences')}))
    raise SystemExit(bool(differences))


if __name__=='__main__':
    main()
