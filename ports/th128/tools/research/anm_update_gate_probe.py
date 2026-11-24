import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet,ROOT,EXE_SHA256,datum,emulate

VM,COMMAND,CONTROL=0x300000,0x360000,0x380000

def probe(executable):
    native=NativeBullet(executable)
    try:
        handler_chunks=[native.chunk(0x45a4a1,0x45a4d6),native.chunk(0x459e1a,0x459e20)]
        gate_chunks=[native.chunk(0x459deb,0x459e20),native.chunk(0x459ec3,0x459ec9),native.chunk(0x45d0ba,0x45d0c1)]
        handlers=[]
        for previous in (0,0xffffffff,0x12345678):
            for value in (0,1,2,3,4,-1):
                vm=bytearray(0x4e8);struct.pack_into('<I',vm,0x4b4,previous)
                command=struct.pack('<HHhHi',111,12,0,0,value)
                result=emulate(handler_chunks,0x45a4a1,
                    [datum('vm',VM,bytes(vm)),datum('command',COMMAND,command,writable=False)],
                    registers={'edi':VM,'esi':COMMAND},stops=(0x459e1a,),budget=64)
                if result['stop']!={'reason':'boundary','address':'0x00459e1a'}:raise ValueError(result['stop'])
                state=bytes.fromhex(result['memory']['vm']['after'])
                if struct.unpack_from('<I',state,0x424)[0]!=COMMAND+12:raise ValueError('111 did not advance actual native PC')
                result.update(previous=previous,value=value,after=struct.unpack_from('<I',state,0x4b4)[0])
                handlers.append(result)
        gates=[]
        for mode in (0,0x40,0x80,0xc0):
            for irq in (0,1,-1):
                for present,control_flags in ((False,0),(True,0),(True,1),(True,2),(True,3)):
                    flags=0xabcdef00|mode
                    vm=bytearray(0x4e8);struct.pack_into('<I',vm,0x4b4,flags);struct.pack_into('<h',vm,0x3f8,irq)
                    control=bytearray(0x64);struct.pack_into('<I',control,0x60,control_flags)
                    result=emulate(gate_chunks,0x459deb,
                        [datum('vm',VM,bytes(vm),writable=False),datum('control',CONTROL,bytes(control),writable=False),
                         datum('controlGlobal',0x4b8954,CONTROL if present else 0,'I',False)],
                        registers={'edi':VM},stops=(0x459e1a,0x459ec3,0x45d0ba),budget=64)
                    if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
                    result.update(flags2=flags,irq=irq,present=present,controlFlags=control_flags,
                        paused=result['stop']['address']=='0x0045d0ba')
                    gates.append(result)
        return {'schema':'th128-original-anm-update-gate/1','executable':{'sha256':EXE_SHA256,
            'sha256After':hashlib.sha256(native.executable.read_bytes()).hexdigest()},
            'handlerCases':handlers,'gateCases':gates,'code':native.code,'constants':native.constants,
            'scope':'complete unmasked111 flag/PC stores; genuine pre-dispatch gate prefix with declared control pointer/flags and pending IRQ; stop before normal dispatch, IRQ dispatch or common return epilogue',
            'nativeCpuTrace':False,'windowsGameProcessLaunched':False,
            'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    finally:native.image.close()

if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);arguments=parser.parse_args()
    report=probe(arguments.exe)
    (ROOT/'local/research/anm-update-gate-fixtures.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8',newline='\n')
    print('18 original111 handlers and60 genuine control/IRQ gate prefixes recorded')
