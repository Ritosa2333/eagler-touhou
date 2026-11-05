"""Original TH12 bind/typed variables sampled without D3D or game startup."""
import pathlib,sys,struct,json
ROOT=pathlib.Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32
from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EIP,UC_X86_REG_ECX,UC_X86_REG_FPCW
from native_anm_resource import check_original,EXE_SHA256
exe=ROOT.parent/'th12/th12.exe';check_original(exe);pe=pefile.PE(str(exe));records=[]
def command(op,fmt='',values=(),mask=0):
    data=struct.pack('<'+fmt,*values);return struct.pack('<HHhH',op,len(data)+8,0,mask)+data
for identifier in range(10000,10026):
    vm=Uc(UC_ARCH_X86,UC_MODE_32);vm.mem_map(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095);vm.mem_write(0x400000,pe.get_memory_mapped_image())
    vm.mem_map(0x1000000,0x10000);vm.mem_map(0x2000000,0x20000)
    obj,resource,table,code,manager,stack,stop=0x2000000,0x2001000,0x2002000,0x2010000,0x2003000,0x100e000,0x100f000
    put=lambda at,fmt,*v:vm.mem_write(at,struct.pack('<'+fmt,*v))
    read=lambda at,fmt:list(struct.unpack('<'+fmt,vm.mem_read(at,struct.calcsize('<'+fmt))))
    script=command(7,'ff',[10004,identifier],3)+command(6,'ii',[10000,identifier],3)+command(63)
    vm.mem_write(code,script);put(resource+0x11c,'I',table);put(table,'I',code)
    put(0x4ce8cc,'I',manager);put(0x4b2ed0,'f',1.);put(0x4ce568,'II',0x1234,0);put(0x4ce560,'II',0xbeef,0)
    put(0x4ced1c,'fff',11,22,33);put(0x4ced40,'fff',44,55,66)
    put(stack,'III',stop,obj,0);vm.reg_write(UC_X86_REG_ESP,stack);vm.reg_write(UC_X86_REG_ECX,resource);vm.reg_write(UC_X86_REG_FPCW,0x37f)
    vm.emu_start(0x454d10,stop,count=100000)
    if vm.reg_read(UC_X86_REG_EIP)!=stop:raise RuntimeError('Binding exceeded native budget')
    records.append({'id':identifier,'floatValue':read(obj+0x40c,'f')[0],'intValue':read(obj+0x3fc,'i')[0],
        'gameSeedCounter':read(0x4ce568,'II'),'displaySeedCounter':read(0x4ce560,'II')})
random_sprites=[]
for display_domain in [0,1]:
    vm=Uc(UC_ARCH_X86,UC_MODE_32);vm.mem_map(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095);vm.mem_write(0x400000,pe.get_memory_mapped_image())
    vm.mem_map(0x1000000,0x10000);vm.mem_map(0x2000000,0x20000)
    obj,resource,table,code,sprites,manager,stack,stop=0x2000000,0x2001000,0x2002000,0x2010000,0x2007000,0x2003000,0x100e000,0x100f000
    put=lambda at,fmt,*v:vm.mem_write(at,struct.pack('<'+fmt,*v))
    read=lambda at,fmt:list(struct.unpack('<'+fmt,vm.mem_read(at,struct.calcsize('<'+fmt))))
    script=command(87,'i',[display_domain])+command(102,'ii',[0,2])+command(63)
    vm.mem_write(code,script);put(resource+0x108,'I',1);put(resource+0x118,'I',sprites);put(resource+0x11c,'I',table);put(table,'I',code)
    for index in range(2):put(sprites+index*72+0x1c,'fffffffffff',1,1,0,1,0,1,8,16,1,1,0)
    put(0x4ce8cc,'I',manager);put(0x4b2ed0,'f',1.);put(0x4ce568,'II',0x1234,0);put(0x4ce560,'II',0xbeef,0)
    put(stack,'III',stop,obj,0);vm.reg_write(UC_X86_REG_ESP,stack);vm.reg_write(UC_X86_REG_ECX,resource);vm.reg_write(UC_X86_REG_FPCW,0x37f)
    vm.emu_start(0x454d10,stop,count=100000)
    if vm.reg_read(UC_X86_REG_EIP)!=stop:raise RuntimeError('Random sprite exceeded native budget')
    random_sprites.append({'displayDomain':display_domain,'sprite':read(obj+0x3e4,'h')[0],
        'gameSeedCounter':read(0x4ce568,'II'),'displaySeedCounter':read(0x4ce560,'II')})
binding=json.loads((ROOT/'artifacts/reverse/anm-binding-rng-samples.json').read_text())['records']
report={'schema':'th12-original-anm-logic-samples/1','originalExeSha256':EXE_SHA256,
    'provider':'Actual 454d10->402520->455630 with constructed sprite-free script; typed reads 4551b0/4553f0',
    'wholeGameDeterminism':False,'wholePresentationOracle':False,'gpuExecuted':False,'binding':binding,'variables':records,'randomSprites':random_sprites}
out=ROOT/'tests/fixtures/anm-logic-original.json';out.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8');print(out)
