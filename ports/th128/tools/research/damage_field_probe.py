import argparse
from collections import Counter
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet,ROOT,EXE_SHA256,datum
from freeze_probe import emulate,STACK_POINTER

PLAYER,SLOT,POSITION=0x300000,0x380000,0x390000
SIZE,OUT_FLAG=0x3a0000,0x3b0000

def pool_regions(payload):
    return [datum('pool'+str(index),PLAYER+0x8edc+index*60000,payload[index*60000:(index+1)*60000])
            for index in range(4)]

def compact_pool(result):
    for index in range(4):
        region=result['memory'].pop('pool'+str(index))
        result.setdefault('poolHashes',[]).append({key:hashlib.sha256(bytes.fromhex(region[key])).hexdigest()
                                                  for key in ('before','after')})
    trace=result.pop('instructionTrace')
    result['traceSha256']=hashlib.sha256('\n'.join(trace).encode()).hexdigest()
    result['instructionVisits']=dict(Counter(trace))
    return result

class NativeDamageField(NativeBullet):
    def motion(self,position):
        image=bytearray.fromhex(self.birth()['memory']['slot']['after'])
        struct.pack_into('<3f',image,0x18,*position)
        result=emulate([self.chunk(0x43ba08,0x43bafc),self.chunk(0x46c1f0,0x46c210),
                        self.chunk(0x46ae80,0x46b00b),self.chunk(0x48fdf0,0x48ff11),
                        self.chunk(0x48fac0,0x48fb6b)],0x43ba08,
            [datum('slot',SLOT,bytes(image)),datum('clock',0x4b6f58,struct.pack('<f',1),writable=False),
             datum('cpu',0x4dcd68,1,writable=False),datum('mathCpu',0x4dbc20,1,writable=False),
             self.constant(0x4a6698,8),self.constant(0x4a6694,4),self.constant(0x4a6590,8),
             self.constant(0x4a6ca0,80)],
            registers={'edi':SLOT+0x24,'ebx':SLOT,'ebp':STACK_POINTER+0x100},
            stack={0x108:struct.pack('<I',1)},stops=[0x43bafc],budget=4096,cpu_state={'mxcsr':0x1f80,'fpcw':0x37f})
        if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
        final=bytes.fromhex(result['memory']['slot']['after'])
        result.update(positionBefore=position,positionAfter=list(struct.unpack_from('<3f',final,0x18)))
        return result

    def pool_birth(self,name,cursor,occupied,full=False):
        pool=bytearray(2000*0x78)
        for index in (range(2000) if full else occupied):
            struct.pack_into('<I',pool,index*0x78+0x74,3)
        result=emulate([self.chunk(0x43df90,0x43e089),self.chunk(0x47bb30,0x47bbaa)],0x43df90,
            [*pool_regions(bytes(pool)),datum('cursor',PLAYER+0x43a8c,cursor),
             datum('position',POSITION,struct.pack('<3f',0,400,.1),writable=False),
             datum('memsetCpu',0x4dcd68,0,writable=False)],registers={'ecx':PLAYER},
            arguments=struct.pack('<Iffiii',POSITION,32,16,30,150,0),budget=40000)
        if result['stop']['reason']!='returned':raise ValueError(result['stop'])
        selected=(result['registers']['eax']-PLAYER-0x8edc)//0x78
        pool_after=b''.join(bytes.fromhex(result['memory']['pool'+str(index)]['after']) for index in range(4))
        result.update(name=name,cursorBefore=cursor,occupied=list(occupied),full=full,
                      selectedSlot=selected,cursorAfter=result['memory']['cursor']['afterI32'],
                      selectedBefore=pool[selected*0x78:(selected+1)*0x78].hex(),
                      selectedAfter=pool_after[selected*0x78:(selected+1)*0x78].hex())
        return compact_pool(result)

    def query(self,name,*,position=(0,400,0),radius=32,previous=28,current=29,
              damage=150,tag=0,accumulated=0,limit=999999,score=0,flags=3,interval=4):
        image=bytearray(0x78)
        struct.pack_into('<f',image,0,radius)
        struct.pack_into('<3f',image,0x18,0,400,.1)
        struct.pack_into('<ii',image,0x4c,previous,current)
        struct.pack_into('<iii iII',image,0x60,damage,accumulated,limit,interval,tag,flags)
        pool=bytearray(2000*0x78);pool[:0x78]=image
        result=emulate([self.chunk(0x43fbf0,0x43ffaf)],0x43fbf0,
            [datum('playerPointer',0x4b8a80,PLAYER,kind='I',writable=False),
             datum('playerTimes',PLAYER+0xf80,struct.pack('<2i',7,8),writable=False),
             datum('shots',PLAYER+0xfa8,bytes(256*0x78),writable=False),
             datum('position',POSITION,struct.pack('<3f',*position),writable=False),
             datum('size',SIZE,struct.pack('<2f',32,32),writable=False),
             datum('outFlag',OUT_FLAG,0),datum('score',0x4b4cc4,score),
             self.constant(0x4a65a8,8),*pool_regions(bytes(pool))],
            arguments=struct.pack('<3I',POSITION,SIZE,OUT_FLAG),budget=40000)
        if result['stop']['reason']!='returned':raise ValueError(result['stop'])
        after=bytes.fromhex(result['memory']['pool0']['after'])[:0x78]
        result.update(name=name,position=position,sourceBefore=image.hex(),sourceAfter=after.hex(),
                      damage=result['registers']['eax'],
                      outFlagAfter=result['memory']['outFlag']['afterI32'],
                      sourceDamageAfter=struct.unpack_from('<i',after,0x60)[0],
                      scoreAfter=result['memory']['score']['afterI32'])
        result['memory'].pop('shots')
        return compact_pool(result)

    def birth(self,duration=30,radius=32,growth=16,damage=150):
        slot=PLAYER+0x8edc
        result=emulate([self.chunk(0x43df90,0x43e089),self.chunk(0x47bb30,0x47bbaa)],0x43df90,
            [datum('slot',slot,bytes(0x78)),datum('cursor',PLAYER+0x43a8c,0),
             datum('position',POSITION,struct.pack('<3f',0,400,.1),writable=False),
             datum('memsetCpu',0x4dcd68,0,writable=False)],registers={'ecx':PLAYER},
            arguments=struct.pack('<Iffiii',POSITION,radius,growth,duration,damage,0),budget=4096)
        if result['stop']['reason']!='returned':raise ValueError(result['stop'])
        result.update(duration=duration,radius=radius,growth=growth,damage=damage)
        return result

    def tail(self,radius=32,growth=16,remaining=30):
        image=bytearray(0x78);struct.pack_into('<2f',image,0,radius,growth)
        struct.pack_into('<ii fII',image,0x4c,remaining-1,remaining,float(remaining),0x4b6f58,1)
        struct.pack_into('<I',image,0x74,3)
        result=emulate([self.chunk(0x43ba82,0x43bafc),self.chunk(0x48fac0,0x48fb6b)],0x43ba82,
            [datum('slot',SLOT,bytes(image)),datum('clock',0x4b6f58,struct.pack('<f',1),writable=False),
             datum('cpu',0x4dcd68,1,writable=False),self.constant(0x4a6698,8),self.constant(0x4a6694,4)],
            registers={'edi':SLOT+0x24,'ebx':SLOT,'ebp':STACK_POINTER+0x100},
            stack={0x108:struct.pack('<I',1)},stops=[0x43bafc],budget=256)
        if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
        result.update(radius=radius,growth=growth,remaining=remaining)
        return result

