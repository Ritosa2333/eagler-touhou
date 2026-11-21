import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet, ROOT, datum, emulate


def probe(executable):
    native=NativeBullet(executable)
    try:
        manager,bullet,profile=0x300000,0x380000,0x390000
        cases=[]
        for policy in (0,1,2,3):
            for age in (0,59,60):
                for override in (0,4):
                    image=bytearray(0x11b8);struct.pack_into('<I',image,0,1);struct.pack_into('<I',image,0x11ac,profile);struct.pack_into('<i',image,0x119c,age)
                    table=bytearray(0x120);struct.pack_into('<i',table,0x10c,1);struct.pack_into('<i',table,0x118,policy)
                    result=emulate([native.chunk(0x40eaf7,0x40eb3b),native.chunk(0x40eed3,0x40eee5)],0x40eaf7,
                        [datum('bullet',bullet,bytes(image),writable=False),datum('profile',profile,bytes(table),writable=False),datum('managerFlags',manager+0x6b1fc,override,kind='I',writable=False)],
                        registers={'ebx':manager},stack={0x10:struct.pack('<I',bullet)},stops=(0x40eb3b,0x40eed3),budget=64)
                    if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
                    result.update(policy=policy,age=age,override=bool(override),eligible=result['stop']['address']=='0x0040eb3b');cases.append(result)
        profiles=[]
        for selected_type in (4,8,10,18,20,23,29):
            payload=native.read(0x4b24d0+selected_type*0x120,0x120)
            profiles.append({'type':selected_type,'bytes':payload.hex(),'sha256':hashlib.sha256(payload).hexdigest(),
                'appearanceKind':struct.unpack_from('<i',payload,0x10c)[0],'freezePolicy':struct.unpack_from('<i',payload,0x118)[0]})
        return {'schema':'th128-original-large-bullet-freeze/1','cases':cases,'profiles':profiles,'code':native.code,
            'sourceExeSha256':hashlib.sha256(native.executable.read_bytes()).hexdigest(),'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
            'apiReturnsAssigned':False,'scope':'Original40eaf7 eligibility prefix reads profile+118, not appearance kind+10c; stops before contact/conversion. Raw typed profiles retained.'}
    finally:native.image.close()


if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);report=probe(parser.parse_args().exe)
    (ROOT/'local/research/large-freeze-fixtures.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf8')
    print(f'{len(report["cases"])} original freeze eligibility branches and{len(report["profiles"])} independent type/eligibility records captured')
