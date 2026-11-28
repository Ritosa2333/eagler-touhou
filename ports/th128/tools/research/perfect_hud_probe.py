import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet,ROOT,EXE_SHA256,datum
from freeze_probe import emulate

CONTROL,MANAGER,ROOT_VM,NODE,CHILD=0x300000,0x500000,0x390000,0x380000,0x3b0000

class NativePerfectHud(NativeBullet):
    def flag85(self):
        target=struct.unpack('<I',self.read(0x45d0d4+86*4,4))[0]
        if target!=0x45c494:raise ValueError('unexpected original ANM85 dispatch')
        result=emulate([self.chunk(0x45c494,0x45c4bf),self.chunk(0x459e1a,0x459e31)],target,
            [datum('flags',ROOT_VM+0x4b0,6),datum('pc',ROOT_VM+0x424,0),
             datum('command',0x370000,struct.pack('<HHhHI',85,12,0,0,1),writable=False)],
            registers={'edi':ROOT_VM,'esi':0x370000},stops=[0x459e1a],budget=64)
        if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
        result['flagsAfter']=struct.unpack('<I',bytes.fromhex(result['memory']['flags']['after']))[0]
        return result
    def notification(self):
        result=emulate([self.chunk(0x424d6c,0x424d9b),self.chunk(0x4679b0,0x4679f6),self.chunk(0x467420,0x467479)],0x424d6c,
            [datum('oldHandle',CONTROL+0x588c,0),datum('resource',CONTROL+0x5a34,0x710000,kind='I',writable=False),
             datum('managerPointer',0x4d2e50,MANAGER,kind='I',writable=False)],registers={'edi':CONTROL},budget=512)
        if result['stop'].get('target')!='0x004670b0':raise ValueError(result['stop'])
        result['argumentsAtFactory']=list(struct.unpack_from('<4I',bytes.fromhex(result['stackHead'])))
        return result
    def charge(self,charge):
        root=bytearray(0x4e8);struct.pack_into('<I',root,0,0x1234)
        struct.pack_into('<I',root,0x10,ROOT_VM)
        struct.pack_into('<2I',root,0x14,CHILD+0x10,0)
        nodes=[]
        for index in range(3):
            location=CHILD+index*0x1000;image=bytearray(0x4e8);struct.pack_into('<I',image,0,0x2000+index)
            struct.pack_into('<h',image,0x41e,12+index);struct.pack_into('<I',image,0x4b0,4)
            struct.pack_into('<3I',image,0x10,location,CHILD+(index+1)*0x1000+0x10 if index<2 else 0,ROOT_VM+0x10 if index==0 else CHILD+(index-1)*0x1000+0x10)
            nodes.append(datum('child'+str(index),location,bytes(image)))
        result=emulate([self.chunk(0x425820,0x425994)],0x425820,
            [datum('charge',0x4b4d70,charge,writable=False),datum('controlRoot',CONTROL+0x58dc,0x1234,kind='I'),
             datum('managerPointer',0x4d2e50,MANAGER,kind='I',writable=False),datum('primary',MANAGER+0x8b9704,NODE,kind='I',writable=False),
             datum('lookupNode',NODE,struct.pack('<2I',ROOT_VM,0),writable=False),datum('root',ROOT_VM,bytes(root)),*nodes],
            arguments=struct.pack('<I',CONTROL),budget=2048)
        if result['stop']['reason']!='returned':raise ValueError(result['stop'])
        result.update(charge=charge,enabled=[bool(struct.unpack_from('<I',bytes.fromhex(result['memory']['child'+str(index)]['after']),0x4b0)[0]&2) for index in range(3)])
        return result

def probe(executable):
    native=NativePerfectHud(executable)
    try:
        notification=native.notification();charges=[native.charge(value) for value in (0,10000,20000,30000)];flag=native.flag85()
        return {'schema':'th128-original-perfect-hud-prefixes/1','executable':{'sha256':EXE_SHA256,
            'sha256After':hashlib.sha256(native.executable.read_bytes()).hexdigest()},'notification':notification,'chargeCases':charges,'flag85':flag,'code':native.code,
            'nativeCpuTrace':False,'windowsGameProcessLaunched':False,'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    finally:native.image.close()

if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);arguments=parser.parse_args()
    report=probe(arguments.exe);output=ROOT/'local/research/perfect-hud-fixtures.json'
    output.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8',newline='\n')
    print('original front47 notice prefix and actual three-child charge draw operation recorded')
