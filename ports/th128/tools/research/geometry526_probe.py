import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet,ROOT,EXE_SHA256,CONTEXT,TASK,STATE,COMMAND,HANDLER_STOP,datum
from enemy_probe import emulate

OBJECT,VM_TABLE,VMS,VERTICES,POSITIONS,STRIPS=0x380000,0x390000,0x3a0000,0x3b0000,0x3c0000,0x3d0000

def probe(executable):
    native=NativeBullet(executable)
    try:
        chunks=[native.chunk(0x412550,0x41267f),native.chunk(0x412700,0x412783)]
        cases=[]
        creations=[]
        for radius,color,previous in ((80,0x00800080,0),(120,0xffffffff,0),(0,0,0),(80,0x00800080,0x370000)):
            context=bytearray(0x4000);struct.pack_into('<I',context,0x3f7c,TASK);struct.pack_into('<I',context,0x3f80,previous)
            state=bytearray(0x1020);struct.pack_into('<I',state,4,COMMAND)
            command=struct.pack('<iHHHBBIfI',0,526,24,0,255,2,0,radius,color)
            result=emulate([native.chunk(0x41c292,0x41c347),native.chunk(0x41e0c0,0x41e0d0),native.chunk(0x41e110,0x41e11e),
                native.chunk(0x46fed0,0x46ff66),native.chunk(0x46ff70,0x470026),native.chunk(0x46a700,0x46a78e),native.chunk(HANDLER_STOP,HANDLER_STOP+2)],0x41c292,
                [datum('context',CONTEXT,bytes(context)),datum('state',STATE,bytes(state)),datum('task',TASK,struct.pack('<2I',0,STATE),writable=False),
                 datum('command',COMMAND,command,writable=False),*[native.constant(address,8) for address in (0x4a6760,0x4a6578,0x4a6580,0x4a65b8)]],
                registers={'ebx':CONTEXT},stops=(HANDLER_STOP,),budget=512)
            target='0x00403970' if previous else '0x0047296c' if radius>0 else None
            if target and result['stop'].get('target')!=target:raise ValueError(result['stop'])
            if not target and result['stop']!={'reason':'boundary','address':'0x0041c52c'}:raise ValueError(result['stop'])
            after=bytes.fromhex(result['memory']['context']['after'])
            result.update(input={'radius':radius,'color':color,'previousObject':previous},
                after={'radius':struct.unpack_from('<f',after,0x3f84)[0],'scale':struct.unpack_from('<f',after,0x3f88)[0],
                    'color':struct.unpack_from('<I',after,0x3f8c)[0],'angles':list(struct.unpack_from('<2f',after,0x3f90))})
            creations.append(result)
        for columns,rows,x,y,width,height in ((17,17,0,0,384,448),(3,2,-320,-16,128,64),
            (2,3,-400,-40,40,80),(4,4,12.25,5.5,96.125,128.25)):
            header=struct.pack('<6I',columns,rows,0,VM_TABLE,VERTICES,POSITIONS)
            pointers=bytearray((columns-1)*4);vms=bytearray((columns-1)*0x4e8)
            for index in range(columns-1):
                struct.pack_into('<I',pointers,index*4,VMS+index*0x4e8)
                struct.pack_into('<I',vms,index*0x4e8+0x4ac,STRIPS+index*rows*56)
            result=emulate(chunks,0x412550,[datum('object',OBJECT,header,writable=False),
                datum('vmTable',VM_TABLE,bytes(pointers),writable=False),datum('vms',VMS,bytes(vms),writable=False),
                datum('vertices',VERTICES,bytes([0xa5])*(columns*rows*28)),
                datum('positions',POSITIONS,bytes([0xa5])*(columns*rows*12)),
                datum('strips',STRIPS,bytes([0xa5])*((columns-1)*rows*56)),
                *[native.constant(address,8) for address in (0x4a65d8,0x4a65f8,0x4a6610,0x4a65c8,0x4a65c0)]],
                registers={'eax':OBJECT},arguments=struct.pack('<4f',x,y,width,height),budget=40000)
            if result['stop']['reason']!='returned':raise ValueError(result['stop'])
            result.update(input={'columns':columns,'rows':rows,'x':x,'y':y,'width':width,'height':height},
                verticesAfter=result['memory']['vertices']['after'],positionsAfter=result['memory']['positions']['after'],
                stripsAfter=result['memory']['strips']['after'])
            cases.append(result)
        return {'schema':'th128-original-geometry526-grid/1','executable':{'sha256':EXE_SHA256,
            'sha256After':hashlib.sha256(native.executable.read_bytes()).hexdigest()},'gridCases':cases,
            'creationCases':creations,'code':native.code,'constants':native.constants,'allocationExecuted':False,'nativeCpuTrace':False,
            'windowsGameProcessLaunched':False,'scope':'complete412550 grid initialization including real412700 strip-copy callback over declared preallocated explicit17x17and small buffers; no allocator/API return values or claimed full effect lifetime',
            'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    finally:native.image.close()

if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);arguments=parser.parse_args()
    report=probe(arguments.exe)
    (ROOT/'local/research/geometry526-fixtures.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8',newline='\n')
    print('four complete original geometry grid/strip callbacks returned without allocation stubs')
