"""Local, isolated 16-bit instruction oracle. Never launches the retail EXE.

Requires Unicorn 2.1.4. No retail code or resource bytes are distributed.
Only device waits, asset/HUD drawing, audio and DOS I/O are substituted.
Native laser clipping/retirement and firewave collision code still execute.
This validates combat state, not visual/audio output or score presentation.
"""
from pathlib import Path
import argparse, hashlib, struct, json, sys
from unicorn import *
from unicorn.x86_const import *

class VM:
    BASE = 0x10000
    DS = 0x309d
    C1 = 0x1ae1
    C3 = 0x2527
    def __init__(self, number, image, resources):
        self.resources=resources
        f=image.read_bytes()
        assert hashlib.sha256(f).hexdigest() == 'c41f6e6b9a97b2433acc576ceaee800707c8d4ea7e498150a259663c8fa7d4f0'
        h=struct.unpack('<14H',f[:28]); self.raw=f[h[4]*16:]
        self.u=Uc(UC_ARCH_X86,UC_MODE_16); self.u.mem_map(0,0x200000)
        self.u.mem_write(self.BASE,self.raw)
        for i in range(h[3]):
            off,seg=struct.unpack_from('<HH',f,h[12]+i*4);a=self.BASE+seg*16+off
            self.put(a,(self.get(a,2)+0x1000)&65535,2)
        self.u.reg_write(UC_X86_REG_DS,self.DS)
        self.u.reg_write(UC_X86_REG_ES,self.DS)
        self.u.reg_write(UC_X86_REG_SS,0x9000)
        self.u.reg_write(UC_X86_REG_EFLAGS,2)
        self.u.hook_add(UC_HOOK_INTR,self.interrupt)
        self.u.hook_add(UC_HOOK_INSN,lambda u,p,s,v,d:None,None,1,0,UC_X86_INS_OUT)
        self.u.hook_add(UC_HOOK_INSN,lambda u,p,s,d:0,None,1,0,UC_X86_INS_IN)
        self.number=number;self.files={};self.nextseg=0x6000;self.quit=False
        # Drawing functions have no game-state role. Explicit original calling
        # conventions, including far Pascal argument cleanup, are preserved.
        for a,far,args in [(0x1ee2,1,2),(0x2d16,1,4),(0x151d6,1,6),
                            (0x14510,0,4),(0xe4b6,0,0),(0xc9a4,0,0),
                            (0xd4c0,0,0),(0xd323,1,0),(0xbd24,0,0),(0x2e1a,1,10),(0x2fe0,1,6)]:
            self.stub(a,far,args)
        # hmem_allocbyte: explicit local arena; no host memory/OS execution.
        self.u.mem_write(self.BASE+0x281a,b'\xca\x02\x00')
        def alloc(u,a,size,data):
            sp=u.reg_read(UC_X86_REG_SP);n=self.get(u.reg_read(UC_X86_REG_SS)*16+sp+4,2)
            u.reg_write(UC_X86_REG_AX,self.nextseg);self.nextseg+=(n+15)//16
        self.u.hook_add(UC_HOOK_CODE,alloc,None,self.BASE+0x281a,self.BASE+0x281a)
        self.stub(0x291e,True,2) # local arena release
        self.stub(0x2719,True,0) # device vsync wait
        self.stub(0x15102,True,2) # device frame delay
        self.stub(0xcec2,True,4) # process replacement: observable Demo endpoint
        def demo_end(u,a,size,data):self.quit=True
        self.u.hook_add(UC_HOOK_CODE,demo_end,None,self.BASE+0xcec2,self.BASE+0xcec2)
        self.stub(0xd85c)
        def gameover(u,a,size,data):self.quit=True;u.reg_write(UC_X86_REG_AX,2)
        self.u.hook_add(UC_HOOK_CODE,gameover,None,self.BASE+0xd85c,self.BASE+0xd85c)
        resident=0x80000;self.resident=resident
        self.w(0x41fc,0,2);self.w(0x41fe,0x8000,2)
        for off,value in {0xd:3,0xe:3,0x11:3,0x14:[0,1,2,3,2][number-1],0x1f:number,0x1d:[3,1,2,4,6][number-1]}.items():self.put(resident+off,value)
        self.w(0x576e,[3,1,2,4,6][number-1])
        self.call(0xb063)
        self.w(0x36f0,1 if number==5 else 128)
        self.w(0x418,318,4)
        self.call(0xb55a)
        # Midboss wrappers are installed by the native reset and setup code.
        self.call(0x1c4a1,self.C3,True)
        self.call([0x14797,0x1460b,0x146c8,0x14871,0x149fe][number-1])
        self.call(0xe5c2)
        self.call(0xc742)
        if number==5:
            self.call(0x139bb)
            self.stub(0xf76,True,8) # VRAM box fill
            self.stub(0xc07c,False,2) # tiny sprite put
            self.stub(0x12f2b) # cheeto drawing only
            self.stub(0x14081) # sprite pattern replacement only
            self.stub(0x14ea4,True,6) # snd_load
            self.stub(0x14f5c,True,2) # snd_kaja_func
            self.w(0x5778,0,2);self.w(0x577a,0x7200,2)
        self.call(0x130c,0x1000,True,args=(32,16,415,383))
        self.call(0xbb9e)
        # Start in stage play rather than simulating the video-device intro.
        self.w(0xc656,0x8bec if number==5 else 0x8be5,2)
        self.rec=self.resources[f'DEMO{number-1}.REC']
        self.frame=0;self.index=0;self.part2=False
        self.w(0x351a,0) # draw-only flag, not the scroll simulator
        self.w(0x4ad6,1) # retail stage initialization selects back page 1
    def get(self,a,n=1,signed=False):return int.from_bytes(self.u.mem_read(a,n),'little',signed=signed)
    def put(self,a,v,n=1):self.u.mem_write(a,int(v & ((1<<(8*n))-1)).to_bytes(n,'little'))
    def r(self,o,n=1,signed=False):return self.get(self.DS*16+o,n,signed)
    def w(self,o,v,n=1):self.put(self.DS*16+o,v,n)
    def stub(self,a,far=False,args=0):self.u.mem_write(self.BASE+a,(b'\xca' if far else b'\xc2')+struct.pack('<H',args))
    def interrupt(self,u,intno,data):
        ax=u.reg_read(UC_X86_REG_AX);ah=ax>>8;ds=u.reg_read(UC_X86_REG_DS);dx=u.reg_read(UC_X86_REG_DX)
        if intno==0x21 and ah==0x3d:
            name=bytes(u.mem_read(ds*16+dx,64)).split(b'\0')[0].decode('cp932')
            self.files[5]=[bytearray(self.resources[name.upper()]),0];u.reg_write(UC_X86_REG_AX,5)
        elif intno==0x21 and ah==0x3f:
            content,pos=self.files[u.reg_read(UC_X86_REG_BX)];n=u.reg_read(UC_X86_REG_CX);chunk=bytes(content[pos:pos+n])
            u.mem_write(ds*16+dx,chunk);self.files[u.reg_read(UC_X86_REG_BX)][1]+=len(chunk);u.reg_write(UC_X86_REG_AX,len(chunk))
        elif intno==0x21 and ah==0x42:
            content,pos=self.files[u.reg_read(UC_X86_REG_BX)]
            off=(u.reg_read(UC_X86_REG_CX)<<16)|dx
            if off&0x80000000:off-=0x100000000
            pos=([0,pos,len(content)][ax&255])+off
            self.files[u.reg_read(UC_X86_REG_BX)][1]=pos
            u.reg_write(UC_X86_REG_AX,pos&65535);u.reg_write(UC_X86_REG_DX,pos>>16)
        elif intno==0x21 and ah==0x3e:pass
        else:raise RuntimeError(f'Unhandled interrupt {intno:x} ax={ax:x} cs:ip={u.reg_read(UC_X86_REG_CS):x}:{u.reg_read(UC_X86_REG_IP):x}')
        u.reg_write(UC_X86_REG_EFLAGS,u.reg_read(UC_X86_REG_EFLAGS)&~1)
    def call(self,a,cs=None,far=False,args=()):
        cs=cs or self.C1;ip=self.BASE+a-cs*16
        assert 0<=ip<65536,(hex(a),hex(cs))
        sp=(0xfefc if far else 0xfefe)-2*len(args)
        self.u.reg_write(UC_X86_REG_CS,cs);self.u.reg_write(UC_X86_REG_SP,sp)
        self.put(0x90000+sp,0xfe00,2)
        if far:self.put(0x90000+sp+2,cs,2)
        for i,value in enumerate(reversed(args)):self.put(0x90000+sp+(4 if far else 2)+i*2,value,2)
        try:self.u.emu_start(self.BASE+a,cs*16+0xfe00,count=2000000)
        except Exception:
            pc=self.u.reg_read(UC_X86_REG_CS)*16+self.u.reg_read(UC_X86_REG_IP)
            print('ERROR',hex(a),'frame',self.frame if hasattr(self,'frame') else -1,'pc',hex(pc-self.BASE), 'sp',hex(self.u.reg_read(UC_X86_REG_SP)),file=sys.stderr)
            raise
        if self.u.reg_read(UC_X86_REG_IP)!=0xfe00:
            pc=self.u.reg_read(UC_X86_REG_CS)*16+self.u.reg_read(UC_X86_REG_IP)
            raise RuntimeError(('instruction limit',hex(a),hex(pc-self.BASE),self.frame))
    def indirect(self,off,far=False):
        cs=self.r(off+2,2) if far else self.C1
        self.call(cs*16+self.r(off,2)-self.BASE,cs,far)
    def step(self):
        n=len(self.rec)//2;key=self.rec[self.index]
        self.w(0x3064,key,2);self.w(0x3066,self.rec[n+self.index])
        self.indirect(0xc656)
        if self.number==5 and not self.part2 and self.r(0x1ee8):
            self.rec=self.resources['DEMO5.REC'];self.index=0;self.part2=True
        self.indirect(0x3f8e,True)
        for a in [0xe1a6,0xe322,0xe086,0xcc3d,0x133db,0x12bad]:self.call(a)
        self.call(0x1c0df,self.C3,True)
        self.call(0x16be7,self.C3,True)
        self.indirect(0xc4ae,True);self.indirect(0xc672,True)
        self.call(0x1d0ad,self.C3,True);self.call(0x170a2,self.C3,True)
        self.indirect(0x4a88)
        if self.r(0x4206):self.indirect(0x4208)
        if self.number==5:self.call(0x12ce5) # native renderer retires clipped laser slots
        if self.number==5:self.indirect(0xc678) # original firewave render also tests collision
        state=self.state()
        self.call(0xc506)
        self.w(0x5764,self.r(0x5764,2)+1,2)
        for o,mask in [(0x5766,1),(0x5767,3),(0x5768,7),(0x5769,15)]:self.w(o,self.r(0x5764,2)&mask)
        if self.r(0x5764,2)%4096==0:self.call(0xcf7a,far=True,args=(1,))
        self.call(0xd4f2)
        self.frame+=1;self.index+=1
        self.w(0x4ad6,1^(self.frame&1))
        return state
    def state(self):
        data=self.u.mem_read(self.DS*16,65536)
        def r(o,n=1,signed=False):return int.from_bytes(data[o:o+n],'little',signed=signed)
        items=[];enemies=[];bullets=[];lasers=[];curves=[]
        for i in range(32):
            o=0x578c+i*24
            if r(o):lasers.append([i,r(o)]+[r(o+k,2,True) for k in [2,4,6,8]]+[r(o+10),r(o+11),r(o+12,2),r(o+14,2),r(o+16,2,True)])
        for i in range(7):
            h=0xb40e+i*26;t=0xc0aa+i*82
            if r(t):curves.append([i,r(t)]+[r(h+k,2,True) for k in [2,4,10,12]]+[r(h+1),r(h+24),r(h+14,2)]+[r(t+2+k*2,2,True) for k in range(32)])
        for i in range(400):
            o=0x5f2a+i*32
            if r(o)==1:bullets.append([i]+[r(o+k,2,True) for k in [2,4,10,12]]+[r(o+k) for k in [16,17,18,19,1]])
        for i in range(40):
            o=0xb084+i*20
            if r(o):items.append([i,r(o)]+[r(o+k,2,True) for k in [2,4,10,12]]+[r(o+14)])
        for i in range(32):
            o=0x93fa+i*64
            if r(o):enemies.append([i,r(o),r(o+14,2)]+[r(o+k,2,True) for k in [2,4,10,12]]+[r(o+38)])
        return dict(lasers=lasers,curves=curves,boss=[r(0x5aba,2,True),r(0x5abc,2,True),r(0x5ac6,2,True),r(0x5ac9),r(0x5aca,2),r(0x5acd)],bullets=bullets,scroll=[r(0x3515),r(0x3518,2)],midboss=[r(0x5aa4,2,True),r(0x5aa6,2,True),r(0x5ab2,2,True),r(0x5ab5),r(0x5ab6,2)],items=items,enemies=enemies,frame=self.frame,x=r(0x36d2,2,True),y=r(0x36d4,2,True),
                    seed=r(0x418,4),ring=r(0x3626),power=r(0x36f0),dream=r(0x36f6),
                    lives=r(0x4ad4),bombs=r(0x4ad5),rank=r(0x5770),
                    hit=r(0x36f3),miss=r(0x36f4),inv=r(0x36ee),lock=r(0x36ef))


