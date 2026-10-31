"""Run original4018f0 font glyph submission, without GPU or game ticks.

402390 sprite lookup and45a570 quad construction execute unchanged. Only
459e50 GPU submission is captured; raw ASCII ANM is installed GPU-free.
"""
import json,pathlib,struct,sys
ROOT=pathlib.Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
from unicorn.x86_const import *
from native_anm_resource import check_original,EXE_SHA256,install_resource
exe=ROOT.parent/'th12/th12.exe';check_original(exe);pe=pefile.PE(str(exe))
mu=Uc(UC_ARCH_X86,UC_MODE_32);mu.mem_map(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095);mu.mem_write(0x400000,pe.get_memory_mapped_image())
STACK,STOP,FONT,TEXT,RESOURCE,MANAGER=0x1000000,0x100f000,0x2000000,0x2200000,0x2210000,0x2500000
mu.mem_map(STACK,0x10000);mu.mem_map(FONT,0x600000)
table=install_resource(mu,(ROOT/'local/retail/ascii.anm').read_bytes(),RESOURCE)
def put(at,fmt,*values):mu.mem_write(at,struct.pack('<'+fmt,*values))
def read(at,fmt):return list(struct.unpack('<'+fmt,mu.mem_read(at,struct.calcsize('<'+fmt))))
glyphs=[]
def hook(uc,address,size,data):
    if address==0x459e50:
        node=mu.reg_read(UC_X86_REG_EAX);sprite=read(node+0x3f4,'I')[0]
        flags=read(node+0x47c,'I')[0];flags2=read(node+0x480,'I')[0]
        position=read(FONT+0x438,'3f');scale=read(node+0x40,'2f')
        glyphs.append({'id':(sprite-table['sprites'])//72,'x':position[0],'y':position[1],'z':position[2],
            'scaleX':scale[0],'scaleY':scale[1],'colorARGB':read(node+0x3bc,'I')[0],
            'anchorX':(flags>>19)&3,'anchorY':(flags>>21)&3,'point':bool(flags2&2),'pixel':True})
        sp=mu.reg_read(UC_X86_REG_ESP);assert read(sp+4,'I')[0]==1
        mu.reg_write(UC_X86_REG_EIP,read(sp,'I')[0]);mu.reg_write(UC_X86_REG_ESP,sp+8)
mu.hook_add(UC_HOOK_CODE,hook);mu.reg_write(UC_X86_REG_FPCW,0x37f);mu.reg_write(UC_X86_REG_MXCSR,0x1f80)
put(0x4ce8cc,'I',MANAGER);put(0x4ce568,'2I',0x1234,0);put(0x4ce560,'2I',0xbeef,0)
cases=[]
texts={1:'0123 /:-*.%$',2:' 12:34.%$-*/',3:'12,345.6*s/',4:'*6.0 12,345'}
for font,text in texts.items():
    for ax in [0,1,2]:
        for ay in [0,1,2]:
            cases.append({'text':text,'font':font,'x':123.25,'y':205.375,'z':0,'scaleX':1,'scaleY':1,
                          'colorARGB':0xc080ffc0,'alignX':ax,'alignY':ay,'drawFlag':0})
    for scale in [.3333,.6,.75,1,2]:
        cases.append({'text':text,'font':font,'x':-24.125,'y':33.5,'z':0,'scaleX':scale,'scaleY':scale,
                      'colorARGB':0xff808080,'alignX':0,'alignY':0,'drawFlag':1})
    for text2 in ['', '  ', '1\n2', '1\x002', '.1', ',1' if font>=3 else '$1']:
        cases.append({'text':text2,'font':font,'x':100,'y':200,'z':0,'scaleX':1,'scaleY':2,
                      'colorARGB':0xffffffff,'alignX':1,'alignY':0,'drawFlag':0})
records=[]
for index,state in enumerate(cases):
    glyphs.clear();mu.mem_write(FONT,bytes(0x19000));mu.mem_write(TEXT,bytes(0x200))
    encoded=state['text'].encode('ascii');mu.mem_write(TEXT,encoded+b'\0')
    put(FONT+0x18fb4,'I',RESOURCE);put(FONT+0x18fac,'i',14);put(FONT+0x14+0x50,'2f',1,1)
    put(TEXT+0x100,'3f',state['x'],state['y'],state['z']);put(TEXT+0x10c,'I',state['colorARGB'])
    put(TEXT+0x110,'2f',state['scaleX'],state['scaleY']);put(TEXT+0x120,'i',state['font'])
    put(TEXT+0x128,'i',state['drawFlag']);put(TEXT+0x130,'2i',state['alignX'],state['alignY'])
    sp=STACK+0xe000;put(sp,'3I',STOP,FONT,TEXT);mu.reg_write(UC_X86_REG_ESP,sp)
    before=read(0x4ce568,'2I')+read(0x4ce560,'2I');mu.emu_start(0x4018f0,STOP,count=100000)
    if mu.reg_read(UC_X86_REG_EIP)!=STOP:raise RuntimeError('Original font exceeded budget')
    assert before==read(0x4ce568,'2I')+read(0x4ce560,'2I')
    # All submission fields are stored as f32 by the native caller.
    for key,offset in [('scaleX',0x110),('scaleY',0x114)]:state[key]=read(TEXT+offset,'f')[0]
    records.append({'input':state,'glyphs':glyphs.copy(),'rngUnchanged':True})
out=ROOT/'tests/fixtures/ascii-glyphs-native.json'
out.write_text(json.dumps({'schema':'th12-native-ascii-glyphs/1','originalExeSha256':EXE_SHA256,
    'provider':'Actual4018f0 ->402390->45a570; no candidate runtime or game/ANM tick.',
    'fixtureCallbacks':['459e50 GPU submission records logical glyph/quad inputs and returns without Direct3D.',
                        'ASCII ANM resource tables are supplied GPU-free; original loader/GPU are not executed.'],
    'wholeGameOracle':False,'wholePresentationOracle':False,'gpuExecuted':False,'records':records},indent=2)+'\n',encoding='utf8')
print(json.dumps({'path':str(out),'cases':len(records),'glyphs':sum(len(row['glyphs']) for row in records)}))
