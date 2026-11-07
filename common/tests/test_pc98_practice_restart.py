from pathlib import Path
import os, struct, subprocess, tempfile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_16
from unicorn.x86_const import UC_X86_REG_CS, UC_X86_REG_DS, UC_X86_REG_SS, UC_X86_REG_SP

root = Path(__file__).resolve().parents[1] / 'pc98'
with tempfile.TemporaryDirectory() as tmp:
    for game in (2, 4):
        source, binary = Path(tmp) / 'restart.asm', Path(tmp) / 'restart.bin'
        source.write_text('bits 16\ndw practice_restart_tick,practice_restart_pending,practice_restart_values,practice_result_selecting,hold,practice_loading_clear_if_present,practice_native_setup,practice_form_values,practice_initial_stage,practice_initial_entry,practice_initial_phase,practice_initial_lives,practice_initial_bombs,practice_initial_power,practice_initial_rank,practice_initial_hp,practice_initial_gengetsu,practice_initial_reimu,target_preparing,target_practice,practice_form_options,practice_native_restrictions\n'
                          'practice_result_selecting: db 0\nhold: db 0\ntarget_preparing: db 0\ntarget_practice: db 0\npractice_outcome: db 0\npractice_initial_entry: db 0\npractice_initial_stage: db 0\npractice_initial_phase: db 0\npractice_initial_lives: db 0\npractice_initial_bombs: db 0\npractice_initial_power: db 0\npractice_initial_rank: dw 0\npractice_initial_hp: dw 0\npractice_initial_gengetsu: db 0\npractice_initial_reimu: db 0\npractice_form_hp: dw 0\npractice_form_values: times 10 db 0\npractice_form_options: times 11 db 0\nsignal: ret\npractice_text_write: ret\n'
                          'practice_form_input: dw 0\npractice_form_open: db 0\npractice_pause_open: db 0\npractice_form_request: db 0\n'
                          'times 0x200-($-$$) db 0\n' +
                          ('%define PRACTICE_RESTART_TH02 1\n%define SOLO_CURRENT_INPUT 0x180\n' if game == 2 else '') +
                          f'%include "{(root / "practice-restart.inc").as_posix()}"\n')
        subprocess.run([os.environ.get('NASM', 'nasm'), '-f', 'bin', str(source), '-o', str(binary)], check=True)
        image = binary.read_bytes()
        (entry,pending,values,selecting,hold,clear_loading,native_setup,form_values,
         initial_stage,initial_entry,initial_phase,initial_lives,initial_bombs,initial_power,
         initial_rank,initial_hp,initial_gen,initial_reimu,preparing,target,form_options,restrictions)=struct.unpack_from('<22H', image)
        u = Uc(UC_ARCH_X86, UC_MODE_16)
        u.mem_map(0, 0x100000)
        code, data, resident = 0x10000, 0x30000, 0x40100
        u.mem_write(code, image)
        ptr = 0x27fc if game == 2 else 0xba86
        u.mem_write(data + ptr, struct.pack('<HH', 0x100, 0x4000))
        u.mem_write(resident + 11, bytes(range(64)))
        u.mem_write(code + pending, b'\1')
        u.mem_write(code + values, bytes([1, 2, 1, 3, 2, 1]))
        for reg, value in ((UC_X86_REG_CS, 0x1000), (UC_X86_REG_DS, 0x3000),
                           (UC_X86_REG_SS, 0x8000), (UC_X86_REG_SP, 0xff00)):
            u.reg_write(reg, value)
        u.mem_write(0x8ff00, struct.pack('<H', 0xfff0))
        u.emu_start(code + entry, code + 0xfff0, count=1000)
        assert u.mem_read(code + pending, 1) == b'\0'
        if game == 2:
            assert bytes(u.mem_read(resident + 11, 1)) == b'\1'
            assert bytes(u.mem_read(resident + 22, 1)) == b'\2'
            assert bytes(u.mem_read(resident + 38, 1)) == b'\1'
            assert bytes(u.mem_read(resident + 20, 2)) == b'\2\3'
            assert bytes(u.mem_read(code + 0x10e, 8)) == b'\xe9\x09\x00main\0'
            assert bytes(u.mem_read(data + 0x2b97, 1)) == b'\1'
        else:
            assert bytes(u.mem_read(resident + 15, 5)) == b'\2\x05\x01' + b'01'
            assert bytes(u.mem_read(resident + 11, 4)) == b'\4\4\2\2'
            assert bytes(u.mem_read(resident + 25, 1)) == b'\1'
            assert bytes(u.mem_read(code + 0x88, 5)) == b'main\0'
            assert bytes(u.mem_read(data + 0x5392, 1)) == b'\1'
        u.mem_write(code + pending, b'\1')
        u.mem_write(code + selecting, b'\1')
        u.mem_write(code + hold, b'\1')
        u.mem_write(code + values, bytes([0, 4, 3, 0, 0, 0]))
        for reg, value in ((UC_X86_REG_CS, 0x1000), (UC_X86_REG_DS, 0x3000),
                           (UC_X86_REG_SS, 0x8000), (UC_X86_REG_SP, 0xff00)):
            u.reg_write(reg, value)
        u.mem_write(0x8ff00, struct.pack('<H', 0xfff0))
        u.emu_start(code + entry, code + 0xfff0, count=1000)
        assert u.mem_read(code + selecting, 1) == b'\0' and u.mem_read(code + hold, 1) == b'\0'
        assert u.mem_read(resident + (22 if game == 2 else 15), 1) == b'\2', 'ordinary retry preserves difficulty'
        text_at=0xa0000+11*160+13*2
        label=b'     LOADING PRACTICE     '
        u.mem_write(text_at,b''.join(bytes((c,0)) for c in label))
        u.mem_write(text_at+0x2000,b'\xe1\0'*26)
        for reg, value in ((UC_X86_REG_CS, 0x1000), (UC_X86_REG_DS, 0x3000),
                           (UC_X86_REG_SS, 0x8000), (UC_X86_REG_SP, 0xff00)):
            u.reg_write(reg, value)
        u.mem_write(0x8ff00, struct.pack('<H', 0xfff0))
        u.emu_start(code+clear_loading,code+0xfff0,count=1000)
        assert bytes(u.mem_read(text_at,52))==b' \0'*26
        assert bytes(u.mem_read(text_at+0x2000,52))==b'\0'*52
        u.mem_write(text_at,b'A\0'+b' \0'*25)
        u.reg_write(UC_X86_REG_SP,0xff00);u.mem_write(0x8ff00,struct.pack('<H',0xfff0))
        u.emu_start(code+clear_loading,code+0xfff0,count=1000)
        assert bytes(u.mem_read(text_at,2))==b'A\0', 'unrelated text must be preserved'
        fields=bytes([1,3,2,2,4,3,2,64,1,16 if game==4 else 2])
        u.mem_write(0x4e0,b'TPR4' if game==4 else b'TPR2')
        u.mem_write(0x4e4,fields+struct.pack('<H',345))
        u.mem_write(0x4f0,bytes([1,0,1,0,1,1,0,0,0,0,3]))
        u.reg_write(UC_X86_REG_SP,0xff00);u.mem_write(0x8ff00,struct.pack('<H',0xfff0))
        u.emu_start(code+native_setup,code+0xfff0,count=1000)
        assert bytes(u.mem_read(0x4e0,4))==b'\0'*4 and bytes(u.mem_read(code+form_values,10))==fields
        assert bytes(u.mem_read(code+form_options,11))==bytes([1,0,1,0,1,1,0,0,0,0,3])
        assert [u.mem_read(code+at,1)[0] for at in (initial_stage,initial_entry,initial_phase,initial_lives,initial_bombs,initial_power,preparing,target)]==[1,2,4,3,2,64,1,1]
        assert struct.unpack('<H',u.mem_read(code+initial_hp,2))[0]==345
        if game==2:assert struct.unpack('<h',u.mem_read(code+initial_rank,2))[0]==-4
        else:
            assert u.mem_read(code+initial_rank,1)==b'\x14'
            assert u.mem_read(code+initial_gen,1)==b'\1' and u.mem_read(code+initial_reimu,1)==b'\1'
            assert u.mem_read(data+0x5394,1)==b'\1' and u.mem_read(resident+19,1)==b'1'
        u.mem_write(code+preparing,b'\0');u.mem_write(code+form_options,bytes([1,1,1,1,1,1,0,0,0,0,3]))
        keys_offset=0x2538 if game==2 else 0x3974
        u.mem_write(data+keys_offset,struct.pack('<H',0x7f))
        u.reg_write(UC_X86_REG_SP,0xff00);u.mem_write(0x8ff00,struct.pack('<H',0xfff0))
        u.emu_start(code+restrictions,code+0xfff0,count=1000)
        assert struct.unpack('<H',u.mem_read(data+keys_offset,2))[0]==0x4f
        if game==2:assert struct.unpack('<h',u.mem_read(data+0x2802,2))[0]==-4
        else:assert u.mem_read(data+0x5395,1)==b'\x14' and u.mem_read(data+0x3976,1)==b'\0'
print('PASS TH02/TH04 native resident configuration and MAIN restart')