def read_resources(directory):
    """Read local TH05 DAT archives without writing extracted game assets."""
    resources = {}
    for name in ('kaiki1.DAT', 'kaiki2.DAT'):
        path = directory / name
        if not path.exists():
            continue
        data = path.read_bytes()
        size = struct.unpack_from('<H', data)[0]
        table = bytearray(data[16:16 + size])
        key = data[6]
        for i in range(size):
            table[i] ^= key
            key = (key - table[i]) & 255
        for offset in range(0, size, 32):
            entry = table[offset:offset + 32]
            if not entry[0]:
                break
            mode = struct.unpack_from('<H', entry)[0]
            key = entry[2]
            filename = bytes(entry[3:16]).split(b'\0')[0].decode('cp932').upper()
            length = struct.unpack_from('<H', entry, 16)[0]
            position = struct.unpack_from('<I', entry, 20)[0]
            source = bytes(x ^ key for x in data[position:position + length])
            result = bytearray()
            if mode == 0xf388:
                result = source
            else:
                last, i = 255, 0
                while i < len(source):
                    value = source[i]
                    i += 1
                    if not result or value != last or i == len(source):
                        result.append(value)
                        last = value
                    else:
                        result.extend([value] * (source[i] + 1))
                        i += 1
            resources[filename] = bytes(result)
    if not resources:
        resources = {p.name.upper(): p.read_bytes() for p in directory.iterdir() if p.is_file()}
    return resources


