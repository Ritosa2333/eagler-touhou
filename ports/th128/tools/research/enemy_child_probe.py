import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import ROOT,EXE_SHA256,STACK_POINTER,RETURN_TRAP,datum,emulate
from bullet_pool_probe import NativePool

PARENT,CHILD,OLD_CHILD,MANAGER,COMMAND,OUT=0x390000,0x3a0000,0x3c0000,0x3b0000,0x360000,0x370000

def probe(executable):
    native=NativePool(executable)
    try:
        if struct.unpack('<I',native.read(0x45d0d4+4*89,4))[0]!=0x45a32f:raise ValueError('unexpected88 dispatch')
        requests=[]
        for script in (186,187,188):
            vm=bytearray(0x4e8);struct.pack_into('<I',vm,0x42c,MANAGER)
            command=struct.pack('<HHhHi',88,12,0,0,script)
            result=emulate([native.chunk(0x45a32f,0x45a367),native.chunk(0x467220,0x467221)],0x45a32f,
                [datum('parent',PARENT,bytes(vm)),datum('command',COMMAND,command,writable=False)],
                registers={'edi':PARENT,'esi':COMMAND},stops=(0x467220,),budget=64)
            if result['stop']!={'reason':'boundary','address':'0x00467220'}:raise ValueError(result['stop'])
            words=struct.unpack_from('<5I',bytes.fromhex(result['stackHead']))
            result['request']={'script':words[2],'parent':words[3],'registrationFlags':words[4],
                'resource':result['registers']['eax']}
            requests.append(result)
        links=[]
        for prior in (False,True):
            parent,child,old=bytearray(0x4e8),bytearray(0x4e8),bytearray(0x4e8)
            struct.pack_into('<I',parent,4,PARENT);struct.pack_into('<I',parent,0x10,PARENT)
            struct.pack_into('<I',parent,0x14,OLD_CHILD+0x10 if prior else 0)
            struct.pack_into('<I',child,0x10,CHILD);struct.pack_into('<I',old,0x10,OLD_CHILD)
            struct.pack_into('<I',old,0x18,PARENT+0x10)
            frame={0x20:struct.pack('<I',0),0x24:struct.pack('<I',RETURN_TRAP),0x28:struct.pack('<I',OUT),
                0x2c:struct.pack('<I',186),0x30:struct.pack('<I',PARENT),0x34:struct.pack('<I',0)}
            result=emulate([native.chunk(0x46729a,0x46732a),native.chunk(0x466d60,0x466ddc)],0x46729a,
                [datum('parent',PARENT,bytes(parent)),datum('child',CHILD,bytes(child)),datum('oldChild',OLD_CHILD,bytes(old)),
                 datum('primaryLinks',MANAGER+0x8b9704,struct.pack('<2I',PARENT+4,PARENT+4)),
                 datum('idCounter',MANAGER+0x8c38fc,7,'I'),datum('output',OUT,0,'I'),
                 datum('managerGlobal',0x4d2e50,MANAGER,'I',False),datum('threadFlags',0x4d33f8,0,writable=False)],
                registers={'esi':CHILD,'ebp':STACK_POINTER+0x20},stack=frame,budget=256)
            if result['stop']['reason']!='returned':raise ValueError(result['stop'])
            after_parent=bytes.fromhex(result['memory']['parent']['after']);after_child=bytes.fromhex(result['memory']['child']['after'])
            head,tail=struct.unpack('<2I',bytes.fromhex(result['memory']['primaryLinks']['after']))
            result.update(priorChild=prior,after={'head':head,'tail':tail,'firstChild':struct.unpack_from('<I',after_parent,0x14)[0],
                'parentOfChild':struct.unpack_from('<I',after_child,0x3c)[0],
                'nextSibling':struct.unpack_from('<I',after_child,0x14)[0],
                'previousSibling':struct.unpack_from('<I',after_child,0x18)[0]})
            links.append(result)
        vm=bytearray(0x4e8);struct.pack_into('<2I',vm,4,PARENT,CHILD+4)
        successor=emulate([native.chunk(0x466b45,0x466b7d),native.chunk(0x459d90,0x459d91)],0x466b45,
            [datum('parent',PARENT,bytes(vm))],registers={'eax':PARENT+4},stops=(0x459d90,),budget=64)
        if successor['stop']!={'reason':'boundary','address':'0x00459d90'}:raise ValueError(successor['stop'])
        successor['cachedNext']=successor['registers']['edi']
        continuation=emulate([native.chunk(0x466ba6,0x466bb6)],0x466ba6,[datum('parentAfterAppend',PARENT,bytes(vm))],
            registers={'edi':0},stops=(0x466bac,),budget=16)
        if continuation['stop']!={'reason':'boundary','address':'0x00466bac'}:raise ValueError(continuation['stop'])
        reset=native.effect_bind_domain()
        if reset['stop'].get('target')!='0x00407080':raise ValueError(reset['stop'])
        return {'schema':'th128-original-enemy-child-prefixes/1','executable':{'sha256':EXE_SHA256,
            'sha256After':hashlib.sha256(native.executable.read_bytes()).hexdigest()},'requestCases':requests,'linkCases':links,
            'successorPrefix':successor,'cachedNullContinuation':continuation,'bindResetFlags2':reset['flags2'],
            'bindResetPrefix':reset,'code':native.code,'constants':native.constants,'allocationExecuted':False,
            'nativeCpuTrace':False,'windowsGameProcessLaunched':False,
            'scope':'actual88 requests stop before allocation; separate original post-bind suffix starts from declared prebound child and synthetic stack, executes real tail registration and parent links; traversal prefixes stop before VM update; no invented native returns',
            'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    finally:native.image.close()

if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);arguments=parser.parse_args()
    report=probe(arguments.exe)
    (ROOT/'local/research/enemy-child-fixtures.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8',newline='\n')
    print('actual88 requests, original tail registration/parent links and saved-successor prefixes recorded')
