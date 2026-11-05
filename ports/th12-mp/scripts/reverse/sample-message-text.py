"""Sample original VM text call geometry, stopping before GDI/D3D rendering."""
import pathlib,sys,struct,json
ROOT=pathlib.Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile,capstone
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
from unicorn.x86_const import *
from native_anm_resource import check_original,EXE_SHA256
exe=ROOT.parent/'th12/th12.exe';check_original(exe);pe=pefile.PE(str(exe))
vm=Uc(UC_ARCH_X86,UC_MODE_32);vm.mem_map(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095);vm.mem_write(0x400000,pe.get_memory_mapped_image())
vm.mem_map(0x1000000,0x10000);vm.mem_map(0x2000000,0x10000)
obj,record,tex,stack,stop,text=0x2000000,0x2001000,0x2002000,0x100e000,0x100f000,0x2003000
put=lambda at,fmt,*values:vm.mem_write(at,struct.pack('<'+fmt,*values))
read=lambda at,fmt:list(struct.unpack('<'+fmt,vm.mem_read(at,struct.calcsize('<'+fmt))))
def cstring(at):
    result=bytearray()
    while True:
        c=vm.mem_read(at,1)[0];at+=1
        if not c:return bytes(result)
        result.append(c)
        if len(result)>1024:raise ValueError('Native string exceeded fixture budget')
calls=[]
def hook(uc,address,size,data):
    if address==0x46cad8:
        # _vsnprintf is only format "%s"/literal text here. Copy a literal
        # fixture line so system locale/CRT startup is outside this geometry probe.
        sp=vm.reg_read(UC_X86_REG_ESP);destination,source=read(sp+4,'2I');raw=cstring(source)
        vm.mem_write(destination,raw+b'\0');vm.reg_write(UC_X86_REG_EAX,len(raw))
        vm.reg_write(UC_X86_REG_EIP,read(sp,'I')[0]);vm.reg_write(UC_X86_REG_ESP,sp+4)
    elif address==0x44e660:
        sp=vm.reg_read(UC_X86_REG_ESP);args=read(sp+4,'6I')
        calls.append({'style':vm.reg_read(UC_X86_REG_EAX),'textHex':cstring(vm.reg_read(UC_X86_REG_ECX)).hex(),
            'offset':vm.reg_read(UC_X86_REG_EDX),'sourceRect':read(args[0],'4i'),'fontHeight':args[1],
            'foregroundColor':args[2],'outlineColor':args[3],'spacing':args[5]})
        vm.reg_write(UC_X86_REG_EIP,read(sp,'I')[0]);vm.reg_write(UC_X86_REG_ESP,sp+4+24)
vm.hook_add(UC_HOOK_CODE,hook);records=[]
for style,offset,spacing,color in [(0,0,0,0xf8f08f),(1,0,0,0x8088ff),(0,7,20,0xd8d8d8)]:
    vm.mem_write(obj,bytes(0x504));put(obj+0x49c,'B',16);put(obj+0x3f4,'I',record);put(record+8,'I',tex);put(tex,'I',0x2002100)
    put(record+0xc,'ffff',0,0,384,24);raw='星蓮船'.encode('cp932');vm.mem_write(text,raw+b'\0')
    put(stack,'6I',stop,color,0,offset,spacing,text);vm.reg_write(UC_X86_REG_ESP,stack);vm.reg_write(UC_X86_REG_ESI,obj);vm.reg_write(UC_X86_REG_EDI,style);vm.reg_write(UC_X86_REG_FPCW,0x37f)
    calls.clear();vm.emu_start(0x460760,stop,count=200000)
    if vm.reg_read(UC_X86_REG_EIP)!=stop:raise RuntimeError('Text sample instruction budget')
    records.append({'input':{'style':style,'offset':offset,'spacing':spacing,'color':color},'calls':calls.copy()})
out=ROOT/'tests/fixtures/message-text-original.json';out.write_text(json.dumps({'schema':'th12-original-message-text-call-samples/1',
    'originalExeSha256':EXE_SHA256,'provider':'Original460760->4606b0 call-site values immediately before44e660 GDI wrapper',
    'fixtureCallbacks':{'46cad8':'copy literal fixture line; CRT formatting/locale startup excluded','44e660':'capture arguments and return; GDI/D3D excluded'},
    'wholeGameDeterminism':False,'wholePresentationOracle':False,'gdiExecuted':False,'gpuExecuted':False,'records':records},indent=2)+'\n',encoding='utf-8')
print(out)
