import argparse
import hashlib
import json
import re
import struct
from bullet_probe import NativeBullet, ROOT

def prepare(executable):
    native=NativeBullet(executable)
    try:
        table=native.read(0x4b1268,76*20);sounds={}
        for row in range(76):
            identifier,wave,packed,loop,unused=struct.unpack_from('<5I',table,row*20)
            if identifier>=76 or wave>64 or str(identifier) in sounds:raise ValueError('unexpected native sound table')
            pointer=struct.unpack('<I',native.read(0x4b1858+wave*4,4))[0]
            name=native.read(pointer,64).split(b'\0',1)[0].decode('ascii')
            if not re.fullmatch(r'se_[A-Za-z0-9_-]+\.wav',name):raise ValueError('invalid native sound asset name')
            sounds[str(identifier)]={'name':name,'volumeRaw':struct.unpack('<h',struct.pack('<H',packed&65535))[0],
                'sourceRow':row,'waveIndex':wave,'loop':bool(loop)}
        output=ROOT/'local/runtime/sound-bindings.json'
        if output.is_symlink():raise ValueError('sound bindings must not be redirected')
        report={'exeSha256':hashlib.sha256(native.binary).hexdigest(),'schema':'th128-prepared-sound-bindings/1',
            'sounds':sounds,'sourceTableAddress':'0x004b1268'}
        output.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8',newline='\n')
        return report
    finally:native.image.close()

if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);arguments=parser.parse_args()
    print(f'{len(prepare(arguments.exe)["sounds"])} actual native sound IDs prepared')
