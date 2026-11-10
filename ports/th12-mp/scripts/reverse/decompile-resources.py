"""Read-only whole-retail script decompilation, distinct from EXE pseudocode.

All generated retail text stays local. Source files, replays, and goldens are
never altered. This does not replace execution-based behavioral verification.
"""
import collections, hashlib, json, pathlib, struct, subprocess, sys

TITLE=pathlib.Path(__file__).resolve().parents[2]
RETAIL=TITLE/'local/retail'
OUT=TITLE/'local/reverse/th12-1.00b/full/resources'
TOOLS=TITLE.parent/'tools/thtk/thtk-bin-12'
EXPECTED='99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417'

def sha(path):return hashlib.sha256(path.read_bytes()).hexdigest()

def ecl_index(path):
    b=path.read_bytes();U16=lambda at:struct.unpack_from('<H',b,at)[0];U32=lambda at:struct.unpack_from('<I',b,at)[0]
    if len(b)<36 or b[:4]!=b'SCPT' or U16(4)!=1:raise ValueError('ECL header: '+path.name)
    table=36+U16(6);count=U16(16)
    if table+count*4>len(b):raise ValueError('ECL routine table')
    offsets=[U32(table+i*4) for i in range(count)];at=table+count*4;names=[]
    for i in range(count):
        end=b.find(b'\0',at)
        if end<0:raise ValueError('ECL routine name')
        names.append(b[at:end].decode('ascii'));at=end+1
    routines=[];totals=collections.Counter()
    for name,start in zip(names,offsets):
        if start+16>len(b) or b[start:start+4]!=b'ECLH':raise ValueError('ECL routine header')
        end=min([off for off in offsets if off>start]+[len(b)]);at=start+16;commands=[]
        while at<end:
            if at+16>end:raise ValueError('ECL truncated instruction')
            time,op,size,mask=struct.unpack_from('<iHHH',b,at)
            if size<16 or size%4 or size>end-at:raise ValueError('ECL instruction size')
            words=list(struct.unpack_from('<'+'I'*((size-16)//4),b,at+16))
            commands.append({'offset':at-start-16,'time':time,'opcode':op,'size':size,
                             'parameterMask':mask,'rankMask':b[at+10],'parameterCount':b[at+11],
                             'operandWords':words})
            totals[op]+=1;at+=size
        routines.append({'name':name,'fileOffset':start,'commands':commands})
    return {'file':path.name,'sha256':sha(path),'bytes':len(b),'routines':routines,
            'opcodeCounts':dict(sorted(totals.items()))}

def sht_index(path):
    b=path.read_bytes()
    if len(b)<0x268 or struct.unpack_from('<H',b)[0]!=3:raise ValueError('SHT header')
    count=struct.unpack_from('<H',b,2)[0]
    if count>64 or 0x268+count*8>len(b):raise ValueError('SHT group count')
    names=('hitboxRadius','attractionSpeed','attractionDiameter','speed','focusSpeed','diagonalSpeed','focusDiagonalSpeed')
    header=dict(zip(names,struct.unpack_from('<7f',b,4)))
    header.update(maxLevel=struct.unpack_from('<i',b,32)[0],filePowerStep=struct.unpack_from('<i',b,36)[0])
    result={'file':path.name,'sha256':sha(path),'header':header,
            'optionOffsets':[list(struct.unpack_from('<2f',b,40+i*8)) for i in range(72)],'groups':[]}
    for i in range(count):
        start,other=struct.unpack_from('<2I',b,0x268+i*8);at=start;shots=[]
        if start<0x268+count*8 or start>=len(b):raise ValueError('SHT group pointer')
        for budget in range(1000):
            if at>=len(b):raise ValueError('SHT truncated shot')
            if struct.unpack_from('<b',b,at)[0]<0:break
            if at+52>len(b):raise ValueError('SHT shot size')
            values=struct.unpack_from('<bbh6fbbhhh4I',b,at)
            fields=('interval','delay','damage','x','y','hitWidth','hitHeight','angle','speed','option','type','animation','hitAnimation','sound','spawnCallbackId','updateCallbackId','hitCallbackId','extraCallbackId')
            shots.append(dict(zip(fields,values)));at+=52
        else:raise ValueError('SHT shot budget')
        result['groups'].append({'index':i,'fileOffset':start,'tableWord1':other,'shots':shots})
    return result

def run():
    if sha(TITLE.parent/'th12/th12.exe')!=EXPECTED:raise RuntimeError('Original EXE identity changed')
    identity=json.loads((TITLE/'local/content-identity.json').read_text())
    for name,record in identity['files'].items():
        if sha(TITLE.parent/'th12'/name)!=record['sha256']:raise RuntimeError('Original file identity changed: '+name)
    OUT.mkdir(parents=True,exist_ok=True)
    tool_hashes={name:sha(TOOLS/name) for name in ('thecl.exe','thanm.exe','thmsg.exe','thstd.exe','thtk.dll')}
    records=[];ecl=[]
    for source in sorted(RETAIL.iterdir()):
        ext=source.suffix.lower()
        if ext not in ('.ecl','.anm','.msg','.std','.sht'):continue
        directory=OUT/ext[1:];directory.mkdir(exist_ok=True)
        destination=directory/(source.name+'.txt')
        if ext=='.sht':
            decoded=sht_index(source);destination=directory/(source.name+'.json')
            destination.write_text(json.dumps(decoded,indent=2)+'\n',encoding='utf-8')
            records.append({'source':source.name,'sourceSha256':sha(source),'bytes':source.stat().st_size,
                            'status':'generated','decoder':'documented TH12 SHT v3 layout',
                            'output':str(destination.relative_to(OUT)),'outputSha256':sha(destination)})
            continue
        if ext=='.ecl':
            command=[str(TOOLS/'thecl.exe'),'-d','12',str(source),str(destination)]
            decoded=ecl_index(source);ecl.append(decoded)
            raw=[str(TOOLS/'thecl.exe'),'-d','12','-r',str(source),str(directory/(source.name+'.raw.txt'))]
            raw_result=subprocess.run(raw,capture_output=True,timeout=30)
            if raw_result.returncode:raise RuntimeError('Raw ECL decoder failed: '+source.name)
        elif ext=='.std':command=[str(TOOLS/'thstd.exe'),'-d','12',str(source),str(destination)]
        elif ext=='.msg':
            command=[str(TOOLS/'thmsg.exe'),'-d','12']
            if source.stem=='staff' or (source.stem.startswith('e') and source.stem[1:].isdigit()):command+=['-e']
            command+=[str(source),str(destination)]
        else:command=[str(TOOLS/'thanm.exe'),'-l',str(source)]
        process=subprocess.run(command,capture_output=True,timeout=30)
        if ext=='.anm' and process.returncode==0:destination.write_bytes(process.stdout)
        log=process.stderr.decode('utf-8','replace');(directory/(source.name+'.log')).write_text(log,encoding='utf-8')
        record={'source':source.name,'sourceSha256':sha(source),'bytes':source.stat().st_size,
                'status':'generated' if process.returncode==0 and destination.exists() and destination.stat().st_size else 'failed',
                'decoder':pathlib.Path(command[0]).name,'exitCode':process.returncode,
                'diagnostics':log,'output':str(destination.relative_to(OUT))}
        if destination.exists():record['outputSha256']=sha(destination)
        if destination.exists() and destination.suffix=='.txt':
            raw=destination.read_bytes()
            for encoding in ('utf-8','cp932'):
                try:readable=raw.decode(encoding)
                except UnicodeDecodeError:continue
                preview=destination.with_suffix('.utf8.txt');preview.write_text(readable,encoding='utf-8')
                record.update(textEncoding=encoding,readableUtf8=str(preview.relative_to(OUT)));break
            else:record.update(textEncoding='unresolved',textDecodeVerified=False)
        records.append(record)
    counts=collections.Counter();uses=collections.defaultdict(list)
    for file in ecl:
        for op,n in file['opcodeCounts'].items():counts[op]+=n;uses[op].append({'file':file['file'],'count':n})
    (OUT/'ecl-instructions.json').write_text(json.dumps({'schema':'th12-retail-ecl-inventory/1',
        'originalExeSha256':EXPECTED,'wholeGameOracle':False,'files':ecl},indent=2)+'\n',encoding='utf-8')
    (OUT/'ecl-opcodes.json').write_text(json.dumps({'schema':'th12-retail-ecl-opcode-coverage/1',
        'originalExeSha256':EXPECTED,'files':len(ecl),'routineCount':sum(len(x['routines']) for x in ecl),
        'instructionCount':sum(counts.values()),'opcodes':{op:{'count':n,'uses':uses[op]} for op,n in sorted(counts.items())}},indent=2)+'\n',encoding='utf-8')
    report={'schema':'th12-whole-retail-script-decompilation/1','originalExeSha256':EXPECTED,
            'originalSourceRecovered':False,'wholeGameOracle':False,'toolSha256':tool_hashes,
            'files':records,'counts':dict(collections.Counter(x['status'] for x in records)),
            'fileTypes':dict(collections.Counter(pathlib.Path(x['source']).suffix for x in records)),
            'copyright':'Original game and retail content belong to Team Shanghai Alice; local research artifacts only.'}
    (OUT/'index.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
    print(json.dumps({k:report[k] for k in ('counts','fileTypes')},indent=2))
    print(json.dumps({'ECLroutines':sum(len(x['routines']) for x in ecl),'ECLinstructions':sum(counts.values()),'uniqueOpcodes':len(counts)}))

if __name__=='__main__':run()
