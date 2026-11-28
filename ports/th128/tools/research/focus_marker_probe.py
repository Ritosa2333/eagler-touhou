import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet,ROOT,datum,emulate

def probe(executable):
    native=NativeBullet(executable)
    try:
        gates=[];positions=[];player=0x300000
        for focused,handle in ((0,0),(0,1234),(1,0),(1,1234)):
            result=emulate([native.chunk(0x43b0ad,0x43b0eb)],0x43b0ad,
                [datum('focus',player+0x43a88,focused,writable=False),datum('handle',player+0x87a8,handle,writable=False),
                 datum('bulletManager',0x4b8930,0x3b0000,writable=False),datum('bank',0x3b0000+0x8a7f9c,0x710000,writable=False)],
                registers={'esi':player},stops=(0x43b120,0x43b0eb),budget=128)
            result.update(focused=focused,handle=handle);gates.append(result)
        for horizontal,vertical,depth in ((0,400,0),(-180,32,.1),(180,432,0),(-121.2890625,284.2109375,.1)):
            result=emulate([native.chunk(0x43b357,0x43b390)],0x43b357,
                [datum('position',player+0xecc,struct.pack('<3f',horizontal,vertical,depth),writable=False),
                 datum('handle',player+0x87a8,1234,writable=False),native.constant(0x4a65f8,8),native.constant(0x4a65d8,8),native.constant(0x4a6610,8)],
                registers={'esi':player},stops=(0x43b390,),budget=128)
            result.update(input=[horizontal,vertical,depth],pixelPosition=list(struct.unpack_from('<3f',bytes.fromhex(result['stackHead']),16)))
            positions.append(result)
        return {'schema':'th128-original-focus-marker/1','sourceExeSha256':hashlib.sha256(native.executable.read_bytes()).hexdigest(),
            'gates':gates,'positions':positions,'code':native.code,'constants':native.constants,
            'scope':'original focus branch, mandatory bullet80 layer12 factory request and post-move pixel-position prefix; factories/lookups are not given fake returns',
            'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),'apiReturnsAssigned':False}
    finally:native.image.close()
if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);args=parser.parse_args();report=probe(args.exe)
    (ROOT/'local/research/focus-marker-fixtures.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf8',newline='\n')
    print(json.dumps({'gates':[sample['stop'] for sample in report['gates']],'positions':[sample['pixelPosition'] for sample in report['positions']]}))
