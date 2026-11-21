import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet,ROOT,EXE_SHA256,datum,emulate

def probe(executable):
    native=NativeBullet(executable)
    try:
        chunks=[native.chunk(0x4293d0,0x429544),native.chunk(0x429970,0x429990)];cases=[]
        for cursor,count,occupied in ((0,0,0),(3,254,0),(4,255,0),(15,511,0),(31,1023,0),(99,1024,0),(50,0,2)):
            manager=0x800000;slot=manager+0x18aa14+cursor*0xa40
            image=bytearray(0xa40);struct.pack_into('<i',image,0xa18,occupied)
            result=emulate(chunks,0x4293d0,[datum('managerGlobal',0x4b8a5c,manager,writable=False),
                datum('counters',manager+0x1cab18,struct.pack('<ii',cursor,count)),datum('item',slot,bytes(image)),
                datum('position',0x390000,struct.pack('<3f',10,200,.1),writable=False)],
                registers={'ecx':0x390000},arguments=struct.pack('<iff',9,0,0),budget=512)
            if result['stop']['reason']!='returned':raise ValueError(result['stop'])
            after=bytes.fromhex(result['memory']['item']['after']);counter=bytes.fromhex(result['memory']['counters']['after'])
            result.update(input={'cursor':cursor,'count':count,'occupied':occupied},cursorAfter=struct.unpack_from('<i',counter)[0],
                birthsAfter=struct.unpack_from('<i',counter,4)[0],state=struct.unpack_from('<i',after,0xa18)[0],delay=struct.unpack_from('<i',after,0xa28)[0])
            cases.append(result)
        return {'schema':'th128-original-ice-item-birth/1','sourceExeSha256':EXE_SHA256,'cases':cases,'code':native.code,
            'scope':'complete original4293d0 kind9 returns including real fsincos, independent100-slot cursor/count/delay; no allocation or API stubs',
            'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),'apiReturnsAssigned':False}
    finally:native.image.close()

if __name__=='__main__':
    import argparse
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);arguments=parser.parse_args()
    report=probe(arguments.exe);(ROOT/'local/research/ice-item-fixtures.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf8',newline='\n')
    print('7 complete native ice-item100-slot birth cases recorded')
