"""Build isolated, hash-pinned TH02 practice overlays; never rewrite disks."""
from pathlib import Path
import copy,gzip,hashlib,json,struct,subprocess,sys,os
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,os.environ.get('PC98_HUD_TOOLS',str(ROOT.parent/'tools')))
from build_native_boss_hud import mz_parts,insert_bridge,mz_finish,Fat12
sha=lambda b:hashlib.sha256(b).hexdigest()
SOURCES={'jp':'eec57979b004fc9ecb356d8f96202b6f98749e1d7ed30738e04650983f732302',
         'cn':'74a315562d6f37ee1ff4b4bd229e98937a3ca4c989becdc51e101798d52f8f5d'}
NAMES=('load','frame','miss','world','entryPause','hold','targetPractice','lockLives','unlimitedBombs','enemyLock',
       'deathGate','lifeConsume','bombGate','bombConsume','livesZero','bombsZero','shotsReturn','dialogBox','dialogPre','dialogPost',
       'powerDirty','lockTime','timeMid1','timeMid2','timeMid3','timeMid4','timeMima','transition','entities','pauseCall','assistFlags','targetPreparing','initialLives','initialBombs','initialPower','initialRank','targetResources','initialPhase','bossPhase','targetAdvance','initialEntry','warpState','targetWarp','arrivalPending','targetArrival','targetReached','targetCheck','initialHp','targetHp','initialStage','targetOutcome','targetOutcomeStep','resultVisible','resultRequest','resultAck','resultInput','waitHold','pauseOpen','pauseRequest','pauseAck','pauseInput')
