import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet, ROOT, datum, emulate


def probe(executable):
    native=NativeBullet(executable)
    try:
        context,task,state,command=0x300000,0x310000,0x320000,0x330000
        cases=[]
        for slot in (0,1,7):
            image=bytearray(0x4000);struct.pack_into('<I',image,0x3f7c,task)
            locals_image=bytearray(0x1020);struct.pack_into('<I',locals_image,4,command)
            records=bytes([0xa5])*(8*16);task_image=bytearray(0x50c4)+records;struct.pack_into('<I',task_image,4,state)
            payload=struct.pack('<iHHHBBIiI',0,421,36,0,255,2,0,slot,12)+b'MBossEscape\0'
            result=emulate([native.chunk(0x41b2fa,0x41b31b),native.chunk(0x41e0c0,0x41e0d0),native.chunk(0x46fed0,0x46ff66),native.chunk(0x41c52c,0x41c52e)],0x41b2fa,
                [datum('context',context,bytes(image)),datum('task',task,bytes(task_image)),
                 datum('state',state,bytes(locals_image)),datum('command',command,payload,writable=False)],
                registers={'ebx':context,'edi':command},stops=(0x41c52c,),budget=256)
            if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
            final=bytes.fromhex(result['memory']['task']['after'])
            result.update(slot=slot,records=[list(struct.unpack_from('<4I',final,0x50c4+index*16)) for index in range(8)],stringPointer=command+24)
            cases.append(result)
        return {'schema':'th128-original-phase421/1','cases':cases,'code':native.code,'constants':native.constants,'apiReturnsAssigned':False,
            'sourceExeSha256':hashlib.sha256(native.executable.read_bytes()).hexdigest(),'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    finally:native.image.close()


if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);report=probe(parser.parse_args().exe)
    (ROOT/'local/research/phase-timeout-override-fixtures.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf8')
    print('3 real421 timeout-only phase target writes recorded')
