"""Execute original MSG text branch rules; ANM/GDI callbacks are recorded fixtures."""
import pathlib,sys,struct,json
ROOT=pathlib.Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
from unicorn.x86_const import *
from native_anm_resource import check_original,EXE_SHA256
exe=ROOT.parent/'th12/th12.exe';check_original(exe);pe=pefile.PE(str(exe))
def encrypt(text):
    key,step=0x77,7;result=bytearray()
    for c in text.encode('cp932')+b'\0':result.append(c^key);key=(key+step)&255;step=(step+16)&255
    return bytes(result)
def command(time,op,text=''):
    data=encrypt(text) if op in [15,16,17] else b''
    return struct.pack('<HBB',time,op,len(data))+data
cases=[('plain-pair',[(17,'Alpha'),(17,'Beta'),(17,'Gamma')]),
    ('formatted-first',[(17,'|7,20,Alpha'),(17,'Beta'),(17,'Gamma')]),
    ('formatted-second',[(17,'Alpha'),(17,'|9,18,Beta'),(17,'Gamma')]),
    ('formatted-repeat',[(17,'|7,20,Alpha'),(17,'|9,18,Beta')]),
    ('serif-speaker',[(9,''),(17,'Alpha'),(17,'|2,10,Beta')]),
    ('legacy-lines',[(15,'Alpha'),(16,'Beta')])]
records=[]
for name,commands in cases:
    mu=Uc(UC_ARCH_X86,UC_MODE_32);mu.mem_map(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095);mu.mem_write(0x400000,pe.get_memory_mapped_image())
    STACK,OBJ,CODE,STOP=0x1000000,0x2000000,0x3000000,0x100f000
    for addr in [STACK,OBJ,CODE]:mu.mem_map(addr,0x10000)
    put=lambda at,fmt,*values:mu.mem_write(at,struct.pack('<'+fmt,*values))
    read=lambda at,fmt:list(struct.unpack('<'+fmt,mu.mem_read(at,struct.calcsize('<'+fmt))))
    handles=[OBJ+0x2000+i*0x1000 for i in range(4)];draws=[];interrupts=[]
    def string(at):
        value=bytearray()
        while True:
            c=mu.mem_read(at,1)[0];at+=1
            if not c:return value.decode('cp932')
            value.append(c)
            if len(value)>512:raise ValueError('Native string budget')
    def ret(cleanup=0):
        sp=mu.reg_read(UC_X86_REG_ESP);mu.reg_write(UC_X86_REG_EIP,read(sp,'I')[0]);mu.reg_write(UC_X86_REG_ESP,sp+4+cleanup)
    def hook(uc,address,size,data):
        sp=mu.reg_read(UC_X86_REG_ESP)
        if address==0x461c50:
            handle=read(mu.reg_read(UC_X86_REG_ESI),'I')[0];mu.reg_write(UC_X86_REG_EAX,handle);ret()
        elif address==0x461920:mu.reg_write(UC_X86_REG_EAX,read(sp+4,'I')[0]);ret(4)
        elif address==0x461dd0:ret()
        elif address==0x460760:
            color,outline,offset,spacing,txt=read(sp+4,'5I');handle=mu.reg_read(UC_X86_REG_ESI)
            draws.append({'slot':handles.index(handle),'text':string(txt),'color':color,'offset':offset,'spacing':spacing,'style':mu.reg_read(UC_X86_REG_EDI)});ret()
        elif address==0x461970:
            handle=read(sp+4,'I')[0]
            if handle in handles:interrupts.append([handles.index(handle),mu.reg_read(UC_X86_REG_EBX)])
            ret(4)
        elif address==0x453d90:ret()
    mu.hook_add(UC_HOOK_CODE,hook);mu.reg_write(UC_X86_REG_FPCW,0x37f);mu.reg_write(UC_X86_REG_MXCSR,0x1f80)
    put(0x4b2ed0,'f',1);put(OBJ+0x18,'iifII',-1,0,0,0x4b2ed0,1);put(OBJ+0x2c,'iifII',-1,0,0,0x4b2ed0,1)
    put(OBJ+0x64,'I',CODE);put(OBJ+0x4c,'4I',*handles);put(OBJ+0xa0,'3I',0xf8f08f,0x8088ff,0xd8d8d8)
    mu.mem_write(CODE,b''.join(command(i*2,op,text) for i,(op,text) in enumerate(commands))+command(len(commands)*2+10,0))
    rows=[]
    for tick in range(len(commands)*2):
        draws.clear();interrupts.clear();sp=STACK+0xe000;put(sp,'II',STOP,OBJ);mu.reg_write(UC_X86_REG_ESP,sp);mu.emu_start(0x41fcc0,STOP,count=200000)
        if mu.reg_read(UC_X86_REG_EIP)!=STOP:raise RuntimeError('Original MSG budget')
        rows.append({'tick':tick,'clock':read(OBJ+0x1c,'i')[0],'line':read(OBJ+0x94,'i')[0],
            'initialized':read(OBJ+0x98,'i')[0],'speaker':read(OBJ+0x9c,'i')[0],'flags':read(OBJ+0x90,'I')[0],
            'draws':draws.copy(),'interrupts':interrupts.copy()})
    records.append({'name':name,'commands':commands,'frames':rows})
out=ROOT/'tests/fixtures/message-formatted-original.json';out.write_text(json.dumps({'schema':'th12-original-message-text-branch-samples/1',
    'originalExeSha256':EXE_SHA256,'provider':'Original41fcc0 text branches,420e40 decryption,atoi/strchr; ANM handle/interrupt and GDI draw callbacks recorded',
    'fixtureCallbacks':['461c50 handle lookup','461920 handle lookup','461dd0 position callback','460760 record text draw','461970 record interrupt','453d90 sound ignored'],
    'wholeGameDeterminism':False,'wholePresentationOracle':False,'gpuExecuted':False,'records':records},indent=2)+'\n',encoding='utf-8');print(out)