def probe(executable):
    native=NativeDamageField(executable)
    try:
        births=[native.birth(duration=duration) for duration in (1,2,30,150)]
        tails=[native.tail(radius=radius,growth=growth,remaining=remaining) for radius,growth,remaining in
               ((32,16,30),(48,16,29),(496,16,1),(0,0,2),(.125,.1,1))]
        queries=[native.query(name,**options) for name,options in (
            ('center',{}),('tangent',{'position':(32,400,0)}),
            ('outside',{'position':(32.000003814697266,400,0)}),
            ('inside',{'position':(31.999998092651367,400,0)}),
            ('diagonal-inside',{'position':(20,420,0)}),
            ('diagonal-outside',{'position':(24,424,0)}),
            ('divisible-clock-change',{'previous':29,'current':28}),
            ('equal-clock',{'previous':28,'current':28}),
            ('tagged',{'tag':1}),('negative-tag',{'tag':0xffffffff}),
            ('limit',{'accumulated':999900}),('limit-equality',{'accumulated':999849}),
            ('zero-damage',{'damage':0}),('total-cap',{'damage':2500}),
            ('score-cap',{'score':999999998}),('inactive',{'flags':2}),
            ('zero-radius',{'radius':0}),('offset-depth',{'position':(0,400,1000)}))]
        pool_births=[native.pool_birth(name,cursor,occupied,full) for name,cursor,occupied,full in (
            ('first',0,[],False),('cursor-last',1999,[],False),
            ('skip-first',0,[0],False),('wrap-search',1999,[1999,0,1],False),
            ('skip-two',20,[20,21],False),('full',17,[],True))]
        motions=[native.motion(position) for position in ((0,400,.1),(-.001,400.123,.1),(132.12,-.0001,0),(-192,448,0),(-0.,-0.,-0.))]
        return {'schema':'th128-original-damage-field/1','executable':{'sha256':EXE_SHA256,
            'sha256After':hashlib.sha256(native.executable.read_bytes()).hexdigest()},'windowsGameProcessLaunched':False,
            'nativeCpuTrace':False,'birthCases':births,'tailCases':tails,'queryCases':queries,
            'poolBirthCases':pool_births,'motionCases':motions,'code':native.code,'constants':native.constants,
            'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    finally:native.image.close()

if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);arguments=parser.parse_args()
    report=probe(arguments.exe);output=ROOT/'local/research/damage-field-fixtures.json'
    output.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8',newline='\n')
    print('original returned damage-field birth and independent timer/growth tails recorded')
