from pathlib import Path
import os, struct, subprocess, tempfile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_16, UC_HOOK_INTR, UC_HOOK_INSN
from unicorn.x86_const import *

root = Path(__file__).resolve().parents[1] / 'pc98'
for game in (2, 4):
    with tempfile.TemporaryDirectory() as tmp:
        source, binary = Path(tmp)/'info.asm', Path(tmp)/'info.bin'
        source.write_text('bits 16\ncpu 386\n%define PRACTICE_PAGE_BACK 0\ndw practice_info_reset,practice_info_tick,practice_info_values,practice_info_misses,practice_info_frames,target_practice,target_preparing,practice_outcome,practice_form_options,practice_info_draw,practice_info_enabled,practice_info_graphics_mask\n'
            'target_practice: db 1\ntarget_preparing: db 0\npractice_outcome: db 0\npractice_form_options: times 11 db 0\npractice_form_scroll: dw 0\n'
            + ('%define PRACTICE_RESTART_TH02 1\n' if game == 2 else '')
            + f'%include "{(root / "practice-info.inc").as_posix()}"\n%include "{(root / "practice-text.inc").as_posix()}"\n')
        subprocess.run([os.environ.get('NASM','nasm'),'-f','bin',str(source),'-o',str(binary)],check=True)
        image=binary.read_bytes()
        reset,tick,values,misses,frames,target,preparing,outcome,options,draw,enabled,mask=struct.unpack_from('<12H',image)
        u=Uc(UC_ARCH_X86,UC_MODE_16);u.mem_map(0,0x100000);code=0x10000;data=0x30000;resident=0x40100
        u.mem_write(code,image)
        def write(at,fmt,*nums):u.mem_write(at,struct.pack(fmt,*nums))
        def run(at):
            for reg,n in ((UC_X86_REG_CS,0x1000),(UC_X86_REG_DS,0x3000),(UC_X86_REG_SS,0x8000),(UC_X86_REG_SP,0xff00)):u.reg_write(reg,n)
            write(0x8ff00,'<H',0xfff0);u.emu_start(code+at,code+0xfff0,count=2000)
        if game==2:
            write(data+0xbde,'<B',7);write(data+0xb2c,'<BB',3,2);write(data+0xaa6,'<B',64);write(data+0x2802,'<h',-3);write(data+0x2538,'<H',49)
        else:
            write(data+0xba86,'<HH',0x100,0x4000);write(resident+50,'<B',7);write(resident+11,'<B',4);write(resident+13,'<B',2);write(data+0x4664,'<B',64);write(data+0x5395,'<B',16);write(data+0x3974,'<H',49)
        run(reset);write(code+misses,'<H',2);write(code+options+7,'<B',1);write(code+frames,'<I',111)
        write((data+0xbde) if game==2 else (resident+50),'<B',10);run(tick)
        result=struct.unpack('<8H',u.mem_read(code+values,16))
        assert result==(3,2,2,3,64,0 if game==2 else 16,2,49),(game,result)
        write(code+preparing,'<B',1);run(tick);assert struct.unpack('<I',u.mem_read(code+frames,4))[0]==112
        write(code+preparing,'<B',0);write(code+outcome,'<B',1);run(tick);assert struct.unpack('<I',u.mem_read(code+frames,4))[0]==112
        planes=(0xa8000,0xb0000,0xb8000,0xe0000)
        pages=[[bytes(((i*17+plane*19+page*41)&255) for i in range(32000)) for plane in range(4)] for page in range(2)]
        original=[[bytes(x) for x in page] for page in pages];current=[0];alloc=[]
        for address,contents in zip(planes,pages[0]):u.mem_write(address,contents)
        def out_hook(uc,port,size,value,user):
            if port!=0xa6:return
            pages[current[0]]=[bytes(uc.mem_read(a,32000)) for a in planes];current[0]=value&1
            for address,contents in zip(planes,pages[current[0]]):uc.mem_write(address,contents)
        def interrupt(uc,number,user):
            assert number==0x21
            if uc.reg_read(UC_X86_REG_AH)==0x48:
                uc.reg_write(UC_X86_REG_AX,0x5000);uc.reg_write(UC_X86_REG_EFLAGS,uc.reg_read(UC_X86_REG_EFLAGS)&~1);alloc.append(True)
            else:assert uc.reg_read(UC_X86_REG_AH)==0x49;alloc.append(False)
        u.hook_add(UC_HOOK_INSN,out_hook,None,1,0,UC_X86_INS_OUT);u.hook_add(UC_HOOK_INTR,interrupt)
        def graphics(at):
            for reg,n in ((UC_X86_REG_CS,0x1000),(UC_X86_REG_DS,0x3000),(UC_X86_REG_SS,0x8000),(UC_X86_REG_SP,0xff00)):u.reg_write(reg,n)
            write(0x8ff00,'<H',0xfff0);u.emu_start(code+at,code+0xfff0,count=500000)
            assert u.reg_read(UC_X86_REG_IP)==0xfff0
        write(code+enabled,'<B',1);graphics(draw);graphics(mask)
        rows=10 if game==2 else 13;offset=176*80+52
        for plane in planes:
            for row in (0,rows*16-1):assert bytes(u.mem_read(plane+offset+row*80,28))==bytes(28)
            assert bytes(u.mem_read(plane+offset+rows*16*80,28))==original[0][planes.index(plane)][offset+rows*16*80:offset+rows*16*80+28]
        write(code+enabled,'<B',0);graphics(draw)
        pages[current[0]]=[bytes(u.mem_read(a,32000)) for a in planes]
        assert pages==original and alloc==[True,False],(game,'graphics restore')
print('PASS TH02/TH04 native information counters')
