"""Execute original registered-ANM linked lists to sample child birth ordering."""
import pathlib,sys,struct,json
ROOT=pathlib.Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32
from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EIP,UC_X86_REG_EAX,UC_X86_REG_ECX,UC_X86_REG_EDI,UC_X86_REG_FPCW
from native_anm_resource import check_original,EXE_SHA256,install_resource
exe=ROOT.parent/'th12/th12.exe';check_original(exe);pe=pefile.PE(str(exe));records=[]
def command(op,fmt='',values=(),time=0,mask=0):
    data=struct.pack('<'+fmt,*values);return struct.pack('<HHhH',op&65535,len(data)+8,time,mask)+data
def fixture_bank(scripts):
    h=bytearray(64+len(scripts)*8);struct.pack_into('<I',h,0,7);struct.pack_into('<H',h,6,len(scripts));off=len(h)
    for i,s in enumerate(scripts):struct.pack_into('<iI',h,64+i*8,i,off);off+=len(s)
    return bytes(h)+b''.join(scripts)
for parent_op in (88,90,91,92,95):
 for suffix in (False,True):
    vm=Uc(UC_ARCH_X86,UC_MODE_32);vm.mem_map(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095);vm.mem_write(0x400000,pe.get_memory_mapped_image())
    vm.mem_map(0x1000000,0x10000);vm.mem_map(0x2000000,0x1000000)
    manager,resource,out,stack,stop=0x2000000,0x2a00000,0x2b00000,0x100e000,0x100f000
    put=lambda at,fmt,*v:vm.mem_write(at,struct.pack('<'+fmt,*v))
    read=lambda at,fmt:list(struct.unpack('<'+fmt,vm.mem_read(at,struct.calcsize('<'+fmt))))
    # Binding parent at clock 0 does not create child. First list pass executes
    # its timed opcode at clock 1, observing the real registration functions.
    parent=command(0)+command(parent_op,'i',[1],time=1)+command(63,time=1)+command(-1,time=1)
    # Child consumes a game RNG32 on every native logical tick; a same-pass
    # extra tick is distinguishable from binding by both timer and RNG counter.
    child=command(7,'ff',[10004,10011],mask=3)+command(4,'ii',[0,0])+command(-1)
    # Avoid a time-0 infinite command loop: jump back to clock -1 so the next
    # due time 0 yields until the following update.
    child=command(7,'ff',[10004,10011],mask=3)+command(4,'ii',[0,-1])+command(-1)
    dummy=command(63)+command(-1)
    tables=install_resource(vm,fixture_bank([parent,child,dummy]),resource)
    put(0x4ce8cc,'I',manager);put(0x4b2ed0,'f',1.);put(0x4ce568,'II',0x1234,0);put(0x4ce560,'II',0xbeef,0);vm.reg_write(UC_X86_REG_FPCW,0x37f)
    def call(at,args=(),registers=None):
        put(stack,'I'*(len(args)+1),stop,*args);vm.reg_write(UC_X86_REG_ESP,stack)
        for reg,value in (registers or {}).items():vm.reg_write(reg,value)
        vm.emu_start(at,stop,count=200000)
        if vm.reg_read(UC_X86_REG_EIP)!=stop:raise RuntimeError(f'Native budget at {at:x}')
    # Use actual allocator 4621c0 and primary registration 461250.
    call(0x4615a0,[resource,out,0,0],{UC_X86_REG_EAX:0,UC_X86_REG_ECX:0})
    if suffix:call(0x4615a0,[resource,out+4,2,0],{UC_X86_REG_EAX:0,UC_X86_REG_ECX:0})
    frames=[]
    for frame in range(3):
        call(0x460fe0,registers={UC_X86_REG_EDI:manager})
        primary=read(manager+0x8856b8,'I')[0];secondary=read(manager+0x8856c0,'I')[0]
        def nodes(head):
            result=[];budget=0
            while head:
                p,nxt=read(head,'II');result.append({'script':read(p+0x3ea,'h')[0],'clock':read(p+0x6c,'i')[0],'float0':read(p+0x40c,'f')[0]});head=nxt;budget+=1
                if budget>10:raise RuntimeError('Unexpected linked list')
            return result
        frames.append({'frame':frame,'gameSeedCounter':read(0x4ce568,'II'),'primary':nodes(primary),'secondary':nodes(secondary)})
        # Secondary registered children execute in the next tick-8 callback,
        # before the next primary pass; sample it explicitly, not concurrently.
        if secondary:call(0x4610d0,registers={UC_X86_REG_EDI:manager})
    records.append({'childOpcode':parent_op,'existingSuccessor':suffix,'frames':frames})
out=ROOT/'artifacts/reverse/anm-scheduler-samples.json';out.write_text(json.dumps({'schema':'th12-original-anm-scheduler-samples/1',
 'originalExeSha256':EXE_SHA256,'provider':'Actual4615a0/4621c0/461250 plus460fe0/4610d0 and child461720/461840; no logic hooks',
 'wholeGameDeterminism':False,'gpuExecuted':False,'records':records},indent=2)+'\n',encoding='utf-8');print(out)
