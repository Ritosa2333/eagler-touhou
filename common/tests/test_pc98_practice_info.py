from pathlib import Path
import os, struct, subprocess, tempfile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_16
from unicorn.x86_const import UC_X86_REG_CS, UC_X86_REG_DS, UC_X86_REG_SS, UC_X86_REG_SP

root = Path(__file__).resolve().parents[1] / 'pc98'
for game in (2, 4):
    with tempfile.TemporaryDirectory() as tmp:
        source, binary = Path(tmp)/'info.asm', Path(tmp)/'info.bin'
        source.write_text('bits 16\ncpu 386\ndw practice_info_reset,practice_info_tick,practice_info_values,practice_info_misses,practice_info_frames,target_practice,target_preparing,practice_outcome,practice_form_options\n'
            'target_practice: db 1\ntarget_preparing: db 0\npractice_outcome: db 0\npractice_form_options: times 11 db 0\npractice_text_write: ret\n'
            + ('%define PRACTICE_RESTART_TH02 1\n' if game == 2 else '')
            + f'%include "{(root / "practice-info.inc").as_posix()}"\n')
        subprocess.run([os.environ.get('NASM','nasm'),'-f','bin',str(source),'-o',str(binary)],check=True)
        image=binary.read_bytes()
        reset,tick,values,misses,frames,target,preparing,outcome,options=struct.unpack_from('<9H',image)
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
print('PASS TH02/TH04 native information counters')