FIELDS = ('frame', 'x', 'y', 'seed', 'ring', 'power', 'dream', 'lives',
          'bombs', 'rank', 'hit', 'miss', 'inv', 'lock')
COLLECTIONS = ('items', 'enemies', 'bullets')


def combat_words(state):
    words = [state[key] for key in FIELDS]
    for key in COLLECTIONS:
        rows = [row for row in state[key] if key == 'bullets' or row[1] == 1]
        words.append(len(rows))
        for row in rows:
            words.extend(row)
    return words


def hash_words(value, words):
    for word in words:
        for shift in (0, 8, 16, 24):
            value = ((value ^ ((word >> shift) & 255)) * 1099511628211) & 0xffffffffffffffff
    return value


def compare(number, image, resources, traces):
    path = traces / f'port-demo-{number}.jsonl'
    # Blocking dialogue does not advance simulation. Retain its final state.
    frames = {s['frame']: s for s in map(json.loads, path.read_text().splitlines())}
    vm = VM(number, image, resources)
    digest = boss_digest = special_digest = 14695981039346656037
    last = max(frames)
    for frame in range(last + 1):
        native = vm.step()
        port = frames[frame]
        left, right = combat_words(native), combat_words(port)
        if left != right:
            fields = [key for key in FIELDS if native[key] != port[key]]
            fields += [key for key in COLLECTIONS
                       if [r for r in native[key] if key == 'bullets' or r[1] == 1]
                       != [r for r in port[key] if key == 'bullets' or r[1] == 1]]
            raise RuntimeError(f'Demo {number}, frame {frame}: mismatch in {fields}')
        for key in ('lasers', 'curves'):
            if native[key] != port[key]:
                raise RuntimeError(f'Demo {number}, frame {frame}: {key} {native[key]} != {port[key]}')
        if number == 5 and frame >= 12156 and native['boss'] != port['boss']:
            raise RuntimeError(f'Demo 5, frame {frame}: boss {native["boss"]} != {port["boss"]}')
        for key in ('lasers', 'curves'):
            special_digest = hash_words(special_digest, [len(native[key])] + [v for row in native[key] for v in row])
        digest = hash_words(digest, left)
        if number == 5 and frame >= 12156:
            boss_digest = hash_words(boss_digest, native["boss"])
        if vm.quit:
            if frame != last:
                raise RuntimeError(f'Demo {number}: native exit at {frame}, port ends at {last}')
            break
    if number < 5 and (last != 4995 or vm.quit):
        raise RuntimeError(f'Demo {number}: incorrect normal input endpoint')
    if number == 5 and not vm.quit:
        raise RuntimeError('Extra port ended before the native endpoint')
    print(f'PASS Demo {number}: {last + 1} frames, combat FNV64 {digest:016x}, boss {boss_digest:016x}, special {special_digest:016x}', flush=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--exe', type=Path, required=True, help='Locally owned original Japanese MAIN.EXE')
    parser.add_argument('--resources', type=Path, required=True, help='Local DAT directory, or extracted resource directory')
    parser.add_argument('--traces', type=Path, required=True, help='Directory written with TH05_DEMO_TRACE=1')
    parser.add_argument('--demo', type=int, choices=range(1, 6), action='append', help='Default: all five Demos')
    args = parser.parse_args()
    resources = read_resources(args.resources)
    for number in args.demo or range(1, 6):
        compare(number, args.exe, resources, args.traces)


if __name__ == '__main__':
    main()
