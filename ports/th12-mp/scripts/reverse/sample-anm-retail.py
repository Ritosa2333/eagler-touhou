"""Bind actual TH12 retail bytecode in the original logical ANM interpreter."""
import pathlib,sys,struct,json,hashlib
ROOT=pathlib.Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32
from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EIP,UC_X86_REG_ECX,UC_X86_REG_FPCW
from native_anm_resource import check_original,EXE_SHA256,install_resource
exe=ROOT.parent/'th12/th12.exe';check_original(exe);pe=pefile.PE(str(exe));records=[]
selection=[('pl00',8,0,20),('pl00',10,0,20),('pl00',11,0,20),('pl00',12,0,20),('pl02',6,0,20),
    ('bullet',3,0,32),('bullet',35,2,20),('bullet',80,0,40),('bullet',93,0,40),
    ('enemy',0,0,20),('enemy',135,0,20),('stgenm01',1,0,20),('enemy',106,0,20),
    ('pl01',7,0,24),('pl01',8,0,24),
    ('text',0,2,16),('text',1,2,16),('text',2,2,16),('text',3,2,16)]
for name,script,interrupt,ticks in selection:
    vm=Uc(UC_ARCH_X86,UC_MODE_32);vm.mem_map(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095);vm.mem_write(0x400000,pe.get_memory_mapped_image())
    vm.mem_map(0x1000000,0x10000);vm.mem_map(0x2000000,0x600000)
    obj,resource,manager,stack,stop=0x2000000,0x2010000,0x2500000,0x100e000,0x100f000
    raw=(ROOT/'local/retail'/f'{name}.anm').read_bytes();install_resource(vm,raw,resource)
    put=lambda at,fmt,*v:vm.mem_write(at,struct.pack('<'+fmt,*v))
    read=lambda at,fmt:list(struct.unpack('<'+fmt,vm.mem_read(at,struct.calcsize('<'+fmt))))
    put(0x4ce8cc,'I',manager);put(0x4b2ed0,'f',1.);put(0x4ce568,'II',0x1234,0);put(0x4ce560,'II',0xbeef,0);vm.reg_write(UC_X86_REG_FPCW,0x37f)
    def call(at,args):
        put(stack,'I'*(len(args)+1),stop,*args);vm.reg_write(UC_X86_REG_ESP,stack)
        if at==0x454d10:vm.reg_write(UC_X86_REG_ECX,resource)
        vm.emu_start(at,stop,count=200000)
        if vm.reg_read(UC_X86_REG_EIP)!=stop:raise RuntimeError(f'Native budget {name}:{script}')
    call(0x454d10,[obj,script]);frames=[]
    for tick in range(ticks):
        if interrupt and tick==1:put(obj+0x3c4,'h',interrupt)
        if tick:call(0x455630,[obj])
        frames.append({'tick':tick,'position':read(obj+0x424,'fff'),'rotation':read(obj+0x24,'fff'),'scale':read(obj+0x40,'ff'),'uv':read(obj+0x60,'ff'),
            'primary':read(obj+0x3bc,'I')[0],'secondary':read(obj+0x3c0,'I')[0],
            'integers':read(obj+0x3fc,'iiii')+read(obj+0x41c,'ii'),'floats':read(obj+0x40c,'ffff'),
            'sprite':read(obj+0x3e4,'h')[0],'layer':read(obj+0x20,'i')[0],'clock':read(obj+0x6c,'i')[0],
            'ended':read(obj+0x3f0,'I')[0]==0,'visible':bool(read(obj+0x47c,'I')[0]&1),
            'gameSeedCounter':read(0x4ce568,'II'),'displaySeedCounter':read(0x4ce560,'II')})
    records.append({'name':name,'script':script,'interrupt':interrupt,'resourceSha256':hashlib.sha256(raw).hexdigest(),'frames':frames})
out=ROOT/'tests/fixtures/anm-retail-original.json';out.write_text(json.dumps({'schema':'th12-original-anm-retail-samples/1',
 'originalExeSha256':EXE_SHA256,'provider':'Actual454d10/454b80/455630 with unmodified retail ANM bytes; GPU-free resource tables',
 'wholeGameDeterminism':False,'wholePresentationOracle':False,'gpuExecuted':False,'records':records},indent=2)+'\n',encoding='utf-8');print(out)
