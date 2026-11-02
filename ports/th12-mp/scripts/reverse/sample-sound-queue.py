"""Actual453d90/453e20/453f10 and consumer's integer pan mean.

The sound device play boundary4544b0 is recorded, with zero device pointers.
No candidate logic executes. Playback/thread scheduling is outside this fixture.
"""
import pathlib,sys,struct,json
ROOT=pathlib.Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
from unicorn.x86_const import *
from native_anm_resource import check_original,EXE_SHA256
exe=ROOT.parent/'th12/th12.exe';check_original(exe);pe=pefile.PE(str(exe));image=pe.get_memory_mapped_image();sounds=[]
for i in range(60):
    a=0xae5a0+i*20;sid,filename=struct.unpack_from('<2I',image,a)
    p=struct.unpack_from('<I',image,0xaea50+filename*4)[0]-0x400000
    sounds.append({'id':sid,'file':image[p:p+200].split(b'\0')[0].decode('ascii'),
        'volume':struct.unpack_from('<h',image,a+8)[0],'loop':bool(struct.unpack_from('<I',image,a+12)[0])})
sounds.sort(key=lambda s:s['id'])
cases=[('positioned',[[7,-192,1],[7,0,1],[7,192,1],[3,70,1],[2,-80,0]]),
       ('overflow-distinct',[[i,i*9,1] for i in range(15)]),
       ('overflow-same',[[7,float(i-67),1] for i in range(135)]),
       ('integer-mean',[[21,-.1,1],[21,-.2,1],[21,.05,1]]),
       ('stop-then-play',[[21,8,1],[21,0,2],[21,50,1]]),
       ('stop',[[21,8,1],[21,0,2]])]
rows=[]
for name,requests in cases:
    mu=Uc(UC_ARCH_X86,UC_MODE_32);mu.mem_map(0x400000,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095);mu.mem_write(0x400000,image);mu.mem_map(0x1000000,0x10000)
    put=lambda a,f,*v:mu.mem_write(a,struct.pack('<'+f,*v))
    get=lambda a,f:list(struct.unpack('<'+f,mu.mem_read(a,struct.calcsize('<'+f))))
    put(0x4cf508,'12i',*([-1]*12));put(0x4cf538,'12i',*([0]*12));mu.reg_write(UC_X86_REG_FPCW,0x37f)
    stack,stop=0x100e000,0x100f000;played=[]
    def hook(v,a,n,d):
      played.append({'id':(v.reg_read(UC_X86_REG_ESI)-0x4d0e70)//24,'pan':struct.unpack('<i',struct.pack('<I',v.reg_read(UC_X86_REG_EAX)))[0]})
      sp=v.reg_read(UC_X86_REG_ESP);v.reg_write(UC_X86_REG_EIP,get(sp,'I')[0]);v.reg_write(UC_X86_REG_ESP,sp+4)
    mu.hook_add(UC_HOOK_CODE,hook,begin=0x4544b0,end=0x4544b0)
    for sid,x,kind in requests:
      put(stack,'I',stop);put(stack+4,'f',x);mu.reg_write(UC_X86_REG_ESP,stack);mu.reg_write(UC_X86_REG_EAX,sid);mu.reg_write(UC_X86_REG_EDX,sid)
      mu.emu_start(0x453e20 if kind==1 else 0x453d90 if kind==0 else 0x453f10,stop,count=10000)
      if mu.reg_read(UC_X86_REG_EIP)!=stop:raise RuntimeError('Native queue budget')
    slots=[]
    for i in range(12):
      sid=get(0x4cf508+i*4,'i')[0];count=get(0x4cf538+i*4,'i')[0]
      if sid<0:break
      slots.append({'id':sid,'count':count,'pans':get(0x4cf568+i*512,'i'*max(0,count)) if count>0 else []})
    put(stack,'I',stop);mu.reg_write(UC_X86_REG_ESP,stack);mu.emu_start(0x4543b1,0x454462,count=10000)
    if mu.reg_read(UC_X86_REG_EIP)!=0x454462:raise RuntimeError('Native queue consumer budget')
    rows.append({'name':name,'requests':requests,'slots':slots,'played':played})
out=ROOT/'tests/fixtures/sound-queue-original.json';out.write_text(json.dumps({'schema':'th12-original-sound-queue/1','originalExeSha256':EXE_SHA256,'wholeGameOracle':False,'audioPlaybackExecuted':False,'provider':'Actual453d90/453e20/453f10/4543b1..454462;4544b0 host playback recorded','sounds':sounds,'rows':rows},indent=2)+'\n',encoding='utf8')
(ROOT/'site/assets/sound-index.json').write_text(json.dumps(sounds,indent=2)+'\n',encoding='utf8')
print(json.dumps({'cases':len(rows),'sounds':len(sounds)}))