NAMES+=('formOpen','formInput','formValues','formRequest','formHp','formOptions','infoEnabled','infoValues')
NAMES+=('restartPending','restartValues','resultSelecting')
NAMES+=('focusHook','focusTest')
def main():
    out=ROOT/'build/practice';out.mkdir(parents=True,exist_ok=True)
    nasm=os.environ.get('NASM','nasm')
    source_web=Path(os.environ.get('PC98_SOURCE_WEB',ROOT/'web'))
    manifest={'schema':1,'version':'th02-browser-practice/1','languages':{}}
    for language,original in json.loads((ROOT/'web/solo-patch.json').read_text())['disks'].items():
        p=copy.deepcopy(original);raw=gzip.decompress((source_web/p['file']).read_bytes())
        assert sha(raw)==p['sha256'],('disk changed',language)
        disk=Fat12(raw);source=disk.read(disk.files()['HUMAROKU/MAIN.EXE'])
        assert sha(source)==SOURCES[language]==p['mainSha256'],('Unaudited TH02 MAIN',language)
        assert p['nativeBossHud']['implementation']=='guest-ems'
        h,image,rel=mz_parts(source);cs=p['codeSegment'];oldgap=p['gap'];insert=0xfac0+oldgap
        # Retired tail remains at 0xf450; new bridge follows the existing HUD.
        dialog_cs=0xfac0//16+oldgap//16
        assert image[0xbca4:0xbca9]==bytes.fromhex('6a019ac801') and 0xbca9 in rel
        delay_segment=struct.unpack_from('<H',image,0xbca9)[0]
        cfg={'PRACTICE_ORG':insert-cs*16,'SOLO_UPDATE':p['entries']['update'],'SOLO_PAUSE':p['entries']['pause'],'SOLO_MAILBOX':p['entries']['mailbox'],'SOLO_CURRENT_INPUT':p['entries']['currentInput'],'PRACTICE_DELAY_SEGMENT':delay_segment,
             'DIALOG_DELTA':p['dataSegment']-dialog_cs}
        (out/'practice-config.inc').write_text(''.join(f'%define {k} {v}\n' for k,v in cfg.items()))
        common=ROOT.parent.parent/'common/pc98'
        subprocess.run([str(nasm),'-f','bin','-I',str(out)+'/', '-I',str(common)+'/', '-I',str(ROOT/'patches')+'/',str(ROOT/'patches/practice.asm'),'-o',str(out/'practice.bin')],check=True)
        code=(out/'practice.bin').read_bytes();assert code[:8]==b'T02PRA01'
        entries=dict(zip(NAMES,struct.unpack_from('<'+str(len(NAMES))+'H',code,8)));gap=(len(code)+15)&~15
        assert gap<=12288 and insert-cs*16+gap<65536
        insert_bridge(h,image,rel,insert,gap);image[insert:insert+len(code)]=code
        edits=[]
        def patch(at,before,after):
            assert image[at:at+len(before)]==before,(language,hex(at),image[at:at+len(before)].hex(),before.hex())
            assert len(before)==len(after);image[at:at+len(after)]=after
            edits.append({'offset':at,'before':before.hex(),'after':after.hex()})
        def near(at,before,name,jump=False):
            patch(at,before,bytes([0xe9 if jump else 0xe8])+struct.pack('<H',(cs*16+entries[name]-at-3)&65535)+b'\x90'*(len(before)-3))
        near(0xb1ca,bytes.fromhex('e8de00'),'load')
        near(0xbdcc,b'\xe8'+struct.pack('<H',(cs*16+p['entries']['update']-0xbdcf)&65535),'frame')
        near(0xbdeb,bytes.fromhex('e8021d'),'entities')
        focus_pattern=b'\x2e\xa1'+struct.pack('<H',p['entries']['currentInput'])+b'\xc3'
        assert image.count(focus_pattern)==1
        focus_at=image.index(focus_pattern)
        near(focus_at,focus_pattern[:4],'focusHook')
        focus_test=b'\x2e\xf7\x06'+struct.pack('<H',p['entries']['currentInput'])+b'\x00\x01'
        near(cs*16+p['entries']['update']+0xae,focus_test,'focusTest')
        near(0xef69,bytes.fromhex('e88600'),'miss')
        near(0xf0da,bytes.fromhex('803e2c0b007526'),'deathGate',True)
        near(0xf142,bytes.fromhex('fe0e2c0b'),'lifeConsume')
        near(0xe28a,bytes.fromhex('803e2d0b007446'),'bombGate',True)
        near(0xe29b,bytes.fromhex('fe0e2d0b'),'bombConsume')
        near(0xdf76,bytes.fromhex('558bec'),'livesZero',True)
        near(0xdfc4,bytes.fromhex('558bec'),'bombsZero',True)
        def far(at,before,name):
            patch(at,before,b'\x9a'+struct.pack('<HH',entries[name],cs)+b'\x90'*(len(before)-5));rel.append(at+3)
        patch(0xbdf7,bytes.fromhex('9aaa410000'),b'\x9a'+struct.pack('<HH',entries['world'],cs))
        # Reuse the original far CALL's existing MZ segment relocation.
        assert 0xbdfa in rel
        # world_hook's original far target is segment zero, relocated by DOS.
        rel.append(cs*16+entries['world']+3)
        # Result-menu wait uses the same relocated far frame-delay target as
        # MAIN's native pause loop.
        needle=bytes.fromhex('6a019ac801')+struct.pack('<H',delay_segment)
        assert code.count(needle)==3
        rel.extend(insert+i+5 for i in range(len(code)) if code.startswith(needle,i))
        # FAR CALL retains the native near epilogue/CS. Reproduce only the
        # damage statistic and AX load; native POP/LEAVE/RET 8 remain in place.
        far(0x129d0+oldgap+gap,bytes.fromhex('01063c3e8b46f8'),'shotsReturn')
        for at,before,name in [(0x13a85,'813eda28b800','timeMid1'),(0x143c7,'813ee02b4006','timeMid2'),
                               (0x11689,'803e385504','timeMid3'),(0x1a3a3,'803e0d930c','timeMid4'),
                               (0x193ba,'a14e923b068292','timeMima'),(0x193a8,'833ee02b00','transition'),
                               (0x16706,'833ee02b00','transition')]:
            far(at+oldgap+gap,bytes.fromhex(before),name)
        # Far thunks retain the near ABI using PUSH CS / RETF. Ordinary full
        # runs execute every displaced instruction in the original segment.
        for at,name in [(0x13055,'dialogBox'),(0x12de0,'dialogPre'),(0x12e95,'dialogPost')]:
            moved=at+oldgap+gap
            # dialogBox is a Pascal near proc, hence argument handling in its
            # special far wrapper. These entry jumps are installed below.
            before=bytes(image[moved:moved+5]);assert before[:4]==(bytes.fromhex('c8040000') if name!='dialogBox' else bytes.fromhex('c8040000')),(name,before.hex())
            # Save audited prologues separately; out-of-segment wrappers execute
            # them before returning to their native body.
            patch(moved,before,b'\xea'+struct.pack('<HH',entries[name],cs));rel.append(moved+3)
        exe=mz_finish(h,image,rel);name=f'practice-{language}.exe.gz';(ROOT/'web'/name).write_bytes(gzip.compress(exe,mtime=0))
        p['dataSegment']+=gap//16;p['gap']+=gap;p['mainSha256']=sha(exe)
        p['practice']={'version':1,'entries':entries,'bytes':gap,'descriptor':'T02PRA01'}
        manifest['languages'][language]={'url':name,'size':len(exe),'sha256':sha(exe),'sourceMainSha256':sha(source),'patch':p}
        (out/f'{language}.json').write_text(json.dumps({'edits':edits,'extraBytes':gap},indent=2)+'\n')
    launcher=(ROOT/'patches/launch.asm').read_text().replace('TH02WEBSTARTv1!','TH02PRASTARTv1!')
    (out/'launch.asm').write_text(launcher)
    subprocess.run([str(nasm),'-f','bin',str(out/'launch.asm'),'-o',str(ROOT/'web/practice-start.com')],check=True)
    data=(ROOT/'web/practice-start.com').read_bytes();manifest['launcher']={'url':'practice-start.com','sha256':sha(data),'size':len(data),'signature':'TH02PRASTARTv1!'}
    (ROOT/'web/practice-assets.json').write_text(json.dumps(manifest,indent=2)+'\n')
    print('Built isolated TH02 practice overlays; all published disks preserved')
if __name__=='__main__':main()
