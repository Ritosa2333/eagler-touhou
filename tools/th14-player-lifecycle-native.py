"""Retail hit/death and pre-tail lifecycle, with ANM/manager delivery boundaries."""
import hashlib,importlib,itertools,json,struct
from pathlib import Path
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
from unicorn.x86_const import *
ROOT=Path(__file__).resolve().parents[1]

def main():
 img,_,_=importlib.import_module('th-anm-handlers').handlers(14)
 m=Uc(UC_ARCH_X86,UC_MODE_32)
 for _,b,size,raw,n in img.sections:
  start=b&~4095;m.mem_map(start,((b+size+4095)&~4095)-start);m.mem_write(b,img.data[raw:raw+n])
 base,stack,end=0x70000000,0x700ff000,0x700fe000;m.mem_map(base,0x100000)
 spell,bomb,hud,enemy,effect,control=(base+o for o in (0x20000,0x22000,0x24000,0x26000,0x28000,0x2a000))
 put=lambda p,v:m.mem_write(p,struct.pack('<I',v&0xffffffff))
 get=lambda p:struct.unpack('<I',m.mem_read(p,4))[0]
 bits=lambda f:struct.unpack('<I',struct.pack('<f',f))[0]
 events=[]
 def point(p):return [get(p+i*4) for i in range(3)]
 def sink(machine,a,size,user):
  sp=m.reg_read(UC_X86_REG_ESP)
  if a==0x47ed10:
   events.append([0,get(sp+8),*point(get(sp+12))]);put(get(sp+4),1);m.reg_write(UC_X86_REG_EAX,get(sp+4))
  elif a==0x46e3d0:events.append([1,get(sp+4)])
  elif a in (0x434ef0,0x434fd0):events.append([2 if a==0x434ef0 else 3,get(sp+4),get(sp+8)])
  elif a==0x41b3f0:events.append([5,*point(get(sp+4)),m.reg_read(UC_X86_REG_XMM2)&0xffffffff,get(sp+8)])
  elif a==0x43ab80:events.append([6,1,*point(get(sp+4)),m.reg_read(UC_X86_REG_XMM2)&0xffffffff,get(sp+8),get(sp+12)])
  elif a==0x43ac00:events.append([6,0,0,0,0,0,get(sp+4),get(sp+8)])
  elif a in (0x449d30,0x448ef0):events.append([7,int(a==0x448ef0)])
  m.reg_write(UC_X86_REG_EAX,0)
 for a,cleanup in ((0x47ed10,28),(0x46e3d0,8),(0x434ef0,8),(0x434fd0,8),(0x40af00,8),(0x46fe50,0),(0x47f140,8),(0x44c2e0,4),(0x44d330,0),(0x450130,12),(0x41b3f0,8),(0x43ab80,12),(0x43ac00,8),(0x46e470,4),(0x449d30,0),(0x448ef0,0)):
  m.mem_write(a,b'\xc2'+struct.pack('<H',cleanup) if cleanup else b'\xc3');m.hook_add(UC_HOOK_CODE,sink,begin=a,end=a)
 m.mem_write(base+0x2c000,b'\xc3')
 def run(kind,lives,bombs,fragments,power,frame,held,available,active,flags,elapsed,scale_flags,rate=1.):
  m.mem_write(base,bytes(0x2b000));events.clear()
  for p,v in ((0x4db67c,base),(0x4db534,spell),(0x4db52c,bomb),(0x4db550,hud),(0x4db544,enemy),(0x4db53c,effect),(0x4db688,control),(effect+0x10,effect),
              (bomb,base+0x2c010),(base+0x2c010,base+0x2c000),(bomb+0x40,active),(enemy+0x10,7),(enemy+0x18,9),(enemy+0xd8,1 if available else 0),(hud+0x194,0),
              (spell+0x7c,flags),(spell+0x28,elapsed),(spell+0x80,12345),(base+0x182d4,scale_flags),(control+0x10,0),
              (0x4f584c,5000000),(0x4f5864,lives),(0x4f5868,fragments),(0x4f5870,bombs),(0x4f5874,fragments),(0x4f5858,power),(0x4d6a90,2),(0x4d6a9c,held),(0x4d8f58,bits(rate)),
              (base+0x684,4 if kind==4 else 3 if kind==3 else 2 if kind==2 else 0 if kind==0 else 1),(base+0x68c,frame-1),(base+0x690,frame),(base+0x694,bits(frame)),(base+0x698,0x4d8f58),(base+0x69c,1),
              (base+0x182c0,19),(base+0x182c4,20),(base+0x182c8,bits(20)),(base+0x182cc,0x4d8f58),(base+0x182d0,1),(base+0x18334,4)):
   put(p,v)
  for off,pos in ((0x5e0,(12.25,400.,3.)),(0x66c,(-25.5,375.25,-2.))):
   for j,f in enumerate(pos):put(base+off+j*4,bits(f))
  put(base+0x5ec,1568);put(base+0x5f0,51200)
  put(stack,end);m.reg_write(UC_X86_REG_ESP,stack);m.reg_write(UC_X86_REG_ECX,base)
  m.reg_write(UC_X86_REG_FPCW,0x37f);m.reg_write(UC_X86_REG_FPSW,0);m.reg_write(UC_X86_REG_FPTAG,0xffff);m.reg_write(UC_X86_REG_MXCSR,0x1f80)
  entry=0x44f7a0 if kind==5 else 0x44f5e0 if kind==6 else 0x44dbd0
  until=end if kind in (5,6) else 0x44dfd1
  m.emu_start(entry,until,count=100000);assert m.reg_read(UC_X86_REG_EIP)==until
  result=[get(base+o) for o in (0x684,0x690,0x182c4,0x5ec,0x5f0,0x5e0,0x5e4,0x5e8,0x66c,0x670,0x674)]
  result += [get(p) for p in (0x4f5864,0x4f5870,0x4f5874,0x4f5858,spell+0x7c,spell+0x80,enemy+0x10,enemy+0x18,0x4d8f58,0x4f584c)]
  return dict(args=[kind,lives,bombs,fragments,power,frame,held,available,active,flags,elapsed,scale_flags,bits(rate)],result=result,events=events[:])
 rows=[]
 for kind,flags,elapsed,active,scale_flags in itertools.product((5,6),(0,1,0x23),(0,59,60,120),(0,1),(0,8)):
  for lives in (-1,0,2):rows.append(run(kind,lives,2,7,100,7,0,1,active,flags,elapsed,scale_flags))
 for fragments in (-1,32768,65537,2147483647):rows.append(run(6,2,2,fragments,100,7,0,1,0,0,0,0))
 for kind,frame,bombs,held,available,active,power in itertools.product((1,4),(0,7,8),(0,1,3,9),(0,2),(0,1),(0,1),(0,100)):
  rows.append(run(kind,2,bombs,5,power,frame,held,available,active,0,0,0))
 for frame in (4,14,15,16):rows.append(run(3,2,3,7,100,frame,0,1,0,0,0,0))
 for lives,frame,fragments,rate in itertools.product((-1,0,2),(29,30,31),(0,7),(.5,1.,1.25)):
  rows.append(run(2,lives,0,fragments,100,frame,0,1,0,0,0,0,rate))
 for frame in tuple(range(61))+(-1,):rows.append(run(0,2,2,7,100,frame,0,1,0,0,0,0))
 out=ROOT/'artifacts/th14/player-lifecycle-native.json';out.parent.mkdir(parents=True,exist_ok=True)
 out.write_text(json.dumps(dict(executableSha256=hashlib.sha256(img.data).hexdigest(),rows=rows))+'\n',encoding='utf8')
 print(f'{len(rows)} retail lifecycle cases')
if __name__=='__main__':main()
