"""Inventory actual typed ANM RNG operands, excluding variable destinations."""
import pathlib,json,hashlib
ROOT=pathlib.Path(__file__).resolve().parents[2]
from native_anm_resource import check_original,EXE_SHA256
check_original(ROOT.parent/'th12/th12.exe')
atlases=json.loads((ROOT/'site/assets/atlases.json').read_text(encoding='utf-8'));sites=[];unknown=[]
# Native typed source operand indices. Masked destinations are pointers, not
# variable-reader calls, and must not be mistaken for builtin RNG reads.
sources={3:'i',4:'ii',5:'-ii',40:'-i',41:'-f',42:'-f',43:'-f',47:'-f',48:'fff',49:'fff',50:'ff',51:'i',52:'iii',53:'fff',54:'ff',
    55:'-i',56:'iifff',57:'iiiii',58:'iii',59:'iifff',60:'iiff',65:'i',66:'i',67:'i',68:'i',70:'f',71:'f',75:'i',76:'iii',77:'i',78:'iiiii',79:'iii',
    84:'i',88:'i',90:'i',91:'i',92:'i',93:'iif',94:'iif',95:'i',96:'iff',97:'iff',100:'ifffffffff',101:'i',102:'ii',103:'ff',104:'fi',105:'fi',106:'ff',107:'iiff',108:'fii',110:'ff'}
for op in range(6,28):sources[op]='-'+('f' if op%2 else 'i')*(2 if op>=18 else 1)
for op in range(28,40):sources[op]=('f' if op%2 else 'i')*2+'ii'
for name,bank in atlases.items():
    if 'scripts' not in bank:continue
    for sid,script in bank['scripts'].items():
        commands=script['commands'];switches=[{'offset':c['offset'],'masked':bool(c['mask']&1),'value':c['ints'][0]} for c in commands if c['op']==87]
        # A script may jump, interrupt, or be rebound with454ee0. Record all
        # domain switches instead of pretending a linear scan proves its domain.
        domains=['game','display'] if switches else ['game']
        for c in commands:
            op=c['op'];base={'bank':name,'script':int(sid),'byteOffset':c['offset'],'op':op,'time':c['time']}
            if op in [40,41,102] and not(op==40 and not(c['mask']&2) and c['ints'][1]==0):
                sites.append({**base,'source':'opcode','domains':['game'] if op==102 else domains,'domainSwitches':switches,
                    'domainEvidence':'455ef1 fixed4ce568' if op==102 else '4551b0/4553f0 respectVM480bit0; constructor402520 resetsgame'})
            for index,kind in enumerate(sources.get(op,'')):
                if kind=='-' or not(c['mask']&(1<<index)):continue
                value=(c['floats'] if kind=='f' else c['ints'])[index]
                if (kind=='f' and value in [10010.,10011.,10012.,10022.]) or (kind=='i' and value==10022):
                    sites.append({**base,'source':'typedOperand','operand':index,'type':'float' if kind=='f' else 'int','builtin':int(value),'domains':domains,'domainSwitches':switches})
            if c['mask'] and op not in sources and op not in [0,1,2,63,64,69,72,73,74,80,82,85,86,87,89]:unknown.append({**base,'mask':c['mask']})
report={'schema':'th12-retail-anm-rng-inventory/1','originalExeSha256':EXE_SHA256,
    'provider':'Unmodified ANM opcode/typed-mask metadata; source-operand types from original455630 dispatch and4551b0/4553f0',
    'bindingEvidence':'454d10->402520 resetsgame and immediatelyticks;454ee0 preservesdomain; opcode102 alwaysgame',
    'limitations':['Domain sets are conservative and not full CFG reachability; switches/interruption/rebind can alter currentdomain',
        'Custom update/draw callbacks and manager scheduling are not bytecode inventory',
        'No seed is consumed while inventorying or drawing'],
    'wholeGameDeterminism':False,'wholePresentationOracle':False,
    'resourceSha256':{n:b['provenance']['sha256'] for n,b in atlases.items() if 'scripts' in b},'sites':sites,'unclassifiedMaskedOpcodes':unknown}
out=ROOT/'artifacts/reverse/anm-random-inventory.json';out.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
print(json.dumps({'output':str(out),'sites':len(sites),'banks':sorted(set(s['bank'] for s in sites)),'unclassifiedMaskedOpcodes':unknown}))
