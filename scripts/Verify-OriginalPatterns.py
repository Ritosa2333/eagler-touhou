"""Capture TH05 projectile births from an owned PC-98 MAIN.EXE in Unicorn.

No retail bytes are distributed. The checked hash, DOS I/O and video stubs
are shared with Verify-OriginalDemos.py. Use browser.mjs --pc98-patterns DIR
to compare the resulting frame traces against the compiled browser runtime.
"""
import argparse
import json
import runpy
import struct
from pathlib import Path

ns = runpy.run_path(str(Path(__file__).with_name('Verify-OriginalDemos.py')))
VM = ns['VM']
last=[8240,6240,9050,11200,7960,1850,11190]
ds=VM.DS*16
hp_cases = [('midboss', 0), ('midboss', 1), ('midboss', 6),
            ('boss', 3), ('boss', 4), ('boss', 5), ('boss', 6)]


def hp_override(stage, kind, phase, tick, end_hp):
    if kind == 'midboss':
        schedules = {(0, 3): {16: 201, 96: 200},
                     (1, 1): {16: 801, 48: 800, 96: 601, 144: 600},
                     (6, 1): {16: 1000, 144: 999}}
        hp = schedules.get((stage, phase), {}).get(tick, -1)
        return hp, -1
    if stage == 3 and phase == 2:
        return {128: (3000, 2500), 192: (2999, 2500),
                448: (1250, 1000), 512: (1249, 1000)}.get(tick, (-1, -1))
    if stage == 4 and phase in (4, 7):
        margin = 600 if phase == 4 else 500
        return {31: end_hp + margin, 32: end_hp + margin - 1}.get(tick, -1), -1
    if stage == 5 and phase == 10:
        return {208: 5601, 288: 5600, 512: 3801, 640: 3800}.get(tick, -1), -1
    if stage == 6 and phase == 17:
        return {144: 2501, 224: 1700, 304: 501, 384: 1}.get(tick, -1), -1
    return -1, -1
def shot_speeds(vm):
    mem=vm.u.mem_read(ds+0x5f2a,400*32)
    return sorted(mem[i+16] for i in range(0,len(mem),32) if mem[i]==1)
def shot_geometry(vm):
    pool = vm.u.mem_read(ds + 0x5f2a, 400 * 32)
    return [
        list(struct.unpack_from('<hh', pool, offset + 2)) +
        list(struct.unpack_from('<hh', pool, offset + 10)) +
        [pool[offset + 17], pool[offset + 16],
         struct.unpack_from('<H', pool, offset + 24)[0], pool[offset + 18], pool[offset + 19]]
        for offset in range(0, len(pool), 32) if pool[offset] == 1
    ]

