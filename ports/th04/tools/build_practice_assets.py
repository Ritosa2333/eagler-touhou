"""Derive practice overlays only from the hash-pinned published solo HUD MAINs."""
from pathlib import Path
import sys,gzip,json,struct,subprocess,hashlib,copy
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT.parent/'tools'))
from build_native_boss_hud import mz_parts,insert_bridge,mz_finish
from disk_image import Fat12
sha=lambda b:hashlib.sha256(b).hexdigest()
# Published guest-EMS HUD revisions, including the audited native divide guards.
SOURCES={'cn':'039e00e3d1b022a6af853a0663fe922414010b0b8dfa2b994dcaa6a3604318b3',
         'jp':'72009d521ba986ae10ca60a4bd12ba027328cc7420e8458ed42276da459aba62'}

def main():
    out=ROOT/'build/practice';out.mkdir(parents=True,exist_ok=True)
    manifest={'schema':1,'version':'th04-browser-practice/1','languages':{}}
    for language in ('cn','jp'):
        suffix='-jp' if language=='jp' else ''
        p=copy.deepcopy(json.loads((ROOT/f'web/solo-patch{suffix}.json').read_text()))
        raw=gzip.decompress((ROOT/f'web/th04-solo{suffix}.hdi.gz').read_bytes())
        assert sha(raw)==p['disk']['sha256']
        disk=Fat12(raw);source=disk.read(disk.files()['GENSO/MAIN.EXE'])
        assert sha(source)==p['sha256']
        assert p['native_boss_hud']['implementation']=='guest-ems'
        # Pin both language revisions, not merely the editable disk manifest.
        assert sha(source)==SOURCES[language],('Unaudited source MAIN',language,sha(source))
        h,image,rel=mz_parts(source);cs=p['code_segment'];insert=p['insert_image_offset']+p['insert_size']
        # Exact original dialog entry/opcode anchors (same executable layout).
        dialog=image.index(bytes.fromhex('c806000056578c5efcc746fa'))
        raise_if=not (0xce00<=dialog<=0xd700)
        assert not raise_if,hex(dialog)
        # dialog_run calls the same near opcode function twice.
        from capstone import Cs,CS_ARCH_X86,CS_MODE_16
        ins=list(Cs(CS_ARCH_X86,CS_MODE_16).disasm(bytes(image[dialog:dialog+600]),dialog))
        calls=[int(i.op_str,16) for i in ins if i.mnemonic=='call']
        repeated=[at for at in set(calls) if calls.count(at)==2 and 0xd000<at<dialog]
        assert len(repeated)==1,(hex(dialog),repeated)
        dialog_op=repeated[0]
        # Native power update recomputes shot level, the character-specific
        # shot function and HUD together. Dream is a score, not dynamic rank.
        power_update=0x11de6;dream_put=0xf07a
        assert image[power_update:power_update+14]==bytes.fromhex('33db33c0a06446b909003b87de1e')
        assert image[power_update+25:power_update+38]==bytes.fromhex('88166546031e9c448b07a39a44')
        assert image[dream_put:dream_put+16]==bytes.fromhex('558bec666811003e00a1ccbc6bc00a50')
        (out/'practice-config.inc').write_text(f'%define PRACTICE_ORG {insert-cs*16}\n%define SOLO_UPDATE {p["entries"][1]}\n%define SOLO_PAUSE {p["entries"][4]}\n%define SOLO_FRAME {p["entries"][7]}\n%define DIALOG_BODY {dialog+4-cs*16}\n%define DIALOG_OP {dialog_op-cs*16}\n%define POWER_UPDATE {power_update-cs*16}\n%define DREAM_PUT {dream_put-cs*16}\n')
        nasm=ROOT/'tools/nasm-2.16.03/nasm.exe'
        subprocess.run([str(nasm),'-f','bin','-I',str(out)+'/',str(ROOT/'patches/practice.asm'),'-o',str(out/'practice.bin')],check=True)
        code=(out/'practice.bin').read_bytes();assert code[:8]==b'T04PRA01'
        names=('load','frame','player','world','entryPause','hold','targetPractice','lockLives','unlimitedBombs','enemyLock','respawnLives','bombGate','bombConsume','livesZero','bombsZero','shotsReturn','dialogRun','resourceDirty','boss')
        entries=dict(zip(names,struct.unpack_from('<19H',code,8)))
        gap=(len(code)+15)&~15;assert gap<=1024 and insert-cs*16+gap<65536
        insert_bridge(h,image,rel,insert,gap);image[insert:insert+len(code)]=code
        edits=[]
        def patch(at,before,after):
            assert image[at:at+len(before)]==before,(language,hex(at),image[at:at+len(before)].hex(),before.hex())
            assert len(before)==len(after)
            image[at:at+len(after)]=after;edits.append({'offset':at,'before':before.hex(),'after':after.hex()})
        def near(at,before,target,kind=0xe8):
            patch(at,before,bytes([kind])+struct.pack('<H',(cs*16+entries[target]-at-3)&65535)+b'\x90'*(len(before)-3))
        near(0xaf10,bytes.fromhex('e8f0fd'),'load')
        near(0xabba,bytes.fromhex('ff168a42'),'frame')
        near(0xabe1,b'\xe8'+struct.pack('<H',(cs*16+p['entries'][1]-0xabe4)&65535),'player')
        near(0xabf5,bytes.fromhex('ff1ed0bc'),'boss')
        # After all bullet/enemy/boss/item collisions; before the next update.
        near(0xacf4,b'\xe8'+struct.pack('<H',(cs*16+p['entries'][7]-0xacf7)&65535),'world')
        near(0x10a8f,bytes.fromhex('26807f0b017621'),'respawnLives',0xe9)
        near(0xffc2,bytes.fromhex('26807f0d00745f'),'bombGate',0xe9)
        near(0xfff1,bytes.fromhex('26fe4f0d'),'bombConsume')
        near(0xeee8,bytes.fromhex('558bec'),'livesZero',0xe9)
        near(0xefa1,bytes.fromhex('558bec'),'bombsZero',0xe9)
        at=0x1070d
        patch(at,bytes.fromhex('8bc75f5ec9cb'),b'\xea'+struct.pack('<HH',entries['shotsReturn'],cs)+b'\x90');rel.append(at+3)
        near(dialog,bytes.fromhex('c8060000'),'dialogRun',0xe9)
        exe=mz_finish(h,image,rel);name=f'practice-{language}.exe.gz'
        (ROOT/'web'/name).write_bytes(gzip.compress(exe,mtime=0))
        p['data_segment']+=gap//16;p['insert_size']+=gap;p['sha256']=sha(exe)
        p['practice']={'version':1,'entries':entries,'bytes':gap}
        manifest['languages'][language]={'url':name,'sha256':sha(exe),'size':len(exe),'sourceMainSha256':sha(source),'patch':p}
        (out/f'{language}.json').write_text(json.dumps({'edits':edits,'extraBytes':gap,'dialog':dialog,'dialogOp':dialog_op},indent=2)+'\n')
    # Reuse the audited standalone resident/display initialization, with a
    # distinct signature. No cooperative resident hook is assembled here.
    full=(ROOT/'patches/launch.asm').read_text().replace('TH04WEBSTARTv1!','TH04PRASTARTv1!')
    (out/'launch.asm').write_text(full)
    subprocess.run([str(nasm),'-f','bin',str(out/'launch.asm'),'-o',str(out/'practice.com')],check=True)
    launcher=(out/'practice.com').read_bytes();(ROOT/'web/practice-start.com').write_bytes(launcher)
    manifest['launcher']={'url':'practice-start.com','sha256':sha(launcher),'size':len(launcher),'signature':'TH04PRASTARTv1!'}
    (ROOT/'web/practice-assets.json').write_text(json.dumps(manifest,indent=2)+'\n')
    print('Built isolated TH04 practice overlays; published HUD disks preserved')
if __name__=='__main__':main()
