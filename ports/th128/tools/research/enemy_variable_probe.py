import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet,ROOT,EXE_SHA256,emulate,datum

def probe(executable):
    native=NativeBullet(executable)
    try:
        cases=[];integers=[]
        for identifier,read_target,read_offset in ((-9981,0x41d438,0x1414),(-9980,0x41d445,0x1418)):
            table=struct.unpack('<I',native.read(0x41daa8+(identifier+10000)*4,4))[0]
            if table!=read_target:raise ValueError('unexpected float reader dispatch')
            reader=native.read(read_target,6)
            if reader[:2]!=b'\xd9\x81' or struct.unpack_from('<I',reader,2)[0]!=read_offset:raise ValueError('unexpected float read field')
            result=emulate([native.chunk(0x41dc20,0x41dd78)],0x41dc20,[native.constant(0x41dd78,181)],
                registers={'ecx':0x300000},arguments=struct.pack('<i',identifier),budget=256)
            if result['stop']['reason']!='returned':raise ValueError(result['stop'])
            result.update(identifier=identifier,nativeOffset=result['registers']['eax']-0x300000,readOffset=read_offset,
                          fieldRelativeToEnemyData=result['registers']['eax']-0x300000-0x11c8)
            cases.append(result)
        for identifier in (-9985,-9984,-9983,-9982):
            result=emulate([native.chunk(0x41d140,0x41d23a)],0x41d140,[native.constant(0x41d23c,127)],
                registers={'ecx':0x300000},arguments=struct.pack('<i',identifier),budget=256)
            if result['stop']['reason']!='returned':raise ValueError(result['stop'])
            result.update(identifier=identifier,nativeOffset=result['registers']['eax']-0x300000);integers.append(result)
        manager=0x380000
        initialization=emulate([native.chunk(0x4152e0,0x415303),native.chunk(0x47bb30,0x47bbaa)],0x4152e0,
            [datum('manager',manager,bytes([0xa5])*0xcc),datum('managerGlobal',0x4b8948,0,'I'),
             datum('memsetCpuFlag',0x4dcd68,0,writable=False)],registers={'esi':manager},budget=512)
        if initialization['stop']['reason']!='returned':raise ValueError(initialization['stop'])
        manager_cases=[]
        for identifier,offset,float_target in ((-9949,0x10,0x41d8b6),(-9948,0x14,0x41d8c6),(-9947,0x18,0x41d8d5)):
            if struct.unpack('<I',native.read(0x41daa8+(identifier+10000)*4,4))[0]!=float_target:
                raise ValueError('unexpected manager float-reader dispatch')
            float_code=native.read(float_target,16)
            if float_code[6:9]!=bytes((0xdb,0x42 if identifier==-9949 else 0x40 if identifier==-9948 else 0x41,offset)):
                if identifier!=-9948 or float_code[5:8]!=bytes((0xdb,0x40,offset)):
                    raise ValueError('manager float reader does not convert the signed integer field')
            for value in (0,1,-34,-2147483648,2147483647):
                state=bytearray(0xcc);struct.pack_into('<i',state,offset,value)
                environment=[datum('managerGlobal',0x4b8948,manager,'I',False),datum('manager',manager,bytes(state),writable=False)]
                lvalue=emulate([native.chunk(0x41d140,0x41d23a)],0x41d140,
                    [native.constant(0x41d23c,127),*environment],registers={'ecx':0x300000},
                    arguments=struct.pack('<i',identifier),budget=256)
                reader=emulate([native.chunk(0x41c950,0x41c970),native.chunk(0x41cbd9,0x41cc08)],0x41c950,
                    [native.constant(0x41cfc4,368),*environment],registers={'ecx':0x300000},
                    arguments=struct.pack('<i',identifier),budget=256)
                float_reader=emulate([native.chunk(0x41d2c0,0x41d2e0),native.chunk(0x41d8b6,0x41d8e5)],0x41d2c0,
                    [native.constant(0x41daa8,368),*environment],registers={'ecx':0x300000},
                    arguments=struct.pack('<i',identifier),budget=256)
                if any(result['stop']['reason']!='returned' for result in (lvalue,reader,float_reader)):
                    raise ValueError('original manager reference did not return')
                manager_cases.append({'identifier':identifier,'input':value,'nativeOffset':lvalue['registers']['eax']-manager,
                    'lvalue':lvalue,'reader':reader,'floatReader':float_reader,'floatReaderOpcode':'fild-int32'})
        return {'schema':'th128-original-enemy-float-lvalues/1','executable':{'sha256':EXE_SHA256,
            'sha256After':hashlib.sha256(native.executable.read_bytes()).hexdigest()},'cases':cases,'integerCases':integers,'code':native.code,'constants':native.constants,
            'managerInitialization':initialization,'managerCases':manager_cases,
            'managerInitializationScope':'preallocated constructor tail with explicit CRT scalar-fill flag0; original memset executes, no allocator/API returns',
            'nativeCpuTrace':False,'windowsGameProcessLaunched':False,'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    finally:native.image.close()

if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);arguments=parser.parse_args()
    report=probe(arguments.exe);output=ROOT/'local/research/enemy-variable-fixtures.json'
    output.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8',newline='\n')
    print('original inherited float lvalue addresses and matching reader fields recorded')