def capture(stage,kind,exe,resources,diff,rank,geometry=False,hp_sweep=False):
    vm=VM(5 if stage==6 else 1,exe,resources,stage=stage,difficulty=4 if stage==6 else diff)
    assert vm.r(0x576f)==(4 if stage==6 else diff)
    if kind in ('mai','yuki'):
        # Exercise both surviving twins' attack scripts separately.
        assert stage==3
        vm.w(0xc67c,0x42bb if kind=='mai' else 0x3795,2)
        vm.w(0x5aba,192*16,2)
        vm.w(0x5abc,64*16,2)
        vm.w(0x5ac6,7900,2)
    vm.w(0x5770,rank)
    vm.w(0x36d2,(-1000 if kind=="enemy" else 192)*16,2)
    vm.w(0x36d4,(1000 if kind=="enemy" else 320)*16,2)
    if kind!='midboss':vm.w(0x5ab0,0xfffe,2)
    start=vm.r(0x5ab0,2) if kind=='midboss' else 1
    ring=list(vm.u.mem_read(ds+0x3526,256))
    result={'stage':stage,'kind':kind,'rank':rank,'difficulty':vm.r(0x576f),'ring':ring,'start':start,'frames':[]}
    if hp_sweep:result['hpSweep']=True
    phases={}
    special={}
    def tune(base):
        return max(8,min(128,base//2+(rank*(base//2))//32))
    def born(u,access,addr,size,value,data):
        off=addr-ds
        if (value&255) not in (1,2) or vm.r(off)!=0:return
        if 0x578c<=off<0x5a8c and (off-0x578c)%24==0:
            special[off]=(0,)
        elif (value&255)==1 and 0xb40e<=off<0xb40e+63*26 and (off-0xb40e)%26==0 and (stage==3 or stage==4 or stage==5):
            special[off]=(1 if stage==4 else 2 if stage==3 else 3,tune(vm.r(0xb3f4+24)))
        elif (value&255)==1 and 0xc0aa<=off<0xc0aa+7*82 and (off-0xc0aa)%82==0 and stage in (3,5,6):
            special[off]=(4,vm.r(0xb3f4+24))
    def trace_special():
        return sorted([[kind[0],kind[1]] if len(kind)>1 else
          ([0,vm.r(off+12,2)] if vm.r(off)==1 else [0,0]) for off,kind in special.items()])
    if kind!='enemy':
        vm.u.hook_add(ns['UC_HOOK_MEM_WRITE'],born,None,ds+0x578c,ds+0x5a8b)
        vm.u.hook_add(ns['UC_HOOK_MEM_WRITE'],born,None,ds+0xb40e,ds+0xc0aa+7*82)

    # Clear the regular pool every tick to measure the firing routine's
    # births, independent of earlier trajectories and collisions. Its
    # same-frame capacity still applies. Special pools are left intact.
    for frame in range(start,(last[stage]+257 if kind=='enemy' else start+40000)):
        special.clear()
        vm.frame=frame
        vm.w(0x5764,frame,2)
        for off,mask in [(0x5766,1),(0x5767,3),(0x5768,7),(0x5769,15)]:vm.w(off,frame&mask)
        vm.w(0x5770,rank)
        vm.w(0x36f3,0)
        vm.u.mem_write(ds+0x5f2a,bytes(400*32))
        vm.call(0xc506)
        if hp_sweep:
            vm.call(0x1c0df,vm.C3,True)
            vm.w(0x4206,0)
            vm.w(0xc648,0,2)
        vm.w(0x4ad6,1^(frame&1))
        scroll=vm.r(0x3518,2,True)//16
        cursor=vm.r(0x3626)
        # Record the retail state before executing the firing routine.
        if kind=='enemy':
            vm.indirect(0x3f8e,True)
            vm.call(0x16be7,vm.C3,True)
            meta=[]
        elif kind=='midboss':
            if frame==start:vm.call(0x1c4c4,vm.C3,True)
            if not vm.r(0xc4aa):break
            phase=vm.r(0x5ab5)
            if phase>=0xfd:break
            phases[phase]=phases.get(phase,0)+1
            hp, _ = hp_override(stage, kind, phase, phases[phase], 0) if hp_sweep else (-1, -1)
            if hp >= 0:vm.w(0x5ab2,hp,2)
            meta=[phase,vm.r(0x5ab6,2),0,vm.r(0x5ab2,2),0,vm.r(0x5aa4,2,True),vm.r(0x5aa6,2,True),0]
            vm.call(0x12bad)
            vm.indirect(0xc4ae,True)
            if hp_sweep:meta.extend([hp,-1])
        else:
            phase=vm.r(0x5ac9)
            if phase>=0xfd:break
            phases[phase]=phases.get(phase,0)+1
            # Advance long HP-gated phases after collecting several cycles.
            damage=phases[phase]==(2400 if stage in (2,5) else 1600)
            if stage==2 and phase in (5,8,11):
                vm.w(0x5ac6,max(vm.r(0x5ac6,2),vm.r(0x5ad0,2)+1),2)
            if damage:
                vm.w(0x5ac6,vm.r(0x5ad0,2),2)
            hp, other_hp = hp_override(stage, kind, phase, phases[phase], vm.r(0x5ad0,2)) if hp_sweep else (-1, -1)
            if hp >= 0:vm.w(0x5ac6,hp,2)
            if other_hp >= 0:vm.w(0x5ade,other_hp,2)
            bombing = hit = False
            if hp_sweep and (stage,phase) in ((5,9),(5,10),(6,2)):
                tick = phases[phase]
                bombing = 25 <= tick < 28
                hit = 25 <= tick <= (70 if stage==6 else 28) and (stage==6 or tick!=26)
                vm.w(0x4206,bombing)
                if stage==5 and phase==9 and tick==25:
                    hp=vm.r(0x5ad0,2)+1
                    vm.w(0x5ac6,hp,2)
                if hit:
                    vm.w(0xc648,1,2)
                    vm.w(0xc4c8,vm.r(0x5aba,2),2)
                    vm.w(0xc4ca,vm.r(0x5abc,2),2)
                    vm.w(0xc4cc,0xba8e,2)
                    vm.w(0xba8e,1)
                    vm.w(0xba8e+14,20)
                    vm.w(0xba8e+16,16)
            meta=[phase,vm.r(0x5aca,2),vm.r(0x5acd),vm.r(0x5ac6,2,True),vm.r(0x5ad0,2,True),vm.r(0x5aba,2,True),vm.r(0x5abc,2,True),int(damage)]
            vm.call(0x12bad)
            vm.indirect(0xc67c,True)
            if hp_sweep:meta.extend([hp,other_hp,int(bombing),int(hit)])
        vm.call(0x170a2,vm.C3,True)
        result['frames'].append([scroll,cursor,shot_speeds(vm),meta,vm.r(0x3626),trace_special()])
        if geometry:result['frames'][-1].append(shot_geometry(vm))
        if kind!='enemy':vm.call(0x12ce5)
        if hp_sweep and geometry:
            trajectory = []
            if kind == 'boss' and stage in (3, 6):
                for i in range(7):
                    head, trail = 0xb40e + i * 26, 0xc0aa + i * 82
                    mode = vm.r(trail)
                    trajectory.extend([mode] + ([vm.r(head + o, 2, True) for o in (2,4,10,12)] +
                                      [vm.r(head + 1), vm.r(head + 24)] if mode else [0] * 6))
            if kind == 'boss' and stage == 5 and meta[0] == 10:
                for i in range(4):
                    laser = 0x578c + i * 24
                    mode = vm.r(laser)
                    trajectory.extend([mode, vm.r(laser + 10) if mode else 0])
            result['frames'][-1].append(trajectory)
    return result,phases

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--exe', type=Path, required=True)
    parser.add_argument('--resources', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--difficulty', type=int, choices=range(4), default=3)
    parser.add_argument('--rank', type=int, choices=range(65), default=32)
    parser.add_argument('--extra-rank', type=int, choices=range(32, 37), help='Override the performance value in Extra')
    parser.add_argument('--geometry', action='store_true', help='Include initial positions, velocities, angles, sprites and motion flags')
    parser.add_argument('--hp-sweep', action='store_true', help='Capture the attack scripts around their HP thresholds')
    args = parser.parse_args()
    resources = ns['read_resources'](args.resources)
    args.output.mkdir(parents=True, exist_ok=True)
    for kind in ('enemy','midboss','boss','mai','yuki'):
        for stage in range(7):
            if (stage==5 and kind=='midboss') or (stage!=3 and kind in ('mai','yuki')):
                continue
            if args.hp_sweep and (kind,stage) not in hp_cases:
                continue
            rank = args.extra_rank if stage == 6 and args.extra_rank is not None else args.rank
            result, phases = capture(stage,kind,args.exe,resources,args.difficulty,rank,args.geometry,args.hp_sweep)
            path = args.output / f'pc98-{kind}-{stage}.json'
            path.write_text(json.dumps(result,separators=(',',':')), encoding='utf-8')
            print(kind,stage,len(result['frames']),sum(len(f[2]) for f in result['frames']),phases,flush=True)


if __name__ == '__main__':
    main()
